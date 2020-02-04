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

class CConfigTabPackage
    : public CDialogImpl<CConfigTabPackage>
    , public ITab
{
public:
    enum
    {
        IDD = IDD_DIALOG_CONF_TAB_PACKAGE
    };

    BEGIN_MSG_MAP( CConfigTabPackage )
        MSG_WM_INITDIALOG( OnInitDialog )
        COMMAND_HANDLER_EX( IDC_EDIT_PACKAGE_NAME, EN_CHANGE, OnUiChange )
        COMMAND_HANDLER_EX( IDC_EDIT_PACKAGE_VERSION, EN_CHANGE, OnUiChange )
        COMMAND_HANDLER_EX( IDC_EDIT_PACKAGE_AUTHOR, EN_CHANGE, OnUiChange )
        COMMAND_HANDLER_EX( IDC_EDIT_PACKAGE_DESCRIPTION, EN_CHANGE, OnUiChange )
        COMMAND_HANDLER_EX( IDC_LIST_PACKAGE_FILES, LVN_ITEMCHANGED, OnUiChange )
        COMMAND_HANDLER_EX( IDC_BUTTON_ADD_FILE, BN_CLICKED, OnAddFile )
        COMMAND_HANDLER_EX( IDC_BUTTON_REMOVE_FILE, BN_CLICKED, OnRemoveFile )
        COMMAND_HANDLER_EX( IDC_BUTTON_RENAME_FILE, BN_CLICKED, OnRenameFile )
        COMMAND_HANDLER_EX( IDC_BUTTON_OPEN_FOLDER, BN_CLICKED, OnOpenContainingFolder )
        COMMAND_HANDLER_EX( IDC_BUTTON_EDIT_SCRIPT, BN_CLICKED, OnEditScript )
        COMMAND_HANDLER_EX( ID_EDIT_WITH_EXTERNAL, BN_CLICKED, OnEditScriptWith )
        COMMAND_HANDLER_EX( ID_EDIT_WITH_INTERNAL, BN_CLICKED, OnEditScriptWith )
        NOTIFY_HANDLER_EX( IDC_BUTTON_EDIT_SCRIPT, BCN_DROPDOWN, OnEditScriptDropDown )
    END_MSG_MAP()

public:
    CConfigTabPackage( CDialogConfNew& parent, smp::config::ParsedPanelSettings_Package& packageSettings );
    ~CConfigTabPackage() override = default;

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

    void OnAddFile( UINT uNotifyCode, int nID, CWindow wndCtl );
    void OnRemoveFile( UINT uNotifyCode, int nID, CWindow wndCtl );
    void OnRenameFile( UINT uNotifyCode, int nID, CWindow wndCtl );
    void OnOpenContainingFolder( UINT uNotifyCode, int nID, CWindow wndCtl );

    void OnEditScript( UINT uNotifyCode, int nID, CWindow wndCtl );
    void OnEditScriptWith( UINT uNotifyCode, int nID, CWindow wndCtl );
    LONG OnEditScriptDropDown( LPNMHDR pnmh );

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
    const std::filesystem::path packagePath_;

    PackageSubOptionWrap<decltype( CfgParsedPackage::name )> name_;
    PackageSubOptionWrap<decltype( CfgParsedPackage::version )> version_;
    PackageSubOptionWrap<decltype( CfgParsedPackage::author )> author_;
    PackageSubOptionWrap<decltype( CfgParsedPackage::description )> description_;
    
    std::array<std::unique_ptr<IUiDdxOption>, 5> ddxOpts_;

    int fileIdx_ = 0;

    std::array<std::unique_ptr<IUiDdx>, 1> ddx_;

    std::vector<std::filesystem::path> files_;
    CListBox filesListBox_;
};

} // namespace smp::ui
