#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/widget.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>
#include <lib/toolbox/stream/file_stream.h>
#include "cmd.h"
#include <stdlib.h>

#define UHF_APPS_DATA_FOLDER EXT_PATH("apps_data")
#define UHF_LOG_FOLDER       \
    UHF_APPS_DATA_FOLDER "/" \
                         "uhf"
#define UHF_LOG_FILE_NAME "log.txt"
#define UHF_LOG_PATH UHF_LOG_FOLDER "/" UHF_LOG_FILE_NAME
#define SCENE_COUNT 2

bool logged = false;
// volatile int array_ptr = 0;
// uint8_t array_log[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
//                        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

char* convertToHexString(const uint8_t* array, size_t length) {
    if(array == NULL || length == 0) {
        return NULL;
    }

    // Each byte takes 3 characters in the hex representation (2 characters + space), plus 1 for the null terminator
    size_t hexLength = (length * 3) + 1;

    char* hexArray = (char*)malloc(hexLength * sizeof(char));
    if(hexArray == NULL) {
        return NULL;
    }

    size_t index = 0;
    for(size_t i = 0; i < length; i++) {
        index += snprintf(&hexArray[index], hexLength - index, "%02x ", array[i]);
    }

    hexArray[hexLength - 1] = '\0';

    return hexArray;
}

void store_message(char* message) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    if(!storage_dir_exists(storage, UHF_APPS_DATA_FOLDER)) {
        storage_simply_mkdir(storage, UHF_APPS_DATA_FOLDER);
    }

    if(!storage_dir_exists(storage, UHF_LOG_FOLDER)) {
        storage_simply_mkdir(storage, UHF_LOG_FOLDER);
    }

    File* file = storage_file_alloc(storage);
    if(storage_file_open(file, UHF_LOG_PATH, FSAM_WRITE, FSOM_OPEN_APPEND)) {
        if(!logged) {
            storage_file_seek(file, 0, true);
            storage_file_truncate(file);
            logged = true;
        }
        storage_file_write(file, message, strlen(message));
        storage_file_write(file, "\n", 1);
        storage_file_close(file);
        storage_file_free(file);
    }
    furi_record_close(RECORD_STORAGE);
}

typedef enum { STARTED, RUNNING, STOPPED } UHFRFIDWorkerState;
typedef enum { SEND_READ_CMD, SEND_WRITE_CMD } UHFRFIDWorkerCMD;

// typedef void (*UHFRFIDWorkerCallback)();
typedef struct UHFReturnData {
    uint8_t* data;
    size_t length;
} UHFReturnData;

UHFReturnData* uhf_return_data_alloc() {
    UHFReturnData* uhf_data = (UHFReturnData*)malloc(sizeof(UHFReturnData));
    uhf_data->data = (uint8_t*)malloc(24 * sizeof(uint8_t));
    uhf_data->length = 0;
    return uhf_data;
}

void uhf_return_data_free(UHFReturnData* uhf_data) {
    free(uhf_data->data);
}

typedef struct UHFRFIDWorker {
    FuriThread* thread;
    UHFReturnData* uhf_data;
    // UHFRFIDWorkerCMD cmd;
    // Storage* storage;
    // UHFRFIDWorkerCallback callback;

} UHFRFIDWorker;

typedef enum { MainMenuRead, MainMenuSaved } MainMenuSceneIndex;
typedef enum { MainMenuReadEvent, MainMenuSavedEvent } MainMenuEvent;

void uhf_rfid_worker_enable_field() {
}

void uhf_rfid_worker_disable_field() {
}

// uint8_t* uhf_rfid_read() {
//     return 0;
// }

void rx_callback(UartIrqEvent event, uint8_t data, void* ctx) {
    UNUSED(event);
    UHFReturnData* uhf_data = ctx;
    uhf_data->data[uhf_data->length++] = data;
    // UNUSED(ctx);
    // UNUSED(data);
    // array_log[array_ptr++] = data;
}

int32_t uhf_rfid_worker_task(void* ctx) {
    UHFRFIDWorker* worker = ctx;
    // UHFRFIDWorker* worker = ctx;
    // uhf_rfid_worker_enable_field();
    // array_ptr = 0;
    furi_hal_console_disable();
    furi_hal_uart_set_br(FuriHalUartIdUSART1, 115200);
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, rx_callback, worker->uhf_data);
    // ArrayList* data_list = worker->context;
    // int prev_size = data_list->size;
    // switch(worker->cmd) {
    // case SEND_READ_CMD:
    //     // furi_hal_console_enable();
    //     // FURI_LOG_E("TAG", "SENDING DATA");
    //     // FURI_LOG_E(
    //     //     "TAG",
    //     //     "%02x%02x%02x%02x%02x%02x%02x",
    //     //     CMD_SINGLE_POLLING.cmd[0],
    //     //     CMD_SINGLE_POLLING.cmd[1],
    //     //     CMD_SINGLE_POLLING.cmd[2],
    //     //     CMD_SINGLE_POLLING.cmd[3],
    //     //     CMD_SINGLE_POLLING.cmd[4],
    //     //     CMD_SINGLE_POLLING.cmd[5],
    //     //     CMD_SINGLE_POLLING.cmd[6]);
    //     // furi_hal_console_disable();
    //     break;
    // default:
    //     break;
    // }
    furi_hal_uart_tx(FuriHalUartIdUSART1, CMD_SINGLE_POLLING.cmd, CMD_SINGLE_POLLING.length);
    // while(prev_size == 0 || !(prev_size != data_list->size)) {
    //     furi_delay_ms(50);
    // }
    // furi_hal_console_enable();
    // FURI_LOG_E("TAG", "DATA READ %s", array_list_to_string(data_list));
    // furi_delay_ms(2000);
    return 0;
}
UHFRFIDWorker* uhf_rfid_worker_alloc() {
    UHFRFIDWorker* uhf_rfid_worker = malloc(sizeof(UHFRFIDWorker));
    uhf_rfid_worker->thread =
        furi_thread_alloc_ex("UHFRFIDWorker", 8 * 1024, uhf_rfid_worker_task, uhf_rfid_worker);
    // uhf_rfid_worker->cmd = SEND_READ_CMD;
    uhf_rfid_worker->uhf_data = uhf_return_data_alloc();
    return uhf_rfid_worker;
}

void uhf_rfid_worker_free(UHFRFIDWorker* uhf_rfid_worker) {
    furi_assert(uhf_rfid_worker);
    furi_thread_free(uhf_rfid_worker->thread);
    // ArrayList* data_list = uhf_rfid_worker->context;
    // array_list_free(data_list);

    // furi_record_close(RECORD_STORAGE);

    free(uhf_rfid_worker);
}

// void uhf_rfid_worker_change_state(UHFRFIDWorker* worker, UHFRFIDWorkerState state) {
//     worker->state = state;
// }
void uhf_rfid_worker_start(UHFRFIDWorker* uhf_rfid_worker, UHFRFIDWorkerState state, void* ctx) {
    UNUSED(state);
    UNUSED(ctx);
    // uhf_rfid_worker_change_state(uhf_rfid_worker, state);
    furi_thread_start(uhf_rfid_worker->thread);
}

void uhf_rfid_worker_stop(UHFRFIDWorker* uhf_rfid_worker) {
    furi_assert(uhf_rfid_worker);
    furi_assert(uhf_rfid_worker->thread);

    if(furi_thread_get_state(uhf_rfid_worker->thread) != FuriThreadStateStopped) {
        // uhf_rfid_worker_change_state(uhf_rfid_worker, STOPPED);
        furi_thread_join(uhf_rfid_worker->thread);
    }
}

typedef enum { MainMenuScene, WaitingReadScene, ReadSuccessScene } UHF_RFID_Scene;

typedef enum { SubmenuView, WidgetView, TextInputView } UHF_RFID_Scene_View;

typedef struct UHFRFIDApp {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    Widget* widget;
    TextInput* text_input;
} UHFRFIDApp;

void scenes_menu_callback(void* ctx, uint32_t index) {
    UHFRFIDApp* app = ctx;
    switch(index) {
    case MainMenuRead:
        scene_manager_handle_custom_event(app->scene_manager, MainMenuReadEvent);
        break;
    case MainMenuSaved:
        // not implemented yet
        furi_hal_console_enable();
        FURI_LOG_E("TAG", "Saved was pressed");
        furi_hal_console_disable();
        break;
    }
}

void scene_main_menu_on_enter(void* ctx) {
    UHFRFIDApp* app = ctx;
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "[UHF]RFID App");
    submenu_add_item(app->submenu, "Read", MainMenuRead, scenes_menu_callback, app);
    submenu_add_item(app->submenu, "Saved", MainMenuSaved, scenes_menu_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, SubmenuView);
}

bool scene_main_menu_on_event(void* ctx, SceneManagerEvent event) {
    UHFRFIDApp* app = ctx;
    bool consumed = false;
    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case MainMenuReadEvent:
            scene_manager_next_scene(app->scene_manager, WaitingReadScene);
            consumed = true;
            break;
        case MainMenuSavedEvent:
            // TODO : not implemented yet
            break;
        }
    default:
        break;
    }
    return consumed;
}

void scene_main_menu_on_exit(void* ctx) {
    UHFRFIDApp* app = ctx;
    submenu_reset(app->submenu);
}

void scene_read_on_enter(void* ctx) {
    UHFRFIDApp* app = ctx;
    widget_reset(app->widget);
    UHFRFIDWorker* worker = uhf_rfid_worker_alloc();
    UHFReturnData* uhf_data = worker->uhf_data;
    uhf_rfid_worker_start(worker, RUNNING, NULL);
    furi_delay_ms(1000);
    // UNUSED(uhf_data);
    char* values = convertToHexString(uhf_data->data, uhf_data->length);
    // UNUSED(values);
    if(values != NULL) {
        store_message(values);
        widget_add_string_element(
            app->widget, 25, 15, AlignCenter, AlignCenter, FontPrimary, values);
    } else {
        store_message("No data found");
        widget_add_string_element(
            app->widget, 25, 15, AlignCenter, AlignCenter, FontPrimary, "No data found");
    }
    // furi_hal_console_enable();
    // FURI_LOG_E("TAG", "data %s", values);

    view_dispatcher_switch_to_view(app->view_dispatcher, WidgetView);
    // furi_delay_ms(2000);
    uhf_rfid_worker_stop(worker);
    uhf_return_data_free(worker->uhf_data);
    uhf_rfid_worker_free(worker);
}

bool scene_read_on_event(void* ctx, SceneManagerEvent event) {
    UNUSED(ctx);
    UNUSED(event);
    return false; // event not handled.
}

void scene_read_on_exit(void* ctx) {
    UNUSED(ctx);
}

bool scene_custom_callback(void* ctx, uint32_t custom_event) {
    furi_assert(ctx);
    UHFRFIDApp* app = ctx;
    return scene_manager_handle_custom_event(app->scene_manager, custom_event);
};

bool scene_back_event_callback(void* ctx) {
    furi_assert(ctx);
    UHFRFIDApp* app = ctx;
    return scene_manager_handle_back_event(app->scene_manager);
}

void (*const scene_on_enter_handlers[])(void*) = {scene_main_menu_on_enter, scene_read_on_enter};
bool (*const scene_on_event_handlers[])(void*, SceneManagerEvent) = {
    scene_main_menu_on_event,
    scene_read_on_event};
void (*const scene_on_exit_handlers[])(void*) = {scene_main_menu_on_exit, scene_read_on_exit};

const SceneManagerHandlers scene_manager_handlers = {
    .on_enter_handlers = scene_on_enter_handlers,
    .on_event_handlers = scene_on_event_handlers,
    .on_exit_handlers = scene_on_exit_handlers,
    .scene_num = SCENE_COUNT};

UHFRFIDApp* uhf_rfid_app_alloc() {
    UHFRFIDApp* app = malloc(sizeof(UHFRFIDApp));
    app->scene_manager = scene_manager_alloc(&scene_manager_handlers, app);
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, scene_custom_callback);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, scene_back_event_callback);
    app->submenu = submenu_alloc();
    view_dispatcher_add_view(app->view_dispatcher, SubmenuView, submenu_get_view(app->submenu));
    app->widget = widget_alloc();
    view_dispatcher_add_view(app->view_dispatcher, WidgetView, widget_get_view(app->widget));
    return app;
}

void uhf_rfid_app_free(UHFRFIDApp* app) {
    furi_assert(app);
    view_dispatcher_remove_view(app->view_dispatcher, SubmenuView);
    view_dispatcher_remove_view(app->view_dispatcher, WidgetView);
    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);
    submenu_free(app->submenu);
    widget_free(app->widget);
    free(app);
}

int32_t uhf_rfid_app2(void* p) {
    UNUSED(p);
    UHFRFIDApp* app = uhf_rfid_app_alloc();

    Gui* gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(app->scene_manager, MainMenuScene);
    view_dispatcher_run(app->view_dispatcher);

    uhf_rfid_app_free(app);
    return 0;
}