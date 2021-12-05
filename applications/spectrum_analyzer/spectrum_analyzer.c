#include "spectrum_analyzer.h"
#include "views/spectrum_analyzer_chart.h"

const uint32_t config_base_width[] = {
    365 * 1000000,
    433,
    866
};

const uint32_t config_step[] = {

};

const char* const config_user_gay[] = {
    "GAY?",
    "PIDOR?"
};

void spectrum_analyzer_config_items_init(SpectrumAnalyzer* instance){
    VariableItem* item;
    instance->base_width = config_base_width[0];
    instance->user_gay = false;

    item = variable_item_list_add(
        instance->variable_item_list, "Width:",
        COUNT_OF(config_base_width), spectrum_analyzer_set_base_width, 
        instance
    );

    variable_item_set_current_value_index(item, 0);
    variable_item_set_current_value_text(item, "365");

    item = variable_item_list_add(
        instance->variable_item_list, "Are you:",
        COUNT_OF(config_user_gay), spectrum_analyzer_set_user_gay,
        instance
    );

    variable_item_set_current_value_index(item, 0);
    variable_item_set_current_value_text(item, "GAY");
}

uint32_t spectrum_analyzer_exit_callback(void* context){
    return VIEW_NONE;
}

uint32_t spectrum_analyzer_previous_callback(void* context) {
    return SpectrumAnalyzerViewMenu;
}

void spectrum_analyzer_menu_callback(void* context, uint32_t index) {
    SpectrumAnalyzer* instance = (SpectrumAnalyzer*)context;

    view_dispatcher_switch_to_view(instance->view_dispatcher, index);
}

//update config and notify worker
void spectrum_analyzer_config_apply(SpectrumAnalyzer* instance){


    gui_update(instance->gui);
}

//edit config items
void spectrum_analyzer_set_base_width(VariableItem* item){
    SpectrumAnalyzer* instance = variable_item_get_context(item);
    uint8_t i = variable_item_get_current_value_index(item);
    string_t tmp;
    string_init(tmp);
    string_cat_printf(tmp, "%u", config_base_width[i]);
    variable_item_set_current_value_text(item, string_get_cstr(tmp));
    string_clear(tmp);
    instance->base_width = config_base_width[i];
    spectrum_analyzer_config_apply(instance);
}

void spectrum_analyzer_set_user_gay(VariableItem* item){
    SpectrumAnalyzer* instance = variable_item_get_context(item);
    uint8_t i = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, config_user_gay[i]);
    instance->user_gay = true;
    
    FURI_LOG_I(TAG, "он нажал");
}

SpectrumAnalyzer* spectrum_analyzer_alloc(){
    SpectrumAnalyzer* instance = furi_alloc(sizeof(SpectrumAnalyzer));

    //TODO: also init worker

    View* view = NULL;

    instance->gui = furi_record_open("gui");
    instance->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(instance->view_dispatcher);
    view_dispatcher_attach_to_gui(instance->view_dispatcher, 
                    instance->gui, ViewDispatcherTypeFullscreen);

    //only alloc. setup in _chart and config
    instance->worker = spectrum_analyzer_worker_alloc();

    //Spectrum Analyzer Chart !!!
    instance->view_spectrum_analyzer_chart = view_spectrum_analyzer_chart_alloc();
    view_spectrum_analyzer_set_data(instance->view_spectrum_analyzer_chart, instance->worker);
    view = view_spectrum_analyzer_chart_get_view(instance->view_spectrum_analyzer_chart);
    view_set_previous_callback(view, spectrum_analyzer_previous_callback);
    view_dispatcher_add_view(instance->view_dispatcher, SpectrumAnalyzerViewChart, view);

    //Config
    instance->variable_item_list = variable_item_list_alloc();
    view = variable_item_list_get_view(instance->variable_item_list);
    view_set_previous_callback(view, spectrum_analyzer_previous_callback);
    view_dispatcher_add_view(instance->view_dispatcher, SpectrumAnalyzerViewConfig, view);
    spectrum_analyzer_config_items_init(instance);

    //Menu
    instance->menu = submenu_alloc();
    view = submenu_get_view(instance->menu);
    view_set_previous_callback(view, spectrum_analyzer_exit_callback);
    view_dispatcher_add_view(instance->view_dispatcher,
                SpectrumAnalyzerViewMenu, view);

    submenu_add_item(instance->menu, "Configure", SpectrumAnalyzerViewConfig,
    spectrum_analyzer_menu_callback, instance);

    submenu_add_item(instance->menu, "Chart", SpectrumAnalyzerViewChart,
    spectrum_analyzer_menu_callback, instance);

    return instance;
}

void spectrum_analyzer_free(SpectrumAnalyzer* instance) {
    //free menu
    view_dispatcher_remove_view(instance->view_dispatcher, SpectrumAnalyzerViewMenu);
    submenu_free(instance->menu);

    //free chart
    view_dispatcher_remove_view(instance->view_dispatcher, SpectrumAnalyzerViewChart);
    view_spectrum_analyzer_chart_free(instance->view_spectrum_analyzer_chart);

    view_dispatcher_remove_view(instance->view_dispatcher, SpectrumAnalyzerViewConfig);
    variable_item_list_free(instance->variable_item_list);

    view_dispatcher_free(instance->view_dispatcher);
    furi_record_close("gui");

    free(instance);
}

int32_t spectrum_analyzer_run(SpectrumAnalyzer* instance) {
    view_dispatcher_switch_to_view(instance->view_dispatcher, SpectrumAnalyzerViewMenu);

    view_dispatcher_run(instance->view_dispatcher);

    return 0;
}

int32_t spectrum_analyzer_app(void* p){
    SpectrumAnalyzer* instance = spectrum_analyzer_alloc();

    int32_t ret = spectrum_analyzer_run(instance);

    spectrum_analyzer_free(instance);

    return ret;
}
