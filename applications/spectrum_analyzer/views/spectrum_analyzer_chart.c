#include "spectrum_analyzer_chart.h"

void view_spectrum_analyzer_chart_move(SpectrumAnalyzer* instance, uint32_t value){
    //inform worker
}

void view_spectrum_analyzer_chart_zoom_in(SpectrumAnalyzer* instance){
    
}

void view_spectrum_analyzer_chart_zoom_out(SpectrumAnalyzer* instance){
    
}

void view_spectrum_analyzer_chart_clear(SpectrumAnalyzer* instance){
    //clear data
}

/* display size: 128 x 64 px
 * drawing:
 * bottom 7px info (frequncies)
 * max rssi's freq
 * chart data
 */
void view_spectrum_analyzer_chart_draw_callback(Canvas* canvas, void* context){
    SpectrumAnalyzerWorker* worker = ((SpectrumAnalyzer*)context)->worker;

    uint32_t dot_space = 128 / DOTS_COUNT;

    uint8_t start_x = 0;
    FrequencyRSSI it;
    for (uint32_t i = 0; i < DOTS_COUNT; ++i){
        it = worker->rssi_buf[i];

        canvas_draw_line(canvas, start_x, 0, start_x, abs(it.rssi + 30));

        start_x += dot_space;
    }

    // float min_rssi = 999999999;
    // float max_rssi = -999999999;

    // FrequencyRSSI it;
    // FrequencyRSSI best_value;

    //calc space between dots
    // uint32_t dot_space = 128 / DOTS_COUNT;

    // uint32_t start_x = dot_space;
    // uint32_t start_y = 57;

    // for (uint32_t i = 0; i < DOTS_COUNT; ++i){
    //     it = worker->rssi_buf[i];

    //     if (it->rssi <= min_rssi){
    //         min_rssi = it->rssi;
    //     }

    //     if (it->rssi >= max_rssi){
    //         max_rssi = it->rssi;
    //         best_value = it;
    //     }
    // }

    // uint32_t freq_step = (max_rssi - min_rssi) / 64;

    string_t tmp;
    string_init(tmp);
    // string_cat_printf(tmp, "max: %u", best_value->frequency);
    string_cat_printf(tmp, "max: %u", worker->start_freq);

    canvas_draw_str(canvas, 0, 64, "x: freq, y: rssi");
    canvas_draw_str(canvas, 60, 64, string_get_cstr(tmp));

    canvas_draw_line(canvas, 0, 57, 128, 57);

    string_clear(tmp);
}

void view_spectrum_analyzer_chart_enter(void* context){
    SpectrumAnalyzer* instance = context;

    osTimerStart(instance->timer, osKernelGetTickFreq() / 32);

    spectrum_analyzer_worker_start(instance->worker);
}

void view_spectrum_analyzer_chart_exit(void* context){
    SpectrumAnalyzer* instance = context;

    osTimerStop(instance->timer);

    spectrum_analyzer_worker_stop(instance->worker);
}

bool view_spectrum_analyzer_chart_input_callback(InputEvent* event, void* context){
    SpectrumAnalyzer* instance = context;

    bool processed = false;

    if(event->type == InputTypeShort || event->type == InputTypeRepeat) {
        if (event->key == InputKeyLeft) {

        } else if (event->key == InputKeyRight){

            view_spectrum_analyzer_chart_move(instance, 1);
            processed = true;

        } else if (event->key == InputKeyLeft){

            view_spectrum_analyzer_chart_move(instance, -1);
            processed = true;

        } else if (event->key == InputKeyUp){

            view_spectrum_analyzer_chart_zoom_in(instance);
            processed = true;

        } else if (event->key == InputKeyDown){

            view_spectrum_analyzer_chart_zoom_out(instance);
            processed = true;

        } else if (event->key == InputKeyBack){
            //do nothing. default will return to menu
        } else if (event->key == InputKeyOk){
            view_spectrum_analyzer_chart_clear(instance);
            processed = true;
        }

    }


    return processed;
}
