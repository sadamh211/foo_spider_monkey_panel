#pragma once

#include "resource.h"

#include <config/panel_config.h>
#include <config/parsed_panel_config.h>
#include <ui/internal/ui_ddx.h>

#include <optional>

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
        COMMAND_HANDLER_EX( IDC_LIST_PACKAGES, LBN_SELCHANGE, OnUiChange )
        COMMAND_HANDLER_EX( IDC_BUTTON_NEW_PACKAGE, BN_CLICKED, OnNewPackage )
        COMMAND_HANDLER_EX( IDC_BUTTON_DELETE_PACKAGE, BN_CLICKED, OnDeletePackage )
        COMMAND_HANDLER_EX( IDC_BUTTON_IMPORT_PACKAGE, BN_CLICKED, OnImportPackage )
        COMMAND_HANDLER_EX( IDC_BUTTON_EXPORT_PACKAGE, BN_CLICKED, OnExportPackage )
        COMMAND_HANDLER_EX( IDC_BUTTON_OPEN_FOLDER, BN_CLICKED, OnOpenFolder )
        COMMAND_RANGE_HANDLER_EX( IDOK, IDCANCEL, OnCloseCmd )
        NOTIFY_HANDLER_EX( IDC_RICHEDIT_PACKAGE_INFO, EN_LINK, OnRichEditLinkClick )
    END_MSG_MAP()

    CDialogPackageManager( const std::u8string& currentPackageId );

    std::optional<smp::config::PanelSettings_Package> GetPackage() const;
    std::optional<smp::config::ParsedPanelSettings_Package> GetParsedPackage() const;

private:
    struct PackageData
    {
        std::wstring displayedName;
        std::u8string id;
        std::optional<smp::config::ParsedPanelSettings_Package> parsedSettings;
        std::wstring errorText;
    };

private:
    LRESULT OnInitDialog( HWND hwndFocus, LPARAM lParam );
    void OnUiChange( UINT uNotifyCode, int nID, CWindow wndCtl );
    void OnNewPackage( UINT uNotifyCode, int nID, CWindow wndCtl );
    void OnDeletePackage( UINT uNotifyCode, int nID, CWindow wndCtl );
    void OnImportPackage( UINT uNotifyCode, int nID, CWindow wndCtl );
    void OnExportPackage( UINT uNotifyCode, int nID, CWindow wndCtl );
    void OnOpenFolder( UINT uNotifyCode, int nID, CWindow wndCtl );
    LRESULT OnCloseCmd( WORD wNotifyCode, WORD wID, HWND hWndCtl );

    LRESULT OnRichEditLinkClick( LPNMHDR pnmh );

    void UpdateUiFromData();
    void UpdateUiButtons();

    void LoadPackages();
    void SortPackages();

    void UpdateListBoxFromData();
    void UpdatedPackageIdx();
    void UpdatedPackageId();
    void UpdatedUiPackageInfo();

    PackageData GeneratePackageData( const smp::config::ParsedPanelSettings_Package& parsedSettings );

private:
    std::u8string currentPackageId_;
    int packageIdx_ = -1;
    std::array<std::unique_ptr<IUiDdx>, 1> ddx_;

    std::vector<PackageData> packages_;
    CListBox packagesListBox_;

    CRichEditCtrl packageInfoEdit_;
};

} // namespace smp::ui
