#include <stdafx.h>

#include "ui_conf_tab_appearance.h"

#include <ui/ui_conf_new.h>
#include <utils/error_popup.h>
#include <utils/file_helpers.h>
#include <utils/scope_helpers.h>
#include <utils/winapi_error_helpers.h>

#include <component_paths.h>

#include <filesystem>

namespace
{

int GetEdgeIdFromEnum( smp::config::EdgeStyle edgeStyle )
{
    switch ( edgeStyle )
    {
    case smp::config::EdgeStyle::NoEdge:
    {
        return IDC_RADIO_EDGE_NO;
    }
    case smp::config::EdgeStyle::SunkenEdge:
    {
        return IDC_RADIO_EDGE_SUNKEN;
    }
    case smp::config::EdgeStyle::GreyEdge:
    {
        return IDC_RADIO_EDGE_GREY;
    }
    default:
    {
        assert( false );
        return IDC_RADIO_EDGE_NO;
    }
    }
}

} // namespace

namespace smp::ui
{

CConfigTabAppearance::CConfigTabAppearance( CDialogConfNew& parent, OptionWrap<config::PanelSettings>& settings )
    : parent_( parent )
    , settings_( settings )
    , edgeStyle_(
          settings_, []( auto& value ) -> auto& { return value.edgeStyle; } )
    , isPseudoTransparent_(
          settings_, []( auto& value ) -> auto& { return value.isPseudoTransparent; } )
    , edgeStyleId_( GetEdgeIdFromEnum( edgeStyle_.GetSavedValue() ), GetEdgeIdFromEnum( edgeStyle_.GetCurrentValue() ) )
    , ddxOpts_( {
          CreateUiDdxOption<UiDdx_CheckBox>( isPseudoTransparent_, IDC_CHECK_PSEUDOTRANSPARENT ),
          CreateUiDdxOption<UiDdx_RadioRange>( edgeStyleId_,
                                               std::initializer_list<int>{
                                                   IDC_RADIO_EDGE_NO,
                                                   IDC_RADIO_EDGE_SUNKEN,
                                                   IDC_RADIO_EDGE_GREY,
                                               } ),
      } )
{
}

HWND CConfigTabAppearance::CreateTab( HWND hParent )
{
    return Create( hParent );
}

CDialogImplBase& CConfigTabAppearance::Dialog()
{
    return *this;
}

const wchar_t* CConfigTabAppearance::Name() const
{
    return L"Appearance";
}

bool CConfigTabAppearance::ValidateState()
{
    return true;
}

bool CConfigTabAppearance::HasChanged()
{
    const bool hasChanged =
        ddxOpts_.cend() != ranges::find_if( ddxOpts_, []( const auto& ddxOpt ) {
            return ddxOpt->Option().HasChanged();
        } );

    return hasChanged;
}

void CConfigTabAppearance::Apply()
{
    assert( ValidateState() );
    for ( auto& ddxOpt: ddxOpts_ )
    {
        ddxOpt->Option().Apply();
    }
    edgeStyle_.Apply();
}

void CConfigTabAppearance::Revert()
{
    for ( auto& ddxOpt: ddxOpts_ )
    {
        ddxOpt->Option().Revert();
    }
    edgeStyle_.Revert();

    InitializeLocalOptions();
    UpdateUiFromData();
}

BOOL CConfigTabAppearance::OnInitDialog( HWND hwndFocus, LPARAM lParam )
{
    for ( auto& ddxOpt: ddxOpts_ )
    {
        ddxOpt->Ddx().SetHwnd( m_hWnd );
    }

    InitializeLocalOptions();
    UpdateUiFromData();

    return TRUE; // set focus to default control
}

void CConfigTabAppearance::OnEditChange( UINT uNotifyCode, int nID, CWindow wndCtl )
{
    auto it = ranges::find_if( ddxOpts_, [nID]( auto& ddxOpt ) {
        return ddxOpt->Ddx().IsMatchingId( nID );
    } );

    if ( ddxOpts_.end() != it )
    {
        ( *it )->Ddx().ReadFromUi();
    }

    switch ( nID )
    {
    case IDC_RADIO_EDGE_NO:
    case IDC_RADIO_EDGE_SUNKEN:
    case IDC_RADIO_EDGE_GREY:
    {
        switch ( edgeStyleId_.GetCurrentValue() )
        {
        case IDC_RADIO_EDGE_NO:
        {
            edgeStyle_ = smp::config::EdgeStyle::NoEdge;
            break;
        }
        case IDC_RADIO_EDGE_SUNKEN:
        {
            edgeStyle_ = smp::config::EdgeStyle::SunkenEdge;
            break;
        }
        case IDC_RADIO_EDGE_GREY:
        {
            edgeStyle_ = smp::config::EdgeStyle::GreyEdge;
            break;
        }
        default:
        {
            assert( false );
            break;
        }
        }

        break;
    }
    }

    OnChanged();
}

void CConfigTabAppearance::OnChanged()
{
    parent_.OnDataChanged();
}

void CConfigTabAppearance::InitializeLocalOptions()
{
    edgeStyleId_.InitializeValue( GetEdgeIdFromEnum( edgeStyle_.GetSavedValue() ), GetEdgeIdFromEnum( edgeStyle_.GetCurrentValue() ) );
}

void CConfigTabAppearance::UpdateUiFromData()
{
    if ( !this->m_hWnd )
    {
        return;
    }

    for ( auto& ddxOpt: ddxOpts_ )
    {
        ddxOpt->Ddx().WriteToUi();
    }
}

} // namespace smp::ui
