#pragma once

#include <config/panel_config.h>
#include <ui/ui_itab.h>
#include <utils/option_wrap.h>

#include <resource.h>

#include <vector>

namespace smp::panel
{
class js_panel_window;
}

namespace smp::ui
{

class CDialogConfNew
    : public CDialogImpl<CDialogConfNew>
{
public:
    enum class Tab : uint8_t
    {
        script,
        appearance,
        properties,
        def = script
    };

public:
    enum
    {
        IDD = IDD_DIALOG_CONF
    };

    BEGIN_MSG_MAP( CDialogConfNew )
        MSG_WM_INITDIALOG( OnInitDialog )
        MSG_WM_PARENTNOTIFY( OnParentNotify )
        COMMAND_ID_HANDLER_EX( IDOK, OnCloseCmd )
        COMMAND_ID_HANDLER_EX( IDCANCEL, OnCloseCmd )
        COMMAND_ID_HANDLER_EX( IDAPPLY, OnCloseCmd )
        COMMAND_HANDLER_EX( IDC_BUTTON_SWITCH_MODE, BN_CLICKED, OnSwitchMode )
        MESSAGE_HANDLER( WM_WINDOWPOSCHANGED, OnWindowPosChanged )
        NOTIFY_HANDLER_EX( IDC_TAB_CONF, TCN_SELCHANGE, OnSelectionChanged )
    END_MSG_MAP()

    CDialogConfNew( smp::panel::js_panel_window* pParent, CDialogConfNew::Tab tabId = CDialogConfNew::Tab::def );
    ~CDialogConfNew() override = default;

    void OnDataChanged();

private:
    BOOL OnInitDialog( HWND hwndFocus, LPARAM lParam );
    LRESULT OnCloseCmd( WORD wNotifyCode, WORD wID, HWND hWndCtl );
    void OnSwitchMode( UINT uNotifyCode, int nID, CWindow wndCtl );
    void OnParentNotify( UINT message, UINT nChildID, LPARAM lParam );
    LRESULT OnWindowPosChanged( UINT, WPARAM, LPARAM lp, BOOL& bHandled );
    LRESULT OnSelectionChanged( LPNMHDR pNmhdr );

    void CreateTabHost();
    void DestroyTabHost();

    bool HasChanged();

    void Apply();
    void Revert();

private:
    panel::js_panel_window* pParent_ = nullptr;

    OptionWrap<config::PanelSettings> settings_;

    std::u8string caption_;

    CTabCtrl cTabs_;
    CDialogImplBase* pcCurTab_ = nullptr;

    size_t activeTabIdx_ = 0;
    std::vector<std::unique_ptr<ITab>> tabs_;
};

} // namespace smp::ui
