#pragma once

#include <config/panel_config.h>
#include <ui/internal/ui_ddx_option.h>
#include <ui/ui_itab.h>
#include <utils/option_wrap.h>

#include <resource.h>

#include <array>

namespace smp::ui
{

class CDialogConfNew;

class ConfigTabSimpleScript
    : public CDialogImpl<ConfigTabSimpleScript>
    , public ITab
{
public:
    enum
    {
        IDD = IDD_DIALOG_CONF_TAB_SIMPLE_SCRIPT
    };

    BEGIN_MSG_MAP( ConfigTabSimpleScript )
        MSG_WM_INITDIALOG( OnInitDialog )
        COMMAND_RANGE_CODE_HANDLER_EX( IDC_RADIO_SRC_SAMPLE, IDC_RADIO_SRC_MEMORY, BN_CLICKED, OnEditChange )
        COMMAND_HANDLER_EX( IDC_BUTTON_SWITCH_MODE, BN_CLICKED, OnSwitchMode )
        COMMAND_HANDLER_EX( IDC_RICHEDIT_SRC_PATH, EN_CHANGE, OnEditChange )
    END_MSG_MAP()

public:
    ConfigTabSimpleScript( CDialogConfNew& parent, OptionWrap<config::PanelSettings>& settings );
    ~ConfigTabSimpleScript() override = default;

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
    void OnEditChange( UINT uNotifyCode, int nID, CWindow wndCtl );
    void OnSwitchMode( UINT uNotifyCode, int nID, CWindow wndCtl );
    void OnChanged();
    void UpdateUiFromData();

private:
    CDialogConfNew& parent_;
    OptionWrap<config::PanelSettings>& settings_;
    SuboptionWrap<OptionWrap<config::PanelSettings>, decltype( config::PanelSettings::payload )> payload_;

    OptionWrap<int> payloadSwitchId_;
    OptionWrap<std::u8string> path_;
    OptionWrap<std::u8string> sampleName_;

    std::array<std::unique_ptr<IUiDdxOption>, 3> ddxOpts_;
};

} // namespace smp::ui
