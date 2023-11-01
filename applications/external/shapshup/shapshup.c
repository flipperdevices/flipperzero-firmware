#include <furi.h>

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/view_stack.h>
#include <gui/modules/text_input.h>
#include <gui/modules/popup.h>
#include <gui/modules/widget.h>
#include <gui/modules/loading.h>

#include <dialogs/dialogs.h>

#include "shapshup_i.h"
#include "shapshup_custom_event.h"

#define TAG "ShapShupApp"
#define DELAY_MS 100

static bool shapshup_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    ShapShupState* instance = context;
    return scene_manager_handle_custom_event(instance->scene_manager, event);
}

static bool shapshup_back_event_callback(void* context) {
    furi_assert(context);
    ShapShupState* instance = context;
    return scene_manager_handle_back_event(instance->scene_manager);
}

static void shapshup_tick_event_callback(void* context) {
    furi_assert(context);
    ShapShupState* instance = context;
    scene_manager_handle_tick_event(instance->scene_manager);
}

ShapShupState* shapshup_alloc() {
    ShapShupState* instance = malloc(sizeof(ShapShupState));

    instance->file_path = furi_string_alloc();
    instance->scene_manager = scene_manager_alloc(&shapshup_scene_handlers, instance);
    instance->view_dispatcher = view_dispatcher_alloc();

    instance->gui = furi_record_open(RECORD_GUI);
    instance->storage = furi_record_open(RECORD_STORAGE);

    view_dispatcher_enable_queue(instance->view_dispatcher);
    view_dispatcher_set_event_callback_context(instance->view_dispatcher, instance);
    view_dispatcher_set_custom_event_callback(
        instance->view_dispatcher, shapshup_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        instance->view_dispatcher, shapshup_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        instance->view_dispatcher, shapshup_tick_event_callback, 500);

    //Dialog
    instance->dialogs = furi_record_open(RECORD_DIALOGS);

    // Notifications
    instance->notifications = furi_record_open(RECORD_NOTIFICATION);

    // TextInput
    instance->text_input = text_input_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher,
        ShapShupViewTextInput,
        text_input_get_view(instance->text_input));

    // Custom Widget
    instance->widget = widget_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher, ShapShupViewWidget, widget_get_view(instance->widget));

    // Popup
    instance->popup = popup_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher, ShapShupViewPopup, popup_get_view(instance->popup));

    // ViewStack
    instance->view_stack = view_stack_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher, ShapShupViewStack, view_stack_get_view(instance->view_stack));

    // ShapShupMainView
    instance->view_main = shapshup_main_view_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher,
        ShapShupViewMain,
        shapshup_main_view_get_view(instance->view_main));

    // Loading
    instance->loading = loading_alloc();

    return instance;
}

void shapshup_free(ShapShupState* instance) {
    furi_assert(instance);

    // Notifications
    notification_message(instance->notifications, &sequence_blink_stop);
    furi_record_close(RECORD_NOTIFICATION);
    instance->notifications = NULL;

    // Loading
    loading_free(instance->loading);

    // View Main
    view_dispatcher_remove_view(instance->view_dispatcher, ShapShupViewMain);
    shapshup_main_view_free(instance->view_main);

    // TextInput
    view_dispatcher_remove_view(instance->view_dispatcher, ShapShupViewTextInput);
    text_input_free(instance->text_input);

    // Custom Widget
    view_dispatcher_remove_view(instance->view_dispatcher, ShapShupViewWidget);
    widget_free(instance->widget);

    // Popup
    view_dispatcher_remove_view(instance->view_dispatcher, ShapShupViewPopup);
    popup_free(instance->popup);

    // ViewStack
    view_dispatcher_remove_view(instance->view_dispatcher, ShapShupViewStack);
    view_stack_free(instance->view_stack);

    //Dialog
    furi_record_close(RECORD_DIALOGS);
    instance->dialogs = NULL;

    // Scene manager
    scene_manager_free(instance->scene_manager);

    // View Dispatcher
    view_dispatcher_free(instance->view_dispatcher);

    // GUI
    furi_record_close(RECORD_GUI);
    instance->gui = NULL;

    // Storage
    furi_record_close(RECORD_STORAGE);
    instance->storage = NULL;

    furi_string_free(instance->file_path);

    // The rest
    free(instance);
}

void shapshup_show_loading_popup(void* context, bool show) {
    TaskHandle_t timer_task = xTaskGetHandle(configTIMER_SERVICE_TASK_NAME);
    ShapShupState* instance = context;
    ViewStack* view_stack = instance->view_stack;
    Loading* loading = instance->loading;

    if(show) {
        // Raise timer priority so that animations can play
        vTaskPrioritySet(timer_task, configMAX_PRIORITIES - 1);
        view_stack_add_view(view_stack, loading_get_view(loading));
    } else {
        view_stack_remove_view(view_stack, loading_get_view(loading));
        // Restore default timer priority
        vTaskPrioritySet(timer_task, configTIMER_TASK_PRIORITY);
    }
}

void shapshup_text_input_callback(void* context) {
    furi_assert(context);
    ShapShupState* instance = context;
    view_dispatcher_send_custom_event(
        instance->view_dispatcher, ShapShupCustomEventTypeTextEditDone);
}

void shapshup_popup_closed_callback(void* context) {
    furi_assert(context);
    ShapShupState* instance = context;
    view_dispatcher_send_custom_event(
        instance->view_dispatcher, ShapShupCustomEventTypePopupClosed);
}

// ENTRYPOINT
__attribute__((unused)) int32_t shapshup_app(void* p) {
    UNUSED(p);

    ShapShupState* instance = shapshup_alloc();
    view_dispatcher_attach_to_gui(
        instance->view_dispatcher, instance->gui, ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(instance->scene_manager, ShapshupSceneStart);
    notification_message(instance->notifications, &sequence_display_backlight_on);

    view_dispatcher_run(instance->view_dispatcher);

    shapshup_free(instance);

    return 0;
}
