#include <stdafx.h>

#include "ui_package_manager.h"

#include <ui/ui_input_box.h>
#include <utils/error_popup.h>

#include <component_paths.h>

#include <filesystem>

namespace fs = std::filesystem;
namespace conf = smp::config;

namespace smp::ui
{

CDialogPackageManager::CDialogPackageManager( const std::u8string& currentPackagePath )
    : currentPackagePath_( currentPackagePath )
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
    SetWindowText( L"Script package manager" );
    ::SetFocus( GetDlgItem( IDC_LIST_PACKAGES ) );
    CenterWindow();

    LoadPackagesData();

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

        auto newPackageNameW = smp::unicode::ToWide( curName );
        for ( auto& ch: newPackageNameW )
        {
            if ( PathGetCharType( ch ) != GCT_LFNCHAR )
            {
                ch = L'_';
            }
        }

        const auto newPackagePath = fs::u8path( get_profile_path() ) / SMP_UNDERSCORE_NAME / "packages" / newPackageNameW;
        if ( fs::exists( newPackagePath ) )
        {
            MessageBox(
                L"Package with this name already exists",
                L"Creating new package",
                MB_OK | MB_ICONWARNING );
            continue;
        }

        fs::create_directories( newPackagePath );

        conf::ParsedPanelSettings_Package newPackageData;
        newPackageData.name = curName;
        newPackageData.Save();

        packages_.emplace_back( newPath );
        packageIdx_ = packages_.size() - 1;
        // TODO: sort files
        UpdateListBoxFromData();

        break;
    };
}

void CDialogPackageManager::OnDeletePackage( UINT uNotifyCode, int nID, CWindow wndCtl )
{
    if ( packages_.empty() )
    {
        return;
    }

    assert( static_cast<size_t>( packageIdx_ ) < packages_.size() );
    try
    {
        // TODO: replace with path
        fs::remove_all( packages_[packageIdx_] );
    }
    catch ( const fs::filesystem_error& e )
    {
        smp::utils::ReportErrorWithPopup( e.what() );
    }

    packages_.erase( packages_.cbegin() + packageIdx_ );
    packagesListBox_.DeleteString( packageIdx_ );
    packageIdx_ = std::min<size_t>( packages_.size(), static_cast<int>( packageIdx_ ) );

    UpdateUiFromData();
}

void CDialogPackageManager::OnImportPackage( UINT uNotifyCode, int nID, CWindow wndCtl )
{
}

void CDialogPackageManager::OnExportPackage( UINT uNotifyCode, int nID, CWindow wndCtl )
{
}

LRESULT CDialogPackageManager::OnCloseCmd( WORD wNotifyCode, WORD wID, HWND hWndCtl )
{
    EndDialog( wID );
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

    UpdateUiButtons();
}

void CDialogPackageManager::UpdateUiButtons()
{
    if ( packageIdx_ < 0 || packageIdx_ > packages_.size() )
    {
        return;
    }

    const auto& currentPackageData = packages_[packageIdx_].attachedData;

    CButton{ GetDlgItem( IDC_BUTTON_DELETE_PACKAGE ) }.EnableWindow( currentPackageData.settings.location != conf::PackageLocation::Sample );
    CButton{ GetDlgItem( IDOK ) }.EnableWindow( !!currentPackageData.parsedSettings );
}

void CDialogPackageManager::LoadPackagesData()
{
    packages_.clear();

    try
    {
        std::vector<conf::PanelSettings_Package> packages;
        {
            const auto getPackages = [&packages]( conf::PackageLocation packageLocationId, const fs::path& packageDir ) {
                for ( const auto& filepath: fs::directory_iterator( packageDir ) )
                {
                    const auto dirIt = fs::directory_iterator( filepath );
                    const auto it = ranges::find_if( dirIt,
                                                     []( const auto& elem ) {
                                                         return fs::is_regular_file( elem.path() ) && elem.path().filename() == L"package.json";
                                                     } );
                    if ( it == ranges::end( dirIt ) )
                    {
                        continue;
                    }

                    packages.emplace_back( packageLocationId, it->path().parent_path().filename().u8string() );
                }
            };

            getPackages( conf::PackageLocation::LocalAppData, fs::u8path( get_profile_path() ) / SMP_UNDERSCORE_NAME / "packages" );
            getPackages( conf::PackageLocation::Fb2k, fs::u8path( get_fb2k_path() ) / SMP_UNDERSCORE_NAME / "packages" );
            getPackages( conf::PackageLocation::Sample, fs::u8path( get_fb2k_component_path() ) / "samples" / "packages" );
        }

        std::vector<PackageData> parsedPackages;
        for ( const auto& package: packages )
        {
            try
            {
                const auto& parsedPackage = conf::ParsedPanelSettings_Package::Parse( package );
                const auto displayedName = [&parsedPackage] {
                    return ( parsedPackage.author.empty()
                                 ? parsedPackage.name
                                 : fmt::format( "{} (by {})", parsedPackage.name, parsedPackage.author ) );
                }();
                const auto valueOrEmpty = []( const std::u8string& str ) -> std::u8string {
                    return ( str.empty() ? "<empty>" : str );
                };
                const auto displayedDescription = fmt::format( "Name: {}\r\n"
                                                               "Version: {}\r\n"
                                                               "Author: {}\r\n"
                                                               "Description:\r\n{}",
                                                               valueOrEmpty( parsedPackage.name ),
                                                               valueOrEmpty( parsedPackage.version ),
                                                               valueOrEmpty( parsedPackage.author ),
                                                               valueOrEmpty( parsedPackage.description ) );

                parsedPackages.emplace_back( DisplayedData{ displayedName, displayedDescription },
                                             AttachedData{ package, parsedPackage } );
            }
            catch ( const SmpException& e )
            {
                parsedPackages.emplace_back( DisplayedData{ fmt::format( "{} (ERROR)", package.folderName ), fmt::format( "Package parsing failed:\r\n{}", e.what() ) },
                                             AttachedData{ package, std::nullopt } );
            }
        }

        packages_ = parsedPackages;
    }
    catch ( const fs::filesystem_error& e )
    {
        smp::utils::ReportErrorWithPopup( e.what() );
    }
}

} // namespace smp::ui
