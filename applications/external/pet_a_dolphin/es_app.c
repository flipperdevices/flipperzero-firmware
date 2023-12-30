#include "es_app.h"

esAppData* es_app_data_alloc() {
    // Custom application data allocation
    esAppData* data = malloc(sizeof(esAppData));
    data->dolphin = furi_record_open(RECORD_DOLPHIN);

    data->counter = 0;

    data->pet_feeder.thread = furi_thread_alloc();
    furi_thread_set_name(data->pet_feeder.thread, "Pet a Dolphin Feeder");
    furi_thread_set_stack_size(data->pet_feeder.thread, 512);
    data->pet_feeder.active = false;

    return data;
}

void es_app_data_free(esAppData* data) {
    //Custom application data free
    furi_record_close(RECORD_DOLPHIN);

    data->pet_feeder.active = false;
    furi_thread_join(data->pet_feeder.thread);

    furi_thread_free(data->pet_feeder.thread);

    free(data);
}

static void input_callback(InputEvent* input, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    AppEvent event = {.type = EventTypeKey, .input = *input};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

esApp* es_app_alloc(void* render_callback, void* timer_callback) {
    esApp* instance = malloc(sizeof(esApp));

    instance->mutex = furi_mutex_alloc(FuriMutexTypeNormal);

    instance->queue = furi_message_queue_alloc(8, sizeof(AppEvent));

    instance->view_port = view_port_alloc();
    view_port_draw_callback_set(instance->view_port, render_callback, instance);
    view_port_input_callback_set(instance->view_port, input_callback, instance->queue);

    // Open GUI and register view_port
    instance->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(instance->gui, instance->view_port, GuiLayerFullscreen);

    instance->data = es_app_data_alloc();
    instance->run = true;

    instance->timer = furi_timer_alloc(timer_callback, FuriTimerTypePeriodic, instance);
    furi_timer_start(instance->timer, furi_kernel_get_tick_frequency() / 4);

    return instance;
}

void es_app_free(esApp* instance) {
    furi_timer_free(instance->timer);

    es_app_data_free(instance->data);

    gui_remove_view_port(instance->gui, instance->view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(instance->view_port);

    furi_message_queue_free(instance->queue);

    furi_mutex_free(instance->mutex);

    free(instance);
}
