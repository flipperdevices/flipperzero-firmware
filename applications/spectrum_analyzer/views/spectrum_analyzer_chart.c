#include "spectrum_analyzer_chart.h"

struct ViewSpectrumAnalyzerChart {
    View* view;
};

ViewSpectrumAnalyzerChart* view_spectrum_analyzer_chart_alloc(){
    ViewSpectrumAnalyzerChart* instance = furi_alloc(sizeof(ViewSpectrumAnalyzerChart));

    instance->view = view_alloc();

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