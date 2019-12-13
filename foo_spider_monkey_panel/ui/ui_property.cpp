#include <stdafx.h>

#include "ui_property.h"

#include <ui/ui_conf_new.h>
#include <utils/array_x.h>
#include <utils/error_popup.h>
#include <utils/file_helpers.h>
#include <utils/type_traits_x.h>

#include <abort_callback.h>
#include <js_panel_window.h>

#include <filesystem>
#include <iomanip>
#include <map>

namespace fs = std::filesystem;

namespace smp::ui
{

CConfigTabProperties::CConfigTabProperties( CDialogConfNew& parent, OptionWrap<config::PanelSettings>& settings )
    : parent_( parent )
    , properties_(
          settings, []( auto& value ) -> auto& { return value.properties; } )
{
}

HWND CConfigTabProperties::CreateTab( HWND hParent )
{
    return Create( hParent );
}

ATL::CDialogImplBase& CConfigTabProperties::Dialog()
{
    return *this;
}

const wchar_t* CConfigTabProperties::Name() const
{
    return L"Properties";
}

bool CConfigTabProperties::ValidateState()
{
    return true;
}

bool CConfigTabProperties::HasChanged()
{
    return properties_.HasChanged();
}

void CConfigTabProperties::Apply()
{
    properties_.Apply();
}

void CConfigTabProperties::Revert()
{
    properties_.Revert();
}

LRESULT CConfigTabProperties::OnInitDialog( HWND, LPARAM )
{
    DlgResize_Init( false, false, WS_CHILD );

    // Subclassing
    propertyListCtrl_.SubclassWindow( GetDlgItem( IDC_LIST_PROPERTIES ) );
    propertyListCtrl_.ModifyStyle( 0, LBS_SORT | LBS_HASSTRINGS );
    propertyListCtrl_.SetExtendedListStyle( PLS_EX_SORTED | PLS_EX_XPLOOK );

    CWindow{ GetDlgItem( IDC_DEL ) }.EnableWindow( propertyListCtrl_.GetCurSel() != -1 );

    UpdateUiFromData();

    return TRUE; // set focus to default control
}

LRESULT CConfigTabProperties::OnPinItemChanged( LPNMHDR pnmh )
{
    auto pnpi = (LPNMPROPERTYITEM)pnmh;

    properties_.ModifyValue( [pnpi]( auto& props ) {
        auto& propValues = props.values;

        if ( auto it = propValues.find( pnpi->prop->GetName() );
             it != propValues.end() )
        {
            auto& val = *( it->second );
            _variant_t var;

            if ( pnpi->prop->GetValue( &var ) )
            {
                return std::visit( [&var]( auto& arg ) {
                    using T = std::decay_t<decltype( arg )>;
                    const auto prevArgValue = arg;
                    if constexpr ( std::is_same_v<T, bool> )
                    {
                        var.ChangeType( VT_BOOL );
                        arg = static_cast<bool>( var.boolVal );
                    }
                    else if constexpr ( std::is_same_v<T, int32_t> )
                    {
                        var.ChangeType( VT_I4 );
                        arg = static_cast<int32_t>( var.lVal );
                    }
                    else if constexpr ( std::is_same_v<T, double> )
                    {
                        if ( VT_BSTR == var.vt )
                        {
                            arg = std::stod( var.bstrVal );
                        }
                        else
                        {
                            var.ChangeType( VT_R8 );
                            arg = var.dblVal;
                        }
                    }
                    else if constexpr ( std::is_same_v<T, std::u8string> )
                    {
                        var.ChangeType( VT_BSTR );
                        arg = smp::unicode::ToU8( var.bstrVal );
                    }
                    else
                    {
                        static_assert( smp::always_false_v<T>, "non-exhaustive visitor!" );
                    }

                    return ( prevArgValue == arg );
                },
                                   val );
            }
        }
        
        return false;
    } );

    parent_.OnDataChanged();

    return 0;
}

LRESULT CConfigTabProperties::OnSelChanged( LPNMHDR )
{
    UpdateUiDelButton();
    return 0;
}

LRESULT CConfigTabProperties::OnClearallBnClicked( WORD, WORD, HWND )
{
    properties_.ModifyValue( []( auto& props ) {
        props.values.clear();
        return true;
    } );
    propertyListCtrl_.ResetContent();

    parent_.OnDataChanged();

    return 0;
}

LRESULT CConfigTabProperties::OnDelBnClicked( WORD, WORD, HWND )
{
    if ( int idx = propertyListCtrl_.GetCurSel();
         idx >= 0 )
    {
        HPROPERTY hproperty = propertyListCtrl_.GetProperty( idx );
        std::wstring name = hproperty->GetName();

        propertyListCtrl_.DeleteItem( hproperty );
        properties_.ModifyValue( [&name]( auto& props ) {
            props.values.erase( name );
            return true;
        } );
    }

    UpdateUiDelButton();
    parent_.OnDataChanged();

    return 0;
}

LRESULT CConfigTabProperties::OnImportBnClicked( WORD, WORD, HWND )
{
    using namespace smp::config;

    constexpr auto k_DialogImportExtFilter = smp::to_array<COMDLG_FILTERSPEC>( {
        { L"Property files", L"*.json;*.smp;*.wsp" },
        { L"All files", L"*.*" },
    } );

    fs::path path( smp::file::FileDialog( L"Import from", false, k_DialogImportExtFilter, L"json", L"props" ) );
    if ( path.empty() )
    {
        return 0;
    }
    path = path.lexically_normal();

    try
    {
        auto& abort = smp::GlobalAbortCallback::GetInstance();
        file_ptr io;
        filesystem::g_open_read( io, path.u8string().c_str(), abort );

        const auto extension = path.extension();
        if ( extension == ".json" )
        {
            properties_ = PanelProperties::FromJson( pfc_x::ReadRawString( *io, abort ) );
        }
        else if ( extension == ".smp" )
        {
            properties_ = PanelProperties::Load( *io, abort, smp::config::SerializationFormat::Binary );
        }
        else if ( extension == ".wsp" )
        {
            properties_ = PanelProperties::Load( *io, abort, smp::config::SerializationFormat::Com );
        }
        else
        { // let's brute-force it!
            const auto loadProps = [&io, &abort]( smp::config::SerializationFormat format ) -> std::optional<PanelProperties> {
                try
                {
                    return PanelProperties::Load( *io, abort, format );
                }
                catch ( const SmpException& )
                {
                    return std::nullopt;
                }
            };

            if ( !loadProps( smp::config::SerializationFormat::Json )
                 && !loadProps( smp::config::SerializationFormat::Binary )
                 && !loadProps( smp::config::SerializationFormat::Com ) )
            {
                throw SmpException( "Failed to parse panel properties: unknown format" );
            }
        }

        UpdateUiFromData();
    }
    catch ( const SmpException& e )
    {
        smp::utils::ReportErrorWithPopup( e.what() );
    }
    catch ( const pfc::exception& e )
    {
        smp::utils::ReportErrorWithPopup( e.what() );
    }

    parent_.OnDataChanged();

    return 0;
}

LRESULT CConfigTabProperties::OnExportBnClicked( WORD, WORD, HWND )
{
    constexpr auto k_DialogExportExtFilter = smp::to_array<COMDLG_FILTERSPEC>(
        {
            { L"Property files", L"*.json" },
            { L"All files", L"*.*" },
        } );

    fs::path path( smp::file::FileDialog( L"Save as", true, k_DialogExportExtFilter, L"json", L"props" ) );
    if ( path.empty() )
    {
        return 0;
    }
    path = path.lexically_normal();

    try
    {
        auto& abort = smp::GlobalAbortCallback::GetInstance();
        file_ptr io;
        filesystem::g_open_write_new( io, path.u8string().c_str(), abort );

        pfc_x::WriteStringRaw( *io, abort, properties_.GetCurrentValue().ToJson() );
    }
    catch ( const pfc::exception& e )
    {
        smp::utils::ReportErrorWithPopup( e.what() );
    }

    return 0;
}

void CConfigTabProperties::UpdateUiFromData()
{
    propertyListCtrl_.ResetContent();

    struct LowerLexCmp
    { // lexicographical comparison but with lower cased chars
        bool operator()( const std::wstring& a, const std::wstring& b ) const
        {
            return ( _wcsicmp( a.c_str(), b.c_str() ) < 0 );
        }
    };
    std::map<std::wstring, HPROPERTY, LowerLexCmp> propMap;
    for ( const auto& [name, pSerializedValue]: properties_.GetCurrentValue().values )
    {
        HPROPERTY hProp = std::visit( [&name = name]( auto&& arg ) {
            using T = std::decay_t<decltype( arg )>;
            if constexpr ( std::is_same_v<T, bool> || std::is_same_v<T, int32_t> )
            {
                return PropCreateSimple( name.c_str(), arg );
            }
            else if constexpr ( std::is_same_v<T, double> )
            {
                const std::wstring strNumber = [arg] {
                    if ( std::trunc( arg ) == arg )
                    { // Most likely uint64_t
                        return std::to_wstring( static_cast<uint64_t>( arg ) );
                    }

                    // std::to_string(double) has precision of float
                    return fmt::format( L"{:.16g}", arg );
                }();
                return PropCreateSimple( name.c_str(), strNumber.c_str() );
            }
            else if constexpr ( std::is_same_v<T, std::u8string> )
            {
                return PropCreateSimple( name.c_str(), smp::unicode::ToWide( arg ).c_str() );
            }
            else
            {
                static_assert( smp::always_false_v<T>, "non-exhaustive visitor!" );
            }
        },
                                      *pSerializedValue );

        propMap.emplace( name, hProp );
    }

    for ( auto& [name, hProp]: propMap )
    {
        propertyListCtrl_.AddItem( hProp );
    }
}

void CConfigTabProperties::UpdateUiDelButton()
{
    CWindow{ GetDlgItem( IDC_DEL ) }.EnableWindow( propertyListCtrl_.GetCurSel() != -1 );
}

} // namespace smp::ui
