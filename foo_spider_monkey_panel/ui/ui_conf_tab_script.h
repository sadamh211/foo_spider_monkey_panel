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

class CConfigTabSimpleScript
    : public CDialogImpl<CConfigTabSimpleScript>
    , public ITab
{
public:
    enum
    {
        IDD = IDD_DIALOG_CONF_TAB_SIMPLE_SCRIPT
    };

    BEGIN_MSG_MAP( CConfigTabSimpleScript )
        MSG_WM_INITDIALOG( OnInitDialog )
        COMMAND_HANDLER_EX( IDC_RADIO_SRC_SAMPLE, BN_CLICKED, OnEditChange )
        COMMAND_HANDLER_EX( IDC_RADIO_SRC_FILE, BN_CLICKED, OnEditChange )
        COMMAND_HANDLER_EX( IDC_RADIO_SRC_MEMORY, BN_CLICKED, OnEditChange )
        COMMAND_HANDLER_EX( IDC_TEXTEDIT_SRC_PATH, EN_CHANGE, OnEditChange )
        COMMAND_HANDLER_EX( IDC_COMBO_SRC_SAMPLE, CBN_SELCHANGE, OnEditChange )
        COMMAND_HANDLER_EX( IDC_BUTTON_EDIT_SCRIPT, BN_CLICKED, OnEditScript )
        COMMAND_HANDLER_EX( IDC_BUTTON_EDIT_SCRIPT, BN_CLICKED, OnEditScript )
        NOTIFY_HANDLER_EX( IDC_BUTTON_EDIT_SCRIPT, BCN_DROPDOWN, OnEditScriptDropDown )
        COMMAND_HANDLER_EX( ID_EDIT_WITH_EXTERNAL, BN_CLICKED, OnEditScriptWith )
        COMMAND_HANDLER_EX( ID_EDIT_WITH_INTERNAL, BN_CLICKED, OnEditScriptWith )
    END_MSG_MAP()

    struct SampleComboBoxElem
    {
        SampleComboBoxElem( std::wstring path, std::wstring displayedName )
            : path( std::move( path ) )
            , displayedName( std::move( displayedName ) )
        {
        }
        std::wstring path;
        std::wstring displayedName;
    };

public:
    CConfigTabSimpleScript( CDialogConfNew& parent, OptionWrap<config::PanelSettings>& settings );
    ~CConfigTabSimpleScript() override = default;

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
    void OnEditScript( UINT uNotifyCode, int nID, CWindow wndCtl );
    LONG OnEditScriptDropDown( LPNMHDR pnmh ) const;
    void OnEditScriptWith( UINT uNotifyCode, int nID, CWindow wndCtl );

    void InitializeLocalOptions();
    void UpdateUiFromData();
    void UpdateUiRadioButtonData();
    void InitializeSamplesComboBox();

private:
    CDialogConfNew& parent_;
    SuboptionWrap<OptionWrap<config::PanelSettings>, decltype( config::PanelSettings::payload )> payload_;

    OptionWrap<int> payloadSwitchId_;
    OptionWrap<std::u8string> path_;
    OptionWrap<int> sampleIdx_;

    std::array<std::unique_ptr<IUiDdxOption>, 3> ddxOpts_;

    static std::vector<SampleComboBoxElem> sampleData_;
    CComboBox samplesComboBox_;
};

} // namespace smp::ui
