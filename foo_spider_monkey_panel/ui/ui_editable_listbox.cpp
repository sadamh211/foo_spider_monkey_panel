
#include <stdafx.h>

#include "ui_editable_listbox.h"

#include <utils/scope_helpers.h>

#include <user_message.h>

namespace smp::ui
{

CEditableListBox::~CEditableListBox()
{
    if ( cedit_ )
    {
        cedit_.DestroyWindow();
    }
}

void CEditableListBox::StartEdit()
{
    // TODO: check if autoscrolled on focus

    const int iSel = GetCurSel();
    if ( iSel == LB_ERR )
    {
        return;
    }

    CRect rc;
    GetItemRect( iSel, rc );
    rc.InflateRect( 0, 2, 0, 2 );

    cedit_.Create( *this, rc, nullptr, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL, 0, 1 );
    cedit_.ModifyStyleEx( 0, WS_EX_CLIENTEDGE );

    cedit_.SetFont( this->GetFont() );

    CString csItem;
    GetText( iSel, csItem );

    cedit_.SetWindowText( csItem );
    cedit_.SetSel( 0, -1, TRUE );
    cedit_.SetFocus();

    editedItemIdx_ = iSel;
}

void CEditableListBox::EndEdit( bool bCommitText )
{
    if ( !cedit_ )
    {
        return;
    }

    smp::utils::final_action autoCEdit( [&] {
        editedItemIdx_ = -1;
        cedit_.DestroyWindow();
    } );

    if ( !bCommitText || editedItemIdx_ == -1 )
    {
        return;
    }

    CString oldText;
    GetText( editedItemIdx_, oldText );

    CString newText;
    cedit_.GetWindowText( newText );

    if ( !oldText.Compare( newText ) )
    {
        return;
    }

    // TODO: do not remove column
    DeleteString( editedItemIdx_ );

    if ( GetStyle() & LBS_SORT )
    {
        editedItemIdx_ = AddString( newText );
    }
    else
    {
        editedItemIdx_ = InsertString( editedItemIdx_, newText );
    }

    SetCurSel( editedItemIdx_ );

    if ( GetStyle() & LBS_NOTIFY )
    {
        GetParent().SendMessage( static_cast<UINT>( smp::MiscMessage::ui_editable_listbox_item_edited ),
                                 (WPARAM)editedItemIdx_,
                                 ( LPARAM ) static_cast<LPCTSTR>( oldText ) );
    }
}

LRESULT CEditableListBox::OnHScroll( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
    bHandled = FALSE; ///< don't suppress base class methods
    EndEdit();
    return 0;
}

LRESULT CEditableListBox::OnVScroll( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
    bHandled = FALSE; ///< don't suppress base class methods
    EndEdit();
    return 0;
}

LRESULT CEditableListBox::OnKeyUp( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
    const auto nChar = (UINT)wParam;
    if ( nChar == VK_RETURN )
    {
        StartEdit();
    }
    return 0;
}

} // namespace smp::ui
