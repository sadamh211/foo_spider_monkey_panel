#pragma once

#include <ui/ui_edit_editable_listbox.h>

namespace smp::ui
{

class CEditableListBox : public CListBox
{
public:
    BEGIN_MSG_MAP( CConfigTabPackage )
        MESSAGE_HANDLER( WM_VSCROLL, OnVScroll )
        MESSAGE_HANDLER( WM_VSCROLL, OnHScroll )
        MESSAGE_HANDLER( WM_KEYUP, OnKeyUp )
    END_MSG_MAP()

public:
    CEditableListBox() = default;
    virtual ~CEditableListBox();

public:
    void StartEdit();
    void EndEdit( bool commitText = true );

private:
    LRESULT OnHScroll( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
    LRESULT OnVScroll( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
    LRESULT OnKeyUp( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );

private:
    CEdit_EditableListBox cedit_;
    int editedItemIdx_ = -1;
};

} // namespace smp::ui
