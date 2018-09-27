/*
 * !!! Do NOT include this whole file !!!
 *
 *  Only include callbacks that you need   
 */

function on_always_on_top_changed(state) {}
// Called when "Always On Top" state changes from using the menu, Alt + A, fb.AlwaysOnTop, etc
// state: boolean.

function on_char(code) {}
// code: UTF16 encoded char.
// In order to use this callback, use window.DlgCode(DLGC_WANTCHARS);
// See flags.txt > DLGC_WANTCHARS.

function on_colours_changed() {}
// Called when colours are changed via default UI/columns UI preferences.
// Use window.GetColourCUI()/window.GetColourDUI() to get new colours.

function on_cursor_follow_playback_changed(state) {}
// Called when "cursor follow playback" state is changed.
// state: boolean. Reflects current "cursor follow playback" value.

function on_drag_drop(action, x, y, mask) {}
function on_drag_enter(action, x, y, mask) {}
function on_drag_leave() {}
function on_drag_over(action, x, y, mask) {}
// See fb.DoDragDrop documentation and samples/basic/DragnDrop.txt

function on_dsp_preset_changed() {}
// This callback is only available in foobar2000 v1.4 and later.
// Called when DSP preset changes.
// Does not get called when presets are added or removed.

function on_focus(is_focused) {}
// Called when the panel gets/loses focus.
// is_focused: boolean.

function on_font_changed() {}
// Called when fonts are changed via default UI/columns UI preferences
// Retrieve fonts using window.GetFontDUI()/window.GetFontCUI()

function on_get_album_art_done(handle, art_id, image, image_path) {}
/*
Called when thread created by utils.GetAlbumArtAsync() is done.
image: IGdiBitmap object or null on failure.
image_path: path to image file (or music file if image is embedded).
*/ 

function on_item_focus_change(playlistIndex, from, to) {}
// Called when playlist focus has been changed.

function on_item_played(handle) {}
// Called when at least one minute of the track has been played or the track has reached
// its end after at least 1/3 of it has been played through.

function on_key_down(vkey) {}
function on_key_up(vkey) {}
/*
Both keyboard related callbacks require "Grab focus" enabled in the Configuration window.
in order to use arrow keys, use window.DlgCode(DLGC_WANTARROWS);
See flags.txt > DLGC_WANTARROWS.
vkey: Virtual Key Code, refer to: http://msdn.microsoft.com/en-us/library/ms927178.aspx

on_key_down only: Keyboard shortcuts defined in the main preferences are always executed first
and are not passed to the callback.
*/

function on_library_items_added(handle_list) {}
function on_library_items_changed(handle_list) {}
function on_library_items_removed(handle_list) {}
// handle_list: affected items.

function on_load_image_done(cookie, image, image_path) {}
/*
Called when thread created by gdi.LoadImageAsync() is done.
cookie: the return value from the gdi.LoadImageAsync call.
image: IGdiBitmap object or null on failure (invalid path/not an image).
image_path: the path that was originally supplied to gdi.LoadImageAsync.
*/ 

function on_main_menu(index) {}
/*
On the main menu>File>JScript Panel, there are 10 menu items and whichever number
is selected is sent as the "index" to this callback. Being main menu items now means you
can bind them to global keyboard shortcuts, standard toolbar buttons, panel stack splitter
buttons, etc. Remember to think carefully about where you use this code as you probably only
want it to run once and so don't include it in common files and scripts where you might have
multiple instances. Also, you should avoid sharing scripts containing this code so as not
to conflict with what other users may already be using.
Example:

function on_main_menu(index) {
	switch (index) {
	case 1: // triggered when File>JScript Panel>1 is run
		do_something();
		break;
	case 2: // triggered when File>JScript Panel>2 is run
		do_something_else();
		break;
	}
}
*/

function on_metadb_changed(handle_list, fromhook) {}
/*
Called when metadb contents change.
handle_list: affected items.
fromhook: boolean, true if notification is not from tag update but a component that provides
tag-like data from a database. foo_playcount and the "RefreshStats" handle/handle list
methods built in to JScript Panel are just 2 examples.
*/

function on_mouse_lbtn_dblclk(x, y, mask) {}
function on_mouse_lbtn_down(x, y, mask) {}
function on_mouse_lbtn_up(x, y, mask) {}
function on_mouse_leave() {}
function on_mouse_mbtn_dblclk(x, y, mask) {}
function on_mouse_mbtn_down(x, y, mask) {}
function on_mouse_mbtn_up(x, y, mask) {}
function on_mouse_move(x, y, mask) {}
function on_mouse_rbtn_dblclk(x, y, mask) {}
function on_mouse_rbtn_down(x, y, mask) {}
// See flags.txt > Mask for mouse callbacks

function on_mouse_rbtn_up(x, y, mask) {}
// You must return true if you want to suppress the default context menu.
// Hold left shift + left windows key to bypass user code and open default context menu.

function on_mouse_wheel(step) {}
// Scroll up/down

function on_mouse_wheel_h(step) {}
// Scroll left/right

function on_notify_data(name, info) {}
// Called in other panels after window.NotifyOthers(name, info) is executed.
// !!! Beware !!! 
// 1. Data from `info` argument is only accessible inside `on_notify_data` callback:
//    if stored and accessed outside of the callback it will throw JS error.
//    This also applies to the data produced from that `info`: e.g. stroring `info.Path` directly (if `info` is IFbMetadbHandle).
// 2. If you want to store the data from `info` you have to perform a deep copy:
//    e.g. `String(info)` for string or `JSON.parse(JSON.stringify(info))` for serializeable objects.
// 3. `info` argument is shared between panels, so it should NOT be modified in any way.

function on_output_device_changed() {}
// This callback is only available in foobar2000 v1.4 and later.
// Called when output device changes. Use fb.GetOutputDevices to retrieve settings.

function on_paint(gr) {}
// Called when window is ready to draw.
// See js_doc\Interfaces.js for all the methods used with gr.

function on_playback_follow_cursor_changed(state) {}
// Called when "playback follow cursor" state is changed
// state: boolean, reflects current "playback follow cursor" value.

function on_playback_dynamic_info() {}
// dynamic info (VBR bitrate etc) change.

function on_playback_dynamic_info_track() {}
// Per-track dynamic info (stream track titles etc) change.
// Happens less often than on_playback_dynamic_info().

function on_playback_edited(handle) {}
/*
Called when currently playing file gets edited. It's also
called by components that provide tag-like data such
as foo_playcount.
*/

function on_playback_new_track(handle) {}

function on_playback_order_changed(new_order_index) {}
/*
Called when playback order is changed.
new_order_index:
0 Default
1 Repeat (Playlist)
2 Repeat (Track)
3 Random
4 Shuffle (tracks)
5 Shuffle (albums)
6 Shuffle (folders)
*/

function on_playback_pause(state) {}
// state: boolean, true when paused, false when unpaused.

function on_playback_queue_changed(origin) {}
// origin: 0 user_added, 1 user_removed, 2 playback advance

function on_playback_seek(time) {}
// time: float value, in seconds.

function on_playback_starting(cmd, is_paused) {}
// cmd: 0 default, 1 play, 2 next, 3 prev, 4 settrack, 5 rand, 6 resume.
// is_paused: boolean.

function on_playback_stop(reason) {}
// reason: 0 user, 1 eof, 2 starting_another.

function on_playback_time(time) {}
// Called every second, for time display.
// time - float value, in sec.

function on_playlist_item_ensure_visible(playlistIndex, playlistItemIndex) {}

function on_playlist_items_added(playlistIndex) {}
function on_playlist_items_removed(playlistIndex, new_count) {}
function on_playlist_items_reordered(playlistIndex) {}

function on_playlist_items_selection_change() {}
// Workaround for some 3rd party playlist viewers not working with on_selection_changed().

function on_playlist_stop_after_current_changed(state) {}
// Called when "stop after current" is enabled/disabled.
// state: boolean, reflects "stop after current" value, either true or false.

function on_playlist_switch() {}

function on_playlists_changed() {}
/*
Called when:
-playlists are added/removed/reordered/renamed.
-a playlist's lock status changes through the use of components such as foo_utils or foo_playlist_attributes.
*/

function on_replaygain_mode_changed(new_mode) {}
/*
This callback is only available in foobar2000 v1.4 and later.
new_mode:
0 None
1 Track
2 Album
3 Track/Album by Playback Order
*/

function on_script_unload() {}
// Called when: 
// - Panel script is reloaded via context menu > Reload.
// - Panel script is changed via panel menu > Configure.
// - fb2k is exiting normally.
// Not called when:
// - Script fails with error.
// - fb2k closed externally (e.g. killed with process manager).
// - fb2k fails with exception.

function on_selection_changed() {}
// Called when selection changes based on "File>Preferences>Display>Selection viewers".

function on_size(width, height) {}
// Called when panel is resized.
// width and height arguments have same values as window.Width and window.Height
// IMPORTANT: DO NOT call window.Repaint()

function on_volume_change(val) {}
// val: volume level in dB. minimum is -100. maximum is 0.
