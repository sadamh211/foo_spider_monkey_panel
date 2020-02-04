// EditHelper.cpp : implementation file
//

#include <stdafx.h>

#include "ui_edit_editable_listbox.h"

#include <ui/ui_editable_listbox.h>

namespace smp::ui
{

CEdit_EditableListBox::CEdit_EditableListBox( CEditableListBox& parent )
    : parent_( parent )
{
}

LRESULT CEdit_EditableListBox::OnKeyUp( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
    bHandled = FALSE; ///< don't suppress base class methods

    const auto nChar = (UINT)wParam;
    switch ( nChar )
    {
    case VK_RETURN:
        parent_.EndEdit( true );
        break;

    case VK_ESCAPE:
        parent_.EndEdit( false );
        break;
    }

    return 0;
}

LRESULT CEdit_EditableListBox::OnKillFocus( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
    bHandled = FALSE; ///< don't suppress base class methods

    parent_.EndEdit( true );

    return 0;
}

} // namespace smp::ui
