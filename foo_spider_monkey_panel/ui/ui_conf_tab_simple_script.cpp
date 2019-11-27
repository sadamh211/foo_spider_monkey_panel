#include <stdafx.h>

#include "ui_conf_tab_simple_script.h"

#include <ui/ui_conf_new.h>
#include <utils/error_popup.h>

namespace smp::ui
{

ConfigTabSimpleScript::ConfigTabSimpleScript( CDialogConfNew& parent, OptionWrap<config::PanelSettings>& settings )
    : parent_( parent )
    , settings_( settings )
    , payload_(
          settings_, []( auto& value ) -> auto& { return value.payload; } )
    , ddxOpts_( {
          CreateUiDdxOption<UiDdx_TextEdit>( path_, IDC_RICHEDIT_SRC_PATH ),
          CreateUiDdxOption<UiDdx_TextEdit>( sampleName_, IDC_COMBO_SRC_SAMPLE ),
          CreateUiDdxOption<UiDdx_RadioRange>( payloadSwitchId_,
                                               std::initializer_list<int>{
                                                   IDC_RADIO_SRC_SAMPLE,
                                                   IDC_RADIO_SRC_MEMORY,
                                                   IDC_RADIO_SRC_FILE,
                                               } ),
      } )
{
}

HWND ConfigTabSimpleScript::CreateTab( HWND hParent )
{
    return Create( hParent );
}

CDialogImplBase& ConfigTabSimpleScript::Dialog()
{
    return *this;
}

const wchar_t* ConfigTabSimpleScript::Name() const
{
    return L"Script";
}

bool ConfigTabSimpleScript::ValidateState()
{
    return std::visit( []( const auto& data ) {
        using T = std::decay_t<decltype( data )>;
        if constexpr ( std::is_same_v<T, smp::config::PanelSettings_File> )
        {
            if ( data.path.empty() )
            {
                smp::utils::ReportErrorWithPopup( fmt::format( "Invalid path to script: {}", data.path ) );
                return false;
            }
            else
            {
                return true;
            }
        }
        else
        {
            return true;
        }
    },
                       payload_.GetCurrentValue() );
}

bool ConfigTabSimpleScript::HasChanged()
{
    const bool hasChanged =
        ddxOpts_.cend() != ranges::find_if( ddxOpts_, []( const auto& ddxOpt ) {
            return ddxOpt->Option().HasChanged();
        } );

    return hasChanged;
}

void ConfigTabSimpleScript::Apply()
{
    assert( ValidateState() );
    for ( auto& ddxOpt: ddxOpts_ )
    {
        ddxOpt->Option().Apply();
    }
}

void ConfigTabSimpleScript::Revert()
{
    for ( auto& ddxOpt: ddxOpts_ )
    {
        ddxOpt->Option().Revert();
    }

    UpdateUiFromData();
}

BOOL ConfigTabSimpleScript::OnInitDialog( HWND hwndFocus, LPARAM lParam )
{
    for ( auto& ddxOpt: ddxOpts_ )
    {
        ddxOpt->Ddx().SetHwnd( m_hWnd );
    }
    UpdateUiFromData();

    return TRUE; // set focus to default control
}

void ConfigTabSimpleScript::OnEditChange( UINT uNotifyCode, int nID, CWindow wndCtl )
{
    auto it = std::find_if( ddxOpts_.begin(), ddxOpts_.end(), [nID]( auto& ddxOpt ) {
        return ddxOpt->Ddx().IsMatchingId( nID );
    } );

    if ( ddxOpts_.end() != it )
    {
        ( *it )->Ddx().ReadFromUi();
    }

    switch ( nID )
    {
    case IDC_RADIO_SRC_SAMPLE:
    case IDC_RADIO_SRC_MEMORY:
    case IDC_RADIO_SRC_FILE:
    {
        switch ( payloadSwitchId_.GetCurrentValue() )
        {
        case IDC_RADIO_SRC_SAMPLE:
        {
            payload_ = config::PanelSettings_Sample{ sampleName_.GetCurrentValue() };
            CWindow{ GetDlgItem( IDC_RICHEDIT_SRC_PATH ) }.EnableWindow( false );
            break;
        }
        case IDC_RADIO_SRC_MEMORY:
        {
            payload_ = config::PanelSettings_InMemory{};
            CWindow{ GetDlgItem( IDC_RICHEDIT_SRC_PATH ) }.EnableWindow( false );
            break;
        }
        case IDC_RADIO_SRC_FILE:
        {
            payload_ = config::PanelSettings_File{ path_.GetCurrentValue() };
            CWindow{ GetDlgItem( IDC_RICHEDIT_SRC_PATH ) }.EnableWindow( true );
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
    case IDC_RICHEDIT_SRC_PATH:
    {
        assert( std::holds_alternative<config::PanelSettings_File>( payload_.GetCurrentValue() ) );
        payload_ = config::PanelSettings_File{ path_.GetCurrentValue() };
        break;
    }
    case IDC_COMBO_SRC_SAMPLE:
    {
        assert( std::holds_alternative<config::PanelSettings_Sample>( payload_.GetCurrentValue() ) );
        payload_ = config::PanelSettings_Sample{ sampleName_.GetCurrentValue() };
        break;
    }
    default:
    {
        break;
    }
    }

    OnChanged();
}

void ConfigTabSimpleScript::OnSwitchMode( UINT uNotifyCode, int nID, CWindow wndCtl )
{
}

void ConfigTabSimpleScript::OnChanged()
{
    parent_.OnDataChanged();
}

void ConfigTabSimpleScript::UpdateUiFromData()
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
