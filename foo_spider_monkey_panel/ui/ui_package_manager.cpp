#include <stdafx.h>

#include "ui_package_manager.h"

#include <ui/ui_input_box.h>
#include <utils/array_x.h>
#include <utils/error_popup.h>
#include <utils/file_helpers.h>
#include <utils/scope_helpers.h>
#include <utils/winapi_error_helpers.h>
#include <utils/zip_utils.h>

#include <component_paths.h>

#include <filesystem>

namespace fs = std::filesystem;
namespace conf = smp::config;

// TODO: reimplement messagebox to enable centering on parent

namespace smp::ui
{

CDialogPackageManager::CDialogPackageManager( const std::u8string& currentPackageId )
    : currentPackageId_( currentPackageId )
    , ddx_( {
          CreateUiDdx<UiDdx_ListBox>( packageIdx_, IDC_LIST_PACKAGES ),
      } )
{
}

std::optional<conf::PanelSettings_Package>
CDialogPackageManager::GetPackage() const
{
    return std::nullopt;
}

std::optional<conf::ParsedPanelSettings_Package>
CDialogPackageManager::GetParsedPackage() const
{
    return std::nullopt;
}

LRESULT CDialogPackageManager::OnInitDialog( HWND, LPARAM )
{
    for ( auto& ddx: ddx_ )
    {
        ddx->SetHwnd( m_hWnd );
    }

    packagesListBox_ = GetDlgItem( IDC_LIST_PACKAGES );
    // TODO: add contetx menu
    packageInfoEdit_ = GetDlgItem( IDC_RICHEDIT_PACKAGE_INFO );
    packageInfoEdit_.SetWindowLong( GWL_EXSTYLE, packageInfoEdit_.GetWindowLong( GWL_EXSTYLE ) & ~WS_EX_CLIENTEDGE );
    packageInfoEdit_.SetEditStyle( SES_HYPERLINKTOOLTIPS | SES_NOFOCUSLINKNOTIFY, SES_HYPERLINKTOOLTIPS | SES_NOFOCUSLINKNOTIFY );
    packageInfoEdit_.SetAutoURLDetect();
    packageInfoEdit_.SetEventMask( packageInfoEdit_.GetEventMask() | ENM_LINK );

    SetWindowText( L"Script package manager" );

    CenterWindow();
    ::SetFocus( packagesListBox_ );

    LoadPackages();
    SortPackages();
    UpdateListBoxFromData();
    UpdateUiFromData();

    return FALSE;
}

void CDialogPackageManager::OnUiChange( UINT uNotifyCode, int nID, CWindow wndCtl )
{
    auto it = ranges::find_if( ddx_, [nID]( auto& ddx ) {
        return ddx->IsMatchingId( nID );
    } );

    if ( ddx_.end() != it )
    {
        ( *it )->ReadFromUi();
    }

    if ( nID == IDC_LIST_PACKAGES )
    {
        UpdatedPackageId();
        UpdatedUiPackageInfo();
    }
}

void CDialogPackageManager::OnNewPackage( UINT uNotifyCode, int nID, CWindow wndCtl )
{
    std::u8string curName;
    while ( true )
    {
        CInputBox dlg( "Enter new package name", "Creating new package", curName );
        if ( dlg.DoModal( m_hWnd ) != IDOK )
        {
            return;
        }

        curName = dlg.GetValue();
        if ( curName.empty() )
        {
            MessageBox(
                L"Can't create package with empty name",
                L"Creating new package",
                MB_OK | MB_ICONWARNING );
            continue;
        }

        break;
    };

    try
    {
        const auto newPackage = conf::ParsedPanelSettings_Package::CreateNewPackage( curName );
        newPackage.Save();

        packages_.emplace_back( GeneratePackageData( newPackage ) );
        currentPackageId_ = newPackage.id;

        SortPackages();
        UpdateListBoxFromData();
        UpdateUiFromData();
    }
    catch ( const SmpException& e )
    {
        smp::utils::ReportErrorWithPopup( e.what() );
    }
}

void CDialogPackageManager::OnDeletePackage( UINT uNotifyCode, int nID, CWindow wndCtl )
{
    if ( packages_.empty() )
    {
        return;
    }

    assert( packageIdx_ >= 0 && static_cast<size_t>( packageIdx_ ) < packages_.size() );
    assert( packages_[packageIdx_].parsedSettings );

    const int iRet = MessageBox( L"Are you sure you want to delete the package?",
                                 L"Deleting package",
                                 MB_YESNO );
    if ( iRet != IDYES )
    {
        return;
    }

    try
    {
        fs::remove_all( fs::u8path( packages_[packageIdx_].parsedSettings->packagePath ) );
    }
    catch ( const fs::filesystem_error& e )
    {
        smp::utils::ReportErrorWithPopup( e.what() );
    }

    packages_.erase( packages_.cbegin() + packageIdx_ );
    packageIdx_ = std::min<size_t>( packages_.size() - 1, static_cast<int>( packageIdx_ ) );
    packagesListBox_.DeleteString( packageIdx_ );

    UpdatedPackageId();
    UpdateUiFromData();
}

void CDialogPackageManager::OnImportPackage( UINT uNotifyCode, int nID, CWindow wndCtl )
{
    smp::file::FileDialogOptions fdOpts{};
    fdOpts.filterSpec.assign( {
        { L"Zip archives", L"*.zip" },
    } );

    const auto pathOpt = smp::file::FileDialog( L"Import package", false, fdOpts );
    if ( !pathOpt )
    {
        return;
    }

    try
    {
        const auto tmpPath = fs::u8path( get_profile_path() ) / SMP_UNDERSCORE_NAME / "tmp" / "unpacked_package";
        if ( fs::exists( tmpPath ) )
        {
            fs::remove_all( tmpPath );
        }
        fs::create_directories( tmpPath );
        smp::utils::final_action autoTmp( [&] {
            try
            {
                fs::remove_all( tmpPath );
            }
            catch ( const fs::filesystem_error& )
            {
            }
        } );

        UnpackZip( fs::path( *pathOpt ), tmpPath );

        const auto parsedPackageNew = [&tmpPath] {
            auto parsedPackage = conf::ParsedPanelSettings_Package::Parse( tmpPath );
            parsedPackage.packagePath = ( fs::u8path( get_profile_path() ) / SMP_UNDERSCORE_NAME / "packages" / parsedPackage.id ).u8string();
            return parsedPackage;
        }();

        if ( const auto packageOldPathOpt = conf::ParsedPanelSettings_Package::FindPackage( parsedPackageNew.id );
             packageOldPathOpt )
        {
            const int iRet = MessageBox(
                L"Another version of this package is present.\n"
                L"Do you want to update?",
                L"Importing package",
                MB_YESNO );
            if ( iRet != IDYES )
            {
                return;
            }

            try
            {
                const auto parsedPackageOld = conf::ParsedPanelSettings_Package::Parse( *packageOldPathOpt );
                if ( parsedPackageOld.name != parsedPackageNew.name )
                {
                    const int iRet = MessageBox(
                        smp::unicode::ToWide(
                            fmt::format( "Currently installed package has a different name from the new one:\n"
                                         "old: '{}' vs new: '{}'\n\n"
                                         "Do you want to continue?",
                                         parsedPackageOld.name,
                                         parsedPackageNew.name ) )
                            .c_str(),
                        L"Importing package",
                        MB_YESNO | MB_ICONWARNING );
                    if ( iRet != IDYES )
                    {
                        return;
                    }
                }
            }
            catch ( const SmpException& )
            {
            }

            fs::remove_all( *packageOldPathOpt );
        }

        fs::copy( tmpPath, fs::u8path( parsedPackageNew.packagePath ), fs::copy_options::recursive );

        auto it =
            ranges::find_if( packages_,
                             [packageId = parsedPackageNew.id]( const auto& elem ) {
                                 return ( packageId == elem.id );
                             } );
        if ( it != packages_.cend() )
        {
            *it = GeneratePackageData( parsedPackageNew );
        }
        else
        {
            packages_.emplace_back( GeneratePackageData( parsedPackageNew ) );
        }
        currentPackageId_ = parsedPackageNew.id;

        SortPackages();
        UpdateListBoxFromData();
        UpdateUiFromData();
    }
    catch ( const fs::filesystem_error& e )
    {
        smp::utils::ReportErrorWithPopup( smp::unicode::ToU8_FromAcpToWide( e.what() ) );
    }
    catch ( const SmpException& e )
    {
        smp::utils::ReportErrorWithPopup( e.what() );
    }
}

void CDialogPackageManager::OnExportPackage( UINT uNotifyCode, int nID, CWindow wndCtl )
{
    assert( packageIdx_ >= 0 && static_cast<size_t>( packageIdx_ ) < packages_.size() );
    assert( packages_[packageIdx_].parsedSettings );

    const auto& currentPackageData = packages_[packageIdx_];

    smp::file::FileDialogOptions fdOpts{};
    fdOpts.filterSpec.assign( {
        { L"Zip archives", L"*.zip" },
    } );

    auto archiveName = currentPackageData.displayedName;
    for ( auto& ch: archiveName )
    {
        const auto ct = PathGetCharType( ch );
        if ( !( ct & GCT_LFNCHAR ) )
        {
            ch = L'_';
        }
    }

    fdOpts.defaultFilename = archiveName;
    fdOpts.defaultExtension = L"zip";

    const auto pathOpt = smp::file::FileDialog( L"Save package as", true, fdOpts );
    if ( !pathOpt )
    {
        return;
    }

    try
    {
        ZipPacker zp{ fs::path( *pathOpt ) };
        zp.AddFolder( fs::u8path( currentPackageData.parsedSettings->packagePath ) );
        zp.Finish();
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

void CDialogPackageManager::OnOpenFolder( UINT uNotifyCode, int nID, CWindow wndCtl )
{
    assert( packageIdx_ >= 0 && static_cast<size_t>( packageIdx_ ) < packages_.size() );
    assert( packages_[packageIdx_].parsedSettings );

    try
    {
        const auto hInstance = ShellExecute( nullptr,
                                             L"explore",
                                             fs::u8path( packages_[packageIdx_].parsedSettings->packagePath ).wstring().c_str(),
                                             nullptr,
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

LRESULT CDialogPackageManager::OnCloseCmd( WORD wNotifyCode, WORD wID, HWND hWndCtl )
{
    EndDialog( wID );
    return 0;
}

LRESULT CDialogPackageManager::OnRichEditLinkClick( LPNMHDR pnmh )
{
    const auto* pEl = reinterpret_cast<ENLINK*>( pnmh );
    if ( pEl->msg == WM_LBUTTONUP )
    {
        std::wstring url;
        url.resize( pEl->chrg.cpMax - pEl->chrg.cpMin );
        packageInfoEdit_.GetTextRange( pEl->chrg.cpMin, pEl->chrg.cpMax, url.data() );

        ShellExecute( nullptr, L"open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL );
    }

    return 0;
}

void CDialogPackageManager::UpdateUiFromData()
{
    if ( !this->m_hWnd )
    {
        return;
    }

    for ( auto& ddx: ddx_ )
    {
        ddx->WriteToUi();
    }

    UpdatedUiPackageInfo();
    UpdateUiButtons();
}

void CDialogPackageManager::UpdateUiButtons()
{
    if ( packageIdx_ < 0 || static_cast<size_t>( packageIdx_ ) >= packages_.size() )
    {
        CButton{ GetDlgItem( IDOK ) }.EnableWindow( false );
        CButton{ GetDlgItem( IDC_BUTTON_DELETE_PACKAGE ) }.EnableWindow( false );
        CButton{ GetDlgItem( IDC_BUTTON_EXPORT_PACKAGE ) }.EnableWindow( false );
        CButton{ GetDlgItem( IDC_BUTTON_OPEN_FOLDER ) }.EnableWindow( false );
        return;
    }

    const auto& currentPackageData = packages_[packageIdx_];

    CButton{ GetDlgItem( IDOK ) }.EnableWindow( !!currentPackageData.parsedSettings );
    CButton{ GetDlgItem( IDC_BUTTON_DELETE_PACKAGE ) }.EnableWindow( currentPackageData.parsedSettings && !currentPackageData.parsedSettings->isSample );
    CButton{ GetDlgItem( IDC_BUTTON_EXPORT_PACKAGE ) }.EnableWindow( currentPackageData.parsedSettings && !currentPackageData.parsedSettings->isSample );
    CButton{ GetDlgItem( IDC_BUTTON_OPEN_FOLDER ) }.EnableWindow( !!currentPackageData.parsedSettings );
}

void CDialogPackageManager::LoadPackages()
{
    packages_.clear();

    try
    {
        // TODO: consider extracting these code
        std::vector<fs::path> packagesDirs{ fs::u8path( get_profile_path() ) / SMP_UNDERSCORE_NAME / "packages",
                                            fs::u8path( get_fb2k_component_path() ) / "samples" / "packages" };
        if ( get_profile_path() != get_fb2k_path() )
        { // these paths might be the same when fb2k is in portable mode
            packagesDirs.emplace_back( fs::u8path( get_fb2k_path() ) / SMP_UNDERSCORE_NAME / "packages" );
        }

        std::vector<std::u8string> packageIds;
        for ( const auto& packagesDir: packagesDirs )
        {
            if ( !fs::exists( packagesDir ) )
            {
                continue;
            }

            for ( const auto dirIt: fs::directory_iterator( packagesDir ) )
            {
                const auto packageJson = dirIt.path() / L"package.json";
                if ( !fs::exists( packageJson ) || !fs::is_regular_file( packageJson ) )
                {
                    continue;
                }

                packageIds.emplace_back( dirIt.path().filename().u8string() );
            }
        }

        std::vector<PackageData> parsedPackages;
        for ( const auto& packageId: packageIds )
        {
            try
            {
                const auto parsedPackage = conf::ParsedPanelSettings_Package::Parse( packageId );
                parsedPackages.emplace_back( GeneratePackageData( parsedPackage ) );
            }
            catch ( const SmpException& e )
            {
                PackageData packageData{ smp::unicode::ToWide( fmt::format( "{} (ERROR)", packageId ) ),
                                         packageId,
                                         std::nullopt,
                                         smp::unicode::ToWide( fmt::format( "Package parsing failed:\r\n{}", e.what() ) ) };
                parsedPackages.emplace_back( packageData );
            }
        }

        packages_ = std::move( parsedPackages );
    }
    catch ( const fs::filesystem_error& e )
    {
        smp::utils::ReportErrorWithPopup( e.what() );
    }
}

void CDialogPackageManager::SortPackages()
{
    // TODO: replace with StrCmpLogicalW
    ranges::sort( packages_,
                  []( const auto& a, const auto& b ) {
                      return ( a.displayedName < b.displayedName );
                  } );
}

void CDialogPackageManager::UpdateListBoxFromData()
{
    packagesListBox_.ResetContent();
    for ( const auto& package: packages_ )
    {
        packagesListBox_.AddString( package.displayedName.c_str() );
    }
    UpdatedPackageIdx();
}

void CDialogPackageManager::UpdatedPackageIdx()
{
    const auto it =
        ranges::find_if( packages_,
                         [&]( const auto& a ) {
                             return ( currentPackageId_ == a.id );
                         } );
    packageIdx_ = ( it == packages_.cend() ? 0 : ranges::distance( packages_.cbegin(), it ) );
}

void CDialogPackageManager::UpdatedPackageId()
{
    if ( packageIdx_ < 0 || static_cast<size_t>( packageIdx_ ) >= packages_.size() )
    {
        currentPackageId_.clear();
    }
    else
    {
        currentPackageId_ = packages_[packageIdx_].id;
    }
}

void CDialogPackageManager::UpdatedUiPackageInfo()
{
    if ( packageIdx_ < 0 || static_cast<size_t>( packageIdx_ ) >= packages_.size() )
    {
        return;
    }

    const auto packageData = packages_[packageIdx_];

    packageInfoEdit_.SetWindowText( L"" );

    CHARFORMAT savedCharFormat{};
    packageInfoEdit_.GetSelectionCharFormat( savedCharFormat );
    const smp::utils::final_action autoFormat( [&] { packageInfoEdit_.SetSelectionCharFormat( savedCharFormat ); } );

    if ( !packageData.parsedSettings )
    {
        CHARFORMAT newCharFormat = savedCharFormat;
        newCharFormat.dwMask = CFM_COLOR;
        newCharFormat.dwEffects = ~CFE_AUTOCOLOR;
        newCharFormat.crTextColor = RGB( 255, 0, 0 );

        packageInfoEdit_.SetSelectionCharFormat( newCharFormat );
        packageInfoEdit_.AppendText( L"Error:\r\n", FALSE );

        packageInfoEdit_.SetSelectionCharFormat( savedCharFormat );
        packageInfoEdit_.AppendText( packages_[packageIdx_].errorText.c_str(), FALSE );
    }
    else
    {
        const auto valueOrEmpty = []( const std::u8string& str ) -> std::wstring {
            return ( str.empty() ? L"<empty>" : smp::unicode::ToWide( str ) );
        };

        const auto& parsedSettings = *packageData.parsedSettings;

        CHARFORMAT newCharFormat = savedCharFormat;
        newCharFormat.dwMask = CFM_UNDERLINE;
        newCharFormat.dwEffects = CFM_UNDERLINE;

        const auto appendText = [&]( const wchar_t* field, const wchar_t* value ) {
            assert( field );
            assert( value );

            packageInfoEdit_.SetSelectionCharFormat( newCharFormat );
            packageInfoEdit_.AppendText( field, FALSE );
            packageInfoEdit_.SetSelectionCharFormat( savedCharFormat );
            packageInfoEdit_.AppendText( L": ", FALSE );
            packageInfoEdit_.AppendText( value, FALSE );
        };

        appendText( L"Name", valueOrEmpty( parsedSettings.name ).c_str() );
        packageInfoEdit_.AppendText( L"\r\n", FALSE );

        appendText( L"Version", valueOrEmpty( parsedSettings.version ).c_str() );
        packageInfoEdit_.AppendText( L"\r\n", FALSE );

        appendText( L"Author", valueOrEmpty( parsedSettings.author ).c_str() );
        packageInfoEdit_.AppendText( L"\r\n", FALSE );

        appendText( L"Description", ( L"\r\n" + valueOrEmpty( parsedSettings.description ) ).c_str() );
    }
}

CDialogPackageManager::PackageData CDialogPackageManager::GeneratePackageData( const conf::ParsedPanelSettings_Package& parsedSettings )
{
    const auto displayedName = [&parsedSettings] {
        return ( parsedSettings.author.empty()
                     ? parsedSettings.name
                     : fmt::format( "{} (by {})", parsedSettings.name, parsedSettings.author ) );
    }();
    const auto valueOrEmpty = []( const std::u8string& str ) -> std::u8string {
        return ( str.empty() ? "<empty>" : str );
    };
    const auto displayedDescription = fmt::format( "Name: {}\r\n"
                                                   "Version: {}\r\n"
                                                   "Author: {}\r\n"
                                                   "Description:\r\n{}",
                                                   valueOrEmpty( parsedSettings.name ),
                                                   valueOrEmpty( parsedSettings.version ),
                                                   valueOrEmpty( parsedSettings.author ),
                                                   valueOrEmpty( parsedSettings.description ) );

    return PackageData{ smp::unicode::ToWide( displayedName ),
                        parsedSettings.id,
                        parsedSettings,
                        L"" };
}

} // namespace smp::ui
