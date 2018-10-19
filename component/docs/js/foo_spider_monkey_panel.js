/**
 * @typedef {number} float
 */

/**
 * Evaluates the script in file.
 * Similar to eval({@link utils.ReadTextFile}), but provides better error reporting
 *
 * @param {string} path Path to JavaScript file
 */
function include(path) {}

/**
 * Load ActiveX object.
 *
 * @constructor
 * @param {string} name
 *
 * @example
 * var xmlhttp = new ActiveXObject('Microsoft.XMLHTTP');
 */
function ActiveXObject(name) {

    /**
     * Emulates COM's weird behaviour of property accessors.
     *
     * @param {number|string} prop_name Name of the property or it's numeric index
     *
     * @return {*}
     *
     * @example
     some_activex.ActiveX_Get('property_name', 'additional_info').DoSmth();
     * // in COM:
     * // some_activex.Item('property_name', 'additional_info').DoSmth();
     */
    this.ActiveX_Get = function (prop_name) { };

    /**
     * Emulates COM's weird behaviour of property accessors.
     *
     * @param {number|string} prop_name Name of the property or it's numeric index
     *
     * @example
     * some_activex.ActiveX_Set('property_name', 'new_value', 'additional_info');
     * // in COM:
     * // some_activex.Item('property_name', 'additional_info') = "new_value";
     */
    this.ActiveX_Set = function (prop_name) { };
}

/**
 * See {@link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Microsoft_JavaScript_extensions/Enumerator}.
 *
 * @constructor
 * @param {ActiveXObject} active_x_object Any ActiveX collection object.
 */
function Enumerator(active_x_object) {

    /**
     * Returns a Boolean value indicating if the enumerator is at the end of the collection.
     *
     * @return {boolean}
     */
    this.atEnd = function () {};

    /**
     * Returns the current item in the collection.
     *
     * @return {*}
     */
    this.item = function () {};

    /**
     * Resets the current item in the collection to the first item.
     *
     * @method
     */
    this.moveFirst = function () {};

    /**
     * Moves the current item to the next item in the collection.
     *
     * @method
     */
    this.moveNext = function () {};
}

/**
 * @namespace
 */
let console = {
    /**
     * See {@link https://developer.mozilla.org/en-US/docs/Web/API/Console/log}
     *
     * @param {...*} var_args
     */
    log: function (var_args) {}, // (void)
};

/**
 * Functions for controlling foobar2000 and accessing it's data.
 *
 * @namespace
 */
let fb = {
    /**
     * @type {boolean}
     *
     * @example
     * fb.AlwaysOnTop = !fb.AlwaysOnTop; // Toggles the current value.
     */
    AlwaysOnTop: undefined, //(boolean) (read, write)

    /**
     * @type {string}
     * @readonly
     *
     * @example
     * console.log(fb.ComponentPath); // C:\Users\User\AppData\Roaming\foobar2000\user-components\foo_spider_monkey_panel\
     */
    ComponentPath: undefined, // (string) (read)

    /** @type {boolean} */
    CursorFollowPlayback: undefined, // (boolean) (read, write)

    /**
     * @type {string}
     * @readonly
     */
    FoobarPath: undefined, // (string) (read)

    /**
     * @type {boolean}
     * @readonly
     */
    IsPaused: undefined, // (boolean) (read)

    /**
     * @type {boolean}
     * @readonly
     */
    IsPlaying: undefined, // (boolean) (read)

    /** @type {boolean} */
    PlaybackFollowCursor: undefined, // (boolean) (read, write)

    /**
     * @type {float}
     * @readonly
     *
     * @example
     * console.log(fb.PlaybackLength); // 322.843414966166
     *
     * @example
     * console.log(Math.round(fb.PlaybackLength)); // 323
     */
    PlaybackLength: undefined, // (double) (read)

    /**
     * @type {float}
     *
     * @example
     * fb.PlaybackTime = 60; // Jumps to the 1 minute mark.
     */
    PlaybackTime: undefined, // (double) (read, write)

    /**
     * @type {string}
     * @readonly
     */
    ProfilePath: undefined, // (string) (read)

    /**
     * 0 - None<br>
     * 1 - Track<br>
     * 2 - Album<br>
     * 3 - Track/Album by Playback Order (only available in foobar2000 v1.3.8 and later)
     *
     * @type {number}
     */
    ReplaygainMode: undefined, // (uint) (read, write)

    /**
     * @type {boolean}
     *
     * @example
     * fb.StopAfterCurrent = !fb.StopAfterCurrent; // Toggles the current value.
     */
    StopAfterCurrent: undefined, // (boolean) (read, write)

    /**
     * @type {float}
     *
     * @example
     * fb.Volume = 0; // Sets the volume to max. -100 is the minimum.
     */
    Volume: undefined, // (float) (read, write),

    /**
     * @return {FbUiSelectionHolder}
     */
    AcquireUiSelectionHolder: function () {}, // (FbUiSelectionHolder)

    /** @method */
    AddDirectory: function () {}, // (void)

    /** @method */
    AddFiles: function () {}, // (void)

    /**
     * Checks Clipboard contents are handles or a file selection from Windows Explorer. Use in conjunction
     * with fb.GetClipboardContents().
     *
     * @return {boolean}
     */
    CheckClipboardContents: function () {}, // (boolean)

    /**
     * Clears active playlist. If you wish to clear a specific playlist, use plman.ClearPlaylist(playlistIndex).
     */
    ClearPlaylist: function () {}, // (void)

    /**
     * Note: items can then be pasted in other playlist viewers or in Windows Explorer as files.
     *
     * @param {FbMetadbHandleList} handle_list
     * @return {boolean}
     *
     * @example <caption>Copy playlist items</caption>
     * var handle_list = plman.GetPlaylistSelectedItems(plman.ActivePlaylist);
     * fb.CopyHandleListToClipboard(handle_list);
     *
     * @example <caption>cut playlist items</caption>
     * var ap = plman.ActivePlaylist;
     * if (!plman.IsPlaylistLocked(ap)) {
     *    var handle_list = plman.GetPlaylistSelectedItems(ap);
     *    if (fb.CopyHandleListToClipboard(handle_list)) {
     *        plman.UndoBackup(ap);
     *        plman.RemovePlaylistSelection(ap);
     *    }
     *  }
     */
    CopyHandleListToClipboard: function (handle_list) {}, // (boolean)

    /**
     * @return {ContextMenuManager}
     *
     * @example
     * // See samples\basic\MainMenuManager All-In-One, samples\basic\Menu Sample.txt
     */
    CreateContextMenuManager: function () {}, // (ContextMenuManager)

    /**
     * Returns an empty handle list.
     *
     * @return {FbMetadbHandleList}
     */
    CreateHandleList: function () {}, // (FbMetadbHandleList)

    /**
     * @return {MainMenuManager}
     *
     * @example
     * // See samples\basic\MainMenuManager All-In-One, samples\basic\Menu Sample.txt
     */
    CreateMainMenuManager: function () {}, // (MainMenuManager)

    /**
     * @param {string=} [name=''] Will be shown in console when used with {@link FbProfiler#Print} method.
     * @return {FbProfiler}
     */
    CreateProfiler: function (name) {}, // (FbProfiler) [name]

    /**
     * Invokes drag-n-drop operation (see {@link https://msdn.microsoft.com/en-us/library/windows/desktop/ms678486.aspx}).<br>
     * <br>
     * Quick tips:<br>
     * - If you need only to drag from your panel with copy (i.e. without physically moving them):
     *      use only fb.DoDragDrop(handles, DROPEFFECT_COPY | DROPEFFECT_LINK ).<br>
     * - If you need only to receive drop to your panel with copy:
     *      handle on_drop_* callbacks, while setting action.effect argument to (DROPEFFECT_COPY | DROPEFFECT_LINK ).<br>
     * <br>
     * Full drag-n-drop interface description:<br>
     * - Drag-n-drop interface is based on Microsoft IDropSource and IDropTarget interfaces, so a lot of info (including examples) could be gathered from MSDN (IDropSource, IDropTarget, DoDragDrop, DROPEFFECT).<br>
     * - Drag operation is started with DoDragDrop (whether it is called by your panel, or externally) with okEffects argument supplied.<br>
     * - DoDragDrop blocks code execution until the drag operation is finished (callbacks will be called properly though). It returns effect from Action.Effect from on_drag_drop after completion.<br>
     * - (Spider Monkey Panel specific) Drag operation is canceled when any mouse button is pressed.<br>
     * - (Spider Monkey Panel specific) All mouse callbacks are suppressed during drag operation (including on_mouse_lbtn_up, but excluding on_mouse_mbtn_up and on_mouse_rbtn_up).<br>
     * - Every drag callback receives Action argument. Action.Effect contains okEffects from DoDragDrop call. Action.Effect should be changed to the desired effect in the callback.
     *   If the returned Action.Effect was not in okEffects or is equal to DROPEFFECT_NONE (=== 0), then drop will be denied:
     *   cursor icon will be changed, on_drag_drop won't be called after releasing lmbtn, on_drag_leave will be called instead.<br>
     * - DROPEFFECT_LINK should be used as fallback in case effect argument does not have DROPEFFECT_COPY (===1), since some external drops only allow DROPEFFECT_LINK effect.<br>
     * - Changing effect on key modifiers is nice (to be in line with native Windows behaviour): see example here: https://github.com/TheQwertiest/CaTRoX_QWR/blob/ab1aa4c7fc19e08d3ccff84d5959779ba46bf704/theme/Scripts/Panel_Playlist.js#L2512.<br>
     *
     * @param {FbMetadbHandleList} handle_list
     * @param {number} effect Allowed effects.
     * @return {number} Effect that was returned in on_drag_drop.
     *
     * @example
     * // See samples/basic/DragnDrop.txt
     */
    DoDragDrop: function (handle_list, effect) {}, // (uint),

    /** @method */
    Exit: function () {}, // (void)

    /**
     * Note: clipboard contents can be handles copied to the clipboard in other components,
     * from {@link fb.CopyHandleListToClipboard} or a file selection, from Windows Explorer and etc.
     *
     * @param {number} window_id
     * @return {FbMetadbHandleList}
     *
     * @example
     * function on_mouse_rbtn_up(x, y) {
     *    var ap = plman.ActivePlaylist;
     *    var menu = window.CreatePopupMenu();
     *    menu.AppendMenuItem(!plman.IsPlaylistLocked(ap) && fb.CheckClipboardContents() ? MF_STRING : MF_GRAYED, 1, "Paste"); // see Flags.js for MF_* definitions
     *    var idx = menu.TrackPopupMenu(x, y);
     *    if (idx == 1) {
     *        var handle_list  = fb.GetClipboardContents(window.ID);
     *        plman.InsertPlaylistItems(ap, plman.PlaylistItemCount(ap), handle_list );
     *    }
     *    return true;
     * }
     */
    GetClipboardContents: function (window_id) {}, // (FbMetadbHandleList)

    /**
     * @param {boolean=} [force=true] When true, it will use the first item of the active playlist if it is unable to get the focus item.
     * @return {FbMetadbHandle}
     */
    GetFocusItem: function (force) {}, // (FbMetadbHandle) [force]

    /**
     * Returns all Media Library items as a handle list.
     *
     * @return {FbMetadbHandleList}
     */
    GetLibraryItems: function () {}, // (FbMetadbHandleList)

    /**
     * Note: do not use this while looping a handle list. Use {@link FbMetadbHandleList#GetLibraryRelativePaths} instead. <br>
     * <br>
     * Returns an empty string when used on track not in Media Library
     *
     * @param {FbMetadbHandle} handle
     * @return {string}
     *
     * @example
     * // The foobar2000 Media Library is configured to watch "D:\Music" and the
     * // path of the now playing item is "D:\Music\Albums\Artist\Some Album\Some Song.flac"
     * var handle = fb.GetNowPlaying();
     * console.log(fb.GetLibraryRelativePath(handle)); // Albums\Artist\Some Album\Some Song.flac*
     */
    GetLibraryRelativePath: function (handle) {}, // (string)

    /**
     * Get handle of now playing item.
     *
     * @return {?FbMetadbHandle} null, if nothing is being played.
     */
    GetNowPlaying: function () {}, // (FbMetadbHandle)

    /**
     * foobar2000 v1.4 and above only. Throws a script error on v1.3. * <br>
     * Returns a JSON array in string form so you need to use JSON.parse() on the result.
     *
     * @return {string}
     *
     * @example
     * var str = fb.GetOutputDevices();
     * var arr = JSON.parse(str);
     * console.log(JSON.stringify(arr, null, 4));
     * // [
     * //     {
     * //         "active": false,
     * //         "device_id": "{5243F9AD-C84F-4723-8194-0788FC021BCC}",
     * //         "name": "Null Output",
     * //         "output_id": "{EEEB07DE-C2C8-44C2-985C-C85856D96DA1}"
     * //     },
     * //     {
     * //         "active": true,
     * //         "device_id": "{00000000-0000-0000-0000-000000000000}",
     * //         "name": "Primary Sound Driver",
     * //         "output_id": "{D41D2423-FBB0-4635-B233-7054F79814AB}"
     * //     },
     * //     {
     * //         "active": false,
     * //         "device_id": "{1C4EC038-97DB-48E7-9C9A-05FDED46847B}",
     * //         "name": "Speakers (Sound Blaster Z)",
     * //         "output_id": "{D41D2423-FBB0-4635-B233-7054F79814AB}"
     * //     },
     * //     {
     * //         "active": false,
     * //         "device_id": "{41B86272-3D6C-4A5A-8907-4FE7EBE39E7E}",
     * //         "name": "SPDIF-Out (Sound Blaster Z)",
     * //         "output_id": "{D41D2423-FBB0-4635-B233-7054F79814AB}"
     * //     },
     * //     {
     * //         "active": false,
     * //         "device_id": "{9CDC0FAE-2870-4AFA-8287-E86099D69076}",
     * //         "name": "3 - BenQ BL3200 (AMD High Definition Audio Device)",
     * //         "output_id": "{D41D2423-FBB0-4635-B233-7054F79814AB}"
     * //     }
     * // ]
     * // As you can see, only one of the items in the array has "active"
     * // set to true so that is the device you'd want to display the name of
     * // or mark as selected in a menu.
     */
    GetOutputDevices: function () {}, // (string)

    /**
     * Note: use try/catch to handle invalid queries. An empty handle list will be returned if the query
     * is valid but there are no results.
     *
     * @param {FbMetadbHandleList} handle_list
     * @param {string} query
     * @return {FbMetadbHandleList} Unsorted results.
     *
     * @example
     * var a = fb.GetQueryItems(plman.GetPlaylistItems(plman.ActivePlaylist), "rating IS 5");
     *
     * @example
     * var b = fb.GetQueryItems(fb.GetLibraryItems(), "rating IS 5");
     */
    GetQueryItems: function (handle_list, query) {}, // (FbMetadbHandleList)

    /**
     * Gets now playing or selected item. What you get will depend on
     * "File>Preferences>Display>Selection viewers".
     *
     * @return {?FbMetadbHandle}
     */
    GetSelection: function () {}, // (FbMetadbHandle)

    /**
     * Works like GetSelection(), but returns a handle list.
     * Always returns a valid handle list instance instead of null.
     *
     * @param {number=} [flags=0] 1 - no now playing
     * @return {FbMetadbHandleList}
     */
    GetSelections: function (flags) {}, // (FbMetadbHandleList) //[flags]

    /**
     * Retrieves what the selection type is.
     *
     * @return {number}
     *     0 - undefined (no item)<br>
     *     1 - active_playlist_selection<br>
     *     2 - caller_active_playlist<br>
     *     3 - playlist_manager<br>
     *     4 - now_playing<br>
     *     5 - keyboard_shortcut_list<br>
     *     6 - media_library_viewer
     */
    GetSelectionType: function () {}, // (uint)

    /**
     * @return {boolean}
     */
    IsLibraryEnabled: function () { }, // (boolean)

    /**
     * Performance note: don't use in `on_paint`.
     *
     * @param {string} command Path to main menu item
     * @return {boolean} true, if the item is checked.
     *
     * @example
     * fb.RunMainMenuCommand("Playback/Scrobble Tracks"); // available with foo_scrobble
     */
    IsMainMenuCommandChecked: function (command) {}, // (boolean)

    /**
     * @param {FbMetadbHandle} handle
     * @return {boolean}
     *
     * @example
     * var np = fb.GetNowplaying();
     * console.log(fb.IsMetadbInMediaLibrary(np)); // If false, playing track is not in Media Library.
     */
    IsMetadbInMediaLibrary: function (handle) {}, // (boolean)

    /** @method */
    LoadPlaylist: function () {}, // (void)

    /** @method */
    Next: function () {}, // (void)

    /** @method */
    Pause: function () {}, // (void)

    /** @method */
    Play: function () {}, // (void)

    /** @method */
    PlayOrPause: function () {}, // (void)

    /** @method */
    Prev: function () {}, // (void)

    /** @method */
    Random: function () {}, // (void)

    /**
     * For now playing file only.
     *
     * @param {string} command
     * @param {number=} [flags=0]
     *     0 - default (depends on whether SHIFT key is pressed, flag_view_reduced or flag_view_full is selected)<br>
     *     4 - flag_view_reduced<br>
     *     8 - flag_view_full. This can be useful if you need to run context commands the user may have hidden
     *         using File>Preferences>Display>Context Menu<br>
     * @return {boolean}
     *
     * @example
     * fb.RunContextCommand("Properties");
     */
    RunContextCommand: function (command, flags) {}, // (boolean) [, flags]

    /**
     * @param {string} command
     * @param {FbMetadbHandle|FbMetadbHandleList} handle_or_handle_list Handles on which to apply context menu
     * @param {number=} flags Same flags as {@link fb.RunContextCommand}
     * @return {boolean}
     */
    RunContextCommandWithMetadb: function (command, handle_or_handle_list, flags) {}, // (boolean) [, flags]

    /**
     * @param {string} command
     * @return {boolean}
     *
     * @example
     * fb.RunMainMenuCommand("File/Add Location...");
     */
    RunMainMenuCommand: function (command) {}, // (boolean)

    /** @method */
    SavePlaylist: function () {}, // (void)

    /**
     * foobar2000 v1.4 and above only. Throws a script error on v1.3.<br>
     * See {@link fb.GetOutputDevices}.
     *
     * @param {string} output
     * @param {string} device
     *
     * @example
     * // To actually change device, you'll need the device_id and output_id
     * // and use them with fb.SetOutputDevice.
     * var str = fb.GetOutputDevices();
     * var arr = JSON.parse(str);
     * // Assuming same list from above, switch output to the last device.
     * fb.SetOutputDevice(arr[4].output_id, arr[4].device_id);
     */
    SetOutputDevice: function (output, device) {}, // (void)

    /** @method */
    ShowConsole: function () {}, // (void)

    /**
     * Opens the Library>Search window populated with the query you set.
     *
     * @param {string} query
     */
    ShowLibrarySearchUI: function (query) {}, // (void)

    /**
     * @param {string} message
     * @param {string=} [title='Spider Monkey Panel']
     */
    ShowPopupMessage: function (message, title) {}, // (void) [, title]

    /** @method */
    ShowPreferences: function () {}, // (void)

    /** @method */
    Stop: function () {}, // (void)

    /**
     * Performance note: try caching `TitleFormat` objects -
     * if you use the same query frequently, just store the result of `TitleFormat` somewhere
     * and use it, instead of creating it every time.
     *
     * @param {string} expression
     * @return {FbTitleFormat}
     */
    TitleFormat: function (expression) {}, // (FbTitleFormat)

    /** @method */
    VolumeDown: function () {}, // (void)

    /** @method */
    VolumeMute: function () {}, // (void)

    /** @method */
    VolumeUp: function () {}, // (void)
};

/**
 * Functions for working with graphics. Most of them are wrappers for Gdi and GdiPlus methods.
 *
 * @namespace
 */
let gdi = {

    /**
     * @param {number} w
     * @param {number} h
     * @return {GdiBitmap}
     */
    CreateImage: function (w, h) {}, // (GdiBitmap)

    /**
     * Performance note: avoid using inside `on_paint`.
     *
     * @param {string} name
     * @param {number} size_px See Helper.js > Point2Pixel function for conversions
     * @param {number=} [style=0] See Flags.js > FontStyle
     * @return {?GdiFont} null, if font is not present.
     */
    Font: function (name, size_px, style) {}, // (GdiFont) [, style]

    /**
     * Load image from file.<br>
     * <br>
     * Performance note: consider using {@link gdi.LoadImageAsync}, if there are a lot of images to load
     * or if the image is big.
     *
     * @param {string} path
     * @return {?GdiBitmap} null, if path doesn't exist or image fails to load.
     *
     * @example
     * var img = gdi.Image('e:\\images folder\\my_image.png');
     */
    Image: function (path) {}, // (GdiBitmap)

    /**
     * Load image from file asynchronously.
     *
     * @param {number} window_id see {@link window.ID}
     * @param {string} path
     * @return {number} a unique id, which is used in `on_load_image_done`.
     */
    LoadImageAsync: function (window_id, path) {}, // (uint)
};

/**
 * Functions for managing foobar2000 playlists.
 *
 * @namespace
 */
let plman = {

    /**
     * @type {number}
     *
     * @example
     * console.log(plman.ActivePlaylist); // Returns -1 if there is no active playlist.
     *
     * @example
     * plman.ActivePlaylist = 1; // Switches to 2nd playlist.
     */
    ActivePlaylist: undefined, // (int) (read, write)

    /**
     * 0 - Default<br>
     * 1 - Repeat (Playlist)<br>
     * 2 - Repeat (Track)<br>
     * 3 - Random<br>
     * 4 - Shuffle (tracks)<br>
     * 5 - Shuffle (albums)<br>
     * 6 - Shuffle (folders)
     *
     * @type {number}
     */
    PlaybackOrder: undefined, // (uint) (read, write)


    /**
     * @type {number}
     *
     * @example
     * console.log(plman.PlayingPlaylist); // Returns -1 if there is no playing playlist
     */
    PlayingPlaylist: undefined, // (int) (read, write)

    /**
     * @type {number}
     * @readonly
     */
    PlaylistCount: undefined, // (uint) (read)

    /**
     * A Recycle Bin for playlists.
     * 
     * @type {FbPlaylistRecycler}
     * @readonly
     */
    PlaylistRecycler: undefined, // (FbPlaylistRecycler) (read)

    /**
     * This operation is asynchronous and may take some time to complete if it's a large array.
     *
     * @param {number} playlistIndex
     * @param {Array<string>} paths An array of files/URLs
     * @param {boolean=} [select=false]
     *        If true, the active playlist will be set to the playlistIndex, the items will
     *        be selected and focus will be set to the first new item.
     *
     * @example
     * plman.AddLocations(plman.ActivePlaylist, ["e:\\1.mp3"]);
     * // This operation is asynchronous, so any code in your script directly
     * // after this line will run immediately without waiting for the job to finish.
     */
    AddLocations: function (playlistIndex, paths, select) {}, // (void) [, select]

    /**
     * @param {number} playlistIndex
     *
     * @example
     * plman.ClearPlaylist(plman.PlayingPlaylist);
     */
    ClearPlaylist: function (playlistIndex) {}, // (void)

    /**
     * @param {number} playlistIndex
     *
     * @example
     * plman.ClearPlaylistSelection(plman.ActivePlaylist);
     */
    ClearPlaylistSelection: function (playlistIndex) {}, // (void)

    /**
     * @param {number} playlistIndex
     * @param {string} name A name for the new Autplaylist
     * @param {string} query A valid query
     * @param {string=} [sort=''] Title formatting pattern.
     * @param {number=} [flags=0] Use 1 to force sort
     * @return {number} -1 on failure
     */
    CreateAutoPlaylist: function (playlistIndex, name, query, sort, flags) {}, // (int) [, sort][, flags]

    /**
     * @param {number} playlistIndex
     * @param {string} name
     * @return {number} -1 on failure.
     *
     * @example
     * // Creates a new playlist named "New playlist", which is put at the beginning of the current playlists.
     * plman.CreatePlaylist(0, '');
     *
     * @example
     * // Create a new playlist named "my favourites", which is put at the end.
     * plman.CreatePlaylist(plman.PlaylistCount, 'my favourites');
     */
    CreatePlaylist: function (playlistIndex, name) {}, // (int)

    /**
     * Note: the duplicated playlist gets inserted directly after the source playlistIndex.<br>
     * It only duplicates playlist content, not the properties of the playlist (e.g. Autoplaylist).
     *
     * @param {number} playlistIndex
     * @param {string} name A name for the new playlist. If name is "", the name of the source playlist is used.
     * @return {number}
     */
    DuplicatePlaylist: function (playlistIndex, name) {}, // (int)

    /**
     * @param {number} playlistIndex
     * @param {number} playlistItemIndex
     */
    EnsurePlaylistItemVisible: function (playlistIndex, playlistItemIndex) {}, // (void)

    /**
     * Starts playback by executing default doubleclick/enter action unless overridden by a lock to do something else.
     *
     * @param {number} playlistIndex
     * @param {number} playlistItemIndex
     * @return {boolean} -1 on failure.
     */
    ExecutePlaylistDefaultAction: function (playlistIndex, playlistItemIndex) {}, // (boolean)

    /**
     * Returns playlistIndex of named playlist or creates new one if not found.<br>
     * If a new playlist is created, the playlistIndex of that will be returned.
     *
     * @param {string} name
     * @param {boolean} unlocked If true, locked playlists are ignored when looking for existing playlists.
     *                           If false, the playlistIndex of any playlist with the matching name will be returned.
     * @return {number}
     */
    FindOrCreatePlaylist: function (name, unlocked) {}, // (int)

    /**
     * @param {string} name Case insensitive.
     * @return {number} playlistIndex or -1 on failure.
     */
    FindPlaylist: function (name) {}, // (int)

    /**
     * Retrieves playlist position of currently playing item.<br>
     * On failure, the property {@link FbPlayingItemLocation#IsValid} will be set to false.
     *
     * @return {FbPlayingItemLocation}
     */
    GetPlayingItemLocation: function () {}, // (FbPlayingItemLocation)

    /**
     * @param {number} playlistIndex
     * @return {number} Returns -1 if nothing is selected
     *
     * @example
     * var focus_item_index = plman.GetPlaylistFocusItemIndex(plman.ActivePlaylist); // 0 would be the first item
     */
    GetPlaylistFocusItemIndex: function (playlistIndex) {}, // (int)

    /**
     * @param {number} playlistIndex
     * @return {FbMetadbHandleList}
     *
     * @example
     * var handle_list = plman.GetPlaylistItems(plman.PlayingPlaylist);
     */
    GetPlaylistItems: function (playlistIndex) {}, // (FbMetadbHandleList)

    /**
     * @param {number} playlistIndex
     * @return {string}
     *
     * @example
     * console.log(plman.GetPlaylistName(plman.ActivePlaylist));
     */
    GetPlaylistName: function (playlistIndex) {}, // (string)

    /**
     * @param {number} playlistIndex
     * @return {FbMetadbHandleList}
     *
     * @example
     * var selected_items = plman.GetPlaylistSelectedItems(plman.ActivePlaylist);
     */
    GetPlaylistSelectedItems: function (playlistIndex) {}, // (FbMetadbHandleList)

    /**
     * @param {number} playlistIndex
     * @param {number} base Position in playlist
     * @param {FbMetadbHandleList} handle_list Items to insert
     * @param {boolean=} [select=false]
     * @return {number}
     *
     * @example <caption>Add all library tracks to beginning of playlist.</caption>
     * var ap = plman.ActivePlaylist;
     * plman.InsertPlaylistItems(ap, 0, fb.GetLibraryItems());
     *
     * @example <caption>Add all library tracks to end of playlist.</caption>
     * var ap = plman.ActivePlaylist;
     * plman.InsertPlaylistItems(ap, plman.PlaylistItemCount(ap), fb.GetLibraryItems());
     */
    InsertPlaylistItems: function (playlistIndex, base, handle_list, select) {}, // (int) [, select]

    /**
     * Same as {@link plman.InsertPlaylistItems} except any duplicates contained in handle_list are removed.
     *
     * @param {number} playlistIndex
     * @param {number} base
     * @param {FbMetadbHandleList} handle_list
     * @param {boolean=} [select=false]
     * @return {number}
     */
    InsertPlaylistItemsFilter: function (playlistIndex, base, handle_list, select) {}, // (int) select = false

    /**
     * @param {number} playlistIndex
     * @return {boolean}
     */
    IsAutoPlaylist: function (playlistIndex) {}, // (boolean)

    /**
     * @param {number} playlistIndex
     * @param {number} playlistItemIndex
     * @return {boolean}
     */
    IsPlaylistItemSelected: function (playlistIndex, playlistItemIndex) {}, // (boolean)

    /**
     * Note: this returns true, if the playlist is an autoplaylist. To determine if a playlist is not an autoplaylist,
     * but locked with something like `foo_utils` or `foo_playlist_attributes`, use with conjunction of {@link plman.IsAutoPlaylist}.
     *
     * @param {number} playlistIndex
     * @return {boolean}
     */
    IsPlaylistLocked: function (playlistIndex) {}, // (boolean)

    /**
     * @param {number} from
     * @param {number} to
     * @return {boolean}
     */
    MovePlaylist: function (from, to) {}, // (boolean)

    /**
     * @param {number} playlistIndex
     * @param {number} delta
     * @return {boolean}
     *
     * @example
     * // Moves selected items to end of playlist.
     * plman.MovePlaylistSelection(plman.ActivePlaylist, plman.PlaylistItemCount(plman.ActivePlaylist));
     */
    MovePlaylistSelection: function (playlistIndex, delta) {}, // (boolean)

    /**
     * @param {number} playlistIndex
     * @return {number}
     *
     * @example
     * console.log(plman.PlaylistItemCount(plman.PlayingPlaylist)); // 12
     */
    PlaylistItemCount: function (playlistIndex) {}, // (uint) (read)

    /**
     * Removes the specified playlist.<br>
     * Note: if removing the active playlist, no playlist will be active after using this. You'll
     * need to set it manually or use {@link plman.RemovePlaylistSwitch} instead.
     *
     * @param {number} playlistIndex
     * @return {boolean}
     */
    RemovePlaylist: function (playlistIndex) {}, // (boolean)

    /**
     * @param {number} playlistIndex
     * @param {boolean=} [crop=false] If crop is true, then removes items that are NOT selected.
     *
     * @example <Remove selected items from playlist>
     * plman.RemovePlaylistSelection(plman.ActivePlaylist);
     *
     * @example <Remove items that are NOT selected>
     * plman.RemovePlaylistSelection(plman.ActivePlaylist, true);
     */
    RemovePlaylistSelection: function (playlistIndex, crop) {}, // (void) [, crop]

    /**
     * Removes the specified playlist.<br>
     * This automatically sets another playlist as active if removing the active playlist.
     *
     * @param {number} playlistIndex
     * @return {boolean}
     */
    RemovePlaylistSwitch: function (playlistIndex) {}, // (boolean)

    /**
     * @param {number} playlistIndex
     * @param {string} name
     * @return {boolean}
     */
    RenamePlaylist: function (playlistIndex, name) {}, // (boolean)

    /**
     * Workaround so you can use the Edit menu or run fb.RunMainMenuCommand("Edit/Something...")
     * when your panel has focus and a dedicated playlist viewer doesn't.
     *
     * @example
     * plman.SetActivePlaylistContext(); // once on startup
     *
     * function on_focus(is_focused) {
     *    if (is_focused) {
     *        plman.SetActivePlaylistContext(); // When the panel gets focus but not on every click
     *    }
     * }
     */
    SetActivePlaylistContext: function () {}, // (void)

    /**
     * @param {number} playlistIndex
     * @param {number} playlistItemIndex
     *
     * @example
     * plman.SetPlaylistFocusItem(plman.ActivePlaylist, 0);
     */
    SetPlaylistFocusItem: function (playlistIndex, playlistItemIndex) {}, // (void)

    /**
     * @param {number} playlistIndex
     * @param {FbMetadbHandle} handle
     *
     * @example
     * var ap = plman.ActivePlaylist;
     * var handle = plman.GetPlaylistItems(ap)[1]; // 2nd item in playlist
     * plman.SetPlaylistFocusItemByHandle(ap, handle);
     */
    SetPlaylistFocusItemByHandle: function (playlistIndex, handle) {}, // (void)

    /**
     * @param {number} playlistIndex
     * @param {Array<number>} affectedItems An array of item indexes.
     * @param {boolean} state
     *
     * @example
     * // Selects first, third and fifth tracks in playlist. This does not affect other selected items.
     * plman.SetPlaylistSelection(plman.ActivePlaylist, [0, 2, 4], true);
     */
    SetPlaylistSelection: function (playlistIndex, affectedItems, state) {}, // (void)

    /**
     * @param {number} playlistIndex
     * @param {number} playlistItemIndex
     * @param {boolean} state
     *
     * @example
     * // Deselects first playlist item. Only works when it is already selected!
     * plman.SetPlaylistSelectionSingle(plman.ActivePlaylist, 0, false);
     *
     * @example
     * var ap = plman.ActivePlaylist;
     * // Selects last item in playlist. This does not affect other selected items.
     * plman.SetPlaylistSelectionSingle(ap, plman.PlaylistItemCount(ap) - 1, true);
     */
    SetPlaylistSelectionSingle: function (playlistIndex, playlistItemIndex, state) {}, // (void)

    /**
     * Shows popup window letting you edit certain autoplaylist properties.<br>
     * Before using, check if your playlist is an autoplaylist by using {@link plman.IsAutoPlaylist};
     *
     * @param {number} playlistIndex
     * @return {boolean}
     *
     * @example
     * fb.ShowAutoPlaylistUI(plman.ActivePlaylist);
     */
    ShowAutoPlaylistUI: function (playlistIndex) {}, // (boolean)

    /**
     * @param {number} playlistIndex Index of playlist to alter.
     * @param {string} pattern Title formatting pattern to sort by. Set to "" to randomise the order of items.
     * @param {boolean=} [selected_items_only=false]
     * @return {boolean} true on success, false on failure (playlist locked etc).
     */
    SortByFormat: function (playlistIndex, pattern, selected_items_only) {}, // (boolean) [, selected_items_only]

    /**
     * @param {number} playlistIndex Index of playlist to alter.
     * @param {string} pattern Title formatting pattern to sort by.
     * @param {number=} [direction=1]
     *     1 - ascending<br>
     *     -1 - descending<br>
     * @return {boolean}
     */
    SortByFormatV2: function (playlistIndex, pattern, direction) {}, // (boolean) [, direction]

    /**
     * @param {number=} [direction=1]
     *     1 - ascending<br>
     *     -1 - descending<br>
     */
    SortPlaylistsByName: function (direction) {}, //(void)

    /**
     * Call before using other {@link plman} methods that add/remove/reorder playlist items so a history will be available from the Edit menu.
     *
     * @param {number} playlistIndex
     */
    UndoBackup: function (playlistIndex) {}, // (void)

    /**
     * @param {FbMetadbHandle} handle
     */
    AddItemToPlaybackQueue: function (handle) {}, // (void)

    /**
     * @param {number} playlistIndex
     * @param {number} playlistItemIndex
     */
    AddPlaylistItemToPlaybackQueue: function (playlistIndex, playlistItemIndex) {}, // (void)

    /**
     * @param {FbMetadbHandle} handle
     * @param {number} playlistIndex
     * @param {number} playlistItemIndex
     * @return {number} -1 on failure
     */
    FindPlaybackQueueItemIndex: function (handle, playlistIndex, playlistItemIndex) {}, // (int)

    /** @method */
    FlushPlaybackQueue: function () {}, // (void)

    /**
     * @return {Array<FbPlaybackQueueItem>}
     *
     * @example
     * var contents = plman.GetPlaybackQueueContents();
     * if (contents.length) {
     *     // access properties of first item
     *     console.log(contents[0].PlaylistIndex, contents[0].PlaylistItemIndex);
     * }
     */
    GetPlaybackQueueContents: function () {}, // (Array)

    /**
     * @return {FbMetadbHandleList}
     *
     * @example
     * var handles = plman.GetPlaybackQueueHandles();
     * if (handles.Count > 0) {
     *    // use "Count" to determine if Playback Queue is active.
     * }
     */
    GetPlaybackQueueHandles: function () {}, // ((FbMetadbHandleList))

    /**
     * @param {number} index
     */
    RemoveItemFromPlaybackQueue: function (index) {}, // (void)

    /**
     * @param {Array<number>} affectedItems Array like [1, 3, 5]
     */
    RemoveItemsFromPlaybackQueue: function (affectedItems) {}, // (void)
};

/**
 * Various utility functions.
 *
 * @namespace
 */
let utils = {

    /**
     * A string corresponding to the version.
     *
     * Component uses semantic versioning (see {@link https://semver.org}).
     *
     * @type {string}
     *
     * @example
     * function is_compatible(requiredVersionStr) {
     *     var requiredVersion = requiredVersionStr.split('.');
     *     var currentVersion = utils.Version.split('.'); // e.g. 0.1.0-alpha.2
     *     if (currentVersion.length > 3) {
     *         currentVersion.length = 3; // We need only numbers
     *     }
     *
     *     for(var i = 0; i<currentVersion.length; ++i) {
     *       if (currentVersion[i] != requiredVersion[i]) {
     *           return currentVersion[i] > requiredVersion[i];
     *       }
     *     }
     *
     *     return true;
     * }
     *
     * var requiredVersionStr = '1.0.0';
     * if (!is_compatible(requiredVersionStr)) {
     *     fb.ShowPopupMessage(`This script requires v${requiredVersionStr}. Current component version is v${utils.Version}.`);
     * }
     */
    Version: undefined, // (string) (read)

    /**
     * @param {string} name
     * @param {boolean=} [is_dll=true] If true, method checks filename as well as the internal name.
     * @return {boolean}
     *
     * @example
     * console.log(utils.CheckComponent("foo_playcount", true));
     */
    CheckComponent: function (name, is_dll) {}, //(boolean)

    /**
     * Check if the font is installed.<br>
     * Note: it cannot detect fonts loaded by `foo_ui_hacks`. However, {@link gdi.Font} can use those fonts.
     *
     * @param {string} name Can be either in English or the localised name in your OS.
     * @return {boolean}
     */
    CheckFont: function (name) {}, // (boolean)

    /**
     * Spawns a windows popup dialog to let you choose a colour.
     *
     * @param {number} window_id {@link window.ID}
     * @param {number} default_colour
     * @return {number}
     *
     * @example
     * var colour = utils.ColourPicker(window.ID, RGB(255, 0, 0));
     * // See docs\Helper.js for RGB function.
     */
    ColourPicker: function (window_id, default_colour) {}, // (int)

    /**
     * @param {string} path
     * @param {string} mode
     *     "chardet" - Guess the charset of a file and return the codepage. It may not be accurate and returns 0 if an error occurred.<br>
     *     "e" - If file path exists, return true.<br>
     *     "s" - Retrieve file size, in bytes.<br>
     *     "d" - If path is a directory, return true.<br>
     *     "split" - Returns an Array.
     * @return {*}
     *
     * @example
     * var arr = utils.FileTest("D:\\Somedir\\Somefile.txt", "split");
     * // arr[0] <= "D:\\Somedir\\" (always includes backslash at the end)
     * // arr[1] <= "Somefile"
     * // arr[2] <= ".txt"
     */
    FileTest: function (path, mode) {}, // (VARIANT)

    /**
     * @param {number} seconds
     * @return {string}
     *
     * @example
     * console.log(utils.FormatDuration(plman.GetPlaylistItems(plman.ActivePlaylist).CalcTotalDuration())); // 1wk 1d 17:25:30
     */
    FormatDuration: function (seconds) {}, // (string)

    /**
     * @param {number} bytes
     * @return {string}
     *
     * @example
     * console.log(utils.FormatFileSize(plman.GetPlaylistItems(plman.ActivePlaylist).CalcTotalSize())); // 7.9 GB
     */
    FormatFileSize: function (bytes) {}, // (string)

    /**
     * @param {number} window_id {@link window.ID}
     * @param {FbMetadbHandle} handle
     * @param {number=} [art_id=0] See Flags.js > AlbumArtId
     * @param {boolean=} [need_stub=true]
     * @param {boolean=} [only_embed=false]
     * @param {boolean=} [no_load=false]  If true, "image" parameter will be null in on_get_album_art_done callback.
     * @return {number}
     *
     * @example
     * // See samples\basic\GetAlbumArtAsync.txt
     */
    GetAlbumArtAsync: function (window_id, handle, art_id, need_stub, only_embed, no_load) {}, // (uint) [, art_id][, need_stub][, only_embed][, no_load]

    /**
     * @param {string} rawpath
     * @param {number=} [art_id=0] See Flags.js > AlbumArtId
     * @return {GdiBitmap}
     *
     * @example
     * var img = utils.GetAlbumArtEmbedded(fb.GetNowPlaying().RawPath, 0);
     */
    GetAlbumArtEmbedded: function (rawpath, art_id) {}, // (GdiBitmap) [, art_id]

    /**
     * See samples\basic\GetAlbumArtV2.txt
     *
     * @param {FbMetadbHandle} handle
     * @param {number=} [art_id=0] See Flags.js > AlbumArtId
     * @param {boolean=} [need_stub=true]
     * @return {GdiBitmap}
     */
    GetAlbumArtV2: function (handle, art_id, need_stub) {}, // (GdiBitmap) [, art_id][, need_stub]

    /**
     * @param {number} index {@link http://msdn.microsoft.com/en-us/library/ms724371%28VS.85%29.aspx}
     * @return {number} 0 if failed
     *
     * @example
     * var splitter_colour = utils.GetSysColour(15);
     */
    GetSysColor: function (index) {}, // (uint)

    /**
     * @param {number} index {@link http://msdn.microsoft.com/en-us/library/ms724385%28VS.85%29.aspx}
     * @return {number} 0 if failed
     */
    GetSystemMetrics: function (index) {}, // (int)

    /**
     * @param {string} pattern
     * @param {number=} [exc_mask=0x10] Default is FILE_ATTRIBUTE_DIRECTORY. See Flags.js > Used in utils.Glob()
     * @param {number=} [inc_mask=0xffffffff]
     * @return {Array<string>}
     *
     * @example
     * var arr = utils.Glob("C:\\*.*");
     */
    Glob: function (pattern, exc_mask, inc_mask) {}, // (Array) [, exc_mask][, inc_mask]

    /**
     * @param {number} window_id
     * @param {string} prompt
     * @param {string} caption
     * @param {string=} [defaultval = '']
     * @param {boolean=} [error_on_cancel = false] If set to true, use try/catch like Example2.
     * @return {string}
     *
     * @example
     * // With "error_on_cancel" not set (or set to false), cancelling the dialog will return "defaultval".
     * var username = utils.InputBox(window.ID, "Enter your username", "Spider Monkey Panel", "");
     *
     * @example
     * // Using Example1, you can't tell if OK or Cancel was pressed if the return value is the same
     * // as "defaultval". If you need to know, set "error_on_cancel" to true which throws a script error
     * // when Cancel is pressed.
     * try {
     *    var username = utils.InputBox(window.ID, "Enter your username", "Spider Monkey Panel", "", true);
     *    // OK was pressed.
     * } catch(e) {
     *     // Dialog was closed by pressing Esc, Cancel or the Close button.
     * }
     */
    InputBox: function (window_id, prompt, caption, defaultval, error_on_cancel) {}, // (string)

    /**
     * @param {number} vkey {@link http://msdn.microsoft.com/en-us/library/ms927178.aspx}. Some are defined in Flags.js > Used with utils.IsKeyPressed().
     * @return {boolean}
     */
    IsKeyPressed: function (vkey) {}, // (boolean)

    /**
     * @param {string} text
     * @param {string} lcid
     * @param {number} flags
     * @return {string}
     */
    MapString: function (text, lcid, flags) {}, // (string)

    /**
     * Using Microsoft MS-DOS wildcards match type. eg "*.txt", "abc?.tx?"
     *
     * @param {string} pattern
     * @param {string} str
     * @return {boolean}
     */
    PathWildcardMatch: function (pattern, str) {}, // (boolean)

    /**
     * @param {string} filename
     * @param {number=} [codepage=0] See Codepages.js. If codepage is 0, text file can be UTF16-BOM, UTF8-BOM or ANSI.
     * @return {string}
     *
     * @example
     * var text = utils.ReadTextFile("E:\\some text file.txt");
     */
    ReadTextFile: function (filename, codepage) {}, // (string) [,codepage]

    /**
     * Note: this only returns up to 255 characters per value.
     *
     * @param {string} filename
     * @param {string} section
     * @param {string} key
     * @param {string=} [defaultval]
     * @return {string}
     *
     * @example
     * var username = utils.ReadINI("e:\\my_file.ini", "Last.fm", "username");
     */
    ReadINI: function (filename, section, key, defaultval) {}, // (string) [, defaultval]

    /**
     * Displays an html dialog, rendered by IE engine.
     * Utilizes the latest non-Edge IE that you have on your system.
     * Dialog is modal (blocks input to the parent window while open).
     *
     * Html code must be IE compatible, meaning:
     * - JavaScript features are limited by IE (see {@link https://www.w3schools.com/js/js_versions.asp}).
     * - Objects passed to `data` are limited to standard JavaScript objects:
     *   - No extensions from Spider Monkey Panel (e.g. no FbMetadbHandle or GdiBitmap).
     *
     * There are also additional limitations:
     * - options.data may contain only the following types:
     *   - Basic types: number, string, boolean, null, undefined.
     *   - Objects as string: the only way to pass objects is to convert them to string and back with `JSON.stringify()` and `JSON.parse()`.
     *   - Arrays: must be cast via `.toArray()` inside html. Each element has same type limitations as options.data.
     *   - Functions: with maximum of 7 arguments. Each argument has same type limitations as options.data.
     *
     * @param {number} window_id {@link window.ID}
     * @param {string} code_or_path Html code or file path. File path must begin with `file://` prefix.
     * @param {object=} [options=undefined]
     * @param {number=} [options.width=250] Window width
     * @param {number=} [options.height=100] Window height
     * @param {number=} [options.x=0] Window horizontal position relative to desktop
     * @param {number=} [options.y=0] Window vertical position relative to desktop
     * @param {boolean=} [options.center=true] If true and if options.x and options.y are not set, will center window relative to fb2k position.
     * @param {boolean=} [options.context_menu=false] If true, will enable right-click context menu.
     * @param {boolean=} [options.resizable=false] If true, will allow to resize the window.
     * @param {boolean=} [options.selection=false] If true, will allow to select everything (label texts, buttons and etc).
     * @param {boolean=} [options.scroll=false] If true, will display scrollbars.
     * @param {*=} [options.data=undefined] Will be saved in `window.external.dialogArguments` and can be accessed from JavaScript executed inside HTML window.
     *                                      This data is read-only and should not be modified. Has type limitations (see above).
     *
     * @example <caption>Dialog from code</caption>
     * // See samples/basic/HtmlDialogWithCheckbox.txt
     *
     * @example <caption>Dialog from file</caption>
     * utils.ShowHtmlDialog(window.ID, `file://${fb.ComponentPath}samples/basic/html/PopupWithCheckBox.html`);
     */
    ShowHtmlDialog: function (window_id, code_or_path, options) { },

    /**
     * @param {string} filename
     * @param {string} section
     * @param {string} key
     * @param {string} val
     * @return {boolean}
     *
     * @example
     * utils.WriteINI("e:\\my_file.ini", "Last.fm", "username", "Bob");
     */
    WriteINI: function (filename, section, key, val) {}, // (boolean)

    /**
     * The parent folder must already exist.
     *
     * @param {string} filename
     * @param {string} content
     * @param {boolean=} [write_bom=true]
     * @return {boolean}
     *
     * @example <caption>Default encoding</caption>
     * // write_bom missing but defaults to true, resulting file is UTF8-BOM
     * utils.WriteTextFile("z:\\1.txt", "test");
     *
     * @example <caption>UTF8 with BOM</caption>
     * utils.WriteTextFile("z:\\2.txt", "test", true);
     *
     * @example <caption>UTF8 without BOM</caption>
     * utils.WriteTextFile("z:\\3.txt", "test", false);
     */
    WriteTextFile: function (filename, content, write_bom) {}, //(boolean)
};

/**
 * Functions for working with the current SMP panel and accessing it's properties.
 *
 * @namespace
 */
let window = {
    /**
     * See Flags.js > With window.DlgCode
     *
     * @return {number}
     *
     * @example
     * window.DlgCode(DLGC_WANTALLKEYS);
     */
    DlgCode: undefined, // (int) (read, write)

    /**
     * Required in fb.GetClipboardContents, utils.ColourPicker, utils.GetAlbumArtAsync,
     * utils.InputBox, utils.LoadImageAsync, etc.
     *
     * @type {number}
     * @readonly
     */
    ID: undefined, // (read) (int)

    /**
     * You need this to determine which GetFontXXX and GetColourXXX methods to use, assuming you want to support both interfaces.<br>
     * <br>
     * 0 - if using Columns UI<br>
     * 1 - if using default UI.
     *
     * @type {number}
     * @readonly
     */
    InstanceType: undefined, // (int)

    /**
     * Depends on setting inside Spider Monkey Panel Configuration window. You generally use it to determine
     * whether or not to draw a background. Only useful within Panel Stack Splitter (Columns UI component)
     *
     * @type {boolean}
     * @readonly
     */
    IsTransparent: undefined, // (boolean) (read)

    /**
     * @type {boolean}
     * @readonly
     */
    IsVisible: undefined, // (boolean) (read)

    /**
     * @type {number}
     * @readonly
     */
    Height: undefined, // (int) (read)

    /**
     * {@link window.MaxHeight}, {@link window.MaxWidth}, {@link window.MinHeight} and {@link window.MinWidth} can be used to lock the panel size.<br>
     * Do not use if panels are contained within Panel Stack Splitter (Columns UI component).
     *
     * @type {number}
     */
    MaxHeight: undefined, // (int) (read, write)

    /**
     * See {@link window.MaxHeight}.
     *
     * @type {number}
     */
    MaxWidth: undefined, // (int) (read, write)

    /**
     * See {@link window.MaxHeight}.
     *
     * @type {number}
     */
    MinHeight: undefined, // (int) (read, write)

    /**
     * See {@link window.MaxHeight}.
     *
     * @type {number}
     */
    MinWidth: undefined, // (int) (read, write)

    /**
     * Returns the author set in {@link window.DefinePanel}.
     * If that isn't present, the GUID of the panel is returned.
     *
     * @type {string}
     * @readonly
     */
    Name: undefined, // (string) (read)

    /**
     * @type {number}
     * @readonly
     */
    Width: undefined, // (int) (read)

    /**
     * @param {number} timerID
     */
    ClearTimeout: function (timerID) {}, // (void)

    /**
     * @param {number} timerID
     */
    ClearInterval: function (timerID) {}, // (void)

    /**
     * Setups the panel information and available features.<br>
     * Can be called only once, so it's better to define it
     * directly in the panel Configure menu.
     *
     * @param {string} name Displayed panel name
     * @param {object=} [options={}]
     * @param {string=} [options.author=''] Script author
     * @param {string=} [options.version=''] Script version
     * @param {object=} [options.features=undefined] Additional script features
     * @param {boolean=} [options.features.drag_n_drop=false] Indicates if drag_n_drop functionality should be enabled
     */
    DefinePanel: function (name, options) {}, // (void)

    /**
     * @return {number}
     */
    SetInterval: function (func, delay) {}, // (uint)

    /**
     * @return {number}
     *
     * @example
     * // See samples\basic\Timer.txt
     */
    SetTimeout: function (func, delay) {}, // (uint)

    /**
     * @return {MenuObject}
     *
     * @example
     * // See samples\basic\MainMenuManager All-In-One, samples\basic\Menu Sample.txt
     */
    CreatePopupMenu: function () {}, // (MenuObject)

    /**
     * @param {Array<string>} class_list {@link http://msdn.microsoft.com/en-us/library/bb773210%28VS.85%29.aspx}
     * @return {ThemeManager}
     *
     * @example
     * // See samples\basic\SimpleThemedButton.txt
     */
    CreateThemeManager: function (class_list) {}, // (ThemeManager)

    /**
     * @param {string=} [font_name='Segoe UI']
     * @param {number=} [font_size_px=12]
     * @param {number=} [font_style=0] See Flags.js > FontStyle
     * @return {FbTooltip}
     */
    CreateTooltip: function (font_name, font_size_px, font_style) {}, // (FbTooltip) [font_name][, font_size_px][, font_style]

    /**
     * @param {number} type
     * @param {string=} client_guid
     * @return {number}
     */
    GetColourCUI: function (type, client_guid) {}, // (uint) [, client_guid]

    /**
     * @param {number} type
     * @return {number}
     */
    GetColourDUI: function (type) {}, // (uint)

    /**
     * @param {number} type See Flags.js > Used in window.GetFontXXX()
     * @param {string=} client_guid See Flags.js > Used in GetFontCUI() as client_guid.
     * @return {GdiFont} null if the component was unable to determine your font.
     */
    GetFontCUI: function (type, client_guid) {}, // (GdiFont) [, client_guid]

    /**
     * @param {number} type See Flags.js > Used in window.GetFontXXX()
     * @return {GdiFont} null if the component was unable to determine your font.
     *
     * @example
     * // To avoid errors when trying to use the font or access its properties, you
     * // should use code something like this...
     * var font = window.GetFontDUI(0);
     * if (!font) {
     *    console.log("Unable to determine your default font. Using Segoe UI instead.");
     *    font = gdi.Font("Segoe UI", 12);
     * }
     */
    GetFontDUI: function (type) {}, // (GdiFont)

    /**
     * This will trigger `on_notify_data(name, info)` in other panels.<br>
     *
     * @param {string} name
     * @param {*} info
     */
    NotifyOthers: function (name, info) {}, // (void)

    /**
     * Reload panel.
     * @method
     */
    Reload: function () {}, // (void)

    /**
     * @param {boolean=} [force=false]
     */
    Repaint: function (force) {}, // (void) [force]

    /**
     * Use this instead of window.Repaint on frequently updated areas
     * such as time, bitrate, seekbar, etc.
     * @param {number} x
     * @param {number} y
     * @param {number} w
     * @param {number} h
     * @param {boolean=} [force=false]
     */
    RepaintRect: function (x, y, w, h, force) {}, // (void) [force]

    /**
     * This would usually be used inside the on_mouse_move callback.<br>
     * Use -1 if you want to hide the cursor.
     *
     * @param {number} id See Flags.js > Used in window.SetCursor()
     */
    SetCursor: function (id) {}, // (void)

    /**
     * Get value of property.<br>
     * If property does not exist and defaultval is not undefined and not null,
     * it will be created with the value of defaultval.<br>
     * <br>
     * Note: leading and trailing whitespace are removed from property name.
     *
     * @param {string} name
     * @param {*=} defaultval
     * @return {*}
     */
    GetProperty: function (name, defaultval) {}, // (VARIANT) [, defaultval]

    /**
     * Set property value.<br>
     * Property will be removed, if val is undefined or null.<br>
     * <br>
     * Property values are saved per panel instance and are remembered between foobar2000 restarts.<br>
     * <br>
     * Note: leading and trailing whitespace are removed from property name.
     *
     * @param {string} name
     * @param {*=} val
     */
    SetProperty: function (name, val) {}, // (void)

    /**
     * Show configuration window of current panel
     * @method
     */
    ShowConfigure: function () {}, // (void)

    /**
     * Show properties window of current panel
     * @method
     */
    ShowProperties: function () {}, // (void)
};

/**
 * @constructor
 * @hideconstructor
 */
function FbMetadbHandle() {
    /**
     * @type {string}
     * @readonly
     *
     * @example
     * var handle = fb.GetFocusItem();
     * console.log(handle.Path); // D:\SomeSong.flac
     */
    this.Path = undefined; // (string) (read)

    /**
     * @type {string}
     * @readonly
     *
     * @example
     * console.log(handle.RawPath); // file://D:\SomeSong.flac
     */
    this.RawPath = undefined; // (string) (read)

    /**
     * @type {number}
     * @readonly
     */
    this.SubSong = undefined; // (int) (read)

    /**
     * Note: requires a system with IE9 or later to work properly.
     *
     * @type {number}
     * @readonly
     */
    this.FileSize = undefined; // (LONGLONG) (read)

    /**
     * @type {float}
     * @readonly
     */
    this.Length = undefined; // (double) (read)

    /**
     * See {@link https://github.com/TheQwertiest/foo_spider_monkey_panel/wiki/Playback-stats}
     *
     * @param {number} playcount Use 0 to clear
     */
    this.SetPlayCount = function (playcount) {}; // (void)

    /**
     * See {@link https://github.com/TheQwertiest/foo_spider_monkey_panel/wiki/Playback-stats}
     *
     * @param {number} loved Use 0 to clear
     */
    this.SetLoved = function (loved) {}; // (void)

    /**
     * See {@link https://github.com/TheQwertiest/foo_spider_monkey_panel/wiki/Playback-stats}
     *
     * @param {string} first_played Use "" to clear
     */
    this.SetFirstPlayed = function (first_played) {}; // (void)

    /**
     * See {@link https://github.com/TheQwertiest/foo_spider_monkey_panel/wiki/Playback-stats}
     *
     * @param {string} last_played Use "" to clear
     */
    this.SetLastPlayed = function (last_played) {}; // (void)

    /**
     * See {@link https://github.com/TheQwertiest/foo_spider_monkey_panel/wiki/Playback-stats}
     *
     * @param {number} rating Use 0 to clear
     */
    this.SetRating = function (rating) {}; // (void)

    /**
     * See {@link https://github.com/TheQwertiest/foo_spider_monkey_panel/wiki/Playback-stats}
     *
     * @method
     */
    this.ClearStats = function () {}; // (void)

    /**
     * See {@link https://github.com/TheQwertiest/foo_spider_monkey_panel/wiki/Playback-stats}
     *
     * @method
     */
    this.RefreshStats = function () {}; // (void)

    /**
     * Compare two {@link FbMetadbHandle} instances, pointer only.<br>
     * If you want to compare them physically, use the "RawPath" property.
     *
     * @param {FbMetadbHandle} handle
     * @return {boolean}
     *
     * @example
     * handle.Compare(handle2);
     */
    this.Compare = function (handle) {}; // (boolean)

    /**
     * @return {FbFileInfo}
     */
    this.GetFileInfo = function () {}; // (FbFileInfo)
}

/**
 * @constructor
 * @hideconstructor
 */
function FbFileInfo() {
    /**
     * @type {number}
     * @readonly
     *
     * @example
     * var handle = fb.GetFocusItem();
     * var file_info = handle.GetFileInfo();
     * console.log(file_info.MetaCount); // 11
     */
    this.MetaCount = undefined; // (read)

    /**
     * @type {number}
     * @readonly
     *
     * @example
     * console.log(file_info.InfoCount); // 9
     */
    this.InfoCount = undefined; // (read)

    /**
     * @param {string} name
     * @return {number} -1 on failure
     */
    this.InfoFind = function (name) {}; //

    /**
     * @param {number} idx
     * @return {string}
     */
    this.InfoName = function (idx) {}; //

    /**
     * @param {number} idx
     * @return {string}
     */
    this.InfoValue = function (idx) {}; //

    /**
     * @param {string} name
     * @return {number} -1 on failure
     */
    this.MetaFind = function (name) {}; //

    /**
     * Note: the case of the tag name returned can be different depending on tag type,
     * so using toLowerCase() or toUpperCase() on the result is recommended
     *
     * @param {number} idx
     * @return {string}
     *
     * @example
     * for (var i = 0; i < f.MetaCount; ++i) {
     *      console.log(file_info.MetaName(i).toUpperCase());
     * }
     */
    this.MetaName = function (idx) {}; //

    /**
     * @param {number} idx
     * @param {number} vidx
     * @return {string}
     */
    this.MetaValue = function (idx, vidx) {}; //

    /**
     * @param {number} idx
     * @return {number}
     */
    this.MetaValueCount = function (idx) {}; //
}

/**
 * Handle list elements can be accessed with array accessor, e.g. handle_list[i]
 *
 * @constructor
 * @hideconstructor
 */
function FbMetadbHandleList() {
    /**
     * @type {number}
     * @readonly
     *
     * @example
     * plman.GetPlaylistItems(plman.ActivePlaylist);
     * console.log(handle_list.Count);
     */
    this.Count = undefined; // (uint) (read)

    /**
     * @param {FbMetadbHandle} handle
     * @return {number}
     *
     * @example
     * handle_list.Add(fb.GetNowPlaying());
     */
    this.Add = function (handle) {}; // (uint)

    /**
     * @param {FbMetadbHandleList} handle_list
     *
     * @example
     * handle_list.AddRange(fb.GetLibraryItems());
     */
    this.AddRange = function (handle_list) {}; // (void)

    /**
     * Note you cannot perform embedded art operations on the currently playing track - it will silently fail.<br>
     * This is a limitation of the foobar2000 SDK. You may consider detecting if your handle list
     * contains the playing track and stopping playback, performing this action and then restarting
     * playback.<br>
     * Any other errors such as invalid path, corrupt image, target file type not supporting
     * embedded art, etc should all silently fail. A progress dialog will be shown for larger file
     * selections.<br>
     * Any existing artwork of the specified type will be overwritten - there is no need to remove it first.
     *
     * @param {FbMetadbHandleList} image_path path to an existing image
     * @param {number=} [art_id=0] See Flags.js > AlbumArtId
     *
     * @example
     * var handle_list = plman.GetPlaylistItems(plman.ActivePlaylist);
     * if (handle_list.Count > 0) {
     *    var img_path = 'C:\\path\\to\\image.jpg';
     *    handle_list.AttachImage(img_path, 0);
     * }
     *
     * @example
     * // since there is no handle method, do this for a single item
     * var handle_list = fb.CreateHandleList();
     * handle_list.Add(fb.GetFocusItem());
     * var img_path = "C:\\path\\to\\image.jpg";
     * handle_list.AttachImage(img_path, 0);
     */
    this.AttachImage = function (image_path, art_id) {}; //(void)

    /**
     * Faster than Find()
     *
     * @param {FbMetadbHandle} handle Must be sorted.
     * @return {number} -1 on failure.
     */
    this.BSearch = function (handle) {}; // (uint)

    /**
     * @return {float|number} total duration in seconds. For display purposes, consider using {@link utils.FormatDuration} on the result.
     */
    this.CalcTotalDuration = function () {}; // (double)

    /**
     * Requires a system with IE9 or later to work properly.<br>
     *
     * @return {number} total size in bytes. For display purposes, consider using utils.FormatFileSize() on the result.
     */
    this.CalcTotalSize = function () {}; // (LONGLONG)

    /**
     * @return {FbMetadbHandleList}
     *
     * @example
     * var handle_list2 = handle_list.Clone();
     */
    this.Clone = function () {}; // (FbMetadbHandleList)

    /**
     * Converts {@link FbMetadbHandleList} to arrau of {@link FbMetadbHandle}.<br>
     * Use this instead of looping through {@link FbMetadbHandleList}, if the playlist is big
     * or if you need to loop multiple times.<br>
     *
     * @return {Array<FbMetadbHandle>}
     *
     * @example
     * var playlist_items_array = plman.GetPlaylistItems(plman.ActivePlaylist).Convert();
     * for (var i = 0; i < playlist_items_array.length; ++i) {
     *    // do something with playlist_items_array[i] which is your handle
     * }
     */
    this.Convert = function () {}; // (Array)

    /**
     * Note: if sorted, use {@link FbMetadbHandleList#BSearch} instead.
     *
     * @param {FbMetadbHandle} handle
     * @return {number} -1 on failure
     */
    this.Find = function (handle) {}; // (int)

    /**
     * See {@link fb.GetLibraryRelativePath}.<br>
     * <br>
     * This should be faster than looping a handle list manually and using the aforementioned method.
     *
     * @return {Array<string>}
     *
     * @example
     * var handle_list = fb.GetLibraryItems();
     * handle_list.OrderByRelativePath();
     * var relative_paths = handle_list.GetLibraryRelativePaths();
     */
    this.GetLibraryRelativePaths = function () {}; //(Array)

    /**
     * @param {number} index
     * @param {FbMetadbHandle} handle
     * @return {number}
     *
     * @example
     * // This inserts at the end of the handle list.
     * handle_list.Insert(handle_list.Count, fb.GetNowPlaying());
     */
    this.Insert = function (index, handle) {}; // (int)

    /**
     * @param {number} index
     * @param {FbMetadbHandleList} handle_list
     * @return {number}
     */
    this.InsertRange = function (index, handle_list) {}; // (int)

    /**
     * @param {FbMetadbHandleList} handle_list Sorted handle list.
     *
     * @example
     * var one = plman.GetPlaylistItems(0);
     * one.Sort();
     *
     * var two = plman.GetPlaylistItems(1);
     * two.Sort();
     *
     * one.MakeDifference(two);
     * // "one" now only contains handles that were unique to "one".
     * // Anything that also existed in "two" will have been removed.
     */
    this.MakeDifference = function (handle_list) {}; // (void)

    /**
     * @param {FbMetadbHandleList} handle_list Sorted handle list.
     *
     * @example
     * var one = plman.GetPlaylistItems(0);
     * one.Sort();
     *
     * var two = plman.GetPlaylistItems(1);
     * two.Sort();
     *
     * one.MakeIntersection(two);
     * // "one" now only contains handles that were in BOTH "one" AND "two"
     */
    this.MakeIntersection = function (handle_list) {}; // (void)

    /**
     * @param {FbMetadbHandleList} handle_list Sorted handle list.
     *
     * @example
     * var one = plman.GetPlaylistItems(0);
     * one.Sort();
     *
     * var two = plman.GetPlaylistItems(1);
     * two.Sort();
     *
     * one.MakeUnion(two);
     * // "one" now contains all handles from "one" AND "two" with any duplicates removed
     */
    this.MakeUnion = function (handle_list) {}; // (void)

    /**
     * @param {FbTitleFormat} tfo An instance of FbTitleFormat.
     * @param {number} direction > 0 - ascending.
     *
     * @example
     * var handle_list = fb.GetLibraryItems();
     * var tfo = fb.TitleFormat("%album artist%|%date%|%album%|%discnumber%|%tracknumber%");
     * handle_list.OrderByFormat(tfo, 1);
     */
    this.OrderByFormat = function (tfo, direction) {}; // (void)

    /**
     * Note: this method should only be used on a handle list containing items that are monitored as part of the Media Library.
     *
     * @method
     */
    this.OrderByPath = function () {}; // (void)

    /** @method */
    this.OrderByRelativePath = function () {}; // (void)

    /**
     * See {@link https://github.com/TheQwertiest/foo_spider_monkey_panel/wiki/Playback-stats}
     *
     * @method
     */
    this.RefreshStats = function () {}; // (void)

    /**
     * @param {FbMetadbHandle} handle
     */
    this.Remove = function (handle) {}; // (void)

    /** @method */
    this.RemoveAll = function () {}; // (void)

    /**
     * See {@link FbMetadbHandleList#AttachImage()} for more info as there are some limitations.
     *
     * @param {number=} [art_id=0] See Flags.js > AlbumArtId
     */
    this.RemoveAttachedImage = function (art_id) {}; // (void)

    /**
     * @param {number} idx
     *
     * @example
     * handle_list.RemoveById(0);
     */
    this.RemoveById = function (idx) {}; // (void)

    /**
     * @param {number} from
     * @param {number} num
     *
     * @example
     * handle_list.RemoveRange(10, 20);
     */
    this.RemoveRange = function (from, num) {}; // (void)

    /**
     * Remove duplicates and optimise for other handle list operations
     *
     * @method
     */
    this.Sort = function () {}; // (void)

    /**
     * Updated metadb tags with new values.
     *
     * @param {string} str JSON string, which contains an object (applies same values to every track)
     *                     or an array of objects (one object per track).
     *
     * @example
     * // assume we've selected one album
     * var handles = plman.GetPlaylistSelectedItems(plman.ActivePlaylist);
     *
     * var arr = [];
     * for (var i = 0; i < handles.Count; ++i) {
     *     // each element of the array must be an object of key names/values, indicated by the curly braces
     *     arr.push({
     *         'tracknumber' : i + 1, // independent values per track
     *         'totaltracks' : handles.Count,
     *         'album' : 'Greatest Hits', // a simple string for a single value
     *         'genre' : ['Rock', 'Hard Rock'], // we can use an array here for multiple value tags
     *         'bad_tag' : '' // blank values will clear any existing tags.
     *     });
     * }
     *
     * handles.UpdateFileInfoFromJSON(JSON.stringify(arr));
     */
    this.UpdateFileInfoFromJSON = function (str) {}; // (void)
}

/**
 * A Recycle Bin for playlists.
 * 
 * @constructor
 * @hideconstructor
 */
function FbPlaylistRecycler() {

    /**
     * @type {number}
     * @readonly
     */
    this.Count = undefined; // (uint) (read)

    /**
     * @param {number} index
     * @return {string}
     */
    this.GetName = function (index) {}; // (string) (read)

    /**
     * @param {number} index
     * @return {FbMetadbHandleList}
     */
    this.GetContent = function (index) {}; // (FbMetadbHandleList) (read)

    /**
     * @param {number} affectedItems array like [1, 3, 5]
     */
    this.Purge = function (affectedItems) {}; // (void)

    /**
     * @param {number} index
     */
    this.Restore = function (index) {}; // (void)
}

/**
 * @constructor
 * @hideconstructor
 *
 * @example
 * var playing_item_location = plman.GetPlayingItemLocation();
 * if (playing_item_location.IsValid) {
 *     console.log(playing_item_location.PlaylistIndex);
 *     console.log(playing_item_location.PlaylistItemIndex);
 * }
 */
function FbPlayingItemLocation() {

    /**
     * @type {boolean}
     * @readonly
     */
    this.IsValid = undefined; // (boolean) (read)

    /**
     * @type {number}
     * @readonly
     */
    this.PlaylistIndex = undefined; // (int) (read)

    /**
     * @type {number}
     * @readonly
     */
    this.PlaylistItemIndex = undefined; // (int) (read)
}

/**
 * @constructor
 * @hideconstructor
 */
function FbPlaybackQueueItem() {

    /**
     * @type {FbMetadbHandle}
     * @readonly
     */
    this.Handle = undefined; // (FbMetadbHandle) (read)

    /**
     * @type {number}
     * @readonly
     */
    this.PlaylistIndex = undefined; // (int) (read)

    /**
     * -1 if added not from a playlist
     *
     * @type {number}
     * @readonly
     */
    this.PlaylistItemIndex = undefined; // (int) (read)
}

/**
 * @constructor
 * @hideconstructor
 *
 * @example
 * let test = fb.CreateProfiler("test");
 * // do something time consuming
 * console.log(test.Time); // Outputs bare time in ms like "789"
 * test.Print(); // Outputs component name/version/assigned name like "Spider Monkey Panel v1.0.0: FbProfiler (test): 789 ms"
 */
function FbProfiler() {

    /**
     * @type {number}
     * @readonly
     */
    this.Time = undefined; // (int) // milliseconds

    /** @method */
    this.Reset = function () {}; // (void)
    /** @method */
    this.Print = function () {}; // (void)
}

/**
 * @constructor
 * @hideconstructor
 */
function FbTitleFormat() {
    /**
     * Always use Eval when you want dynamic info such as %playback_time%, %bitrate% etc.
     * EvalWithMetadb(fb.GetNowplaying()) will not give the results you want.
     *
     * @param {boolean=} [force=false] If true, you can process text that doesn't contain
     *     title formatting even when foobar2000 isn't playing. When playing, you
     *     should always get a result.
     * @return {string}
     *
     * @example
     * var tfo = fb.TitleFormat("%artist%");
     * console.log(tfo.Eval());
     */
    this.Eval = function (force) {}; // [force]

    /**
     * @param {FbMetadbHandle} handle
     * @return {string}
     *
     * @example
     * var tfo = fb.TitleFormat("%artist%");
     * console.log(tfo.EvalWithMetadb(fb.GetFocusItem()));
     */
    this.EvalWithMetadb = function (handle) {}; //

    /**
     * @param handle_list {FbMetadbHandleList}
     * @return {Array<string>}
     *
     * @example
     * var tfo = fb.TitleFormat("%artist%");
     * let handle_list = fb.GetLibraryItems();
     * let artists = tfo.EvalWithMetadbs(handle_list);
     * console.log(handle_list.Count === artists.length); // should always be true!
     */
    this.EvalWithMetadbs = function (handle_list) {}; //(Array)
}

/**
 * @constructor
 * @hideconstructor
 */
function FbTooltip() {
    /**
     * @type {string}
     *
     * @example
     * var tooltip = window.CreateTooltip();
     * tooltip.Text = "Whoop";
     */
    this.Text = undefined; // (string) (read, write)

    /** @type {boolean} */
    this.TrackActivate = undefined; // (boolean) (write)

    /**
     * Note: only do this when text has changed, otherwise it will flicker.
     *
     * @method
     *
     * @example
     * var text = "...";
     * if (tooltip.Text != text) {
     *    tooltip.Text = text;
     *    tooltip.Activate();
     * }
     */
    this.Activate = function () {}; // (void)

    /** @method */
    this.Deactivate = function () {}; // (void)

    /**
     * @param {number} type
     * @return {number}
     */
    this.GetDelayTime = function (type) {}; // (int)

    /**
     * @param {number} type See Flags.js > Used in {@link FbTooltip#GetDelayTime} and {@link FbTooltip#SetDelayTime}
     * @param {number} time
     */
    this.SetDelayTime = function (type, time) {}; // (void)

    /**
     * Use if you want multi-line tooltips.<br>
     * Use \n as a new line separator.
     *
     * @param {number} width
     *
     * @example
     * tooltip.SetMaxWidth(800);
     * tooltip.Text = "Line1\nLine2";
     */
    this.SetMaxWidth = function (width) {}; // (void)

    /**
     * Note: check that x, y positions have changed from the last time, otherwise it will flicker.
     *
     * @param {number} x
     * @param {number} y
     */
    this.TrackPosition = function (x, y) {}; // (void)
}

/**
 * This is typically used to update the selection used by the default UI artwork panel
 * or any other panel that makes use of the preferences under
 * File > Preferences > Display > Selection viewers. Use in conjunction with the on_focus
 * callback. See Callbacks.js.
 *
 * @constructor
 * @hideconstructor
 *
 * @example <caption>For playlist viewers</caption>
 * var selection_holder = fb.AcquireUiSelectionHolder();
 * selection_holder.SetPlaylistSelectionTracking();
 *
 * function on_focus(is_focused) {
 *     if (is_focused) { // Updates the selection when panel regains focus
 *         selection_holder.SetPlaylistSelectionTracking();
 *     }
 * }
 *
 * @example <caption>For library viewers</caption>
 * var selection_holder = fb.AcquireUiSelectionHolder();
 * var handle_list = null;
 *
 * function on_mouse_lbtn_up(x, y) { // Presumably going to select something here...
 *    handle_list = ...;
 *    selection_holder.SetSelection(handle_list);
 * }
 *
 * function on_focus(is_focused) {
 *    if (is_focused) { // Updates the selection when panel regains focus
 *        if (handle_list && handle_list.Count)
 *            selection_holder.SetSelection(handle_list);
 *    }
 * }
 */
function FbUiSelectionHolder() {

    /**
     * Sets the selected items.
     *
     * @param {FbMetadbHandleList} handle_list
     */
    this.SetSelection = function (handle_list) {}; // (void)

    /**
     * Sets selected items to playlist selection and enables tracking.<br>
     * When the playlist selection changes, the stored selection is automatically
     * updated. Tracking ends when a set method is called on any ui_selection_holder
     * or when the last reference to this ui_selection_holder is released.
     */
    this.SetPlaylistSelectionTracking = function () {}; // (void)

    /**
     * Sets selected items to playlist contents and enables tracking.<br>
     * When the playlist selection changes, the stored selection is automatically
     * updated. Tracking ends when a set method is called on any ui_selection_holder
     * or when the last reference to this ui_selection_holder is released.
     */
    this.SetPlaylistTracking = function () {}; // (void)
}

/**
 * @constructor
 * @hideconstructor
 */
function GdiBitmap() {

    /**
     * @type {number}
     * @readonly
     */
    this.Height = undefined;// (int) (read)

    /**
     * @type {number}
     * @readonly
     */
    this.Width = undefined;// (int) (read)

    /**
     * @param {number} alpha Valid values 0-255.
     * @return {GdiBitmap}
     */
    this.ApplyAlpha = function (alpha) {}; // (GdiBitmap)

    /**
     * Changes will be saved in the current bitmap. See samples\basic\Apply Mask.txt
     *
     * @param {GdiBitmap} img
     * @return {boolean}
     */
    this.ApplyMask = function (img) {}; // (boolean)

    /**
     * @param {number} x
     * @param {number} y
     * @param {number} w
     * @param {number} h
     * @return {GdiBitmap}
     */
    this.Clone = function (x, y, w, h) {}; // (GdiBitmap)

    /**
     * Create a DDB bitmap from GdiBitmap, which is used in {@link gdi.GdiDrawBitmap}
     *
     * @return {GdiRawBitmap}
     */
    this.CreateRawBitmap = function () {}; // (GdiRawBitmap)

    /**
     * @param {number} max_count
     * @return {Array<number>}
     */
    this.GetColourScheme = function (max_count) {}; // (Array)

    /**
     * Returns a JSON array in string form so you need to use JSON.parse() on the result.<br>
     * Each entry in the array is an object which contains colour and frequency values.<br>
     * Uses a different method for calculating colours than GetColourScheme.<br>
     * Image is automatically resized during processing for performance reasons so there's no
     * need to resize before calling the method.
     *
     * @param {number} max_count
     * @return {string}
     *
     * @example
     * // See docs\Helpers.js for "toRGB" function.
     * img = ... // use utils.GetAlbumArtV2 / gdi.Image / etc
     * colours = JSON.parse(img.GetColourSchemeJSON(5));
     * console.log(colours[0].col); // -4194304
     * console.log(colours[0].freq); // 0.34
     * console.log(toRGB(colours[0].col)); // [192, 0, 0]
     */
    this.GetColourSchemeJSON = function (max_count) {}; // (string)

    /**
     * Note: don't forget to use {@link GdiGraphics#ReleaseGraphics()} after operations on GdiGraphics interface is done!
     *
     * @return {GdiGraphics}
     */
    this.GetGraphics = function () {};

    /**
     * @param {GdiGraphics} gr
     * @return {GdiGraphics}
     */
    this.ReleaseGraphics = function (gr) {}; // (GdiGraphics)

    /**
     * @param {number} w
     * @param {number} h
     * @param {number=} [mode=0] See Flags.js > InterpolationMode
     * @return{GdiBitmap}
     */
    this.Resize = function (w, h, mode) {}; // (GdiBitmap) [, mode]

    /**
     * @param {number} mode See Flags.js > RotateFlipType
     */
    this.RotateFlip = function (mode) {}; // (void)

    /**
     * @param {string} path Full path including file extension. The parent folder must already exist.
     * @param {string=} [format='image/png']
     *      "image/png"<br>
     *      "image/bmp"<br>
     *      "image/jpeg"<br>
     *      "image/gif"<br>
     *      "image/tiff"
     * @return {boolean}
     *
     * @example
     * var img = utils.GetAlbumArtEmbedded(fb.GetFocusItem().RawPath, 0);
     * if (img) {
     *     img.SaveAs("D:\\export.jpg", "image/jpeg");
     * }
     */
    this.SaveAs = function (path, format) {}; // (boolean) [, format]

    /**
     * @param {number} radius Valid values 2-254. S
     *
     * @example <caption>Blur image<caption>
     * // samples\basic\StackBlur (image).txt
     *
     * @example <caption>Blur text<caption>
     * // samples\basic\StackBlur (text).txt
     */
    this.StackBlur = function (radius) {}; // (void)
}

/**
 * @constructor
 * @hideconstructor
 */
function GdiFont() {
    /**
     * @type {number}
     * @readonly
     *
     * @example
     * console.log(my_font.Height); // 15
     */
    this.Height = undefined;//    (int)(read)

    /**
     * @type {string}
     * @readonly
     *
     * @example
     * console.log(my_font.Name); // Segoe UI
     */
    this.Name = undefined;//    (string)(read)

    /**
     * @type {float}
     * @readonly
     *
     * @example
     * console.log(my_font.Size); // 12
     */
    this.Size = undefined;//    (float)(read)

    /**
     * See Flags.js > FontStyle for value interpretation.
     *
     * @type {number}
     * @readonly
     *
     * @example
     * console.log(my_font.Style);
     */
    this.Style = undefined;//    (int)(read)
}

/**
 * Typically used inside `on_paint`.<br>
 *
 * Note: there are many different ways to get colours:
 * window.GetColourDUI/window.GetColourCUI,
 * RGB function from Helpers.js, utils.ColourPicker and
 * etc.
 *
 * @constructor
 * @hideconstructor
 */
function GdiGraphics() {
    /**
     * Calculates text height for {@link GdiGraphics#GdiDrawText}.<br>
     * Note: this will only calculate the text height of one line.
     *
     * @param {string} str
     * @param {GdiFont} font
     * @return {number}
     */
    this.CalcTextHeight = function (str, font) {}; // (uint)

    /**
     * Calculates text width for {@link GdiGraphics#GdiDrawText}.
     *
     * @param {string} str
     * @param {GdiFont} font
     * @return {number}
     */
    this.CalcTextWidth = function (str, font) {}; // (uint)

    /**
     * @param {number} x
     * @param {number} y
     * @param {number} w
     * @param {number} h
     * @param {number} line_width
     * @param {number} colour
     */
    this.DrawEllipse = function (x, y, w, h, line_width, colour) {}; // (void)

    /**
     * @param {GdiBitmap} img
     * @param {number} dstX
     * @param {number} dstY
     * @param {number} dstW
     * @param {number} dstH
     * @param {number} srcX
     * @param {number} srcY
     * @param {number} srcW
     * @param {number} srcH
     * @param {number=} [angle=0]
     * @param {number=} [alpha=255] Valid values 0-255.
     */
    this.DrawImage = function (img, dstX, dstY, dstW, dstH, srcX, srcY, srcW, srcH, angle, alpha) {}; // (void) [, angle][, alpha]

    /**
     * @param {number} x1
     * @param {number} y1
     * @param {number} x2
     * @param {number} y2
     * @param {number} line_width
     * @param {number} colour
     */
    this.DrawLine = function (x1, y1, x2, y2, line_width, colour) {}; // (void)

    /**
     * @param {number} colour
     * @param {number} line_width
     * @param {Array<Array<number>>} points
     */
    this.DrawPolygon = function (colour, line_width, points) {}; // (void)

    /**
     * Should be only used when {@link GdiGraphics#GdiDrawText} is not applicable.
     *
     * @param {string} str
     * @param {GdiFont} font
     * @param {number} colour
     * @param {number} x
     * @param {number} y
     * @param {number} w
     * @param {number} h
     * @param {number=} [flags=0] See Flags.js > StringFormatFlags
     */
    this.DrawString = function (str, font, colour, x, y, w, h, flags) {}; // (void) [, flags]

    /**
     * @param {number} x
     * @param {number} y
     * @param {number} w
     * @param {number} h
     * @param {number} line_width
     * @param {number} colour
     */
    this.DrawRect = function (x, y, w, h, line_width, colour) {}; // (void)

    /**
     * @param {number} x
     * @param {number} y
     * @param {number} w
     * @param {number} h
     * @param {number} arc_width
     * @param {number} arc_height
     * @param {number} line_width
     * @param {number} colour
     */
    this.DrawRoundRect = function (x, y, w, h, arc_width, arc_height, line_width, colour) {}; // (void)

    /**
     * @param {string} str
     * @param {GdiFont} font
     * @param {number} max_width
     * @return {Array<Array>}
     *    index | meaning <br>
     *    [0] text line 1 <br>
     *    [1] width of text line 1 (in pixel) <br>
     *    [2] text line 2 <br>
     *    [3] width of text line 2 (in pixel) <br>
     *    ... <br>
     *    [2n + 2] text line n <br>
     *    [2n + 3] width of text line n (px)
     */
    this.EstimateLineWrap = function (str, font, max_width) {}; // (Array)

    /**
     * @param {number} x
     * @param {number} y
     * @param {number} w
     * @param {number} h
     * @param {number} colour
     */
    this.FillEllipse = function (x, y, w, h, colour) {}; // (void)

    /**
     * Note: this may appear buggy depending on rectangle size. The easiest fix is
     * to adjust the "angle" by a degree or two.
     *
     * @param {number} x
     * @param {number} y
     * @param {number} w
     * @param {number} h
     * @param {number} angle
     * @param {number} colour1
     * @param {number} colour2
     * @param {float} [focus=1.0] Specify where the centred colour will be at its highest intensity. Valid values between 0 and 1.
     */
    this.FillGradRect = function (x, y, w, h, angle, colour1, colour2, focus) {}; // (void) [, focus]

    /**
     * @param {number} colour
     * @param {number} fillmode 0 alternate, 1 winding.
     * @param {Array<Array<number>>} points
     */
    this.FillPolygon = function (colour, fillmode, points) {}; // (void)

    /**
     * @param {number} x
     * @param {number} y
     * @param {number} w
     * @param {number} h
     * @param {number} arc_width
     * @param {number} arc_height
     * @param {number} colour
     */
    this.FillRoundRect = function (x, y, w, h, arc_width, arc_height, colour) {}; // (void)

    /**
     * @param {number} x
     * @param {number} y
     * @param {number} w
     * @param {number} h
     * @param {number} colour
     */
    this.FillSolidRect = function (x, y, w, h, colour) {}; // (void)

    /**
     * @param {GdiRawBitmap} img
     * @param {number} dstX
     * @param {number} dstY
     * @param {number} dstW
     * @param {number} dstH
     * @param {number} srcX
     * @param {number} srcY
     * @param {number} srcW
     * @param {number} srcH
     * @param {number=} [alpha=255] Valid values 0-255.
     */
    this.GdiAlphaBlend = function (img, dstX, dstY, dstW, dstH, srcX, srcY, srcW, srcH, alpha) {}; // (void) [, alpha]

    /**
     * Always faster than {@link GdiGraphics#DrawImage}, does not support alpha channel.
     *
     * @param {GdiRawBitmap} img
     * @param {number} dstX
     * @param {number} dstY
     * @param {number} dstW
     * @param {number} dstH
     * @param {number} srcX
     * @param {number} srcY
     * @param {number} srcW
     * @param {number} srcH
     */
    this.GdiDrawBitmap = function (img, dstX, dstY, dstW, dstH, srcX, srcY, srcW, srcH) {}; // (void)

    /**
     * Provides faster and better rendering than {@link GdiGraphics#DrawString}.
     *
     * Do not use this to draw text on transparent background or
     * with GdiGraphics other than the one passed in on_paint callback:
     * this will result in visual artifacts caused by ClearType hinting.
     * Use {@link GdiGraphics#DrawString} instead in such cases.*
     *
     * To calculate text dimensions use {@link GdiGraphics#CalcTextHeight}, {@link GdiGraphics#CalcTextWidth} or DT_CALCRECT flag.
     *
     * @param {string} str
     * @param {GdiFont} font
     * @param {number} colour
     * @param {number} x
     * @param {number} y
     * @param {number} w
     * @param {number} h
     * @param {number=} [format=0] See Flags.js > DT_*
     * @return {Array<number>}
     *     index | meaning <br>
     *     [0] left   (DT_CALCRECT) <br>
     *     [1] top    (DT_CALCRECT) <br>
     *     [2] right  (DT_CALCRECT) <br>
     *     [3] bottom (DT_CALCRECT) <br>
     *     [4] characters drawn
     */
    this.GdiDrawText = function (str, font, colour, x, y, w, h, format) {}; // (Array) [, format]

    /**
     * Calculates text dimensions for {@link GdiGraphics#DrawString}.
     *
     * @param {string} str
     * @param {GdiFont} font
     * @param {number} x
     * @param {number} y
     * @param {number} w
     * @param {number} h
     * @param {number=} [flags=0] See Flags.js > StringFormatFlags
     * @return {MeasureStringInfo}
     */
    this.MeasureString = function (str, font, x, y, w, h, flags) {}; // (MeasureStringInfo) [, flags]

    /**
     * @constructor
     * @hideconstructor
     *
     * @example
     * include(fb.ComponentPath + 'docs\\Flags.js');
     * include(fb.ComponentPath + 'docs\\Helpers.js');
     *
     * var sf = StringFormat(StringAlignment.Near, StringAlignment.Near);
     * var text = utils.ReadTextFile("z:\\info.txt");
     * var font = window.GetFontDUI(0);
     *
     * function on_paint(gr) {
     *     gr.DrawString(text, font, RGB(255, 0, 0), 0, 0, window.Width, window.Height, sf);
     *     var temp = gr.MeasureString(text, font, 0, 0, window.Width, 10000, sf);
     *     // If we want to calculate height, we must set the height to be far larger than what
     *     // the text could possibly be.
     *
     *     console.log(temp.Height); // 2761.2421875 // far larger than my panel height!
     *     console.log(temp.Chars); // 7967
     * }
     */
    function MeasureStringInfo() {

        /**
         * @type {number}
         * @readonly
         */
        this.chars = undefined; // (int) (read)

        /**
         * @type {float}
         * @readonly
         */
        this.Height = undefined; // (float) (read)

        /**
         * @type {number}
         * @readonly
         */
        this.lines = undefined; // (int) (read)

        /**
         * @type {float}
         * @readonly
         */
        this.x = undefined; // (float) (read)

        /**
         * @type {float}
         * @readonly
         */
        this.y = undefined; // (float) (read)

        /**
         * @type {float}
         * @readonly
         */
        this.Width = undefined; // (float) (read)
    }

    /**
     * See Flags.js > InterpolationMode
     *
     * @param {number=} [mode=0]
     */
    this.SetInterpolationMode = function (mode) {}; // (void)

    /**
     * @param {number=} [mode=0] See Flags.js > SmoothingMode
     */
    this.SetSmoothingMode = function (mode) {}; // (void)

    /**
     * @param {number=} [mode=0] See Flags.js > TextRenderingHint
     */
    this.SetTextRenderingHint = function (mode) {}; // (void)
}

/**
 * @constructor
 * @hideconstructor
 */
function GdiRawBitmap() {

    /**
     * @type {number}
     * @readonly
     */
    this.Width = undefined; // (int) (read)

    /**
     * @type {number}
     * @readonly
     */
    this.Height = undefined; // (int) (read)
}

/**
 * @constructor
 * @hideconstructor
 */
function DropTargetAction() {

    /** @type {number} */
    this.Base = undefined; // (write)

    /**
     * See {@link https://msdn.microsoft.com/en-us/library/windows/desktop/ms693457.aspx}
     *
     * @type {number}
     */
    this.Effect = undefined; //(read, write)

    /**
     * Active playlist.<br>
     * -1 by default.
     *
     * @type {number}
     */
    this.Playlist = undefined; // (write)

    /** @type {boolean} */
    this.ToSelect = undefined; // (boolean) (write)
}

/**
 * @constructor
 * @hideconstructor
 */
function ContextMenuManager() {
    /**
     * @param {MenuObject} menuObj
     * @param {number} base_id
     * @param {number=} [max_id=-1]
     */
    this.BuildMenu = function (menuObj, base_id, max_id) {}; // (void)

    /**
     * @param {number} id
     * @return {boolean}
     */
    this.ExecuteByID = function (id) {}; // (boolean)

    /**
     * @param {FbMetadbHandleList} handle_list
     */
    this.InitContext = function (handle_list) { }; // (void)

    /**
     * Shows playlist specific options that aren't available when passing a
     * handle list to {@link ContextMenuManager#InitContext}.
     */
    this.InitContextPlaylist = function () { }; // (void)

    /** @method */
    this.InitNowPlaying = function () {}; // (void)
}

/**
 * @constructor
 * @hideconstructor
 */
function MainMenuManager() {
    /**
     * @param {MenuObject} menuObj
     * @param {number} base_id
     * @param {number} count
     */
    this.BuildMenu = function (menuObj, base_id, count) {}; // (void)

    /**
     * @param {number} id
     * @return {boolean}
     */
    this.ExecuteByID = function (id) {}; // (boolean)

    /** @method */
    this.Init = function (root_name) {}; // (void)
}

/**
 * @constructor
 * @hideconstructor
 */
function MenuObject() {

    /**
     * @param {number} flags See Flags.js > Used in AppendMenuItem()
     * @param {number} item_id Integer greater than 0. Each menu item needs a unique id.
     * @param {string} text
     */
    this.AppendMenuItem = function (flags, item_id, text) {}; // (void)

    /** @method */
    this.AppendMenuSeparator = function () {}; // (void)

    /**
     * @param {MenuObject} parentMenu
     * @param {number} flags
     * @param {string} text
     */
    this.AppendTo = function (parentMenu, flags, text) {}; // (void)

    /**
     * @param {number} item_id
     * @param {boolean} check
     */
    this.CheckMenuItem = function (item_id, check) {}; // (void)

    /**
     * @param {number} first_item_id
     * @param {number} last_item_id
     * @param {number} selected_item_id
     */
    this.CheckMenuRadioItem = function (first_item_id, last_item_id, selected_item_id) {}; // (void)

    /**
     * @param {number} x
     * @param {number} y
     * @param {number=} [flags=0] See Flags.js > Used in TrackPopupMenu().
     * @return {number}
     */
    this.TrackPopupMenu = function (x, y, flags) {}; // (int) [, flags]
}

/**
 * @constructor
 * @hideconstructor
 */
function ThemeManager() {
    /**
     * @param {GdiGraphics} gr
     * @param {number} x
     * @param {number} y
     * @param {number} w
     * @param {number} h
     * @param {number=} [clip_x=0]
     * @param {number=} [clip_y=0]
     * @param {number=} [clip_w=0]
     * @param {number=} [clip_h=0]
     */
    this.DrawThemeBackground = function (gr, x, y, w, h, clip_x, clip_y, clip_w, clip_h) {}; // (void) [, clip_x][, clip_y][, clip_w][, clip_h]

    /**
     * @param {number} partid
     * @return {boolean}
     */
    this.IsThemePartDefined = function (partid) {}; // (boolean)

    /**
     * See {@link http://msdn.microsoft.com/en-us/library/bb773210%28VS.85%29.aspx}
     *
     * @param {number} partid
     * @param {number=} [stateid=0]
     */
    this.SetPartAndStateID = function (partid, stateid) {}; // (void)
}