#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/widget.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>
#include <lib/toolbox/random_name.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <dialogs/dialogs.h>
#include <storage/storage.h>

extern const GpioPin* const pinD0;
extern const GpioPin* const pinD1;
extern volatile int bit_count;
#define MAX_BITS 42
extern volatile bool data[];
extern volatile uint32_t data_fall[];
extern volatile uint32_t data_rise[];
extern bool data_saved;

#define WIEGAND_KEY_NAME_SIZE 25
#define WIEGAND_APPS_DATA_FOLDER EXT_PATH("apps_data")
#define WIEGAND_SAVE_FOLDER      \
    WIEGAND_APPS_DATA_FOLDER "/" \
                             "wiegand"
#define WIEGAND_SAVE_EXTENSION ".wgn"

#define TAG "WIEGAND_APP"

typedef enum {
    WiegandMainMenuScene,
    WiegandInstructionsScene,
    WiegandReadScene,
    WiegandDataScene,
    WiegandSaveScene,
    WiegandLoadScene,
    WiegandSceneCount,
} WiegandScene;

typedef enum {
    WiegandSubmenuView,
    WiegandWidgetView,
    WiegandTextInputView,
} WiegandView;

typedef struct App {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    Widget* widget;
    TextInput* text_input;
    DialogsApp* dialogs;
    FuriString* file_path;
    char file_name[WIEGAND_KEY_NAME_SIZE];
    FuriTimer* timer;
} App;

typedef enum {
    WiegandMainMenuInstructions,
    WiegandMainMenuRead,
    WiegandMainMenuLoad,
} WiegandMainMenuSceneIndex;

typedef enum {
    WiegandMainMenuUnknownEvent,
    WiegandMainMenuInstructionsEvent,
    WiegandMainMenuReadEvent,
    WiegandMainMenuLoadEvent,
} WiegandMainMenuEvent;

typedef enum {
    WiegandDataSceneSaveButtonEvent,
    WiegandDataScenePlayButtonEvent,
    WiegandDataSceneSaveFileEvent,
} WiegandDataEvent;

void wiegand_menu_callback(void* context, uint32_t index);

void wiegand_main_menu_scene_on_enter(void* context);
bool wiegand_main_menu_scene_on_event(void* context, SceneManagerEvent event);
// void wiegand_main_menu_scene_on_exit(void* context);

void wiegand_instructions_scene_on_enter(void* context);
// bool wiegand_instructions_scene_on_event(void* context, SceneManagerEvent event);
// void wiegand_instructions_scene_on_exit(void* context);
void wiegand_isr_d0(void* context);
void wiegand_isr_d1(void* context);
void wiegand_start_read(void* context);
void wiegand_stop_read(void* context);
void wiegand_timer_callback(void* context);
void wiegand_read_scene_on_enter(void* context);
// bool wiegand_read_scene_on_event(void* context, SceneManagerEvent event);
void wiegand_read_scene_on_exit(void* context);
void wiegand_add_info_4bit_8bit(FuriString* buffer);
void wiegand_add_info_26bit(FuriString* buffer);
void wiegand_add_info_24bit(FuriString* buffer);
void wiegand_add_info(FuriString* buffer);
void single_vibro();
void wiegand_play();
void wiegand_button_callback(GuiButtonType result, InputType type, void* context);
void wiegand_data_scene_on_enter(void* context);
bool wiegand_data_scene_on_event(void* context, SceneManagerEvent event);
// void wiegand_data_scene_on_exit(void* context);
void wiegand_data_scene_save_name_text_input_callback(void* context);
void ensure_dir_exists(Storage* storage);
void wiegand_save(void* context);
void wiegand_save_scene_on_enter(void* context);
bool wiegand_save_scene_on_event(void* context, SceneManagerEvent event);
// void wiegand_save_scene_on_exit(void* context);
void wiegand_load_scene_on_enter(void* context);
// bool wiegand_load_scene_on_event(void* context, SceneManagerEvent event);
// void wiegand_load_scene_on_exit(void* context);
bool wiegand_custom_callback(void* context, uint32_t custom_event);
bool wiegand_back_event_callback(void* context);

// Methods with no implementation.
bool wiegand_empty_scene_on_event(void* context, SceneManagerEvent event);
void wiegand_empty_scene_on_exit(void* _ctx);

App* app_alloc();
void app_free(void* context);
