#include <stdafx.h>

#include "ui_conf_new.h"

#include <ui/ui_conf_tab_appearance.h>
#include <ui/ui_conf_tab_simple_script.h>
#include <ui/ui_property.h>

#include <js_panel_window.h>

// TODO: fix default button highlighting and handling
// TODO: fix `TAB` button handling (cycle between controls)

namespace
{

WINDOWPLACEMENT g_WindowPlacement{};

}

namespace smp::ui
{

CDialogConfNew::CDialogConfNew( smp::panel::js_panel_window* pParent, CDialogConfNew::Tab tabId )
    : pParent_( pParent )
    , caption_( "Panel Configuration" )
    , settings_( pParent->GetSettings() )
{
    tabs_.emplace_back( std::make_unique<CConfigTabSimpleScript>( *this, settings_ ) );
    tabs_.emplace_back( std::make_unique<CConfigTabAppearance>( *this, settings_ ) );
    tabs_.emplace_back( std::make_unique<CConfigTabProperties>( *this, settings_ ) );

    switch ( tabId )
    {
    case smp::ui::CDialogConfNew::Tab::script:
        activeTabIdx_ = 0;
        break;
    case smp::ui::CDialogConfNew::Tab::appearance:
        activeTabIdx_ = 1;
        break;
    case smp::ui::CDialogConfNew::Tab::properties:
        activeTabIdx_ = 2;
        break;
    default:
        assert( false );
        break;
    }
}

void CDialogConfNew::OnDataChanged()
{
    CButton{ GetDlgItem( IDAPPLY ) }.EnableWindow( HasChanged() );
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
    //pParent_->GetSettings() = settings_.GetSavedValue();
    OnDataChanged();
    //pParent_->update_script();
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

    for ( size_t i = 0; i < tabs_.size(); ++i )
    {
        cTabs_.InsertItem( i, tabs_[i]->Name() );
    }

    cTabs_.SetCurSel( activeTabIdx_ );
    CreateTabHost();

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
                Apply();
                EndDialog( IDOK );
                break;

            case IDCANCEL:
                return 0;
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

void CDialogConfNew::OnSwitchMode( UINT uNotifyCode, int nID, CWindow wndCtl )
{
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
    CreateTabHost();

    return 0;
}

LRESULT CDialogConfNew::OnWindowPosChanged( UINT, WPARAM, LPARAM lp, BOOL& bHandled )
{
    auto lpwp = reinterpret_cast<LPWINDOWPOS>( lp );
    // TODO: remove
    // Temporary workaround for various bugs that occur due to foobar2000 1.0+
    // having a dislike for destroying preference pages
    if ( lpwp->flags & SWP_HIDEWINDOW )
    {
        DestroyTabHost();
    }
    else if ( lpwp->flags & SWP_SHOWWINDOW && !pcCurTab_ )
    {
        CreateTabHost();
    }

    bHandled = FALSE;

    return 0;
}

void CDialogConfNew::CreateTabHost()
{
    DestroyTabHost();

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

void CDialogConfNew::DestroyTabHost()
{
    if ( pcCurTab_ && static_cast<HWND>( *pcCurTab_ ) )
    {
        pcCurTab_->ShowWindow( SW_HIDE );
        pcCurTab_->DestroyWindow();
    }
}

} // namespace smp::ui
