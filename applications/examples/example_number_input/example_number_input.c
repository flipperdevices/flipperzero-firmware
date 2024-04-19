#include "example_number_input.h"

bool example_number_input_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    ExampleNumberInput* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static ExampleNumberInput* example_number_input_alloc() {
    ExampleNumberInput* app = malloc(sizeof(ExampleNumberInput));
    app->gui = furi_record_open(RECORD_GUI);
    app->notification = furi_record_open(RECORD_NOTIFICATION);
    //Turn backlight on, believe me this makes testing your app easier
    notification_message(app->notification, &sequence_display_backlight_on);

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    app->scene_manager = scene_manager_alloc(&example_number_input_scene_handlers, app);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, example_number_input_custom_event_callback);
    app->number_input = number_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        ExampleNumberInputViewIdNumberInput,
        number_input_get_view(app->number_input));
    app->show_number = example_number_input_show_number_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        ExampleNumberInputViewIdShowNumber,
        example_number_input_show_number_get_view(app->show_number));
    app->view_stack = view_stack_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, ExampleNumberInputViewIdStack, view_stack_get_view(app->view_stack));

    return app;
}

static void example_number_input_free(ExampleNumberInput* app) {
    furi_assert(app);
    view_dispatcher_remove_view(app->view_dispatcher, ExampleNumberInputViewIdShowNumber);
    view_dispatcher_remove_view(app->view_dispatcher, ExampleNumberInputViewIdNumberInput);
    view_dispatcher_remove_view(app->view_dispatcher, ExampleNumberInputViewIdStack);
    scene_manager_free(app->scene_manager);
    number_input_free(app->number_input);
    view_stack_free(app->view_stack);
    view_dispatcher_free(app->view_dispatcher);
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);
    app->gui = NULL;
    app->notification = NULL;

    //Remove whatever is left
    free(app);
}

int32_t example_number_input(void* p) {
    UNUSED(p);
    ExampleNumberInput* app = example_number_input_alloc();

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    scene_manager_next_scene(app->scene_manager, ExampleNumberInputSceneShowNumber);

    furi_hal_power_suppress_charge_enter();

    view_dispatcher_run(app->view_dispatcher);

    furi_hal_power_suppress_charge_exit();

    example_number_input_free(app);

    return 0;
}
