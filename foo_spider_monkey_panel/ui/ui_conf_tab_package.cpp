#include <stdafx.h>

#include "ui_conf_tab_package.h"

#include <config/smp_config.h>
#include <ui/ui_conf_new.h>
#include <ui/ui_input_box.h>
#include <utils/edit_text.h>
#include <utils/error_popup.h>
#include <utils/file_helpers.h>
#include <utils/winapi_error_helpers.h>

namespace fs = std::filesystem;

// TODO: add `New` button

namespace smp::ui
{

CConfigTabPackage::CConfigTabPackage( CDialogConfNew& parent, smp::config::ParsedPanelSettings_Package& packageSettings )
    : parent_( parent )
    , packageSettings_( packageSettings )
    , packagePath_( fs::u8path( packageSettings.packagePath ) )
    , name_(
          packageSettings_, []( auto& value ) -> auto& { return value.name; } )
    , version_(
          packageSettings_, []( auto& value ) -> auto& { return value.version; } )
    , author_(
          packageSettings_, []( auto& value ) -> auto& { return value.author; } )
    , description_(
          packageSettings_, []( auto& value ) -> auto& { return value.description; } )
    , ddxOpts_( {
          CreateUiDdxOption<UiDdx_TextEdit>( name_, IDC_EDIT_PACKAGE_NAME ),
          CreateUiDdxOption<UiDdx_TextEdit>( version_, IDC_EDIT_PACKAGE_VERSION ),
          CreateUiDdxOption<UiDdx_TextEdit>( author_, IDC_EDIT_PACKAGE_AUTHOR ),
          CreateUiDdxOption<UiDdx_TextEdit>( description_, IDC_EDIT_PACKAGE_DESCRIPTION ),
      } )
    , ddx_( {
          CreateUiDdx<UiDdx_ListBox>( fileIdx_, IDC_LIST_PACKAGE_FILES ),
      } )
{
    assert( fs::exists( packagePath_ ) );
}

HWND CConfigTabPackage::CreateTab( HWND hParent )
{
    return Create( hParent );
}

CDialogImplBase& CConfigTabPackage::Dialog()
{
    return *this;
}

const wchar_t* CConfigTabPackage::Name() const
{
    return L"Package";
}

bool CConfigTabPackage::ValidateState()
{
    const auto validateString = [&]( const std::u8string& str, int id, const char* fieldName ) {
        if ( str.empty() )
        {
            ::SetFocus( GetDlgItem( id ) );
            smp::utils::ReportErrorWithPopup( fmt::format( "Empty {}", fieldName ) );
            return false;
        }
        else
        {
            return true;
        }
    };

    if ( !validateString( name_.GetCurrentValue(), IDC_EDIT_PACKAGE_NAME, "package name" )
         || !validateString( version_.GetCurrentValue(), IDC_EDIT_PACKAGE_VERSION, "package author" )
         || !validateString( author_.GetCurrentValue(), IDC_EDIT_PACKAGE_AUTHOR, "package version" ) )
    {
        return false;
    }

    return true;
}

bool CConfigTabPackage::HasChanged()
{
    const bool hasChanged =
        ddxOpts_.cend() != ranges::find_if( ddxOpts_, []( const auto& ddxOpt ) {
            return ddxOpt->Option().HasChanged();
        } );

    return hasChanged;
}

void CConfigTabPackage::Apply()
{
    assert( ValidateState() );
    packageSettings_.Apply();
    for ( auto& ddxOpt: ddxOpts_ )
    {
        ddxOpt->Option().Apply();
    }

    try
    {
        packageSettings_.GetCurrentValue().Save();
    }
    catch ( const SmpException& e )
    {
        smp::utils::ReportErrorWithPopup( e.what() );
    }
}

void CConfigTabPackage::Revert()
{
    for ( auto& ddxOpt: ddxOpts_ )
    {
        ddxOpt->Option().Revert();
    }

    UpdateUiFromData();
}

BOOL CConfigTabPackage::OnInitDialog( HWND hwndFocus, LPARAM lParam )
{
    for ( auto& ddx: ddx_ )
    {
        ddx->SetHwnd( m_hWnd );
    }
    for ( auto& ddxOpt: ddxOpts_ )
    {
        ddxOpt->Ddx().SetHwnd( m_hWnd );
    }

    InitializeFilesListBox();
    UpdateUiFromData();

    return TRUE; // set focus to default control
}

void CConfigTabPackage::OnUiChange( UINT uNotifyCode, int nID, CWindow wndCtl )
{
    {
        auto it = ranges::find_if( ddx_, [nID]( auto& ddx ) {
            return ddx->IsMatchingId( nID );
        } );

        if ( ddx_.end() != it )
        {
            ( *it )->ReadFromUi();
        }
    }
    {
        auto it = ranges::find_if( ddxOpts_, [nID]( auto& ddxOpt ) {
            return ddxOpt->Ddx().IsMatchingId( nID );
        } );

        if ( ddxOpts_.end() != it )
        {
            ( *it )->Ddx().ReadFromUi();
        }
    }

    UpdateUiButtons();

    parent_.OnDataChanged();
}

void CConfigTabPackage::OnAddFile( UINT uNotifyCode, int nID, CWindow wndCtl )
{
    // TODO: check that it handles duplicates well

    smp::file::FileDialogOptions fdOpts{};
    fdOpts.filterSpec.assign( {
        { L"JavaScript files", L"*.js" },
        { L"All files", L"*.*" },
    } );
    fdOpts.defaultExtension = L"js";

    const auto pathOpt = smp::file::FileDialog( L"Add file", false, fdOpts );
    if ( !pathOpt )
    {
        return;
    }

    try
    {
        const fs::path path( *pathOpt );
        const auto it = ranges::find( files_, path );
        if ( it != files_.cend() )
        {
            fileIdx_ = ranges::distance( files_.cbegin(), it );
        }
        else
        {
            const fs::path newPath = ( path.extension() == ".js"
                                           ? packagePath_ / "scripts" / path.filename()
                                           : packagePath_ / "assets" / path.filename() );

            fs::copy( path, newPath );
            files_.emplace_back( newPath );
            fileIdx_ = files_.size() - 1;
            // TODO: sort files
            UpdateListBoxFromData();
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

    UpdateUiFromData();
}

void CConfigTabPackage::OnRemoveFile( UINT uNotifyCode, int nID, CWindow wndCtl )
{
    if ( files_.empty() )
    {
        return;
    }

    assert( static_cast<size_t>( fileIdx_ ) < files_.size() );
    try
    {
        if ( fs::exists( files_[fileIdx_] ) )
        {
            fs::remove( files_[fileIdx_] );
        }
    }
    catch ( const fs::filesystem_error& e )
    {
        smp::utils::ReportErrorWithPopup( e.what() );
    }

    files_.erase( files_.cbegin() + fileIdx_ );
    filesListBox_.DeleteString( fileIdx_ );
    fileIdx_ = std::min<size_t>( files_.size(), fileIdx_ );

    UpdateUiFromData();
}

void CConfigTabPackage::OnRenameFile( UINT uNotifyCode, int nID, CWindow wndCtl )
{
    if ( files_.empty() )
    {
        return;
    }

    assert( static_cast<size_t>( fileIdx_ ) < files_.size() );

    auto& filepath = files_[fileIdx_];

    CInputBox dlg( "Enter new file name", "Rename file", filepath.filename().u8string().c_str() );
    if ( dlg.DoModal( m_hWnd ) != IDOK )
    {
        return;
    }

    try
    {
        const auto newFilePath = filepath.parent_path() / fs::u8path( dlg.GetValue() );
        fs::rename( filepath, newFilePath );
        filepath = newFilePath;
        // TODO: sort files
    }
    catch ( const fs::filesystem_error& e )
    {
        smp::utils::ReportErrorWithPopup( e.what() );
    }

    UpdateListBoxFromData();
    UpdateUiFromData();
}

void CConfigTabPackage::OnOpenContainingFolder( UINT uNotifyCode, int nID, CWindow wndCtl )
{
    const std::wstring arg = [&] {
        if ( files_.empty() )
        {
            return std::wstring{};
        }
        else
        {
            std::wstring tmp = files_[fileIdx_].wstring();
            return L"\"" + tmp + L"\"";
        }
    }();

    try
    {
        const auto hInstance = ShellExecute( nullptr,
                                             L"explore",
                                             packagePath_.wstring().c_str(),
                                             ( arg.empty() ? nullptr : arg.c_str() ),
                                             nullptr,
                                             SW_SHOWNORMAL );
        if ( (int)hInstance < 32 )
        { // As per WinAPI
            smp::error::CheckWin32( (int)hInstance, "ShellExecute" );
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

void CConfigTabPackage::OnEditScript( UINT uNotifyCode, int nID, CWindow wndCtl )
{
    try
    {
        if ( packageSettings_.GetCurrentValue().isSample )
        {
            const int iRet = MessageBox(
                L"Are you sure?\n\n"
                L"You are trying to edit a sample script.\n"
                L"Any changes performed to the script will be applied to every panel that are using this sample.\n"
                L"These changes will also be lost when updating the component.",
                L"Editing script",
                MB_YESNO );
            if ( iRet != IDYES )
            {
                return;
            }
        }

        const auto filePath = files_[fileIdx_];
        SmpException::ExpectTrue( fs::exists( filePath ), "Script is missing: {}", filePath.u8string() );

        smp::EditTextFile( *this, filePath );
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

void CConfigTabPackage::OnEditScriptWith( UINT uNotifyCode, int nID, CWindow wndCtl )
{ // TODO: extract common code (see tab_script)
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

LONG CConfigTabPackage::OnEditScriptDropDown( LPNMHDR pnmh )
{
    const auto dropDown = reinterpret_cast<NMBCDROPDOWN*>( pnmh );

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

void CConfigTabPackage::UpdateUiFromData()
{
    if ( !this->m_hWnd )
    {
        return;
    }

    for ( auto& ddx: ddx_ )
    {
        ddx->WriteToUi();
    }
    for ( auto& ddxOpt: ddxOpts_ )
    {
        ddxOpt->Ddx().WriteToUi();
    }

    UpdateUiButtons();
}

void CConfigTabPackage::UpdateUiButtons()
{
    const bool enableFileActions = !!fileIdx_; ///< fileIdx == 0 <> main script file is selected
    CWindow{ GetDlgItem( IDC_BUTTON_REMOVE_FILE ) }.EnableWindow( enableFileActions );
    CWindow{ GetDlgItem( IDC_BUTTON_RENAME_FILE ) }.EnableWindow( enableFileActions );
}

void CConfigTabPackage::InitializeFilesListBox()
{
    try
    {
        filesListBox_ = GetDlgItem( IDC_LIST_PACKAGE_FILES );

        std::vector<fs::path> files;
        files.emplace_back( fs::u8path( packageSettings_.GetCurrentValue().mainScriptPath ) );

        if ( const auto scriptsDir = packagePath_ / "scripts";
             fs::exists( scriptsDir ) )
        {
            for ( const auto& it: fs::directory_iterator( scriptsDir ) )
            {
                if ( it.path().extension() == ".js" )
                {
                    files.emplace_back( it.path() );
                }
            }
        }

        if ( const auto assetsDir = packagePath_ / "assets";
             fs::exists( assetsDir ) )
        {
            for ( const auto& it: fs::directory_iterator( assetsDir ) )
            {
                files.emplace_back( it.path() );
            }
        }

        files_ = files;
        UpdateListBoxFromData();
    }
    catch ( const fs::filesystem_error& e )
    {
        smp::utils::ReportErrorWithPopup( e.what() );
    }
}

void CConfigTabPackage::UpdateListBoxFromData()
{
    try
    {
        filesListBox_.ResetContent();
        for ( const auto& file: files_ )
        {
            filesListBox_.AddString( fs::relative( file, packagePath_ ).c_str() );
        }
    }
    catch ( const fs::filesystem_error& e )
    {
        smp::utils::ReportErrorWithPopup( e.what() );
    }
}

} // namespace smp::ui
