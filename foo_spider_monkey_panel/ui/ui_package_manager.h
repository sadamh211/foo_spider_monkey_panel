#pragma once

#include "resource.h"

#include <config/panel_config.h>
#include <config/parsed_panel_config.h>
#include <ui/internal/ui_ddx.h>

namespace smp::ui
{

// TODO: add drag-n-drop support

class CDialogPackageManager : public CDialogImpl<CDialogPackageManager>
{
public:
    enum
    {
        IDD = IDD_DIALOG_PACKAGE_MANAGER
    };

    BEGIN_MSG_MAP( CDialogPackageManager )
        MSG_WM_INITDIALOG( OnInitDialog )
        COMMAND_HANDLER_EX( IDC_LIST_PACKAGES, LVN_ITEMCHANGED, OnUiChange )
        COMMAND_HANDLER_EX( IDC_BUTTON_NEW_PACKAGE, BN_CLICKED, OnNewPackage )
        COMMAND_HANDLER_EX( IDC_BUTTON_DELETE_PACKAGE, BN_CLICKED, OnDeletePackage )
        COMMAND_HANDLER_EX( IDC_BUTTON_IMPORT_PACKAGE, BN_CLICKED, OnImportPackage )
        COMMAND_HANDLER_EX( IDC_BUTTON_EXPORT_PACKAGE, BN_CLICKED, OnExportPackage )
        COMMAND_RANGE_HANDLER_EX( IDOK, IDCANCEL, OnCloseCmd )
    END_MSG_MAP()

    CDialogPackageManager( const std::u8string& currentPackagePath );

    std::optional<smp::config::PanelSettings_Package> GetPackage() const;
    std::optional<smp::config::ParsedPanelSettings_Package> GetParsedPackage() const;

private:
    LRESULT OnInitDialog( HWND hwndFocus, LPARAM lParam );
    void OnUiChange( UINT uNotifyCode, int nID, CWindow wndCtl );
    void OnNewPackage( UINT uNotifyCode, int nID, CWindow wndCtl );
    void OnDeletePackage( UINT uNotifyCode, int nID, CWindow wndCtl );
    void OnImportPackage( UINT uNotifyCode, int nID, CWindow wndCtl );
    void OnExportPackage( UINT uNotifyCode, int nID, CWindow wndCtl );
    LRESULT OnCloseCmd( WORD wNotifyCode, WORD wID, HWND hWndCtl );

    void UpdateUiFromData();
    void UpdateUiButtons();

    void LoadPackagesData();

private:
    struct DisplayedData
    {
        std::u8string name;
        std::u8string info;
    };

    struct AttachedData
    {
        smp::config::PanelSettings_Package settings;
        std::optional<smp::config::ParsedPanelSettings_Package> parsedSettings;
    };

    struct PackageData
    {
        DisplayedData displayedData;
        AttachedData attachedData;
    };

    std::u8string currentPackagePath_;

    int packageIdx_ = -1;
    std::array<std::unique_ptr<IUiDdx>, 1> ddx_;

    std::vector<PackageData> packages_;
    CListBox packagesListBox_;
};

} // namespace smp::ui
