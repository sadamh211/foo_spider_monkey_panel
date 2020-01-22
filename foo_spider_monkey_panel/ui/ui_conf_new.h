#pragma once

#include <config/panel_config.h>
#include <config/parsed_panel_config.h>
#include <ui/ui_itab.h>
#include <utils/option_wrap.h>

#include <resource.h>

#include <optional>
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
        properties,
        def = script,
    };

public:
    enum class TabLayoutType : uint8_t
    {
        simple,
        package,
    };

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
        MESSAGE_HANDLER( WM_WINDOWPOSCHANGED, OnWindowPosChanged )
        NOTIFY_HANDLER_EX( IDC_TAB_CONF, TCN_SELCHANGE, OnSelectionChanged )
    END_MSG_MAP()

    // TODO: add help button

    CDialogConfNew( smp::panel::js_panel_window* pParent, CDialogConfNew::Tab tabId = CDialogConfNew::Tab::def );
    ~CDialogConfNew() override = default;

    bool IsCleanSlate() const;

    void OnDataChanged();
    void OnScriptTypeChange();

private:
    BOOL OnInitDialog( HWND hwndFocus, LPARAM lParam );
    LRESULT OnCloseCmd( WORD wNotifyCode, WORD wID, HWND hWndCtl );
    void OnParentNotify( UINT message, UINT nChildID, LPARAM lParam );
    LRESULT OnWindowPosChanged( UINT, WPARAM, LPARAM lp, BOOL& bHandled );
    LRESULT OnSelectionChanged( LPNMHDR pNmhdr );

    bool ValidateAndParseSettings();
    void InitializeTabData( CDialogConfNew::Tab tabId = CDialogConfNew::Tab::def );
    void InitializeCTabs();
    void CreateChildTab();
    void DestroyChildTab();

    bool HasChanged();

    void Apply();
    void Revert();

private:
    panel::js_panel_window* pParent_ = nullptr;
    OptionWrap<config::PanelSettings> settings_;
    std::optional<config::ParsedPanelSettings_Package> parsedPackageSettings_;
    bool isCleanSlate_ = false;

    std::u8string caption_;

    const CDialogConfNew::Tab startingTabId_;
    TabLayoutType curTabLayout_;
    CTabCtrl cTabs_;
    CDialogImplBase* pcCurTab_ = nullptr;

    size_t activeTabIdx_ = 0;
    std::vector<std::unique_ptr<ITab>> tabs_;
};

} // namespace smp::ui
