#pragma once

#include "resource.h"

namespace smp::ui
{

class CInputBox : public CDialogImpl<CInputBox>
{
public:
    enum
    {
        IDD = IDD_DIALOG_INPUT
    };

    BEGIN_MSG_MAP( CInputBox )
        MSG_WM_INITDIALOG( OnInitDialog )
        MSG_WM_COMMAND( OnCommand )
    END_MSG_MAP()

    CInputBox( const std::u8string& p_prompt, const std::u8string& p_caption, const std::u8string& p_value );

    LRESULT OnInitDialog( HWND hwndFocus, LPARAM lParam );
    LRESULT OnCommand( UINT codeNotify, int id, HWND hwndCtl );
    std::u8string GetValue();

private:
    std::u8string m_prompt;
    std::u8string m_caption;
    std::u8string m_value;
};

} // namespace smp::ui
