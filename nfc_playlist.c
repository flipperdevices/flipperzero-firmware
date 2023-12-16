#include <furi.h>
#include <string.h>
#include <storage/storage.h>
#include <toolbox/stream/stream.h>
#include <toolbox/stream/file_stream.h>
#include <nfc_playlist_worker.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/popup.h>
#include <gui/modules/variable_item_list.h>

// Define log tag
#define TAG "NfcPlaylist"

// IDs for all scenes used by the app
typedef enum {
   NfcPlaylistScene_MainMenu,
   NfcPlaylistScene_EmulatingPopup,
   NfcPlaylistScene_count
} NfcPlaylistScene;

// The app context struct
typedef struct {
   SceneManager* scene_manager;
   ViewDispatcher* view_dispatcher;
   VariableItemList* variable_item_list;
   Popup* popup;
   NfcPlaylistWorker* nfc_worker;
   uint8_t emulate_timeout;
   uint8_t emulate_delay;
} NfcPlaylist;

// All options for the emulate timeout and delay
const int options_emulate_timeout[] = { 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000 };
const int options_emulate_delay[] = { 0000, 1000, 2000, 3000, 4000, 5000 };

/* main menu scene */
// Indices for menu items
typedef enum { NfcPlaylistSettings_Timeout, NfcPlaylistSettings_Delay, NfcPlaylistMenuSelection_Start } NfcPlaylistMenuSelection;
// All custom events
typedef enum { NfcPlaylistEvent_ShowEmulatingPopup } NfcPlaylistEvent;
// IDs for the view used by the app
typedef enum { NfcPlaylistView_Menu, NfcPlaylistView_Popup } NfcPlaylistView;

// Main menu callback - sends a custom event to the scene manager based on the menu selection
void nfc_playlist_menu_callback_main_menu(void* context, uint32_t index) {
   FURI_LOG_T(TAG, "nfc_playlist_menu_callback_main_menu");
   NfcPlaylist* app = context;
   switch(index) {
      case NfcPlaylistMenuSelection_Start:
         scene_manager_handle_custom_event(app->scene_manager, NfcPlaylistEvent_ShowEmulatingPopup);
         break;
      default:
         break;
   }
}

static void nfc_playlist_settings_change_callback(VariableItem* item) {
   NfcPlaylist* app = variable_item_get_context(item);

   uint8_t current_option = variable_item_list_get_selected_item_index(app->variable_item_list);
   uint8_t option_value_index = variable_item_get_current_value_index(item);

   switch(current_option) {
      case NfcPlaylistSettings_Timeout: ;
         app->emulate_timeout = option_value_index;
         char emulate_timeout_text[9];
         snprintf(emulate_timeout_text, 9, "%d", (options_emulate_timeout[option_value_index]/1000));
         variable_item_set_current_value_text(item, (char*)emulate_timeout_text);
         break;
      case NfcPlaylistSettings_Delay: ;
         app->emulate_delay = option_value_index;
         char emulate_delay_text[9];
         snprintf(emulate_delay_text, 9, "%d", (options_emulate_delay[option_value_index]/1000));
         variable_item_set_current_value_text(item, (char*)emulate_delay_text);
         break;
   }
}

// Resets the menu, gives it content, callbacks and selection enums
void nfc_playlist_scene_on_enter_main_menu(void* context) {
   FURI_LOG_T(TAG, "nfc_playlist_scene_on_enter_main_menu");
   NfcPlaylist* app = context;
   variable_item_list_set_header(app->variable_item_list, "NFC Playlist");
   VariableItem* emulation_timeout_settings = variable_item_list_add(
      app->variable_item_list,
      "Timeout",
      10,
      nfc_playlist_settings_change_callback,
      app);
   variable_item_set_current_value_index(emulation_timeout_settings, app->emulate_timeout);
   char emulation_timeout_settings_text[9];
   snprintf(emulation_timeout_settings_text, 9, "%d", (options_emulate_timeout[app->emulate_timeout]/1000));
   variable_item_set_current_value_text(emulation_timeout_settings, (char*)emulation_timeout_settings_text);
   VariableItem* emulation_delay_settings = variable_item_list_add(
      app->variable_item_list,
      "Delay",
      6,
      nfc_playlist_settings_change_callback,
      app);
   variable_item_set_current_value_index(emulation_delay_settings, app->emulate_delay);
   char emulation_delay_settings_text[9];
   snprintf(emulation_delay_settings_text, 9, "%d", (options_emulate_delay[app->emulate_delay]/1000));
   variable_item_set_current_value_text(emulation_delay_settings, (char*)emulation_delay_settings_text);
   variable_item_list_add(app->variable_item_list, "Start", 0, NULL, NULL);
   variable_item_list_set_enter_callback(app->variable_item_list, nfc_playlist_menu_callback_main_menu, app);
   view_dispatcher_switch_to_view(app->view_dispatcher, NfcPlaylistView_Menu);
}

// Main menu event handler - switches scene based on the event
bool nfc_playlist_scene_on_event_main_menu(void* context, SceneManagerEvent event) {
   FURI_LOG_T(TAG, "nfc_playlist_scene_on_event_main_menu");
   NfcPlaylist* app = context;
   bool consumed = false;
   switch(event.type) {
      case SceneManagerEventTypeCustom:
         switch(event.event) {
            case NfcPlaylistEvent_ShowEmulatingPopup:
               scene_manager_next_scene(app->scene_manager, NfcPlaylistScene_EmulatingPopup);
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
   variable_item_list_reset(app->variable_item_list);
}

// Emulating scene
void nfc_playlist_scene_on_enter_popup_emulating(void* context) {
   FURI_LOG_T(TAG, "nfc_playlist_scene_on_enter_popup_emulating");
   NfcPlaylist* app = context;

   // open/alloc resources
   Storage* storage = furi_record_open(RECORD_STORAGE);
   Stream* stream = file_stream_alloc(storage);
   FuriString* line = furi_string_alloc();
   app->nfc_worker = nfc_playlist_worker_alloc();
   // Read file
   if(file_stream_open(stream, APP_DATA_PATH("playlist.txt"), FSAM_READ, FSOM_OPEN_EXISTING)) {
      popup_reset(app->popup);
      popup_set_context(app->popup, app);
      popup_set_header(app->popup, "Emulating:", 64, 10, AlignCenter, AlignTop);
      view_dispatcher_switch_to_view(app->view_dispatcher, NfcPlaylistView_Popup);

      int file_position = 0;
      // read the file line by line and print the text
      while(stream_read_line(stream, line)) {
         if (options_emulate_delay[app->emulate_delay] > 0) {
            if (file_position > 0) {
               int time_counter_delay_ms = options_emulate_timeout[app->emulate_delay];
               do {
                  char display_text[30];
                  snprintf(display_text, 30, "%s\n\n%ds", "Delaying...", (time_counter_delay_ms/1000));
                  popup_set_text(app->popup, display_text, 64, 25, AlignCenter, AlignTop);
                  furi_delay_ms(500);
                  time_counter_delay_ms -= 500;
               } while(time_counter_delay_ms > 0);
            } else {
               file_position++;
            }
         }

         char* file_path = (char*)furi_string_get_cstr(line);
         char* file_name = &strrchr(file_path, '/')[1];
         int time_counter_ms = options_emulate_timeout[app->emulate_timeout];

         if (storage_file_exists(storage, file_path) == false) {
            char* popup_text_unformatted = strcat(file_name, "\nnot found");
            int popup_text_size = (strlen(popup_text_unformatted) + 4);
            char popup_text[popup_text_size];

            do {
               snprintf(popup_text, popup_text_size, "%s\n%ds", file_name, (time_counter_ms/1000));
               popup_set_text(app->popup, popup_text, 64, 25, AlignCenter, AlignTop);
               furi_delay_ms(500);
               time_counter_ms -= 500;
            } while(time_counter_ms > 0);
         } else {
            nfc_playlist_worker_set_nfc_data(app->nfc_worker, file_path);
            nfc_playlist_worker_start(app->nfc_worker);

            int popup_text_size = (strlen(file_name) + 4);
            char popup_text[popup_text_size];

            do {
               snprintf(popup_text, popup_text_size, "%s\n%ds", file_name, (time_counter_ms/1000));
               popup_set_text(app->popup, popup_text, 64, 25, AlignCenter, AlignTop);
               furi_delay_ms(500);
               time_counter_ms -= 500;
            } while(nfc_playlist_worker_is_emulating(app->nfc_worker) && time_counter_ms > 0);

            if (nfc_playlist_worker_is_emulating(app->nfc_worker)) {
               nfc_playlist_worker_stop(app->nfc_worker);
            }
         }
      }
      popup_reset(app->popup);
      scene_manager_previous_scene(app->scene_manager);
   } else {
      popup_reset(app->popup);
      popup_set_context(app->popup, app);
      popup_set_header(app->popup, "Error:", 64, 10, AlignCenter, AlignTop);
      popup_set_text(app->popup, "Failed to open file\n/ext/apps_data/nfc_playlist/playlist.txt", 64, 25, AlignCenter, AlignTop);
      view_dispatcher_switch_to_view(app->view_dispatcher, NfcPlaylistView_Popup);
      FURI_LOG_E(TAG, "Failed to open file");
   }
   // Free/close resources
   furi_string_free(line);
   file_stream_close(stream);
   stream_free(stream);
   nfc_playlist_worker_free(app->nfc_worker);
   app->nfc_worker = NULL;
   // Close storage
   furi_record_close(RECORD_STORAGE);
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

// Collection of all scene on_enter handlers - in the same order as their enum
void (*const nfc_playlist_scene_on_enter_handlers[])(void*) = {
   nfc_playlist_scene_on_enter_main_menu,
   nfc_playlist_scene_on_enter_popup_emulating};

// Collection of all scene on event handlers - in the same order as their enum
bool (*const nfc_playlist_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
   nfc_playlist_scene_on_event_main_menu,
   nfc_playlist_scene_on_event_popup_emulating};

// Collection of all scene on exit handlers - in the same order as their enum
void (*const nfc_playlist_scene_on_exit_handlers[])(void*) = {
   nfc_playlist_scene_on_exit_main_menu,
   nfc_playlist_scene_on_exit_popup_emulating};

// Collection of all on_enter, on_event, on_exit handlers */
const SceneManagerHandlers nfc_playlist_scene_event_handlers = {
   .on_enter_handlers = nfc_playlist_scene_on_enter_handlers,
   .on_event_handlers = nfc_playlist_scene_on_event_handlers,
   .on_exit_handlers = nfc_playlist_scene_on_exit_handlers,
   .scene_num = NfcPlaylistScene_count};

// Custom event handler - passes the event to the scene manager
bool nfc_playlist_scene_manager_custom_event_callback(void* context, uint32_t custom_event) {
   FURI_LOG_T(TAG, "nfc_playlist_scene_manager_custom_event_callback");
   furi_assert(context);
   NfcPlaylist* app = context;
   return scene_manager_handle_custom_event(app->scene_manager, custom_event);
}

// Navigation event handler - passes the event to the scene manager
bool nfc_playlist_scene_manager_navigation_event_callback(void* context) {
   FURI_LOG_T(TAG, "nfc_playlist_scene_manager_navigation_event_callback");
   furi_assert(context);
   NfcPlaylist* app = context;
   return scene_manager_handle_back_event(app->scene_manager);
}

// Initialise the scene manager with all handlers
void nfc_playlist_scene_manager_init(NfcPlaylist* app) {
   FURI_LOG_T(TAG, "nfc_playlist_scene_manager_init");
   app->scene_manager = scene_manager_alloc(&nfc_playlist_scene_event_handlers, app);
}

// Initialise the views, and initialise the view dispatcher with all views
void nfc_playlist_view_dispatcher_init(NfcPlaylist* app) {
   FURI_LOG_T(TAG, "nfc_playlist_view_dispatcher_init");
   app->view_dispatcher = view_dispatcher_alloc();
   view_dispatcher_enable_queue(app->view_dispatcher);

   // allocate each view
   FURI_LOG_D(TAG, "nfc_playlist_view_dispatcher_init allocating views");
   app->variable_item_list = variable_item_list_alloc();
   app->popup = popup_alloc();
   app->emulate_timeout = 4;
   app->emulate_delay = 0;

   // assign callback that pass events from views to the scene manager
   FURI_LOG_D(TAG, "nfc_playlist_view_dispatcher_init setting callbacks");
   view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
   view_dispatcher_set_custom_event_callback( app->view_dispatcher, nfc_playlist_scene_manager_custom_event_callback);
   view_dispatcher_set_navigation_event_callback(app->view_dispatcher, nfc_playlist_scene_manager_navigation_event_callback);

   // add views to the dispatcher, indexed by their enum value
   FURI_LOG_D(TAG, "nfc_playlist_view_dispatcher_init adding view menu");
   view_dispatcher_add_view(app->view_dispatcher, NfcPlaylistView_Menu, variable_item_list_get_view(app->variable_item_list));

   FURI_LOG_D(TAG, "nfc_playlist_view_dispatcher_init adding view popup");
   view_dispatcher_add_view(app->view_dispatcher, NfcPlaylistView_Popup, popup_get_view(app->popup));
}

// Initialise app data, scene manager, and view dispatcher
NfcPlaylist* nfc_playlist_init() {
   FURI_LOG_T(TAG, "nfc_playlist_init");
   NfcPlaylist* app = malloc(sizeof(NfcPlaylist));
   nfc_playlist_scene_manager_init(app);
   nfc_playlist_view_dispatcher_init(app);
   return app;
}

// Free all app data, scene manager, and view dispatcher
void nfc_playlist_free(NfcPlaylist* app) {
   FURI_LOG_T(TAG, "nfc_playlist_free");
   scene_manager_free(app->scene_manager);
   view_dispatcher_remove_view(app->view_dispatcher, NfcPlaylistView_Menu);
   view_dispatcher_remove_view(app->view_dispatcher, NfcPlaylistView_Popup);
   view_dispatcher_free(app->view_dispatcher);
   variable_item_list_free(app->variable_item_list);
   popup_free(app->popup);
   free(app);
}

// Go to trace log level in the dev environment
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
   FURI_LOG_I(TAG, "NFC PLaylist starting...");
   NfcPlaylist* app = nfc_playlist_init();

   // set the scene and launch the main loop
   Gui* gui = furi_record_open(RECORD_GUI);
   view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
   scene_manager_next_scene(app->scene_manager, NfcPlaylistScene_MainMenu);
   FURI_LOG_D(TAG, "Starting dispatcher...");
   view_dispatcher_run(app->view_dispatcher);

   // free all memory
   FURI_LOG_I(TAG, "NFC PLaylist finishing...");
   furi_record_close(RECORD_GUI);
   nfc_playlist_free(app);

   return 0;
}