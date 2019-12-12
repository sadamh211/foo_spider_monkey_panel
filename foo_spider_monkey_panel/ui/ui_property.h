#pragma once

#include <config/panel_config.h>
#include <property_list/PropertyList.h>
#include <ui/ui_itab.h>
#include <utils/option_wrap.h>

#include <resource.h>

namespace smp::panel
{
class js_panel_window;
}

namespace smp::ui
{

class CDialogConfNew;

class CConfigTabProperties
    : public CDialogImpl<CConfigTabProperties>
    , public CDialogResize<CConfigTabProperties>
    , public ITab
{
public:
    enum
    {
        IDD = IDD_DIALOG_PROPERTIES
    };

    BEGIN_DLGRESIZE_MAP( CConfigTabProperties )
        DLGRESIZE_CONTROL( IDC_LIST_PROPERTIES, DLSZ_SIZE_X | DLSZ_SIZE_Y )
        DLGRESIZE_CONTROL( IDC_DEL, DLSZ_MOVE_Y )
        DLGRESIZE_CONTROL( IDC_CLEARALL, DLSZ_MOVE_Y )
        DLGRESIZE_CONTROL( IDC_IMPORT, DLSZ_MOVE_Y )
        DLGRESIZE_CONTROL( IDC_EXPORT, DLSZ_MOVE_Y )
    END_DLGRESIZE_MAP()

    BEGIN_MSG_MAP( CConfigTabProperties )
        MSG_WM_INITDIALOG( OnInitDialog )
        COMMAND_HANDLER_EX( IDC_CLEARALL, BN_CLICKED, OnClearallBnClicked )
        COMMAND_HANDLER_EX( IDC_DEL, BN_CLICKED, OnDelBnClicked )
        COMMAND_HANDLER_EX( IDC_IMPORT, BN_CLICKED, OnImportBnClicked )
        COMMAND_HANDLER_EX( IDC_EXPORT, BN_CLICKED, OnExportBnClicked )
#pragma warning( push )
#pragma warning( disable : 26454 ) // Arithmetic overflow
        NOTIFY_CODE_HANDLER_EX( PIN_ITEMCHANGED, OnPinItemChanged )
#pragma warning( pop )
        CHAIN_MSG_MAP( CDialogResize<CConfigTabProperties> )
        REFLECT_NOTIFICATIONS()
    END_MSG_MAP()

    CConfigTabProperties( CDialogConfNew& parent, OptionWrap<config::PanelSettings>& settings );

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
    LRESULT OnInitDialog( HWND hwndFocus, LPARAM lParam );
    LRESULT OnClearallBnClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl );
    LRESULT OnDelBnClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl );
    LRESULT OnExportBnClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl );
    LRESULT OnImportBnClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl );
    LRESULT OnPinItemChanged( LPNMHDR pnmh );

    void UpdateUiFromData();

private:
    CPropertyListCtrl propertyListCtrl_;
    CDialogConfNew& parent_;
    SuboptionWrap<OptionWrap<config::PanelSettings>, decltype( config::PanelSettings::properties ), false> properties_;
};

} // namespace smp::ui
