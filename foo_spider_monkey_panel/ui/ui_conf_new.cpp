#include <stdafx.h>

#include "ui_conf_new.h"

#include <ui/ui_conf_tab_appearance.h>
#include <ui/ui_conf_tab_package.h>
#include <ui/ui_conf_tab_script_source.h>
#include <ui/ui_property.h>
#include <utils/error_popup.h>

#include <js_panel_window.h>

// TODO: fix default button highlighting and handling
// TODO: fix `TAB` button handling (cycle between controls)

namespace
{

bool IsCleanSlate( const smp::config::PanelSettings& settings )
{
    if ( std::holds_alternative<smp::config::PanelSettings_InMemory>( settings.payload ) )
    {
        const auto memSettings = std::get<smp::config::PanelSettings_InMemory>( settings.payload );
        return ( memSettings.script == smp::config::PanelSettings_InMemory::GetDefaultScript() );
    }
    else
    {
        return false;
    }
}

} // namespace

namespace
{

WINDOWPLACEMENT g_WindowPlacement{};

}

namespace smp::ui
{

CDialogConfNew::CDialogConfNew( smp::panel::js_panel_window* pParent, Tab tabId )
    : pParent_( pParent )
    , caption_( "Panel Configuration" )
    , settings_( pParent->GetSettings() )
    , isCleanSlate_( ::IsCleanSlate( pParent->GetSettings() ) )
    , startingTabId_( tabId )
    , curTabLayout_( std::holds_alternative<config::PanelSettings_Package>( pParent->GetSettings().payload ) ? TabLayoutType::package : TabLayoutType::simple )
{
}

bool CDialogConfNew::IsCleanSlate() const
{
    return isCleanSlate_;
}

void CDialogConfNew::OnDataChanged()
{
    CButton{ GetDlgItem( IDAPPLY ) }.EnableWindow( HasChanged() );
}

void CDialogConfNew::OnScriptTypeChange()
{
    assert( !HasChanged() ); ///< all settings should be saved or abandoned

    const bool isPackagePayload = std::holds_alternative<config::PanelSettings_Package>( settings_.GetCurrentValue().payload );
    const bool hasConsistentTabTypes =
        ( ( isPackagePayload && curTabLayout_ == TabLayoutType::package )
          || ( !isPackagePayload && curTabLayout_ != TabLayoutType::package ) );

    if ( !hasConsistentTabTypes )
    {
        curTabLayout_ = ( isPackagePayload ? TabLayoutType::package : TabLayoutType::simple );
        if ( !ValidateAndParseSettings() )
        {
            const int iRet = MessageBox(
                L"Your panel settings will be reset",
                L"Configuring panel",
                MB_OK );
            settings_.InitializeValue( config::PanelSettings{} );
            curTabLayout_ = TabLayoutType::simple;
        }
        InitializeTabData();
        InitializeCTabs();
    }

    isCleanSlate_ = true;
}

bool CDialogConfNew::HasChanged()
{
    return ( settings_.HasChanged()
             || ranges::any_of( tabs_, []( const auto& pTab ) { return pTab->HasChanged(); } ) );
}

void CDialogConfNew::Apply()
{
    if ( ranges::any_of( tabs_, []( const auto& pTab ) { return !pTab->ValidateState(); } ) )
    {
        return;
    }

    for ( auto& tab: tabs_ )
    {
        tab->Apply();
    }

    settings_.Apply();
    OnDataChanged();
    pParent_->UpdateSettings( settings_ );
}

void CDialogConfNew::Revert()
{
    for ( auto& tab: tabs_ )
    {
        tab->Revert();
    }

    settings_.Revert();
    OnDataChanged();
}

BOOL CDialogConfNew::OnInitDialog( HWND hwndFocus, LPARAM lParam )
{
    cTabs_ = GetDlgItem( IDC_TAB_CONF );

    if ( !ValidateAndParseSettings() )
    {
        const int iRet = MessageBox(
            L"Do you want to reset your panel settings and continue?",
            L"Configuring panel",
            MB_YESNO );
        if ( iRet == IDYES )
        {
            settings_.InitializeValue( config::PanelSettings{} );
            curTabLayout_ = TabLayoutType::simple;
        }
        else
        {
            EndDialog( IDCANCEL );
            return FALSE;
        }
    }
    InitializeTabData( startingTabId_ );
    InitializeCTabs();

    CButton{ GetDlgItem( IDAPPLY ) }.EnableWindow( false );

    return TRUE; // set focus to default control
}

LRESULT CDialogConfNew::OnCloseCmd( WORD wNotifyCode, WORD wID, HWND hWndCtl )
{
    { // Window position
        WINDOWPLACEMENT tmpPlacement{};
        tmpPlacement.length = sizeof( WINDOWPLACEMENT );
        if ( GetWindowPlacement( &tmpPlacement ) )
        {
            g_WindowPlacement = tmpPlacement;
        }
    }

    switch ( wID )
    {
    case IDOK:
    {
        Apply();
        EndDialog( IDOK );
        break;
    }
    case IDAPPLY:
    {
        Apply();
        break;
    }
    case IDCANCEL:
    {
        if ( HasChanged() )
        {
            const int ret = uMessageBox( m_hWnd, "Do you want to apply your changes?", caption_.c_str(), MB_ICONWARNING | MB_SETFOREGROUND | MB_YESNOCANCEL );
            switch ( ret )
            {
            case IDYES:
            {
                Apply();
                EndDialog( IDOK );
                break;
            }
            case IDCANCEL:
            {
                return 0;
            }
            }
        }

        EndDialog( IDCANCEL );
        break;
    }
    default:
    {
        assert( 0 );
    }
    }

    return 0;
}

void CDialogConfNew::OnParentNotify( UINT message, UINT nChildID, LPARAM lParam )
{
    if ( WM_DESTROY == message && pcCurTab_ && reinterpret_cast<HWND>( lParam ) == static_cast<HWND>( *pcCurTab_ ) )
    {
        pcCurTab_ = nullptr;
    }
}

LRESULT CDialogConfNew::OnSelectionChanged( LPNMHDR pNmhdr )
{
    activeTabIdx_ = TabCtrl_GetCurSel( GetDlgItem( IDC_TAB_CONF ) );
    CreateChildTab();

    return 0;
}

LRESULT CDialogConfNew::OnWindowPosChanged( UINT, WPARAM, LPARAM lp, BOOL& bHandled )
{
    auto lpwp = reinterpret_cast<LPWINDOWPOS>( lp );
    if ( lpwp->flags & SWP_HIDEWINDOW )
    {
        DestroyChildTab();
    }
    else if ( lpwp->flags & SWP_SHOWWINDOW && !pcCurTab_ )
    {
        CreateChildTab();
    }

    bHandled = FALSE;

    return 0;
}

bool CDialogConfNew::ValidateAndParseSettings()
{
    if ( curTabLayout_ == TabLayoutType::package )
    {
        try
        {
            const auto& payload = settings_.GetCurrentValue().payload;
            assert( std::holds_alternative<config::PanelSettings_Package>( payload ) );
            parsedPackageSettings_ = config::ParsedPanelSettings_Package::Parse( std::get<config::PanelSettings_Package>( payload ) );
            return true;
        }
        catch ( const SmpException& e )
        {
            smp::utils::ReportErrorWithPopup( e.what() );
            return false;
        }
    }

    return true;
}

void CDialogConfNew::InitializeTabData( smp::ui::CDialogConfNew::Tab tabId )
{
    tabs_.clear();
    switch ( curTabLayout_ )
    {
    case TabLayoutType::simple:
    {
        tabs_.emplace_back( std::make_unique<CConfigTabScriptSource>( *this, settings_ ) );
        tabs_.emplace_back( std::make_unique<CConfigTabAppearance>( *this, settings_ ) );
        tabs_.emplace_back( std::make_unique<CConfigTabProperties>( *this, settings_ ) );

        switch ( tabId )
        {
        case Tab::script:
            activeTabIdx_ = 0;
            break;
        case Tab::properties:
            activeTabIdx_ = 2;
            break;
        default:
            assert( false );
            activeTabIdx_ = 0;
            break;
        }

        break;
    }
    case TabLayoutType::package:
    {
        tabs_.emplace_back( std::make_unique<CConfigTabScriptSource>( *this, settings_ ) );
        assert( parsedPackageSettings_ );
        tabs_.emplace_back( std::make_unique<CConfigTabPackage>( *this, *parsedPackageSettings_ ) );
        tabs_.emplace_back( std::make_unique<CConfigTabAppearance>( *this, settings_ ) );
        tabs_.emplace_back( std::make_unique<CConfigTabProperties>( *this, settings_ ) );

        switch ( tabId )
        {
        case Tab::script:
            activeTabIdx_ = 0;
            break;
        case Tab::properties:
            activeTabIdx_ = 3;
            break;
        default:
            assert( false );
            activeTabIdx_ = 0;
            break;
        }

        break;
    }
    default:
        assert( 0 );
        break;
    }
}

// TODO: rename
void CDialogConfNew::InitializeCTabs()
{
    cTabs_.DeleteAllItems();
    for ( auto&& [i, pTab]: ranges::view::enumerate( tabs_ ) )
    {
        cTabs_.InsertItem( i, pTab->Name() );
    }

    cTabs_.SetCurSel( activeTabIdx_ );
    CreateChildTab();
}

void CDialogConfNew::CreateChildTab()
{
    DestroyChildTab();

    RECT tabRc;

    cTabs_.GetWindowRect( &tabRc );
    ::MapWindowPoints( HWND_DESKTOP, m_hWnd, (LPPOINT)&tabRc, 2 );

    cTabs_.AdjustRect( FALSE, &tabRc );

    if ( activeTabIdx_ >= tabs_.size() )
    {
        activeTabIdx_ = 0;
    }

    auto& pCurTab = tabs_[activeTabIdx_];
    pcCurTab_ = &pCurTab->Dialog();
    pCurTab->CreateTab( m_hWnd );

    EnableThemeDialogTexture( static_cast<HWND>( *pcCurTab_ ), ETDT_ENABLETAB );

    pcCurTab_->SetWindowPos( nullptr, tabRc.left, tabRc.top, tabRc.right - tabRc.left, tabRc.bottom - tabRc.top, SWP_NOZORDER );
    cTabs_.SetWindowPos( HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );

    pcCurTab_->ShowWindow( SW_SHOWNORMAL );
}

void CDialogConfNew::DestroyChildTab()
{
    if ( pcCurTab_ && static_cast<HWND>( *pcCurTab_ ) )
    {
        pcCurTab_->ShowWindow( SW_HIDE );
        pcCurTab_->DestroyWindow();
        pcCurTab_ = nullptr;
    }
}

} // namespace smp::ui
