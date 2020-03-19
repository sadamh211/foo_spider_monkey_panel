#include <stdafx.h>

#include "fb_utils.h"

#include <com_objects/drop_source_impl.h>
#include <js_engine/js_to_native_invoker.h>
#include <js_objects/context_menu_manager.h>
#include <js_objects/fb_metadb_handle.h>
#include <js_objects/fb_metadb_handle_list.h>
#include <js_objects/fb_profiler.h>
#include <js_objects/fb_title_format.h>
#include <js_objects/fb_ui_selection_holder.h>
#include <js_objects/gdi_bitmap.h>
#include <js_objects/main_menu_manager.h>
#include <js_utils/js_error_helper.h>
#include <js_utils/js_object_helper.h>
#include <js_utils/js_property_helper.h>
#include <utils/array_x.h>
#include <utils/art_helpers.h>
#include <utils/delayed_executor.h>
#include <utils/menu_helpers.h>
#include <utils/string_helpers.h>

#include <component_paths.h>
#include <message_blocking_scope.h>
#include <stats.h>

#include <range/v3/algorithm.hpp>

using namespace smp;

namespace
{

using namespace mozjs;

JSClassOps jsOps = {
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    JsFbUtils::FinalizeJsObject,
    nullptr,
    nullptr,
    nullptr,
    nullptr
};

JSClass jsClass = {
    "FbUtils",
    kDefaultClassFlags,
    &jsOps
};

MJS_DEFINE_JS_FN_FROM_NATIVE( AcquireUiSelectionHolder, JsFbUtils::AcquireUiSelectionHolder )
MJS_DEFINE_JS_FN_FROM_NATIVE( AddDirectory, JsFbUtils::AddDirectory )
MJS_DEFINE_JS_FN_FROM_NATIVE( AddFiles, JsFbUtils::AddFiles )
MJS_DEFINE_JS_FN_FROM_NATIVE( CheckClipboardContents, JsFbUtils::CheckClipboardContents )
MJS_DEFINE_JS_FN_FROM_NATIVE( ClearPlaylist, JsFbUtils::ClearPlaylist )
MJS_DEFINE_JS_FN_FROM_NATIVE( CopyHandleListToClipboard, JsFbUtils::CopyHandleListToClipboard )
MJS_DEFINE_JS_FN_FROM_NATIVE( CreateContextMenuManager, JsFbUtils::CreateContextMenuManager )
MJS_DEFINE_JS_FN_FROM_NATIVE( CreateHandleList, JsFbUtils::CreateHandleList )
MJS_DEFINE_JS_FN_FROM_NATIVE( CreateMainMenuManager, JsFbUtils::CreateMainMenuManager )
MJS_DEFINE_JS_FN_FROM_NATIVE_WITH_OPT( CreateProfiler, JsFbUtils::CreateProfiler, JsFbUtils::CreateProfilerWithOpt, 1 )
MJS_DEFINE_JS_FN_FROM_NATIVE_WITH_OPT( DoDragDrop, JsFbUtils::DoDragDrop, JsFbUtils::DoDragDropWithOpt, 1 )
MJS_DEFINE_JS_FN_FROM_NATIVE( Exit, JsFbUtils::Exit )
MJS_DEFINE_JS_FN_FROM_NATIVE( GetClipboardContents, JsFbUtils::GetClipboardContents )
MJS_DEFINE_JS_FN_FROM_NATIVE( GetDSPPresets, JsFbUtils::GetDSPPresets )
MJS_DEFINE_JS_FN_FROM_NATIVE_WITH_OPT( GetFocusItem, JsFbUtils::GetFocusItem, JsFbUtils::GetFocusItemWithOpt, 1 )
MJS_DEFINE_JS_FN_FROM_NATIVE( GetLibraryItems, JsFbUtils::GetLibraryItems )
MJS_DEFINE_JS_FN_FROM_NATIVE( GetLibraryRelativePath, JsFbUtils::GetLibraryRelativePath )
MJS_DEFINE_JS_FN_FROM_NATIVE( GetNowPlaying, JsFbUtils::GetNowPlaying )
MJS_DEFINE_JS_FN_FROM_NATIVE( GetOutputDevices, JsFbUtils::GetOutputDevices )
MJS_DEFINE_JS_FN_FROM_NATIVE( GetQueryItems, JsFbUtils::GetQueryItems )
MJS_DEFINE_JS_FN_FROM_NATIVE( GetSelection, JsFbUtils::GetSelection )
MJS_DEFINE_JS_FN_FROM_NATIVE_WITH_OPT( GetSelections, JsFbUtils::GetSelections, JsFbUtils::GetSelectionsWithOpt, 1 )
MJS_DEFINE_JS_FN_FROM_NATIVE( GetSelectionType, JsFbUtils::GetSelectionType )
MJS_DEFINE_JS_FN_FROM_NATIVE( IsLibraryEnabled, JsFbUtils::IsLibraryEnabled )
MJS_DEFINE_JS_FN_FROM_NATIVE( IsMainMenuCommandChecked, JsFbUtils::IsMainMenuCommandChecked )
MJS_DEFINE_JS_FN_FROM_NATIVE( IsMetadbInMediaLibrary, JsFbUtils::IsMetadbInMediaLibrary )
MJS_DEFINE_JS_FN_FROM_NATIVE( LoadPlaylist, JsFbUtils::LoadPlaylist )
MJS_DEFINE_JS_FN_FROM_NATIVE( Next, JsFbUtils::Next )
MJS_DEFINE_JS_FN_FROM_NATIVE( Pause, JsFbUtils::Pause )
MJS_DEFINE_JS_FN_FROM_NATIVE( Play, JsFbUtils::Play )
MJS_DEFINE_JS_FN_FROM_NATIVE( PlayOrPause, JsFbUtils::PlayOrPause )
MJS_DEFINE_JS_FN_FROM_NATIVE( Prev, JsFbUtils::Prev )
MJS_DEFINE_JS_FN_FROM_NATIVE( Random, JsFbUtils::Random )
MJS_DEFINE_JS_FN_FROM_NATIVE_WITH_OPT( RunContextCommand, JsFbUtils::RunContextCommand, JsFbUtils::RunContextCommandWithOpt, 1 )
MJS_DEFINE_JS_FN_FROM_NATIVE_WITH_OPT( RunContextCommandWithMetadb, JsFbUtils::RunContextCommandWithMetadb, JsFbUtils::RunContextCommandWithMetadbWithOpt, 1 )
MJS_DEFINE_JS_FN_FROM_NATIVE( RunMainMenuCommand, JsFbUtils::RunMainMenuCommand )
MJS_DEFINE_JS_FN_FROM_NATIVE( SavePlaylist, JsFbUtils::SavePlaylist )
MJS_DEFINE_JS_FN_FROM_NATIVE( SetDSPPreset, JsFbUtils::SetDSPPreset )
MJS_DEFINE_JS_FN_FROM_NATIVE( SetOutputDevice, JsFbUtils::SetOutputDevice )
MJS_DEFINE_JS_FN_FROM_NATIVE( ShowConsole, JsFbUtils::ShowConsole )
MJS_DEFINE_JS_FN_FROM_NATIVE( ShowLibrarySearchUI, JsFbUtils::ShowLibrarySearchUI )
MJS_DEFINE_JS_FN_FROM_NATIVE_WITH_OPT( ShowPopupMessage, JsFbUtils::ShowPopupMessage, JsFbUtils::ShowPopupMessageWithOpt, 1 )
MJS_DEFINE_JS_FN_FROM_NATIVE( ShowPreferences, JsFbUtils::ShowPreferences )
MJS_DEFINE_JS_FN_FROM_NATIVE( Stop, JsFbUtils::Stop )
MJS_DEFINE_JS_FN_FROM_NATIVE( TitleFormat, JsFbUtils::TitleFormat )
MJS_DEFINE_JS_FN_FROM_NATIVE( VolumeDown, JsFbUtils::VolumeDown )
MJS_DEFINE_JS_FN_FROM_NATIVE( VolumeMute, JsFbUtils::VolumeMute )
MJS_DEFINE_JS_FN_FROM_NATIVE( VolumeUp, JsFbUtils::VolumeUp )

constexpr auto jsFunctions = smp::to_array<JSFunctionSpec>(
    {
        JS_FN( "AcquireUiSelectionHolder", AcquireUiSelectionHolder, 0, kDefaultPropsFlags ),
        JS_FN( "AddDirectory", AddDirectory, 0, kDefaultPropsFlags ),
        JS_FN( "AddFiles", AddFiles, 0, kDefaultPropsFlags ),
        JS_FN( "CheckClipboardContents", CheckClipboardContents, 0, kDefaultPropsFlags ),
        JS_FN( "ClearPlaylist", ClearPlaylist, 0, kDefaultPropsFlags ),
        JS_FN( "CopyHandleListToClipboard", CopyHandleListToClipboard, 1, kDefaultPropsFlags ),
        JS_FN( "CreateContextMenuManager", CreateContextMenuManager, 0, kDefaultPropsFlags ),
        JS_FN( "CreateHandleList", CreateHandleList, 0, kDefaultPropsFlags ),
        JS_FN( "CreateMainMenuManager", CreateMainMenuManager, 0, kDefaultPropsFlags ),
        JS_FN( "CreateProfiler", CreateProfiler, 0, kDefaultPropsFlags ),
        JS_FN( "DoDragDrop", DoDragDrop, 3, kDefaultPropsFlags ),
        JS_FN( "Exit", Exit, 0, kDefaultPropsFlags ),
        JS_FN( "GetClipboardContents", GetClipboardContents, 0, kDefaultPropsFlags ),
        JS_FN( "GetDSPPresets", GetDSPPresets, 0, kDefaultPropsFlags ),
        JS_FN( "GetFocusItem", GetFocusItem, 0, kDefaultPropsFlags ),
        JS_FN( "GetLibraryItems", GetLibraryItems, 0, kDefaultPropsFlags ),
        JS_FN( "GetLibraryRelativePath", GetLibraryRelativePath, 1, kDefaultPropsFlags ),
        JS_FN( "GetNowPlaying", GetNowPlaying, 0, kDefaultPropsFlags ),
        JS_FN( "GetOutputDevices", GetOutputDevices, 0, kDefaultPropsFlags ),
        JS_FN( "GetQueryItems", GetQueryItems, 2, kDefaultPropsFlags ),
        JS_FN( "GetSelection", GetSelection, 0, kDefaultPropsFlags ),
        JS_FN( "GetSelections", GetSelections, 0, kDefaultPropsFlags ),
        JS_FN( "GetSelectionType", GetSelectionType, 0, kDefaultPropsFlags ),
        JS_FN( "IsLibraryEnabled", IsLibraryEnabled, 0, kDefaultPropsFlags ),
        JS_FN( "IsMainMenuCommandChecked", IsMainMenuCommandChecked, 1, kDefaultPropsFlags ),
        JS_FN( "IsMetadbInMediaLibrary", IsMetadbInMediaLibrary, 1, kDefaultPropsFlags ),
        JS_FN( "LoadPlaylist", LoadPlaylist, 0, kDefaultPropsFlags ),
        JS_FN( "Next", Next, 0, kDefaultPropsFlags ),
        JS_FN( "Pause", Pause, 0, kDefaultPropsFlags ),
        JS_FN( "Play", Play, 0, kDefaultPropsFlags ),
        JS_FN( "PlayOrPause", PlayOrPause, 0, kDefaultPropsFlags ),
        JS_FN( "Prev", Prev, 0, kDefaultPropsFlags ),
        JS_FN( "Random", Random, 0, kDefaultPropsFlags ),
        JS_FN( "RunContextCommand", RunContextCommand, 1, kDefaultPropsFlags ),
        JS_FN( "RunContextCommandWithMetadb", RunContextCommandWithMetadb, 2, kDefaultPropsFlags ),
        JS_FN( "RunMainMenuCommand", RunMainMenuCommand, 1, kDefaultPropsFlags ),
        JS_FN( "SavePlaylist", SavePlaylist, 0, kDefaultPropsFlags ),
        JS_FN( "SetDSPPreset", SetDSPPreset, 1, kDefaultPropsFlags ),
        JS_FN( "SetOutputDevice", SetOutputDevice, 2, kDefaultPropsFlags ),
        JS_FN( "ShowConsole", ShowConsole, 0, kDefaultPropsFlags ),
        JS_FN( "ShowLibrarySearchUI", ShowLibrarySearchUI, 1, kDefaultPropsFlags ),
        JS_FN( "ShowPopupMessage", ShowPopupMessage, 1, kDefaultPropsFlags ),
        JS_FN( "ShowPreferences", ShowPreferences, 0, kDefaultPropsFlags ),
        JS_FN( "Stop", Stop, 0, kDefaultPropsFlags ),
        JS_FN( "TitleFormat", TitleFormat, 1, kDefaultPropsFlags ),
        JS_FN( "VolumeDown", VolumeDown, 0, kDefaultPropsFlags ),
        JS_FN( "VolumeMute", VolumeMute, 0, kDefaultPropsFlags ),
        JS_FN( "VolumeUp", VolumeUp, 0, kDefaultPropsFlags ),
        JS_FS_END,
    } );

MJS_DEFINE_JS_FN_FROM_NATIVE( get_AlwaysOnTop, JsFbUtils::get_AlwaysOnTop )
MJS_DEFINE_JS_FN_FROM_NATIVE( get_ComponentPath, JsFbUtils::get_ComponentPath )
MJS_DEFINE_JS_FN_FROM_NATIVE( get_CursorFollowPlayback, JsFbUtils::get_CursorFollowPlayback )
MJS_DEFINE_JS_FN_FROM_NATIVE( get_FoobarPath, JsFbUtils::get_FoobarPath )
MJS_DEFINE_JS_FN_FROM_NATIVE( get_IsPaused, JsFbUtils::get_IsPaused )
MJS_DEFINE_JS_FN_FROM_NATIVE( get_IsPlaying, JsFbUtils::get_IsPlaying )
MJS_DEFINE_JS_FN_FROM_NATIVE( get_PlaybackFollowCursor, JsFbUtils::get_PlaybackFollowCursor )
MJS_DEFINE_JS_FN_FROM_NATIVE( get_PlaybackLength, JsFbUtils::get_PlaybackLength )
MJS_DEFINE_JS_FN_FROM_NATIVE( get_PlaybackTime, JsFbUtils::get_PlaybackTime )
MJS_DEFINE_JS_FN_FROM_NATIVE( get_ProfilePath, JsFbUtils::get_ProfilePath )
MJS_DEFINE_JS_FN_FROM_NATIVE( get_ReplaygainMode, JsFbUtils::get_ReplaygainMode )
MJS_DEFINE_JS_FN_FROM_NATIVE( get_StopAfterCurrent, JsFbUtils::get_StopAfterCurrent )
MJS_DEFINE_JS_FN_FROM_NATIVE( get_Volume, JsFbUtils::get_Volume )
MJS_DEFINE_JS_FN_FROM_NATIVE( put_AlwaysOnTop, JsFbUtils::put_AlwaysOnTop )
MJS_DEFINE_JS_FN_FROM_NATIVE( put_CursorFollowPlayback, JsFbUtils::put_CursorFollowPlayback )
MJS_DEFINE_JS_FN_FROM_NATIVE( put_PlaybackFollowCursor, JsFbUtils::put_PlaybackFollowCursor )
MJS_DEFINE_JS_FN_FROM_NATIVE( put_PlaybackTime, JsFbUtils::put_PlaybackTime )
MJS_DEFINE_JS_FN_FROM_NATIVE( put_ReplaygainMode, JsFbUtils::put_ReplaygainMode )
MJS_DEFINE_JS_FN_FROM_NATIVE( put_StopAfterCurrent, JsFbUtils::put_StopAfterCurrent )
MJS_DEFINE_JS_FN_FROM_NATIVE( put_Volume, JsFbUtils::put_Volume )

constexpr auto jsProperties = smp::to_array<JSPropertySpec>(
    {
        JS_PSGS( "AlwaysOnTop", get_AlwaysOnTop, put_AlwaysOnTop, kDefaultPropsFlags ),
        JS_PSG( "ComponentPath", get_ComponentPath, kDefaultPropsFlags ),
        JS_PSGS( "CursorFollowPlayback", get_CursorFollowPlayback, put_CursorFollowPlayback, kDefaultPropsFlags ),
        JS_PSG( "FoobarPath", get_FoobarPath, kDefaultPropsFlags ),
        JS_PSG( "IsPaused", get_IsPaused, kDefaultPropsFlags ),
        JS_PSG( "IsPlaying", get_IsPlaying, kDefaultPropsFlags ),
        JS_PSGS( "PlaybackFollowCursor", get_PlaybackFollowCursor, put_PlaybackFollowCursor, kDefaultPropsFlags ),
        JS_PSG( "PlaybackLength", get_PlaybackLength, kDefaultPropsFlags ),
        JS_PSGS( "PlaybackTime", get_PlaybackTime, put_PlaybackTime, kDefaultPropsFlags ),
        JS_PSG( "ProfilePath", get_ProfilePath, kDefaultPropsFlags ),
        JS_PSGS( "ReplaygainMode", get_ReplaygainMode, put_ReplaygainMode, kDefaultPropsFlags ),
        JS_PSGS( "StopAfterCurrent", get_StopAfterCurrent, put_StopAfterCurrent, kDefaultPropsFlags ),
        JS_PSGS( "Volume", get_Volume, put_Volume, kDefaultPropsFlags ),
        JS_PS_END,
    } );

} // namespace

namespace mozjs
{

const JSClass JsFbUtils::JsClass = jsClass;
const JSFunctionSpec* JsFbUtils::JsFunctions = jsFunctions.data();
const JSPropertySpec* JsFbUtils::JsProperties = jsProperties.data();

JsFbUtils::JsFbUtils( JSContext* cx )
    : pJsCtx_( cx )
{
}

std::unique_ptr<JsFbUtils>
JsFbUtils::CreateNative( JSContext* cx )
{
    return std::unique_ptr<JsFbUtils>( new JsFbUtils( cx ) );
}

size_t JsFbUtils::GetInternalSize()
{
    return 0;
}

JSObject* JsFbUtils::AcquireUiSelectionHolder()
{
    return JsFbUiSelectionHolder::CreateJs( pJsCtx_, ui_selection_manager::get()->acquire() );
}

void JsFbUtils::AddDirectory()
{
    standard_commands::main_add_directory();
}

void JsFbUtils::AddFiles()
{
    standard_commands::main_add_files();
}

bool JsFbUtils::CheckClipboardContents()
{
    pfc::com_ptr_t<IDataObject> pDO;
    HRESULT hr = OleGetClipboard( pDO.receive_ptr() );
    if ( FAILED( hr ) )
    {
        return false;
    }

    bool native;
    DWORD drop_effect = DROPEFFECT_COPY;
    hr = ole_interaction::get()->check_dataobject( pDO, drop_effect, native );
    return SUCCEEDED( hr );
}

void JsFbUtils::ClearPlaylist()
{
    standard_commands::main_clear_playlist();
}

bool JsFbUtils::CopyHandleListToClipboard( JsFbMetadbHandleList* handles )
{
    SmpException::ExpectTrue( handles, "handles argument is null" );

    pfc::com_ptr_t<IDataObject> pDO = ole_interaction::get()->create_dataobject( handles->GetHandleList() );
    return SUCCEEDED( OleSetClipboard( pDO.get_ptr() ) );
}

JSObject* JsFbUtils::CreateContextMenuManager()
{
    return JsContextMenuManager::CreateJs( pJsCtx_ );
}

JSObject* JsFbUtils::CreateHandleList()
{
    return JsFbMetadbHandleList::Constructor( pJsCtx_, JS::UndefinedHandleValue );
}

JSObject* JsFbUtils::CreateMainMenuManager()
{
    return JsMainMenuManager::CreateJs( pJsCtx_ );
}

JSObject* JsFbUtils::CreateProfiler( const std::u8string& name )
{
    return JsFbProfiler::Constructor( pJsCtx_, name );
}

JSObject* JsFbUtils::CreateProfilerWithOpt( size_t optArgCount, const std::u8string& name )
{
    switch ( optArgCount )
    {
    case 0:
        return CreateProfiler( name );
    case 1:
        return CreateProfiler();
    default:
        throw SmpException( fmt::format( "Internal error: invalid number of optional arguments specified: {}", optArgCount ) );
    }
}

uint32_t JsFbUtils::DoDragDrop( uint32_t hWindow, JsFbMetadbHandleList* handles, uint32_t okEffects, JS::HandleValue options )
{
    SmpException::ExpectTrue( handles, "handles argument is null" );
    const metadb_handle_list& handleList = handles->GetHandleList();
    const size_t handleCount = handleList.get_count();

    std::unique_ptr<Gdiplus::Bitmap> autoImage;
    auto parsedOptions = ParseDoDragDropOptions( options );
    if ( !parsedOptions.pCustomImage && parsedOptions.useAlbumArt && handleCount )
    {
        metadb_handle_ptr metadb;
        (void)playlist_manager::get()->activeplaylist_get_focus_item_handle( metadb );
        if ( !metadb.is_valid() || pfc_infinite == handleList.find_item( metadb ) )
        {
            metadb = handleList[handleCount - 1];
        }

        autoImage = smp::art::GetBitmapFromMetadbOrEmbed( metadb, 0, false, false, false, nullptr );
        if ( autoImage )
        {
            parsedOptions.pCustomImage = autoImage.get();
        }
    }

    if ( MessageBlockingScope::IsBlocking() || !handleCount || okEffects == DROPEFFECT_NONE )
    {
        return DROPEFFECT_NONE;
    }

    MessageBlockingScope scope;

    pfc::com_ptr_t<IDataObject> pDO = ole_interaction::get()->create_dataobject( handleList );
    // Such HWND cast works only on x86
    pfc::com_ptr_t<com::IDropSourceImpl> pIDropSource = new com::IDropSourceImpl( (HWND)hWindow,
                                                                                  pDO.get_ptr(),
                                                                                  handleCount,
                                                                                  parsedOptions.useTheming,
                                                                                  parsedOptions.showText,
                                                                                  parsedOptions.pCustomImage );

    DWORD returnEffect;
    HRESULT hr = SHDoDragDrop( nullptr, pDO.get_ptr(), pIDropSource.get_ptr(), okEffects, &returnEffect );
    return ( DRAGDROP_S_CANCEL == hr ? DROPEFFECT_NONE : returnEffect );
}

uint32_t JsFbUtils::DoDragDropWithOpt( size_t optArgCount, uint32_t hWindow, JsFbMetadbHandleList* handles, uint32_t okEffects, JS::HandleValue options )
{
    switch ( optArgCount )
    {
    case 0:
        return DoDragDrop( hWindow, handles, okEffects, options );
    case 1:
        return DoDragDrop( hWindow, handles, okEffects );
    default:
        throw SmpException( fmt::format( "Internal error: invalid number of optional arguments specified: {}", optArgCount ) );
    }
}

void JsFbUtils::Exit()
{
    standard_commands::main_exit();
}

JSObject* JsFbUtils::GetClipboardContents( uint32_t hWindow )
{
    auto api = ole_interaction::get();
    pfc::com_ptr_t<IDataObject> pDO;
    metadb_handle_list items;

    if ( SUCCEEDED( OleGetClipboard( pDO.receive_ptr() ) ) )
    {
        DWORD drop_effect = DROPEFFECT_COPY;
        bool native;

        if ( SUCCEEDED( api->check_dataobject( pDO, drop_effect, native ) ) )
        {
            dropped_files_data_impl data;
            if ( SUCCEEDED( api->parse_dataobject( pDO, data ) ) )
            { // Such cast will work only on x86
                data.to_handles( items, native, (HWND)hWindow );
            }
        }
    }

    return JsFbMetadbHandleList::CreateJs( pJsCtx_, items );
}

std::u8string JsFbUtils::GetDSPPresets()
{
    SmpException::ExpectTrue( static_api_test_t<dsp_config_manager_v2>(), "This method requires foobar2000 v1.4 or later" );

    using json = nlohmann::json;

    json j = json::array();
    auto api = dsp_config_manager_v2::get();

    const auto selectedPreset = api->get_selected_preset();
    pfc::string8_fast name;
    for ( const auto i: ranges::views::indices( api->get_preset_count() ) )
    {
        api->get_preset_name( i, name );

        j.push_back( { { "active", selectedPreset == i },
                       { "name", name.get_ptr() } } );
    }

    return j.dump();
}

JSObject* JsFbUtils::GetFocusItem( bool force )
{
    metadb_handle_ptr metadb;
    auto api = playlist_manager::get();

    if ( !api->activeplaylist_get_focus_item_handle( metadb ) && force )
    {
        api->activeplaylist_get_item_handle( metadb, 0 );
    }

    if ( metadb.is_empty() )
    {
        return nullptr;
    }

    return JsFbMetadbHandle::CreateJs( pJsCtx_, metadb );
}

JSObject* JsFbUtils::GetFocusItemWithOpt( size_t optArgCount, bool force )
{
    switch ( optArgCount )
    {
    case 0:
        return GetFocusItem( force );
    case 1:
        return GetFocusItem();
    default:
        throw SmpException( fmt::format( "Internal error: invalid number of optional arguments specified: {}", optArgCount ) );
    }
}

JSObject* JsFbUtils::GetLibraryItems()
{
    metadb_handle_list items;
    library_manager::get()->get_all_items( items );

    return JsFbMetadbHandleList::CreateJs( pJsCtx_, items );
}

pfc::string8_fast JsFbUtils::GetLibraryRelativePath( JsFbMetadbHandle* handle )
{
    SmpException::ExpectTrue( handle, "handle argument is null" );

    pfc::string8_fast temp;
    library_manager::get()->get_relative_path( handle->GetHandle(), temp );
    return temp;
}

JSObject* JsFbUtils::GetNowPlaying()
{
    metadb_handle_ptr metadb;
    if ( !playback_control::get()->get_now_playing( metadb ) )
    {
        return nullptr;
    }

    return JsFbMetadbHandle::CreateJs( pJsCtx_, metadb );
}

std::u8string JsFbUtils::GetOutputDevices()
{
    SmpException::ExpectTrue( static_api_test_t<output_manager_v2>(), "This method requires foobar2000 v1.4 or later" );

    using json = nlohmann::json;

    json j = json::array();
    auto api = output_manager_v2::get();

    outputCoreConfig_t config;
    api->getCoreConfig( config );

    api->listDevices( [&j, &config]( const std::u8string& name, const GUID& output_id, const GUID& device_id ) {
        const std::u8string output_string = fmt::format( "{{{}}}", pfc::print_guid( output_id ) );
        const std::u8string device_string = fmt::format( "{{{}}}", pfc::print_guid( device_id ) );

        j.push_back(
            { { "name", name },
              { "output_id", output_string },
              { "device_id", device_string },
              { "active", config.m_output == output_id && config.m_device == device_id } } );
    } );

    return j.dump();
}

JSObject* JsFbUtils::GetQueryItems( JsFbMetadbHandleList* handles, const std::u8string& query )
{
    SmpException::ExpectTrue( handles, "handles argument is null" );

    const metadb_handle_list& handles_ptr = handles->GetHandleList();
    metadb_handle_list dst_list( handles_ptr );
    search_filter_v2::ptr filter;

    try
    {
        filter = search_filter_manager_v2::get()->create_ex( query.c_str(),
                                                             fb2k::service_new<completion_notify_dummy>(),
                                                             search_filter_manager_v2::KFlagSuppressNotify );
    }
    catch ( const pfc::exception& e )
    {
        throw SmpException( e.what() );
    }

    pfc::array_t<bool> mask;
    mask.set_size( dst_list.get_count() );
    filter->test_multi( dst_list, mask.get_ptr() );
    dst_list.filter_mask( mask.get_ptr() );

    return JsFbMetadbHandleList::CreateJs( pJsCtx_, dst_list );
}

JSObject* JsFbUtils::GetSelection()
{
    metadb_handle_list items;
    ui_selection_manager::get()->get_selection( items );

    if ( !items.get_count() )
    {
        return nullptr;
    }

    return JsFbMetadbHandle::CreateJs( pJsCtx_, items[0] );
}

JSObject* JsFbUtils::GetSelections( uint32_t flags )
{
    metadb_handle_list items;
    ui_selection_manager_v2::get()->get_selection( items, flags );

    return JsFbMetadbHandleList::CreateJs( pJsCtx_, items );
}

JSObject* JsFbUtils::GetSelectionsWithOpt( size_t optArgCount, uint32_t flags )
{
    switch ( optArgCount )
    {
    case 0:
        return GetSelections( flags );
    case 1:
        return GetSelections();
    default:
        throw SmpException( fmt::format( "Internal error: invalid number of optional arguments specified: {}", optArgCount ) );
    }
}

uint32_t JsFbUtils::GetSelectionType()
{
    const std::array<const GUID*, 7> guids = {
        &contextmenu_item::caller_undefined,
        &contextmenu_item::caller_active_playlist_selection,
        &contextmenu_item::caller_active_playlist,
        &contextmenu_item::caller_playlist_manager,
        &contextmenu_item::caller_now_playing,
        &contextmenu_item::caller_keyboard_shortcut_list,
        &contextmenu_item::caller_media_library_viewer
    };

    const GUID type = ui_selection_manager_v2::get()->get_selection_type( 0 );

    const auto it = ranges::find_if( guids, [type]( const auto pGuid ) { return type == *pGuid; } );
    return ( ranges::end( guids ) != it
                 ? ranges::distance( ranges::begin( guids ), it )
                 : 0 );
}

bool JsFbUtils::IsLibraryEnabled()
{
    return library_manager::get()->is_library_enabled();
}

bool JsFbUtils::IsMainMenuCommandChecked( const std::u8string& command )
{
    t_uint32 status;
    utils::get_mainmenu_command_status_by_name( command, status );

    return ( mainmenu_commands::flag_checked & status
             || mainmenu_commands::flag_radiochecked & status );
}

bool JsFbUtils::IsMetadbInMediaLibrary( JsFbMetadbHandle* handle )
{
    SmpException::ExpectTrue( handle, "handle argument is null" );

    return library_manager::get()->is_item_in_library( handle->GetHandle() );
}

void JsFbUtils::LoadPlaylist()
{
    standard_commands::main_load_playlist();
}

void JsFbUtils::Next()
{
    standard_commands::main_next();
}

void JsFbUtils::Pause()
{
    standard_commands::main_pause();
}

void JsFbUtils::Play()
{
    standard_commands::main_play();
}

void JsFbUtils::PlayOrPause()
{
    standard_commands::main_play_or_pause();
}

void JsFbUtils::Prev()
{
    standard_commands::main_previous();
}

void JsFbUtils::Random()
{
    standard_commands::main_random();
}

bool JsFbUtils::RunContextCommand( const std::u8string& command, uint32_t flags )
{
    metadb_handle_list dummy_list;
    return utils::execute_context_command_by_name( command, dummy_list, flags );
}

bool JsFbUtils::RunContextCommandWithOpt( size_t optArgCount, const std::u8string& command, uint32_t flags )
{
    switch ( optArgCount )
    {
    case 0:
        return RunContextCommand( command, flags );
    case 1:
        return RunContextCommand( command );
    default:
        throw SmpException( fmt::format( "Internal error: invalid number of optional arguments specified: {}", optArgCount ) );
    }
}

bool JsFbUtils::RunContextCommandWithMetadb( const std::u8string& command, JS::HandleValue handle, uint32_t flags )
{
    SmpException::ExpectTrue( handle.isObject(), "handle argument is invalid" );

    JS::RootedObject jsObject( pJsCtx_, &handle.toObject() );

    auto* jsHandle = GetInnerInstancePrivate<JsFbMetadbHandle>( pJsCtx_, jsObject );
    auto* jsHandleList = GetInnerInstancePrivate<JsFbMetadbHandleList>( pJsCtx_, jsObject );
    SmpException::ExpectTrue( jsHandle || jsHandleList, "handle argument is invalid" );

    metadb_handle_list handle_list;
    if ( jsHandleList )
    {
        handle_list = jsHandleList->GetHandleList();
    }
    else
    {
        handle_list.add_item( jsHandle->GetHandle() );
    }

    return utils::execute_context_command_by_name( command, handle_list, flags );
}

bool JsFbUtils::RunContextCommandWithMetadbWithOpt( size_t optArgCount, const std::u8string& command, JS::HandleValue handle, uint32_t flags )
{
    switch ( optArgCount )
    {
    case 0:
        return RunContextCommandWithMetadb( command, handle, flags );
    case 1:
        return RunContextCommandWithMetadb( command, handle );
    default:
        throw SmpException( fmt::format( "Internal error: invalid number of optional arguments specified: {}", optArgCount ) );
    }
}

bool JsFbUtils::RunMainMenuCommand( const std::u8string& command )
{
    return utils::execute_mainmenu_command_by_name( command );
}

void JsFbUtils::SavePlaylist()
{
    standard_commands::main_save_playlist();
}

void JsFbUtils::SetDSPPreset( uint32_t idx )
{
    SmpException::ExpectTrue( static_api_test_t<dsp_config_manager_v2>(), "This method requires foobar2000 v1.4 or later" );

    auto api = dsp_config_manager_v2::get();
    t_size count = api->get_preset_count();

    SmpException::ExpectTrue( idx < count, "Index is out of bounds" );

    api->select_preset( idx );
}

void JsFbUtils::SetOutputDevice( const std::wstring& output, const std::wstring& device )
{
    SmpException::ExpectTrue( static_api_test_t<output_manager_v2>(), "This method requires foobar2000 v1.4 or later" );

    GUID output_id;
    GUID device_id;
    if ( CLSIDFromString( output.c_str(), &output_id ) == NOERROR
         && CLSIDFromString( device.c_str(), &device_id ) == NOERROR )
    {
        output_manager_v2::get()->setCoreConfigDevice( output_id, device_id );
    }
}

void JsFbUtils::ShowConsole()
{
    constexpr GUID guid_main_show_console = { 0x5b652d25, 0xce44, 0x4737, { 0x99, 0xbb, 0xa3, 0xcf, 0x2a, 0xeb, 0x35, 0xcc } };
    standard_commands::run_main( guid_main_show_console );
}

void JsFbUtils::ShowLibrarySearchUI( const std::u8string& query )
{
    library_search_ui::get()->show( query.c_str() );
}

void JsFbUtils::ShowPopupMessage( const std::u8string& msg, const std::u8string& title )
{
    smp::utils::DelayedExecutor::GetInstance().AddTask( [msg, title] {
        popup_message::g_show( msg.c_str(), title.c_str() );
    } );
}

void JsFbUtils::ShowPopupMessageWithOpt( size_t optArgCount, const std::u8string& msg, const std::u8string& title )
{
    switch ( optArgCount )
    {
    case 0:
        return ShowPopupMessage( msg, title );
    case 1:
        return ShowPopupMessage( msg );
    default:
        throw SmpException( fmt::format( "Internal error: invalid number of optional arguments specified: {}", optArgCount ) );
    }
}

void JsFbUtils::ShowPreferences()
{
    standard_commands::main_preferences();
}

void JsFbUtils::Stop()
{
    standard_commands::main_stop();
}

JSObject* JsFbUtils::TitleFormat( const std::u8string& expression )
{
    return JsFbTitleFormat::Constructor( pJsCtx_, expression );
}

void JsFbUtils::VolumeDown()
{
    standard_commands::main_volume_down();
}

void JsFbUtils::VolumeMute()
{
    standard_commands::main_volume_mute();
}

void JsFbUtils::VolumeUp()
{
    standard_commands::main_volume_up();
}

bool JsFbUtils::get_AlwaysOnTop()
{
    return config_object::g_get_data_bool_simple( standard_config_objects::bool_ui_always_on_top, false );
}

std::u8string JsFbUtils::get_ComponentPath()
{
    return smp::get_fb2k_component_path();
}

bool JsFbUtils::get_CursorFollowPlayback()
{
    return config_object::g_get_data_bool_simple( standard_config_objects::bool_cursor_follows_playback, false );
}

std::u8string JsFbUtils::get_FoobarPath()
{
    return smp::get_fb2k_path();
}

bool JsFbUtils::get_IsPaused()
{
    return playback_control::get()->is_paused();
}

bool JsFbUtils::get_IsPlaying()
{
    return playback_control::get()->is_playing();
}

bool JsFbUtils::get_PlaybackFollowCursor()
{
    return config_object::g_get_data_bool_simple( standard_config_objects::bool_playback_follows_cursor, false );
}

double JsFbUtils::get_PlaybackLength()
{
    return playback_control::get()->playback_get_length();
}

double JsFbUtils::get_PlaybackTime()
{
    return playback_control::get()->playback_get_position();
}

std::u8string JsFbUtils::get_ProfilePath()
{
    return smp::get_profile_path();
}

uint32_t JsFbUtils::get_ReplaygainMode()
{
    t_replaygain_config rg;
    replaygain_manager::get()->get_core_settings( rg );
    return rg.m_source_mode;
}

bool JsFbUtils::get_StopAfterCurrent()
{
    return playback_control::get()->get_stop_after_current();
}

float JsFbUtils::get_Volume()
{
    return playback_control::get()->get_volume();
}

void JsFbUtils::put_AlwaysOnTop( bool p )
{
    config_object::g_set_data_bool( standard_config_objects::bool_ui_always_on_top, p );
}

void JsFbUtils::put_CursorFollowPlayback( bool p )
{
    config_object::g_set_data_bool( standard_config_objects::bool_cursor_follows_playback, p );
}

void JsFbUtils::put_PlaybackFollowCursor( bool p )
{
    config_object::g_set_data_bool( standard_config_objects::bool_playback_follows_cursor, p );
}

void JsFbUtils::put_PlaybackTime( double time )
{
    playback_control::get()->playback_seek( time );
}

void JsFbUtils::put_ReplaygainMode( uint32_t p )
{
    // Take care when changing this array:
    // guid indexes are part of SMP API
    const std::array<const GUID*, 4> guids = {
        &standard_commands::guid_main_rg_disable,
        &standard_commands::guid_main_rg_set_track,
        &standard_commands::guid_main_rg_set_album,
        &standard_commands::guid_main_rg_byorder
    };

    SmpException::ExpectTrue( p < guids.size(), "Invalid replay gain mode: {}", p );

    standard_commands::run_main( *guids[p] );
    playback_control_v3::get()->restart();
}

void JsFbUtils::put_StopAfterCurrent( bool p )
{
    playback_control::get()->set_stop_after_current( p );
}

void JsFbUtils::put_Volume( float value )
{
    playback_control::get()->set_volume( value );
}

JsFbUtils::DoDragDropOptions JsFbUtils::ParseDoDragDropOptions( JS::HandleValue options )
{
    DoDragDropOptions parsedoptions;
    if ( !options.isNullOrUndefined() )
    {
        SmpException::ExpectTrue( options.isObject(), "options argument is not an object" );
        JS::RootedObject jsOptions( pJsCtx_, &options.toObject() );

        parsedoptions.useTheming = GetOptionalProperty<bool>( pJsCtx_, jsOptions, "use_theming" ).value_or( true );
        parsedoptions.useAlbumArt = GetOptionalProperty<bool>( pJsCtx_, jsOptions, "use_album_art" ).value_or( true );
        parsedoptions.showText = GetOptionalProperty<bool>( pJsCtx_, jsOptions, "show_text" ).value_or( true );
        auto jsImage = GetOptionalProperty<JsGdiBitmap*>( pJsCtx_, jsOptions, "custom_image" ).value_or( nullptr );
        if ( jsImage )
        {
            parsedoptions.pCustomImage = jsImage->GdiBitmap();
        }
    }

    return parsedoptions;
}

} // namespace mozjs
