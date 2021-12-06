#include "spectrum_analyzer_chart.h"

ViewSpectrumAnalyzerChart* view_spectrum_analyzer_chart_alloc(){
    ViewSpectrumAnalyzerChart* instance = furi_alloc(sizeof(ViewSpectrumAnalyzerChart));

    instance->view = view_alloc();
    view_set_context(instance->view, instance);
    view_allocate_model(instance->view, ViewModelTypeLockFree, sizeof(ViewSpectrumAnalyzerModel));
    view_set_draw_callback(instance->view, view_spectrum_analyzer_draw_callback);
    view_set_enter_callback(instance->view, view_spectrum_analyzer_enter_callback);
    view_set_exit_callback(instance->view, view_spectrum_analyzer_exit_callback);

    instance->timer = osTimerNew(view_spectrum_analyzer_timer_callback, 
        osTimerPeriodic, instance, NULL);
    
    return instance;
}

void view_spectrum_analyzer_chart_free(ViewSpectrumAnalyzerChart* instance){
    furi_assert(instance);

    osTimerDelete(instance->timer);
    view_free(instance->view);
    free(instance);
}

View* view_spectrum_analyzer_chart_get_view(ViewSpectrumAnalyzerChart* instance){
    furi_assert(instance);
    return instance->view;
}

void view_spectrum_analyzer_draw_callback(Canvas* canvas, void* context){
    ViewSpectrumAnalyzerModel* model = context;

    canvas_clear(canvas);

    uint32_t start_x = 0;
    for (uint32_t i = 0; i < DOTS_COUNT; ++i){

        canvas_draw_line(canvas, start_x, 0, 
            start_x, abs(model->rssi_buf[i].rssi));

        start_x += 2;
    }
}

void view_spectrum_analyzer_enter_callback(void* context){
    ViewSpectrumAnalyzerChart* instance = context;

    osTimerStart(instance->timer, osKernelGetTickFreq() / 32);
}

void view_spectrum_analyzer_exit_callback(void* context){
    ViewSpectrumAnalyzerChart* instance = context;

    osTimerStop(instance->timer);
}

void view_spectrum_analyzer_timer_callback(void* context){
    ViewSpectrumAnalyzerChart* instance = context;
    with_view_model(
        instance->view, (ViewSpectrumAnalyzerModel * model) {
            return true;
        });
}
