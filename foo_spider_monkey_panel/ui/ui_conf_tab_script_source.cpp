#include <stdafx.h>

#include "ui_conf_tab_script_source.h"

#include <config/smp_config.h>
#include <ui/ui_conf_new.h>
#include <utils/edit_text.h>
#include <utils/error_popup.h>
#include <utils/file_helpers.h>
#include <utils/scope_helpers.h>
#include <utils/type_traits_x.h>
#include <utils/winapi_error_helpers.h>

#include <component_paths.h>

#include <filesystem>

namespace
{

using namespace smp;
using namespace smp::ui;

std::vector<CConfigTabScriptSource::SampleComboBoxElem> GetSampleFileData()
{
    namespace fs = std::filesystem;

    auto sampleFolderPath = fs::u8path( get_fb2k_component_path() ) / "samples";

    std::vector<CConfigTabScriptSource::SampleComboBoxElem> elems;

    for ( const auto& subdir: { "complete", "jsplaylist-mod", "js-smooth", "basic" } )
    {
        for ( const auto& filepath: fs::directory_iterator( sampleFolderPath / subdir ) )
        {
            if ( filepath.path().extension() == ".js" )
            {
                elems.emplace_back( filepath.path().wstring(), fs::relative( filepath, sampleFolderPath ) );
            }
        }
    }

    return elems;
}

} // namespace

namespace smp::ui
{

std::vector<CConfigTabScriptSource::SampleComboBoxElem> CConfigTabScriptSource::sampleData_;

CConfigTabScriptSource::CConfigTabScriptSource( CDialogConfNew& parent, OptionWrap<config::PanelSettings>& settings )
    : parent_( parent )
    , settings_( settings )
    , payload_(
          settings, []( auto& value ) -> auto& { return value.payload; } )
    , ddx_( {
          CreateUiDdx<UiDdx_TextEdit>( path_, IDC_TEXTEDIT_SRC_PATH ),
          CreateUiDdx<UiDdx_ComboBox>( sampleIdx_, IDC_COMBO_SRC_SAMPLE ),
          CreateUiDdx<UiDdx_RadioRange>( payloadSwitchId_,
                                         std::initializer_list<int>{
                                             IDC_RADIO_SRC_SAMPLE,
                                             IDC_RADIO_SRC_MEMORY,
                                             IDC_RADIO_SRC_FILE,
                                         } ),
      } )
{
    if ( sampleData_.empty() )
    { // can't initialize it during global initialization
        sampleData_ = GetSampleFileData();
    }
}

HWND CConfigTabScriptSource::CreateTab( HWND hParent )
{
    return Create( hParent );
}

CDialogImplBase& CConfigTabScriptSource::Dialog()
{
    return *this;
}

const wchar_t* CConfigTabScriptSource::Name() const
{
    return L"Script";
}

bool CConfigTabScriptSource::ValidateState()
{
    namespace fs = std::filesystem;

    if ( std::holds_alternative<config::PanelSettings_File>( payload_.GetCurrentValue() ) )
    {
        try
        {
            const auto& path = std::get<config::PanelSettings_File>( payload_.GetCurrentValue() ).path;
            if ( !fs::exists( fs::u8path( path ) ) )
            {
                ::SetFocus( GetDlgItem( IDC_TEXTEDIT_SRC_PATH ) );
                smp::utils::ReportErrorWithPopup( fmt::format( "Invalid path to script: {}", ( path.empty() ? std::u8string( "<empty>" ) : path ) ) );
                return false;
            }
        }
        catch ( const fs::filesystem_error& e )
        {
            smp::utils::ReportErrorWithPopup( e.what() );
        }
    }

    return true;
}

bool CConfigTabScriptSource::HasChanged()
{
    return payload_.HasChanged();
}

void CConfigTabScriptSource::Apply()
{
    payload_.Apply();
}

void CConfigTabScriptSource::Revert()
{
    payload_.Revert();
}

BOOL CConfigTabScriptSource::OnInitDialog( HWND hwndFocus, LPARAM lParam )
{
    for ( auto& ddx: ddx_ )
    {
        ddx->SetHwnd( m_hWnd );
    }

    InitializeSamplesComboBox();
    InitializeLocalOptions();
    UpdateUiFromData();

    return TRUE; // set focus to default control
}

void CConfigTabScriptSource::OnScriptSrcChange( UINT uNotifyCode, int nID, CWindow wndCtl )
{
    if ( isInitializingUi_ )
    {
        return;
    }

    if ( !parent_.IsCleanSlate() && !RequestConfirmationForReset() )
    {
        UpdateUiFromData();
        return;
    }

    auto it = ranges::find_if( ddx_, [nID]( auto& ddx ) { return ddx->IsMatchingId( nID ); } );
    if ( ddx_.end() != it )
    {
        ( *it )->ReadFromUi();
    }

    config::PanelSettings newSettings;
    switch ( payloadSwitchId_ )
    {
    case IDC_RADIO_SRC_SAMPLE:
    {
        newSettings.payload = ( sampleData_.empty()
                                    ? config::PanelSettings_Sample{}
                                    : config::PanelSettings_Sample{ smp::unicode::ToU8( sampleData_[sampleIdx_].displayedName ) } );
        break;
    }
    case IDC_RADIO_SRC_FILE:
    {
        newSettings.payload = config::PanelSettings_File{ path_ };
        break;
    }
    case IDC_RADIO_SRC_MEMORY:
    {
        newSettings.payload = config::PanelSettings_InMemory{};
        break;
    }
    case IDC_RADIO_SRC_PACKAGE:
    {
        // TODO: implement
        newSettings.payload = config::PanelSettings_InMemory{};
        break;
    }
    default:
    {
        assert( false );
        break;
    }
    }

    settings_.InitializeValue( newSettings );
    UpdateUiRadioButtonData();
    parent_.OnScriptTypeChange();
}

void CConfigTabScriptSource::OnBrowseFile( UINT uNotifyCode, int nID, CWindow wndCtl )
{
    if ( !parent_.IsCleanSlate() && !RequestConfirmationForReset() )
    {
        return;
    }

    smp::file::FileDialogOptions fdOpts{};
    fdOpts.filterSpec.assign( {
        { L"JavaScript files", L"*.js" },
        { L"Text files", L"*.txt" },
        { L"All files", L"*.*" },
    } );
    fdOpts.defaultExtension = L"js";

    const auto pathOpt = smp::file::FileDialog( L"Open script file", false, fdOpts );
    if ( !pathOpt )
    {
        return;
    }

    path_ = smp::unicode::ToU8( *pathOpt );

    config::PanelSettings newSettings;
    newSettings.payload = config::PanelSettings_File{ path_ };

    settings_.InitializeValue( newSettings );
    UpdateUiFromData();
    UpdateUiRadioButtonData();
    parent_.OnScriptTypeChange();
}

void CConfigTabScriptSource::OnOpenPackageManager( UINT uNotifyCode, int nID, CWindow wndCtl )
{
}

void CConfigTabScriptSource::OnEditScript( UINT uNotifyCode, int nID, CWindow wndCtl )
{
    namespace fs = std::filesystem;

    try
    {
        switch ( payloadSwitchId_ )
        {
        case IDC_RADIO_SRC_SAMPLE:
        {
            const int iRet = MessageBox(
                L"Are you sure?\n\n"
                L"You are trying to edit a sample script.\n"
                L"Any changes performed to the script will be applied to every panel that are using this sample.\n"
                L"These changes will also be lost when updating the component.",
                L"Editing script",
                MB_YESNO | MB_ICONWARNING );
            if ( iRet != IDYES )
            {
                break;
            }

            const auto filePath = fs::path( sampleData_[sampleIdx_].path );
            if ( !fs::exists( filePath ) )
            {
                smp::utils::ReportErrorWithPopup( fmt::format( "Sample script is missing: {}", filePath.u8string() ) );
                break;
            }

            smp::EditTextFile( *this, filePath );
            break;
        }
        case IDC_RADIO_SRC_FILE:
        {
            const auto filePath = fs::u8path( path_ );
            if ( !fs::exists( filePath ) )
            {
                smp::utils::ReportErrorWithPopup( fmt::format( "Script is missing: {}", filePath.u8string() ) );
                break;
            }

            smp::EditTextFile( *this, filePath );
            break;
        }
        case IDC_RADIO_SRC_MEMORY:
        {
            auto script = std::get<config::PanelSettings_InMemory>( payload_.GetCurrentValue() ).script;
            smp::EditText( *this, script );
            payload_ = config::PanelSettings_InMemory{ script };
            parent_.OnDataChanged();
            break;
        }
        default:
        {
            assert( false );
            break;
        }
        }
    }
    catch ( const fs::filesystem_error& e )
    {
        smp::utils::ReportErrorWithPopup( e.what() );
    }
    catch ( const SmpException& e )
    {
        smp::utils::ReportErrorWithPopup( e.what() );
    }
}

LONG CConfigTabScriptSource::OnEditScriptDropDown( LPNMHDR pnmh ) const
{
    auto const dropDown = reinterpret_cast<NMBCDROPDOWN*>( pnmh );

    POINT pt{ dropDown->rcButton.left, dropDown->rcButton.bottom };

    CWindow button = dropDown->hdr.hwndFrom;
    button.ClientToScreen( &pt );

    CMenu menu;
    if ( menu.CreatePopupMenu() )
    {
        menu.AppendMenu( MF_BYPOSITION, ID_EDIT_WITH_EXTERNAL, L"Edit with..." );
        menu.AppendMenu( MF_BYPOSITION, ID_EDIT_WITH_INTERNAL, L"Edit with internal editor" );
        menu.TrackPopupMenu( TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, m_hWnd, nullptr );
    }

    return 0;
}

void CConfigTabScriptSource::OnEditScriptWith( UINT uNotifyCode, int nID, CWindow wndCtl )
{
    namespace fs = std::filesystem;

    switch ( nID )
    {
    case ID_EDIT_WITH_EXTERNAL:
    {
        // TODO: add ability to choose editor (e.g. like default context menu `Open With`)

        smp::file::FileDialogOptions fdOpts{};
        fdOpts.filterSpec.assign( { { L"Executable files", L"*.exe" } } );
        fdOpts.defaultExtension = L"exe";
        fdOpts.rememberLocation = false;

        try
        {
            const auto editorPathOpt = smp::file::FileDialog( L"Choose text editor", false, fdOpts );
            if ( editorPathOpt )
            {
                const fs::path editorPath = *editorPathOpt;
                SmpException::ExpectTrue( fs::exists( editorPath ), "Invalid path" );

                smp::config::default_editor = editorPath.u8string();
            }
        }
        catch ( const fs::filesystem_error& e )
        {
            smp::utils::ReportErrorWithPopup( e.what() );
        }
        break;
    }
    case ID_EDIT_WITH_INTERNAL:
    {
        smp::config::default_editor = "";
        break;
    }
    default:
    {
        assert( false );
        break;
    }
    }

    OnEditScript( uNotifyCode, nID, wndCtl );
}

void CConfigTabScriptSource::InitializeLocalOptions()
{
    const auto& payload = settings_.GetCurrentValue().payload;

    payloadSwitchId_ = std::visit( [&]( const auto& data ) {
        using T = std::decay_t<decltype( data )>;
        if constexpr ( std::is_same_v<T, smp::config::PanelSettings_InMemory> )
        {
            return IDC_RADIO_SRC_MEMORY;
        }
        else if constexpr ( std::is_same_v<T, smp::config::PanelSettings_File> )
        {
            return IDC_RADIO_SRC_FILE;
        }
        else if constexpr ( std::is_same_v<T, smp::config::PanelSettings_Sample> )
        {
            return IDC_RADIO_SRC_SAMPLE;
        }
        else if constexpr ( std::is_same_v<T, smp::config::PanelSettings_Package> )
        {
            return IDC_RADIO_SRC_PACKAGE;
        }
        else
        {
            static_assert( smp::always_false_v<T>, "non-exhaustive visitor!" );
        }
    },
                                   payload );

    path_ = std::visit( [&]( const auto& data ) {
        using T = std::decay_t<decltype( data )>;
        if constexpr ( std::is_same_v<T, smp::config::PanelSettings_File> )
        {
            return data.path;
        }
        else
        {
            return std::u8string{};
        }
    },
                        payload );

    sampleIdx_ = std::visit( [&]( const auto& data ) {
        using T = std::decay_t<decltype( data )>;
        if constexpr ( std::is_same_v<T, smp::config::PanelSettings_Sample> )
        {
            if ( data.sampleName.empty() )
            {
                return 0;
            }

            const auto it = ranges::find_if( sampleData_, [sampleName = smp::unicode::ToWide( data.sampleName )]( const auto& elem ) {
                return ( sampleName == elem.displayedName );
            } );

            if ( it == sampleData_.cend() )
            {
                smp::utils::ReportErrorWithPopup( fmt::format( "Can't find sample `{}`. Your settings will be reset.", data.sampleName ) );
                settings_ = smp::config::PanelSettings{};
                return 0;
            }

            assert( it != sampleData_.cend() );
            return ranges::distance( sampleData_.cbegin(), it );
        }
        else
        {
            return 0;
        }
    },
                             payload );
}

void CConfigTabScriptSource::UpdateUiFromData()
{
    if ( !this->m_hWnd )
    {
        return;
    }

    isInitializingUi_ = true;
    const smp::utils::final_action autoInit( [&] { isInitializingUi_ = false; } );

    for ( auto& ddx: ddx_ )
    {
        ddx->WriteToUi();
    }
    UpdateUiRadioButtonData();
}

void CConfigTabScriptSource::UpdateUiRadioButtonData()
{
    if ( !this->m_hWnd )
    {
        return;
    }

    switch ( payloadSwitchId_ )
    {
    case IDC_RADIO_SRC_SAMPLE:
    {
        CWindow{ GetDlgItem( IDC_COMBO_SRC_SAMPLE ) }.EnableWindow( true );
        CWindow{ GetDlgItem( IDC_TEXTEDIT_SRC_PATH ) }.EnableWindow( false );
        CWindow{ GetDlgItem( IDC_BUTTON_BROWSE ) }.EnableWindow( false );
        CWindow{ GetDlgItem( IDC_BUTTON_OPEN_PKG_MGR ) }.EnableWindow( false );
        break;
    }
    case IDC_RADIO_SRC_MEMORY:
    {
        CWindow{ GetDlgItem( IDC_COMBO_SRC_SAMPLE ) }.EnableWindow( false );
        CWindow{ GetDlgItem( IDC_TEXTEDIT_SRC_PATH ) }.EnableWindow( false );
        CWindow{ GetDlgItem( IDC_BUTTON_BROWSE ) }.EnableWindow( false );
        CWindow{ GetDlgItem( IDC_BUTTON_OPEN_PKG_MGR ) }.EnableWindow( false );
        break;
    }
    case IDC_RADIO_SRC_FILE:
    {
        CWindow{ GetDlgItem( IDC_COMBO_SRC_SAMPLE ) }.EnableWindow( false );
        CWindow{ GetDlgItem( IDC_TEXTEDIT_SRC_PATH ) }.EnableWindow( true );
        CWindow{ GetDlgItem( IDC_BUTTON_BROWSE ) }.EnableWindow( true );
        CWindow{ GetDlgItem( IDC_BUTTON_OPEN_PKG_MGR ) }.EnableWindow( false );
        break;
    }
    case IDC_RADIO_SRC_PACKAGE:
    {
        CWindow{ GetDlgItem( IDC_COMBO_SRC_SAMPLE ) }.EnableWindow( false );
        CWindow{ GetDlgItem( IDC_TEXTEDIT_SRC_PATH ) }.EnableWindow( false );
        CWindow{ GetDlgItem( IDC_BUTTON_BROWSE ) }.EnableWindow( false );
        CWindow{ GetDlgItem( IDC_BUTTON_OPEN_PKG_MGR ) }.EnableWindow( true );
        break;
    }
    default:
    {
        assert( false );
        break;
    }
    }
}

void CConfigTabScriptSource::InitializeSamplesComboBox()
{
    samplesComboBox_ = GetDlgItem( IDC_COMBO_SRC_SAMPLE );
    for ( const auto& elem: sampleData_ )
    {
        samplesComboBox_.AddString( elem.displayedName.c_str() );
    }
}

bool CConfigTabScriptSource::RequestConfirmationForReset()
{
    if ( payloadSwitchId_ == IDC_RADIO_SRC_MEMORY )
    {
        assert( std::holds_alternative<config::PanelSettings_InMemory>( payload_.GetCurrentValue() ) );
        const auto& script = std::get<config::PanelSettings_InMemory>( payload_.GetCurrentValue() ).script;
        if ( script == config::PanelSettings_InMemory::GetDefaultScript() )
        {
            return true;
        }
        else
        {
            const int iRet = MessageBox(
                L"!!! Changing script type will reset all panel settings !!!\n"
                L"!!! Your whole script will be unrecoverably lost !!!\n\n"
                L"Are you sure?",
                L"Changing script type",
                MB_YESNO | MB_ICONWARNING );
            return ( iRet == IDYES );
        }
    }
    else if ( payloadSwitchId_ == IDC_RADIO_SRC_PACKAGE )
    {
        const int ret = uMessageBox( m_hWnd, "Do you want to apply your changes?", "SMP package", MB_ICONWARNING | MB_SETFOREGROUND | MB_YESNOCANCEL );
        if ( ret == IDYES )
        {
            Apply();
        }

        return !( ret == IDCANCEL );
    }
    else
    {
        const int iRet = MessageBox(
            L"!!! Changing script type will reset all panel settings !!!\n\n"
            L"Are you sure?",
            L"Changing script type",
            MB_YESNO | MB_ICONWARNING );
        return ( iRet == IDYES );
    }
}

} // namespace smp::ui
