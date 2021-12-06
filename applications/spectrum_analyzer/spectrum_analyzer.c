#include "spectrum_analyzer.h"

//base freq mhz
static const uint32_t config_base_freq[] = {
    400,
    433,
    866
};

//freq step hz
static uint16_t const config_freq_steps[] = {
    812, 
    650, 
    541, 
    464, 
    406, 
    325, 
    270, 
    232, 
    203, 
    162, 
    135, 
    116, 
    102, 
    81, 
    68, 
    58
};

void spectrum_analyzer_draw_bottom_menu(Canvas* canvas, void* context){
    SpectrumAnalyzer* instance = context;

    canvas_draw_line(canvas, 0, 56, 128, 56);

    string_t tmp;
    string_init(tmp);
    string_cat_printf(tmp, "Base: %u MHz Step: %u Hz", 
        config_base_freq[instance->curr_freq_id], config_freq_steps[instance->curr_step_id]);

    canvas_draw_str(canvas, 1, 64, string_get_cstr(tmp));

    string_clear(tmp);
}

void spectrum_analyzer_draw_callback(Canvas* canvas, void* context){
    SpectrumAnalyzer* instance = context;
    SpectrumAnalyzerWorker* model = instance->worker;

    uint32_t start_x = 0;
    uint32_t end_x = 0;
    for (uint32_t i = 0; i < DOTS_COUNT; ++i){

        end_x = abs(model->rssi_buf[i].rssi + 60);

        canvas_draw_line(canvas, start_x, 0, 
            start_x, end_x);

        start_x += 2;
    }

    spectrum_analyzer_draw_bottom_menu(canvas, context);
}

void spectrum_analyzer_timer_callback(void* context){
    SpectrumAnalyzer* instance = context;

    furi_assert(instance);

    SpectrumAnalyzerEvent event = {.type = SpectrumAnalyzerEventTypeTick};
    osMessageQueuePut(instance->event_queue, &event, 0, 0);
}

void spectrum_analyzer_input_callback(InputEvent* input_event, void* context){
    SpectrumAnalyzer* instance = context;

    if (input_event->type != InputTypeRelease){
        return;
    }

    SpectrumAnalyzerEvent event = {.type = SpectrumAnalyzerEventTypeKey,
        .input = *input_event};

    osMessageQueuePut(instance->event_queue, &event, 0, osWaitForever);
}

void spectrum_analyzer_free(SpectrumAnalyzer* instance){
    furi_assert(instance);

    //free timer
    osTimerDelete(instance->timer);

    //free queue
    osMessageQueueDelete(instance->event_queue);

    //free worker
    spectrum_analyzer_worker_free(instance->worker);

    //free view_port
    view_port_enabled_set(instance->view_port, false);
    gui_remove_view_port(instance->gui, instance->view_port);
    view_port_free(instance->view_port);

    //stop gui
    furi_record_close("gui");

    free(instance);
}

SpectrumAnalyzer* spectrum_analyzer_alloc(){
    SpectrumAnalyzer* instance = furi_alloc(sizeof(SpectrumAnalyzer));

    instance->curr_freq_id = 0;
    instance->curr_step_id = 0;

    //init timer
    instance->timer = osTimerNew(
        spectrum_analyzer_timer_callback,
        osTimerPeriodic,
        instance,
        NULL
    );

    //init queue
    instance->event_queue = osMessageQueueNew(10, sizeof(SpectrumAnalyzerEvent), NULL);

    //worker init
    instance->worker = spectrum_analyzer_worker_alloc();

    //init viewport
    instance->view_port = view_port_alloc();
    view_port_draw_callback_set(instance->view_port, spectrum_analyzer_draw_callback, instance);
    view_port_input_callback_set(instance->view_port, spectrum_analyzer_input_callback, instance);

    //init gui
    instance->gui = furi_record_open("gui");
    gui_add_view_port(instance->gui, instance->view_port, GuiLayerFullscreen);

    return instance;
}

static void spectrum_analyzer_run(SpectrumAnalyzer* instance){

    osTimerStart(instance->timer, osKernelGetTickFreq() / 32);

    SpectrumAnalyzerEvent event;

    instance->worker->start_freq = config_base_freq[instance->curr_freq_id] * 1000000; 
    // instance->worker->bandwidth = config_freq_steps[instance->curr_step_id];
    instance->worker->bandwidth = instance->curr_step_id;

    spectrum_analyzer_worker_start(instance->worker);

    for (bool is_running = true; is_running;){

        osStatus_t event_status = osMessageQueueGet(instance->event_queue, &event, NULL, 100);

        if (event_status == osOK){

            if (event.type == SpectrumAnalyzerEventTypeKey){
                switch(event.input.key) {
                    case InputKeyOk:
                    break;
                    case InputKeyBack:
                    is_running = false;
                    break;
                    case InputKeyUp:
                    instance->curr_freq_id++;
                    if (instance->curr_freq_id >= COUNT_OF(config_base_freq)){
                        instance->curr_freq_id = 0;
                    }
                    break;
                    case InputKeyDown:
                    instance->curr_freq_id--;
                    if (instance->curr_freq_id >= COUNT_OF(config_base_freq)){
                        instance->curr_freq_id = COUNT_OF(config_base_freq) - 1;
                    }
                    break;
                    default:
                    break;
                }
            } else if (event.type == SpectrumAnalyzerEventTypeTick) {

            }

        } else {
            //pizda
        }

        view_port_update(instance->view_port);
    }

    osTimerStop(instance->timer);

    spectrum_analyzer_worker_stop(instance->worker);
}

int32_t spectrum_analyzer_app(void* p){
    SpectrumAnalyzer* instance = spectrum_analyzer_alloc();

    spectrum_analyzer_run(instance);

    spectrum_analyzer_free(instance);

    return 0;
}
