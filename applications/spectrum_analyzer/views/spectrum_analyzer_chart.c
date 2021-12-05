#include "spectrum_analyzer_chart.h"
#include "../spectrum_analyzer.h"

struct ViewSpectrumAnalyzerChart {
    View* view;
    void* _data;///Superhack for worker
};

static void view_spectrum_analyzer_chart_move_right(ViewSpectrumAnalyzerChart* instance, uint32_t value){
    //inform worker
}

static void view_spectrum_analyzer_chart_zoom_in(ViewSpectrumAnalyzerChart* instance){
    
}

static void view_spectrum_analyzer_chart_zoom_out(ViewSpectrumAnalyzerChart* instance){
    
}

static void view_spectrum_analyzer_chart_clear(ViewSpectrumAnalyzerChart* instance){
    //clear data
    //update gui
}

/* display size: 128 x 64 px
 * drawing:
 * bottom 7px info (frequncies)
 * max rssi's freq
 * chart data
 */
static void view_spectrum_analyzer_chart_draw_callback(Canvas* canvas, void* context){
    SpectrumAnalyzerWorker* worker = context;

    float min_rssi = 999999999;
    float max_rssi = -999999999;

    FrequencyRSSI* it;
    FrequencyRSSI* best_value;

    //calc space between dots
    uint32_t dot_space = 128 / DOTS_COUNT;

    uint32_t start_x = dot_space;
    uint32_t start_y = 57;

    for (uint32_t i = 0; i < DOTS_COUNT; ++i){
        it = worker->rssi_buf[i];

        if (it->rssi <= min_rssi){
            min_rssi = it->rssi;
        }

        if (it->rssi >= max_rssi){
            max_rssi = it->rssi;
            best_value = it;
        }
    }

    uint32_t freq_step = (max_rssi - min_rssi) / 64;

    string_t tmp;
    string_init(tmp);
    string_cat_printf(tmp, "max: %u", best_value->frequency);

    canvas_draw_str(canvas, 0, 64, "x: freq, y: rssi");
    canvas_draw_str(canvas, 60, 64, string_get_cstr(tmp));

    canvas_draw_line(canvas, 0, 57, 128, 57);

    string_clear(tmp);
}

static void view_spectrum_analyzer_chart_enter(void* context){
    ViewSpectrumAnalyzerChart* instance = context;

    spectrum_analyzer_worker_start(instance->ptr);
}

static void view_spectrum_analyzer_chart_exit(void* context){
    // ViewSpectrumAnalyzerChart* instance = context;
    
    spectrum_analyzer_worker_stop(instance->ptr);
}

static bool view_spectrum_analyzer_chart_input_callback(InputEvent* event, void* context){
    ViewSpectrumAnalyzerChart* instance = context;

    bool processed = false;

    if(event->type == InputTypeShort || event->type == InputTypeRepeat) {
        if (event->key == InputKeyLeft) {

        } else if (event->key == InputKeyRight){

            view_spectrum_analyzer_chart_move_right(instance, 1);
            processed = true;

        } else if (event->key == InputKeyLeft){

            view_spectrum_analyzer_chart_move_right(instance, -1);
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

ViewSpectrumAnalyzerChart* view_spectrum_analyzer_chart_alloc(){
    ViewSpectrumAnalyzerChart* instance = furi_alloc(sizeof(ViewSpectrumAnalyzerChart));

    instance->view = view_alloc();
    view_set_context(instance->view, instance);

    view_set_draw_callback(instance->view, view_spectrum_analyzer_chart_draw_callback);
    view_set_input_callback(instance->view, view_spectrum_analyzer_chart_input_callback);
    view_set_enter_callback(instance->view, view_spectrum_analyzer_chart_enter);
    view_set_exit_callback(instance->view, view_spectrum_analyzer_chart_exit);

    return instance;
}

void view_spectrum_analyzer_set_data(ViewSpectrumAnalyzerChart* instance, void* ptr){
    instance->_data = ptr;
}

void view_spectrum_analyzer_chart_free(ViewSpectrumAnalyzerChart* instance){
    furi_assert(instance);
    view_free(instance->view);
    free(instance);
}

View* view_spectrum_analyzer_chart_get_view(ViewSpectrumAnalyzerChart* instance){
    return instance->view;
}
