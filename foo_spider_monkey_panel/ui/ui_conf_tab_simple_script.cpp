#include <stdafx.h>

#include "ui_conf_tab_simple_script.h"

#include <ui/ui_conf_new.h>
#include <utils/error_popup.h>
#include <utils/file_helpers.h>
#include <utils/scope_helpers.h>
#include <utils/winapi_error_helpers.h>

#include <component_paths.h>

#include <filesystem>

namespace
{

using namespace smp;
using namespace smp::ui;

// TODO: move

/// @throw smp::SmpException
void ExecuteApp( const std::wstring& app, const std::wstring& params, bool shouldWait = false )
{
    if ( shouldWait )
    {
        // TODO: replace with proper emulation (+ FlashWindowEx + SetFocus), since brute blocking makes horrible UX
        SHELLEXECUTEINFO ShExecInfo{};
        ShExecInfo.cbSize = sizeof( SHELLEXECUTEINFO );
        ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
        ShExecInfo.lpFile = app.c_str();
        ShExecInfo.lpParameters = params.c_str();
        ShExecInfo.nShow = SW_SHOW;

        BOOL bRet = ShellExecuteEx( &ShExecInfo );
        smp::error::CheckWinApi( bRet, "ShellExecuteEx" );

        WaitForSingleObject( ShExecInfo.hProcess, INFINITE );
        CloseHandle( ShExecInfo.hProcess );
    }
    else
    {
        const auto hInstance = ShellExecute( nullptr,
                                             L"open",
                                             app.c_str(),
                                             params.c_str(),
                                             nullptr,
                                             SW_SHOW );
        smp::error::CheckWinApi( hInstance, "ShellExecute" );
    }
}

/// @throw smp::SmpException
void EditAsTmpFile( const std::wstring& app, std::u8string& data )
{
    namespace fs = std::filesystem;

    std::wstring path;
    path.resize( MAX_PATH + 1 );

    DWORD dwRet = GetTempPath( path.size() - 1,
                               path.data() );
    smp::error::CheckWinApi( dwRet && dwRet <= MAX_PATH, "GetTempPath" );

    std::wstring filename;
    filename.resize( MAX_PATH + 1 );
    UINT uRet = GetTempFileName( path.c_str(),
                                 L"smp",
                                 0,
                                 filename.data() ); // buffer for name
    smp::error::CheckWinApi( uRet, "GetTempFileName" );

    fs::path fsPath( path );
    fsPath /= filename;

    if ( !smp::file::WriteFile( fsPath.wstring().c_str(), data ) )
    {
        throw SmpException( fmt::format( "Failed to create temporary file: {}", fsPath.u8string() ) );
    }
    const smp::utils::final_action autoRemove( [&fsPath] { fs::remove( fsPath ); } );

    std::wstring params;
    if ( std::wstring::npos != app.find( L"notepad++.exe" ) )
    {
        params += L"-multiInst -notabbar -nosession -noPlugin ";
    }
    else if ( std::wstring::npos != app.find( L"Code.exe" )
              || std::wstring::npos != app.find( L"subl.exe" ) )
    {
        params += L"--new-window --wait ";
    }

    params += fsPath.wstring();

    ExecuteApp( app, params, true );

    data = smp::file::ReadFile( fsPath.u8string(), CP_UTF8 );
}

std::vector<std::filesystem::path> GetFilesInDirectoryRecursive( const std::filesystem::path& dirPath )
{
    namespace fs = std::filesystem;

    std::vector<std::filesystem::path> filepaths;
    for ( const auto& it: fs::directory_iterator( dirPath ) )
    {
        filepaths.emplace_back( it.path() );
    }

    return filepaths;
}

std::vector<ConfigTabSimpleScript::SampleComboBoxElem> GetSampleFileData()
{
    namespace fs = std::filesystem;

    auto sampleFolderPath = fs::u8path( get_fb2k_component_path() ) / "samples";

    std::vector<ConfigTabSimpleScript::SampleComboBoxElem> elems;

    for ( const auto& subdir: { "complete", "jsplaylist-mod", "js-smooth", "basic" } )
    {
        for ( const auto& filepath: GetFilesInDirectoryRecursive( sampleFolderPath / subdir ) )
        {
            if ( filepath.extension() == ".js" )
            {
                elems.emplace_back( filepath, fs::relative( filepath, sampleFolderPath ) );
            }
        }
    }

    return elems;
}

} // namespace

namespace smp::ui
{

std::vector<ConfigTabSimpleScript::SampleComboBoxElem> ConfigTabSimpleScript::sampleData_;

ConfigTabSimpleScript::ConfigTabSimpleScript( CDialogConfNew& parent, OptionWrap<config::PanelSettings>& settings )
    : parent_( parent )
    , settings_( settings )
    , payload_(
          settings_, []( auto& value ) -> auto& { return value.payload; } )
    , ddxOpts_( {
          CreateUiDdxOption<UiDdx_TextEdit>( path_, IDC_TEXTEDIT_SRC_PATH ),
          CreateUiDdxOption<UiDdx_ComboBox>( sampleIdx_, IDC_COMBO_SRC_SAMPLE ),
          CreateUiDdxOption<UiDdx_RadioRange>( payloadSwitchId_,
                                               std::initializer_list<int>{
                                                   IDC_RADIO_SRC_SAMPLE,
                                                   IDC_RADIO_SRC_MEMORY,
                                                   IDC_RADIO_SRC_FILE,
                                               } ),
      } )
{
    if ( sampleData_.empty() )
    {// can't initialize it during global initialization
        sampleData_ = GetSampleFileData();
    }
}

HWND ConfigTabSimpleScript::CreateTab( HWND hParent )
{
    return Create( hParent );
}

CDialogImplBase& ConfigTabSimpleScript::Dialog()
{
    return *this;
}

const wchar_t* ConfigTabSimpleScript::Name() const
{
    return L"Script";
}

bool ConfigTabSimpleScript::ValidateState()
{
    if ( std::holds_alternative<config::PanelSettings_File>( payload_.GetCurrentValue() ) )
    {
        const auto& path = std::get<config::PanelSettings_File>( payload_.GetCurrentValue() ).path;
        if ( path.empty() )
        { // TODO: check that file exists
            ::SetFocus( GetDlgItem( IDC_TEXTEDIT_SRC_PATH ) );
            smp::utils::ReportErrorWithPopup( fmt::format( "Invalid path to script: {}", ( path.empty() ? std::u8string( "<empty>" ) : path ) ) );
            return false;
        }
    }

    return true;
}

bool ConfigTabSimpleScript::HasChanged()
{
    const bool hasChanged =
        ddxOpts_.cend() != ranges::find_if( ddxOpts_, []( const auto& ddxOpt ) {
            return ddxOpt->Option().HasChanged();
        } );

    return hasChanged;
}

void ConfigTabSimpleScript::Apply()
{
    assert( ValidateState() );
    for ( auto& ddxOpt: ddxOpts_ )
    {
        ddxOpt->Option().Apply();
    }
    payload_.Apply();
}

void ConfigTabSimpleScript::Revert()
{
    for ( auto& ddxOpt: ddxOpts_ )
    {
        ddxOpt->Option().Revert();
    }
    payload_.Revert();

    InitializeLocalOptions();
    UpdateUiFromData();
}

BOOL ConfigTabSimpleScript::OnInitDialog( HWND hwndFocus, LPARAM lParam )
{
    for ( auto& ddxOpt: ddxOpts_ )
    {
        ddxOpt->Ddx().SetHwnd( m_hWnd );
    }

    InitializeSamplesComboBox();
    InitializeLocalOptions();
    UpdateUiFromData();

    return TRUE; // set focus to default control
}

void ConfigTabSimpleScript::OnEditChange( UINT uNotifyCode, int nID, CWindow wndCtl )
{
    auto it = ranges::find_if( ddxOpts_, [nID]( auto& ddxOpt ) {
        return ddxOpt->Ddx().IsMatchingId( nID );
    } );

    if ( ddxOpts_.end() != it )
    {
        ( *it )->Ddx().ReadFromUi();
    }

    switch ( nID )
    {
    case IDC_RADIO_SRC_SAMPLE:
    case IDC_RADIO_SRC_MEMORY:
    case IDC_RADIO_SRC_FILE:
    {
        switch ( payloadSwitchId_.GetCurrentValue() )
        {
        case IDC_RADIO_SRC_SAMPLE:
        {
            payload_ = ( sampleData_.empty()
                             ? config::PanelSettings_Sample{}
                             : config::PanelSettings_Sample{ smp::unicode::ToU8( sampleData_[sampleIdx_.GetCurrentValue()].displayedName ) } );
            break;
        }
        case IDC_RADIO_SRC_FILE:
        {
            payload_ = config::PanelSettings_File{ path_ };
            break;
        }
        case IDC_RADIO_SRC_MEMORY:
        {
            payload_ = config::PanelSettings_InMemory{};
            break;
        }
        default:
        {
            assert( false );
            break;
        }
        }

        UpdateUiRadioButtonData();
        break;
    }
    case IDC_TEXTEDIT_SRC_PATH:
    {
        if ( std::holds_alternative<config::PanelSettings_File>( payload_.GetCurrentValue() ) )
        {
            payload_ = config::PanelSettings_File{ path_ };
        }
        break;
    }
    case IDC_COMBO_SRC_SAMPLE:
    {
        if ( std::holds_alternative<config::PanelSettings_Sample>( payload_.GetCurrentValue() ) )
        {
            payload_ = ( sampleData_.empty()
                             ? config::PanelSettings_Sample{}
                             : config::PanelSettings_Sample{ smp::unicode::ToU8( sampleData_[sampleIdx_.GetCurrentValue()].displayedName ) } );
        }
        break;
    }
    default:
    {
        break;
    }
    }

    OnChanged();
}

void ConfigTabSimpleScript::OnEditScript( UINT uNotifyCode, int nID, CWindow wndCtl )
{
    namespace fs = std::filesystem;

    // TODO: extract editor code + add ability to chose editor + detect default editors like Notepad++, VSCode, Sublime, maybe IntelliJ
    // https://github.com/desktop/desktop/blob/development/app/src/lib/editors/win32.ts

    try
    {
        switch ( payloadSwitchId_.GetCurrentValue() )
        {
        case IDC_RADIO_SRC_SAMPLE:
        {
            // TODO: display warning about editing sample

            ExecuteApp( L"C:\\Program Files\\Notepad++\\notepad++.exe",
                        sampleData_[sampleIdx_.GetCurrentValue()].path );

            break;
        }
        case IDC_RADIO_SRC_FILE:
        {
            ExecuteApp( L"C:\\Program Files\\Notepad++\\notepad++.exe",
                        fs::u8path( path_.GetCurrentValue() ).lexically_normal().wstring() );
            break;
        }
        case IDC_RADIO_SRC_MEMORY:
        {
            // TODO: display warning about blocking (+ "dont remind me again")

            auto script = std::get<config::PanelSettings_InMemory>( payload_.GetCurrentValue() ).script;
            EditAsTmpFile( L"C:\\Program Files\\Notepad++\\notepad++.exe", script );
            payload_ = config::PanelSettings_InMemory{ script };

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

void ConfigTabSimpleScript::OnChanged()
{
    parent_.OnDataChanged();
}

void ConfigTabSimpleScript::InitializeLocalOptions()
{
    bool resetSettingsSoft = false;
    bool resetSettingsHard = false;

    const auto getSwitchIdFromVariant = [&]( const auto& vrt ) {
        return std::visit( [&]( const auto& data ) {
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
                assert( false );
                if ( !resetSettingsHard )
                {
                    smp::utils::ReportErrorWithPopup( "Internal error: package type in simple type panel" );
                    resetSettingsHard = true;
                }
                return IDC_RADIO_SRC_SAMPLE;
            }
            else
            {
                static_assert( false, "non-exhaustive visitor!" );
            }
        },
                           vrt );
    };

    const auto getPathFromVariant = [&]( const auto& vrt ) {
        return std::visit( [&]( const auto& data ) {
            using T = std::decay_t<decltype( data )>;
            if constexpr ( std::is_same_v<T, smp::config::PanelSettings_File> )
            {
                return data.path;
            }
            else if constexpr ( std::is_same_v<T, smp::config::PanelSettings_Sample> || std::is_same_v<T, smp::config::PanelSettings_InMemory> )
            {
                return std::u8string{};
            }
            else if constexpr ( std::is_same_v<T, smp::config::PanelSettings_Package> )
            {
                assert( false );
                if ( !resetSettingsHard )
                {
                    smp::utils::ReportErrorWithPopup( "Internal error: package type in simple type panel" );
                    resetSettingsHard = true;
                }
                return std::u8string{};
            }
            else
            {
                static_assert( false, "non-exhaustive visitor!" );
            }
        },
                           vrt );
    };

    const auto getSampleIdxFromVariant = [&]( const auto& vrt ) {
        return std::visit( [&]( const auto& data ) {
            using T = std::decay_t<decltype( data )>;
            if constexpr ( std::is_same_v<T, smp::config::PanelSettings_Sample> )
            {
                const auto it = ranges::find_if( sampleData_, [sampleName = smp::unicode::ToWide( data.sampleName )]( const auto& elem ) {
                    return ( sampleName == elem.displayedName );
                } );

                if ( !data.sampleName.empty() && it == sampleData_.cend() && !resetSettingsSoft )
                {
                    smp::utils::ReportErrorWithPopup( fmt::format( "Can't find sample `{}`. Your settings will be reset.", data.sampleName ) );
                    resetSettingsSoft = true;
                    return 0;
                }
                else
                {
                    assert( it != sampleData_.cend() );
                    return ranges::distance( sampleData_.cbegin(), it );
                }
            }
            else if constexpr ( std::is_same_v<T, smp::config::PanelSettings_File> || std::is_same_v<T, smp::config::PanelSettings_InMemory> )
            {
                return 0;
            }
            else if constexpr ( std::is_same_v<T, smp::config::PanelSettings_Package> )
            {
                assert( false );
                if ( !resetSettingsHard )
                {
                    smp::utils::ReportErrorWithPopup( "Internal error: package type in simple type panel" );
                    resetSettingsHard = true;
                }
                return 0;
            }
            else
            {
                static_assert( false, "non-exhaustive visitor!" );
            }
        },
                           vrt );
    };

    payloadSwitchId_.InitializeValue( getSwitchIdFromVariant( payload_.GetSavedValue() ), getSwitchIdFromVariant( payload_.GetCurrentValue() ) );
    path_.InitializeValue( getPathFromVariant( payload_.GetSavedValue() ), getPathFromVariant( payload_.GetCurrentValue() ) );
    sampleIdx_.InitializeValue( getSampleIdxFromVariant( payload_.GetSavedValue() ), getSampleIdxFromVariant( payload_.GetCurrentValue() ) );
    if ( resetSettingsHard )
    {
        payload_.InitializeValue( smp::config::PanelSettings_InMemory{} );
    }
    else if ( resetSettingsSoft )
    {
        payload_ = smp::config::PanelSettings_InMemory{};
    }
}

void ConfigTabSimpleScript::UpdateUiFromData()
{
    if ( !this->m_hWnd )
    {
        return;
    }

    for ( auto& ddxOpt: ddxOpts_ )
    {
        ddxOpt->Ddx().WriteToUi();
    }
    UpdateUiRadioButtonData();
}

void ConfigTabSimpleScript::UpdateUiRadioButtonData()
{
    if ( !this->m_hWnd )
    {
        return;
    }

    switch ( payloadSwitchId_.GetCurrentValue() )
    {
    case IDC_RADIO_SRC_SAMPLE:
    {
        CWindow{ GetDlgItem( IDC_COMBO_SRC_SAMPLE ) }.EnableWindow( true );
        CWindow{ GetDlgItem( IDC_TEXTEDIT_SRC_PATH ) }.EnableWindow( false );
        break;
    }
    case IDC_RADIO_SRC_MEMORY:
    {
        CWindow{ GetDlgItem( IDC_COMBO_SRC_SAMPLE ) }.EnableWindow( false );
        CWindow{ GetDlgItem( IDC_TEXTEDIT_SRC_PATH ) }.EnableWindow( false );
        break;
    }
    case IDC_RADIO_SRC_FILE:
    {
        CWindow{ GetDlgItem( IDC_COMBO_SRC_SAMPLE ) }.EnableWindow( false );
        CWindow{ GetDlgItem( IDC_TEXTEDIT_SRC_PATH ) }.EnableWindow( true );
        break;
    }
    default:
    {
        assert( false );
        break;
    }
    }
}

void ConfigTabSimpleScript::InitializeSamplesComboBox()
{
    samplesComboBox_ = GetDlgItem( IDC_COMBO_SRC_SAMPLE );

    for ( auto&& [i, elem]: ranges::view::enumerate( sampleData_ ) )
    {
        samplesComboBox_.AddString( elem.displayedName.c_str() );
        samplesComboBox_.SetItemDataPtr( i, (void*)elem.path.c_str() );
    }
}

} // namespace smp::ui
