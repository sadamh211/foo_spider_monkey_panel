#pragma once

namespace smp::ui
{

class CEditableListBox;

class CEdit_EditableListBox 
    : public CWindowImpl<CEdit_EditableListBox, CEdit>
{
public:
    BEGIN_MSG_MAP( CEdit_EditableListBox )
        MESSAGE_HANDLER( WM_KEYUP, OnKeyUp )
        MESSAGE_HANDLER( WM_KILLFOCUS, OnKillFocus )
    END_MSG_MAP()

    CEdit_EditableListBox( CEditableListBox& parent );
    virtual ~CEdit_EditableListBox() = default;

private:
    LRESULT OnKeyUp( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
    LRESULT OnKillFocus( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );

private:
    CEditableListBox& parent_;
};

} // namespace smp::ui
