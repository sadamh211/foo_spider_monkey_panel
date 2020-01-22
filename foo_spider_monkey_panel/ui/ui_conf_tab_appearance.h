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

class CConfigTabAppearance
    : public CDialogImpl<CConfigTabAppearance>
    , public ITab
{
public:
    enum
    {
        IDD = IDD_DIALOG_CONF_TAB_APPEARANCE
    };

    BEGIN_MSG_MAP( CConfigTabAppearance )
        MSG_WM_INITDIALOG( OnInitDialog )
        COMMAND_HANDLER_EX( IDC_RADIO_EDGE_NO, BN_CLICKED, OnEditChange )
        COMMAND_HANDLER_EX( IDC_RADIO_EDGE_GREY, BN_CLICKED, OnEditChange )
        COMMAND_HANDLER_EX( IDC_RADIO_EDGE_SUNKEN, BN_CLICKED, OnEditChange )
        COMMAND_HANDLER_EX( IDC_CHECK_PSEUDOTRANSPARENT, BN_CLICKED, OnEditChange )
    END_MSG_MAP()

public:
    CConfigTabAppearance( CDialogConfNew& parent, OptionWrap<config::PanelSettings>& settings );
    ~CConfigTabAppearance() override = default;

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
    void OnChanged();

    void InitializeLocalOptions();
    void UpdateUiFromData();

private:
    CDialogConfNew& parent_;
    OptionWrap<config::PanelSettings>& settings_;
    SuboptionWrap<OptionWrap<config::PanelSettings>, decltype( config::PanelSettings::edgeStyle )> edgeStyle_;
    SuboptionWrap<OptionWrap<config::PanelSettings>, decltype( config::PanelSettings::isPseudoTransparent )> isPseudoTransparent_;

    OptionWrap<int> edgeStyleId_;

    std::array<std::unique_ptr<IUiDdxOption>, 2> ddxOpts_;
};

} // namespace smp::ui
