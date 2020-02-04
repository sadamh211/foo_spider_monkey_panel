#pragma once

#include <config/parsed_panel_config.h>
#include <ui/internal/ui_ddx_option.h>
#include <ui/ui_itab.h>
#include <utils/option_wrap.h>

#include <resource.h>

#include <array>
#include <filesystem>

namespace smp::ui
{

class CDialogConfNew;

class CConfigTabPackageOpts
    : public CDialogImpl<CConfigTabPackageOpts>
    , public ITab
{
public:
    enum
    {
        IDD = IDD_DIALOG_CONF_TAB_PACKAGE_OPTS
    };

    BEGIN_MSG_MAP( CConfigTabPackageOpts )
        MSG_WM_INITDIALOG( OnInitDialog )
        COMMAND_HANDLER_EX( IDC_LIST_MENU_ACTIONS, LVN_ITEMCHANGED, OnUiChange )
        COMMAND_HANDLER_EX( IDC_BUTTON_NEW_ACTION, BN_CLICKED, OnNewAction )
        COMMAND_HANDLER_EX( IDC_BUTTON_DELETE_ACTION, BN_CLICKED, OnDeleteAction )
        COMMAND_HANDLER_EX( IDC_BUTTON_EDIT_ACTION, BN_CLICKED, OnEditActionFile )
        COMMAND_HANDLER_EX( IDC_CHECK_ENABLE_DRAG_N_DROP, BN_CLICKED, OnUiChange )
        COMMAND_HANDLER_EX( IDC_CHECK_SHOULD_GRAB_FOCUS, BN_CLICKED, OnUiChange )
    END_MSG_MAP()

public:
    CConfigTabPackageOpts( CDialogConfNew& parent, smp::config::ParsedPanelSettings_Package& packageSettings );
    ~CConfigTabPackageOpts() override = default;

    // > IUiTab
    HWND CreateTab( HWND hParent ) override;
    CDialogImplBase& Dialog() override;
    const wchar_t* Name() const override;
    bool ValidateState() override;
    bool HasChanged() override;
    void Apply() override;
    void Revert() override;
    // < IUiTab

private:
    BOOL OnInitDialog( HWND hwndFocus, LPARAM lParam );
    void OnUiChange( UINT uNotifyCode, int nID, CWindow wndCtl );

    void OnNewAction( UINT uNotifyCode, int nID, CWindow wndCtl );
    void OnDeleteAction( UINT uNotifyCode, int nID, CWindow wndCtl );
    void OnEditActionFile( UINT uNotifyCode, int nID, CWindow wndCtl );

    void UpdateUiFromData();
    void UpdateUiButtons();
    void InitializeFilesListBox();
    void UpdateListBoxFromData();

private:
    using CfgParsedPackage = smp::config::ParsedPanelSettings_Package;

    template<typename T>
    using PackageSubOptionWrap = SuboptionWrap<OptionWrap<CfgParsedPackage>, T>;

    CDialogConfNew& parent_;
    OptionWrap<smp::config::ParsedPanelSettings_Package> packageSettings_;

    OptionWrap<bool> enableDragDrop_{ false };
    OptionWrap<bool> shouldGrabFocus_{ true };
    
    std::array<std::unique_ptr<IUiDdxOption>, 5> ddxOpts_;

    int actionIdx_ = 0;

    std::array<std::unique_ptr<IUiDdx>, 1> ddx_;

    PackageSubOptionWrap<decltype( CfgParsedPackage::menuActions )> menuActions_;
    CListBox actionsListBox_;
};

} // namespace smp::ui
