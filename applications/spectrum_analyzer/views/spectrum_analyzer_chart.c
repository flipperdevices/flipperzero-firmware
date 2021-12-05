#include "spectrum_analyzer_chart.h"
#include "../spectrum_analyzer.h"

struct ViewSpectrumAnalyzerChart {
    View* view;
    SpectrumAnalyzerWorker* _worker;///Superhack
};

ViewSpectrumAnalyzerChart* view_spectrum_analyzer_chart_alloc(){
    ViewSpectrumAnalyzerChart* instance = furi_alloc(sizeof(ViewSpectrumAnalyzerChart));

    instance->view = view_alloc();
    view_set_context(instance->view, instance);

    view_set_draw_callback(instance->view, ViewModelTypeLockFree, sizeof(ViewSpectrumAnalyzerChart));
    view_set_input_callback(instance->view, view_spectrum_analyzer_chart_input_callback);
    view_set_enter_callback(instance->view, view_spectrum_analyzer_chart_enter);
    view_set_exit_callback(instance->view, view_spectrum_analyzer_chart_exit);

    return instance;
}

void view_spectrum_analyzer_chart_free(ViewSpectrumAnalyzerChart* instance){
    furi_assert(instance);
    view_free(instance->view);
    free(instance);
}

View* view_spectrum_analyzer_chart_get_view(ViewSpectrumAnalyzerChart* instance){
    return instance->view;
}

static void view_spectrum_analyzer_chart_input_callback(InputEvent* event, void* context){
    ViewSpectrumAnalyzerChart* instance = context;

    bool processed = false;

    if(event->type == InputTypeShort || event->type == InputTypeRepeat) {
        if (event->key == InputKeyLeft) {

        } else if (event->key == InputKeyRight){

            view_spectrum_analyzer_chart_set_step(instance, 1);
            processed = true;

        } else if (event->key == InputKeyLeft){

            view_spectrum_analyzer_chart_set_step(instance, -1);
            processed = true;

        } else if (event->key == InputKeyUp){

            view_spectrum_analyzer_chart_zoom_in();
            processed = true;

        } else if (event->key == InputKeyDown){

            view_spectrum_analyzer_chart_zoom_out();
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

static void view_spectrum_analyzer_chart_set_step(ViewSpectrumAnalyzerChart* instance, uint32_t value){
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

static bool view_spectrum_analyzer_chart_draw_callback(Canvas* canvas, void* model){
    
}

static void view_spectrum_analyzer_chart_enter(void* context){
    // ViewSpectrumAnalyzerChart* instance = context;

    //check if worker is worker Ok
}

static void view_spectrum_analyzer_chart_exit(void* context){
    // ViewSpectrumAnalyzerChart* instance = context;
    
    //view is switched. may free something or idle worker
}

