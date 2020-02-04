#include <stdafx.h>

#include "ui_conf_tab_package_options.h"

#include <config/smp_config.h>
#include <ui/ui_conf_new.h>
#include <ui/ui_input_box.h>
#include <utils/edit_text.h>
#include <utils/error_popup.h>
#include <utils/file_helpers.h>
#include <utils/winapi_error_helpers.h>

namespace fs = std::filesystem;

namespace smp::ui
{

CConfigTabPackageOpts::CConfigTabPackageOpts( CDialogConfNew& parent, smp::config::ParsedPanelSettings_Package& packageSettings )
    : parent_( parent )
    , packageSettings_( packageSettings )
    , ddxOpts_( {
          CreateUiDdxOption<UiDdx_CheckBox>( enableDragDrop_, IDC_CHECK_ENABLE_DRAG_N_DROP),
          CreateUiDdxOption<UiDdx_CheckBox>( shouldGrabFocus_, IDC_CHECK_SHOULD_GRAB_FOCUS ),
      } )
    , ddx_( {
          CreateUiDdx<UiDdx_ListBox>( actionIdx_, IDC_LIST_MENU_ACTIONS ),
      } )
    , menuActions_(
          packageSettings_, []( auto& value ) -> auto& { return value.menuActions; } )
{
}

HWND CConfigTabPackageOpts::CreateTab( HWND hParent )
{
    return Create( hParent );
}

CDialogImplBase& CConfigTabPackageOpts::Dialog()
{
    return *this;
}

const wchar_t* CConfigTabPackageOpts::Name() const
{
    return L"Package";
}

bool CConfigTabPackageOpts::ValidateState()
{
    return true;
}

bool CConfigTabPackageOpts::HasChanged()
{
    const bool hasChanged =
        ddxOpts_.cend() != ranges::find_if( ddxOpts_, []( const auto& ddxOpt ) {
            return ddxOpt->Option().HasChanged();
        } );

    return hasChanged;
}

void CConfigTabPackageOpts::Apply()
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

void CConfigTabPackageOpts::Revert()
{
    for ( auto& ddxOpt: ddxOpts_ )
    {
        ddxOpt->Option().Revert();
    }

    UpdateUiFromData();
}

BOOL CConfigTabPackageOpts::OnInitDialog( HWND hwndFocus, LPARAM lParam )
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

void CConfigTabPackageOpts::OnUiChange( UINT uNotifyCode, int nID, CWindow wndCtl )
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

void CConfigTabPackageOpts::OnNewAction( UINT uNotifyCode, int nID, CWindow wndCtl )
{
}

void CConfigTabPackageOpts::OnDeleteAction( UINT uNotifyCode, int nID, CWindow wndCtl )
{
}

void CConfigTabPackageOpts::OnEditActionFile( UINT uNotifyCode, int nID, CWindow wndCtl )
{
}

void CConfigTabPackageOpts::UpdateUiFromData()
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

void CConfigTabPackageOpts::UpdateUiButtons()
{
}

void CConfigTabPackageOpts::InitializeFilesListBox()
{ /*
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
    }*/
}

void CConfigTabPackageOpts::UpdateListBoxFromData()
{ /*
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
    }*/
}

} // namespace smp::ui
