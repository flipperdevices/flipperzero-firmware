#include "nfc_playlist.h"
#include "nfc_playlist_i.h"

static void (*const nfc_playlist_scene_on_enter_handlers[])(void*) = {
    nfc_playlist_main_menu_scene_on_enter,
    nfc_playlist_emulation_scene_on_enter,
};

static bool (*const nfc_playlist_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
    nfc_playlist_main_menu_scene_on_event,
    nfc_playlist_emulation_scene_on_event,
};

static void (*const nfc_playlist_scene_on_exit_handlers[])(void*) = {
    nfc_playlist_main_menu_scene_on_exit,
    nfc_playlist_emulation_scene_on_exit,
};

static const SceneManagerHandlers nfc_playlist_scene_manager_handlers = {
    .on_enter_handlers = nfc_playlist_scene_on_enter_handlers,
    .on_event_handlers = nfc_playlist_scene_on_event_handlers,
    .on_exit_handlers = nfc_playlist_scene_on_exit_handlers,
    .scene_num = NfcPlaylistScene_count,
};

static bool nfc_playlist_custom_callback(void* context, uint32_t custom_event) {
    furi_assert(context);
    NfcPlaylist* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, custom_event);
}

static bool nfc_playlist_back_event_callback(void* context) {
    furi_assert(context);
    NfcPlaylist* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static NfcPlaylist* nfc_playlist_alloc() {

    NfcPlaylist* app = malloc(sizeof(NfcPlaylist));
    furi_assert(app);
    app->scene_manager = scene_manager_alloc(&nfc_playlist_scene_manager_handlers, app);

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);

    app->variable_item_list = variable_item_list_alloc();
    app->popup = popup_alloc();
    app->emulate_timeout = default_emulate_timeout;
    app->emulate_delay = default_emulate_delay;

    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback( app->view_dispatcher, nfc_playlist_custom_callback);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, nfc_playlist_back_event_callback);

    view_dispatcher_add_view(app->view_dispatcher, NfcPlaylistView_Menu, variable_item_list_get_view(app->variable_item_list));

    view_dispatcher_add_view(app->view_dispatcher, NfcPlaylistView_Popup, popup_get_view(app->popup));

    return app;
}

static void nfc_playlist_free(NfcPlaylist* app) {
    furi_assert(app);
    scene_manager_free(app->scene_manager);
    view_dispatcher_remove_view(app->view_dispatcher, NfcPlaylistView_Menu);
    view_dispatcher_remove_view(app->view_dispatcher, NfcPlaylistView_Popup);
    view_dispatcher_free(app->view_dispatcher);
    variable_item_list_free(app->variable_item_list);
    popup_free(app->popup);
    free(app);
}

int32_t nfc_playlist_main(void* p) {
   // Mark argument as unused
   UNUSED(p);

   NfcPlaylist* app = nfc_playlist_alloc();

   Gui* gui = furi_record_open(RECORD_GUI);
   view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
   scene_manager_next_scene(app->scene_manager, NfcPlaylistScene_MainMenu);
   view_dispatcher_run(app->view_dispatcher);

   // free all memory
   furi_record_close(RECORD_GUI);
   nfc_playlist_free(app);

   return 0;
}
