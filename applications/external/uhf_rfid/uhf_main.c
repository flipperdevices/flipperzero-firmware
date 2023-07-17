#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/widget.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>
#include <lib/toolbox/stream/file_stream.h>
#include "arraylist.h"
#include "cmd.h"

#define SCENE_COUNT 1

typedef enum { STARTED, RUNNING, STOPPED } UHFRFIDWorkerState;
typedef enum { SEND_READ_CMD, SEND_WRITE_CMD } UHFRFIDWorkerCMD;

// typedef void (*UHFRFIDWorkerCallback)();

typedef struct UHFRFIDWorker {
    FuriThread* thread;
    UHFRFIDWorkerCMD cmd;
    // Storage* storage;
    // UHFRFIDWorkerCallback callback;
    void* context;
    UHFRFIDWorkerState state;

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

void rx_callback(UartIrqEvent ev, uint8_t data, void* ctx) {
    UNUSED(ev);
    ArrayList* data_list = ctx;
    array_list_append(data_list, data);
}

int32_t uhf_rfid_worker_task(void* ctx) {
    UHFRFIDWorker* worker = ctx;
    // uhf_rfid_worker_enable_field();
    furi_hal_uart_set_br(FuriHalUartIdUSART1, 115200);
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, rx_callback, worker->context);
    ArrayList* data_list = worker->context;
    int prev_size = data_list->size;
    furi_hal_console_disable();
    switch(worker->cmd) {
    case SEND_READ_CMD:
        furi_hal_console_enable();
        FURI_LOG_D("TAG", "SENDING DATA");
        FURI_LOG_D(
            "TAG",
            "%02x%02x%02x%02x%02x%02x%02x",
            CMD_SINGLE_POLLING.cmd[0],
            CMD_SINGLE_POLLING.cmd[1],
            CMD_SINGLE_POLLING.cmd[2],
            CMD_SINGLE_POLLING.cmd[3],
            CMD_SINGLE_POLLING.cmd[4],
            CMD_SINGLE_POLLING.cmd[5],
            CMD_SINGLE_POLLING.cmd[6]);
        furi_hal_console_disable();
        furi_hal_uart_tx(FuriHalUartIdUSART1, CMD_SINGLE_POLLING.cmd, CMD_SINGLE_POLLING.length);
        break;
    default:
        break;
    }
    furi_hal_console_enable();
    while(prev_size != data_list->size) {
        furi_delay_ms(50);
    }
    FURI_LOG_D("TAG", "DATA READ %s", array_list_to_string(data_list));
    return 0;
}
UHFRFIDWorker* uhf_rfid_worker_alloc() {
    UHFRFIDWorker* uhf_rfid_worker = malloc(sizeof(UHFRFIDWorker));

    ArrayList* data_list = array_list_alloc();
    uhf_rfid_worker->context = data_list;
    // Worker thread attributes
    uhf_rfid_worker->thread =
        furi_thread_alloc_ex("UHFRFIDWorker", 8 * 1024, uhf_rfid_worker_task, uhf_rfid_worker);
    // uhf_rfid_worker->storage = furi_record_open(RECORD_STORAGE);

    // picopass_worker_change_state(picopass_worker, PicopassWorkerStateReady);

    return uhf_rfid_worker;
}

void uhf_rfid_worker_free(UHFRFIDWorker* uhf_rfid_worker) {
    furi_assert(uhf_rfid_worker);

    furi_thread_free(uhf_rfid_worker->thread);
    ArrayList* data_list = uhf_rfid_worker->context;
    array_list_free(data_list);

    // furi_record_close(RECORD_STORAGE);

    free(uhf_rfid_worker);
}

void uhf_rfid_worker_change_state(UHFRFIDWorker* worker, UHFRFIDWorkerState state) {
    worker->state = state;
}
void uhf_rfid_worker_start(UHFRFIDWorker* uhf_rfid_worker, UHFRFIDWorkerState state, void* context) {
    uhf_rfid_worker->cmd = (UHFRFIDWorkerCMD)context;
    uhf_rfid_worker_change_state(uhf_rfid_worker, state);
    furi_thread_start(uhf_rfid_worker->thread);
}

void uhf_rfid_worker_stop(UHFRFIDWorker* uhf_rfid_worker) {
    furi_assert(uhf_rfid_worker);
    furi_assert(uhf_rfid_worker->thread);

    if(furi_thread_get_state(uhf_rfid_worker->thread) != FuriThreadStateStopped) {
        uhf_rfid_worker_change_state(uhf_rfid_worker, STOPPED);
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
        FURI_LOG_D("TAG", "Saved was pressed");
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
    ArrayList* data_list = worker->context;
    uhf_rfid_worker_start(worker, RUNNING, SEND_READ_CMD);
    furi_delay_ms(2000);
    furi_hal_console_enable();
    FURI_LOG_D("TAG", "data %s", array_list_to_string(data_list));
    widget_add_string_element(
        app->widget, 5, 15, AlignCenter, AlignCenter, FontPrimary, array_list_to_string(data_list));
    view_dispatcher_switch_to_view(app->view_dispatcher, WidgetView);
    furi_delay_ms(2000);
    uhf_rfid_worker_stop(worker);
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