#include <furi.h>

#include <storage/storage.h>
#include <toolbox/stream/stream.h>
#include <toolbox/stream/file_stream.h>

#include <nfc_playlist_worker.h>

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/menu.h>
#include <gui/modules/submenu.h>
#include <gui/modules/popup.h>

// Define log tag
#define TAG "NfcPlaylist"

/** ids for all scenes used by the app */
typedef enum {
   NfcPlaylistScene_MainMenu,
   NfcPlaylistScene_FirstPopup,
   NfcPlaylistScene_count
} NfcPlaylistScene;

/** ids for the 2 types of view used by the app */
typedef enum { NfcPlaylistView_Menu, NfcPlaylistView_Popup } NfcPlaylistView;

/** the app context struct */
typedef struct {
   SceneManager* scene_manager;
   ViewDispatcher* view_dispatcher;
   Submenu* menu;
   Popup* popup;
   int emulate_timeout;
} NfcPlaylist;

/** all custom events */
typedef enum { NfcPlaylistEvent_ShowPopupOne } NfcPlaylistEvent;

/* main menu scene */

/** indices for menu items */
typedef enum { NfcPlaylistMenuSelection_One } NfcPlaylistMenuSelection;

/** main menu callback - sends a custom event to the scene manager based on the menu selection */
void nfc_playlist_menu_callback_main_menu(void* context, uint32_t index) {
   FURI_LOG_T(TAG, "nfc_playlist_menu_callback_main_menu");
   NfcPlaylist* app = context;
   switch(index) {
   case NfcPlaylistMenuSelection_One:
      scene_manager_handle_custom_event(app->scene_manager, NfcPlaylistEvent_ShowPopupOne);
      break;
   }
}

/** resets the menu, gives it content, callbacks and selection enums */
void nfc_playlist_scene_on_enter_main_menu(void* context) {
   FURI_LOG_T(TAG, "nfc_playlist_scene_on_enter_main_menu");
   NfcPlaylist* app = context;
   submenu_reset(app->menu);
   submenu_set_header(app->menu, "NFC Playlist");
   submenu_add_item(
      app->menu,
      "Start",
      NfcPlaylistMenuSelection_One,
      nfc_playlist_menu_callback_main_menu,
      app);
   view_dispatcher_switch_to_view(app->view_dispatcher, NfcPlaylistView_Menu);
}

/** main menu event handler - switches scene based on the event */
bool nfc_playlist_scene_on_event_main_menu(void* context, SceneManagerEvent event) {
   FURI_LOG_T(TAG, "nfc_playlist_scene_on_event_main_menu");
   NfcPlaylist* app = context;
   bool consumed = false;
   switch(event.type) {
   case SceneManagerEventTypeCustom:
      switch(event.event) {
      case NfcPlaylistEvent_ShowPopupOne:
         scene_manager_next_scene(app->scene_manager, NfcPlaylistScene_FirstPopup);
         consumed = true;
         break;
      }
      break;
   default: // eg. SceneManagerEventTypeBack, SceneManagerEventTypeTick
      consumed = false;
      break;
   }
   return consumed;
}

void nfc_playlist_scene_on_exit_main_menu(void* context) {
   FURI_LOG_T(TAG, "nfc_playlist_scene_on_exit_main_menu");
   NfcPlaylist* app = context;
   submenu_reset(app->menu);
}

/* popup 1 scene */

void nfc_playlist_scene_on_enter_popup_emulating(void* context) {
   FURI_LOG_T(TAG, "nfc_playlist_scene_on_enter_popup_emulating");
   NfcPlaylist* app = context;

   // open/alloc resources
   Storage* storage = furi_record_open(RECORD_STORAGE);
   Stream* stream = file_stream_alloc(storage);
   FuriString* line = furi_string_alloc();
   NfcPlaylistWorker* nfc_worker = nfc_playlist_worker_alloc();
   // Read file
   if(file_stream_open(stream, APP_DATA_PATH("playlist.txt"), FSAM_READ, FSOM_OPEN_EXISTING)) {
      popup_reset(app->popup);
      // read the file line by line and print the text
      while(stream_read_line(stream, line)) {

         char* str = (char*)furi_string_get_cstr(line);
         
         popup_set_context(app->popup, app);
         popup_set_header(app->popup, "Emulating", 64, 10, AlignCenter, AlignTop);
         popup_set_text(app->popup, str, 64, 30, AlignCenter, AlignTop);
         view_dispatcher_switch_to_view(app->view_dispatcher, NfcPlaylistView_Popup);

         nfc_playlist_worker_set_nfc_data(nfc_worker, str);
         nfc_playlist_worker_start(nfc_worker);

         furi_delay_ms(app->emulate_timeout);

         nfc_playlist_worker_stop(nfc_worker);
         furi_string_reset(line);

      }
   } else {
      FURI_LOG_E(TAG, "Failed to open file");
   }
   // Free/close resources
   furi_string_free(line);
   file_stream_close(stream);
   stream_free(stream);
   nfc_playlist_worker_free(nfc_worker);
   // Close storage
   furi_record_close(RECORD_STORAGE);

   popup_reset(app->popup);
   scene_manager_previous_scene(app->scene_manager);
}

bool nfc_playlist_scene_on_event_popup_emulating(void* context, SceneManagerEvent event) {
   FURI_LOG_T(TAG, "nfc_playlist_scene_on_event_popup_emulating");
   UNUSED(context);
   UNUSED(event);
   return false;
}

void nfc_playlist_scene_on_exit_popup_emulating(void* context) {
   FURI_LOG_T(TAG, "nfc_playlist_scene_on_exit_popup_emulating");
   NfcPlaylist* app = context;
   popup_reset(app->popup);
}

/** collection of all scene on_enter handlers - in the same order as their enum */
void (*const nfc_playlist_scene_on_enter_handlers[])(void*) = {
   nfc_playlist_scene_on_enter_main_menu,
   nfc_playlist_scene_on_enter_popup_emulating};

/** collection of all scene on event handlers - in the same order as their enum */
bool (*const nfc_playlist_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
   nfc_playlist_scene_on_event_main_menu,
   nfc_playlist_scene_on_event_popup_emulating};

/** collection of all scene on exit handlers - in the same order as their enum */
void (*const nfc_playlist_scene_on_exit_handlers[])(void*) = {
   nfc_playlist_scene_on_exit_main_menu,
   nfc_playlist_scene_on_exit_popup_emulating};

/** collection of all on_enter, on_event, on_exit handlers */
const SceneManagerHandlers nfc_playlist_scene_event_handlers = {
   .on_enter_handlers = nfc_playlist_scene_on_enter_handlers,
   .on_event_handlers = nfc_playlist_scene_on_event_handlers,
   .on_exit_handlers = nfc_playlist_scene_on_exit_handlers,
   .scene_num = NfcPlaylistScene_count};

/** custom event handler - passes the event to the scene manager */
bool nfc_playlist_scene_manager_custom_event_callback(void* context, uint32_t custom_event) {
   FURI_LOG_T(TAG, "nfc_playlist_scene_manager_custom_event_callback");
   furi_assert(context);
   NfcPlaylist* app = context;
   return scene_manager_handle_custom_event(app->scene_manager, custom_event);
}

/** navigation event handler - passes the event to the scene manager */
bool nfc_playlist_scene_manager_navigation_event_callback(void* context) {
   FURI_LOG_T(TAG, "nfc_playlist_scene_manager_navigation_event_callback");
   furi_assert(context);
   NfcPlaylist* app = context;
   return scene_manager_handle_back_event(app->scene_manager);
}

/** initialise the scene manager with all handlers */
void nfc_playlist_scene_manager_init(NfcPlaylist* app) {
   FURI_LOG_T(TAG, "nfc_playlist_scene_manager_init");
   app->scene_manager = scene_manager_alloc(&nfc_playlist_scene_event_handlers, app);
}

/** initialise the views, and initialise the view dispatcher with all views */
void nfc_playlist_view_dispatcher_init(NfcPlaylist* app) {
   FURI_LOG_T(TAG, "nfc_playlist_view_dispatcher_init");
   app->view_dispatcher = view_dispatcher_alloc();
   view_dispatcher_enable_queue(app->view_dispatcher);

   // allocate each view
   FURI_LOG_D(TAG, "nfc_playlist_view_dispatcher_init allocating views");
   app->menu = submenu_alloc();
   app->popup = popup_alloc();
   app->emulate_timeout = 2000;

   // assign callback that pass events from views to the scene manager
   FURI_LOG_D(TAG, "nfc_playlist_view_dispatcher_init setting callbacks");
   view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
   view_dispatcher_set_custom_event_callback(
      app->view_dispatcher, nfc_playlist_scene_manager_custom_event_callback);
   view_dispatcher_set_navigation_event_callback(
      app->view_dispatcher, nfc_playlist_scene_manager_navigation_event_callback);

   // add views to the dispatcher, indexed by their enum value
   FURI_LOG_D(TAG, "nfc_playlist_view_dispatcher_init adding view menu");
   view_dispatcher_add_view(app->view_dispatcher, NfcPlaylistView_Menu, submenu_get_view(app->menu));

   FURI_LOG_D(TAG, "nfc_playlist_view_dispatcher_init adding view popup");
   view_dispatcher_add_view(app->view_dispatcher, NfcPlaylistView_Popup, popup_get_view(app->popup));
}

/** initialise app data, scene manager, and view dispatcher */
NfcPlaylist* nfc_playlist_init() {
   FURI_LOG_T(TAG, "nfc_playlist_init");
   NfcPlaylist* app = malloc(sizeof(NfcPlaylist));
   nfc_playlist_scene_manager_init(app);
   nfc_playlist_view_dispatcher_init(app);
   return app;
}

/** free all app data, scene manager, and view dispatcher */
void nfc_playlist_free(NfcPlaylist* app) {
   FURI_LOG_T(TAG, "nfc_playlist_free");
   scene_manager_free(app->scene_manager);
   view_dispatcher_remove_view(app->view_dispatcher, NfcPlaylistView_Menu);
   view_dispatcher_remove_view(app->view_dispatcher, NfcPlaylistView_Popup);
   view_dispatcher_free(app->view_dispatcher);
   submenu_free(app->menu);
   popup_free(app->popup);
   free(app);
}

/** go to trace log level in the dev environment */
void nfc_playlist_set_log_level() {
#ifdef FURI_DEBUG
   furi_log_set_level(FuriLogLevelTrace);
#else
   furi_log_set_level(FuriLogLevelInfo);
#endif
}

// Application entry point
int32_t nfc_playlist_main(void* p) {
   // Mark argument as unused
   UNUSED(p);

   nfc_playlist_set_log_level();

   // create the app context struct, scene manager, and view dispatcher
   FURI_LOG_I(TAG, "Test app starting...");
   NfcPlaylist* app = nfc_playlist_init();

   // set the scene and launch the main loop
   Gui* gui = furi_record_open(RECORD_GUI);
   view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
   scene_manager_next_scene(app->scene_manager, NfcPlaylistScene_MainMenu);
   FURI_LOG_D(TAG, "Starting dispatcher...");
   view_dispatcher_run(app->view_dispatcher);

   // free all memory
   FURI_LOG_I(TAG, "Test app finishing...");
   furi_record_close(RECORD_GUI);
   nfc_playlist_free(app);

   return 0;
}