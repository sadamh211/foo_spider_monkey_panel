#include <stdafx.h>

#include "ui_property.h"

#include <utils/array_x.h>
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

CDialogProperty::CDialogProperty( smp::panel::js_panel_window* p_parent )
    : parentPanel_( p_parent )
{
}

LRESULT CDialogProperty::OnInitDialog( HWND, LPARAM )
{
    DlgResize_Init();

    // Subclassing
    propertyListCtrl_.SubclassWindow( GetDlgItem( IDC_LIST_PROPERTIES ) );
    propertyListCtrl_.ModifyStyle( 0, LBS_SORT | LBS_HASSTRINGS );
    propertyListCtrl_.SetExtendedListStyle( PLS_EX_SORTED | PLS_EX_XPLOOK );

    LoadProperties();

    return TRUE; // set focus to default control
}

LRESULT CDialogProperty::OnCloseCmd( WORD, WORD wID, HWND )
{
    switch ( wID )
    {
    case IDAPPLY:
        Apply();
        return 0;
    case IDOK:
        Apply();
        EndDialog( wID );
        return 0;
    default:
        EndDialog( wID );
        return 0;
    }
}

LRESULT CDialogProperty::OnPinItemChanged( LPNMHDR pnmh )
{
    auto pnpi = reinterpret_cast<LPNMPROPERTYITEM>( pnmh );

    auto& localPropertyValues = localProperties_.values;
    if ( auto it = localPropertyValues.find( pnpi->prop->GetName() );
         it != localPropertyValues.end() )
    {
        auto& val = *( it->second );
        _variant_t var;

        if ( pnpi->prop->GetValue( &var ) )
        {
            std::visit( [&var]( auto& arg ) {
                using T = std::decay_t<decltype( arg )>;
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
                    arg = smp::unicode::ToU8( std::wstring_view{ var.bstrVal ? var.bstrVal : L"" } );
                }
                else
                {
                    static_assert( smp::always_false_v<T>, "non-exhaustive visitor!" );
                }
            },
                        val );
        }
    }

    return 0;
}

LRESULT CDialogProperty::OnClearallBnClicked( WORD, WORD, HWND )
{
    localProperties_.values.clear();
    propertyListCtrl_.ResetContent();

    return 0;
}

void CDialogProperty::Apply()
{
    // Copy back
    parentPanel_->GetSettings().properties = localProperties_;
    parentPanel_->update_script();
    LoadProperties();
}

void CDialogProperty::LoadProperties( bool reload )
{
    propertyListCtrl_.ResetContent();

    if ( reload )
    {
        localProperties_ = parentPanel_->GetSettings().properties;
    }

    struct LowerLexCmp
    { // lexicographical comparison but with lower cased chars
        bool operator()( const std::wstring& a, const std::wstring& b ) const
        {
            return ( _wcsicmp( a.c_str(), b.c_str() ) < 0 );
        }
    };
    std::map<std::wstring, HPROPERTY, LowerLexCmp> propMap;
    for ( const auto& [name, pSerializedValue]: localProperties_.values )
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

LRESULT CDialogProperty::OnDelBnClicked( WORD, WORD, HWND )
{
    if ( int idx = propertyListCtrl_.GetCurSel();
         idx )
    {
        HPROPERTY hproperty = propertyListCtrl_.GetProperty( idx );
        std::wstring name = hproperty->GetName();

        propertyListCtrl_.DeleteItem( hproperty );
        localProperties_.values.erase( name );
    }

    return 0;
}

LRESULT CDialogProperty::OnImportBnClicked( WORD, WORD, HWND )
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
            localProperties_ = PanelProperties::FromJson( pfc_x::ReadRawString( *io, abort ) );
        }
        else if ( extension == ".smp" )
        {
            localProperties_ = PanelProperties::Load( *io, abort, smp::config::SerializationFormat::Binary );
        }
        else if ( extension == ".wsp" )
        {
            localProperties_ = PanelProperties::Load( *io, abort, smp::config::SerializationFormat::Com );
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

        LoadProperties( false );
    }
    catch ( const SmpException& )
    {
        // TODO: popup error message
    }
    catch ( const pfc::exception& )
    {
        // TODO: popup error message
    }

    return 0;
}

LRESULT CDialogProperty::OnExportBnClicked( WORD, WORD, HWND )
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

        pfc_x::WriteStringRaw( *io, abort, localProperties_.ToJson() );
    }
    catch ( const pfc::exception& )
    {
        // TODO: error popup
    }

    return 0;
}

} // namespace smp::ui
