#include "spectrum_analyzer.h"
#include "views/spectrum_analyzer_chart.h"

//base freq mhz
static const uint32_t config_base_freq[] = {
    400,
    433,
    866
};

//freq step hz
static uint16_t const config_freq_steps[] = {812, 650, 541, 464, 406, 325, 270, 232, 203, 162, 135, 116, 102, 81, 68, 58};

static const char* const config_user_gay[] = {
    "Gay?",
    "Pidor?"
};

void spectrum_analyzer_config_items_init(SpectrumAnalyzer* instance){
    VariableItem* item;
    instance->base_freq = config_base_freq[0];
    instance->step = config_freq_steps[0];
    instance->user_gay = false;

    item = variable_item_list_add(
        instance->variable_item_list, "Freq:",
        COUNT_OF(config_base_freq), spectrum_analyzer_config_set_base_width, 
        instance
    );

    variable_item_set_current_value_index(item, 0);
    variable_item_set_current_value_text(item, "400");

    item = variable_item_list_add(
        instance->variable_item_list, "Step:",
        COUNT_OF(config_freq_steps), spectrum_analyzer_config_set_step, 
        instance
    );

    variable_item_set_current_value_index(item, 0);
    variable_item_set_current_value_text(item, "812");

    item = variable_item_list_add(
        instance->variable_item_list, "Are you:",
        COUNT_OF(config_user_gay), spectrum_analyzer_config_set_user_gay,
        instance
    );

    variable_item_set_current_value_index(item, 0);
    variable_item_set_current_value_text(item, "Gay?");
}

uint32_t spectrum_analyzer_exit_callback(void* context){
    return VIEW_NONE;
}

//when user presses 'back'
uint32_t spectrum_analyzer_previous_callback(void* context) {
    return SpectrumAnalyzerViewMenu;
}

void spectrum_analyzer_menu_callback(void* context, uint32_t index) {
    SpectrumAnalyzer* instance = (SpectrumAnalyzer*)context;

    view_dispatcher_switch_to_view(instance->view_dispatcher, index);
}

//update config and notify worker
void spectrum_analyzer_config_apply(SpectrumAnalyzer* instance){
    instance->worker->start_freq = instance->base_freq * 1000000;
    instance->worker->bandwidth = instance->step;
}

//edit config items
void spectrum_analyzer_config_set_base_width(VariableItem* item){
    SpectrumAnalyzer* instance = variable_item_get_context(item);
    uint8_t i = variable_item_get_current_value_index(item);
    string_t tmp;
    string_init(tmp);
    string_cat_printf(tmp, "%u", config_base_freq[i]);
    variable_item_set_current_value_text(item, string_get_cstr(tmp));
    string_clear(tmp);
    instance->base_freq = config_base_freq[i];
    spectrum_analyzer_config_apply(instance);
}

void spectrum_analyzer_config_set_user_gay(VariableItem* item){
    SpectrumAnalyzer* instance = variable_item_get_context(item);
    uint8_t i = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, config_user_gay[i]);
    instance->user_gay = true;
    
    FURI_LOG_I(TAG, "он нажал");
}

void spectrum_analyzer_config_set_step(VariableItem* item){
    SpectrumAnalyzer* instance = variable_item_get_context(item);
    uint8_t i = variable_item_get_current_value_index(item);
    string_t tmp;
    string_init(tmp);
    string_cat_printf(tmp, "%u", config_freq_steps[i]);
    variable_item_set_current_value_text(item, string_get_cstr(tmp));
    string_clear(tmp);
    instance->step = config_freq_steps[i];
    spectrum_analyzer_config_apply(instance);
}

SpectrumAnalyzer* spectrum_analyzer_alloc(){
    SpectrumAnalyzer* instance = furi_alloc(sizeof(SpectrumAnalyzer));

    View* view = NULL;

    instance->gui = furi_record_open("gui");
    instance->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(instance->view_dispatcher);
    view_dispatcher_attach_to_gui(instance->view_dispatcher, 
                    instance->gui, ViewDispatcherTypeFullscreen);

    //only alloc. setup in spectrum_analyzer_config_apply
    instance->worker = spectrum_analyzer_worker_alloc();

    //init chart
    instance->view_spectrum_analyzer_chart = view_alloc();
    view = instance->view_spectrum_analyzer_chart;
    view_set_previous_callback(view, spectrum_analyzer_previous_callback);
    view_dispatcher_add_view(instance->view_dispatcher, SpectrumAnalyzerViewChart, view);

    view_set_context(view, instance);

    view_set_draw_callback(view, view_spectrum_analyzer_chart_draw_callback);
    view_set_input_callback(view, view_spectrum_analyzer_chart_input_callback);
    view_set_enter_callback(view, view_spectrum_analyzer_chart_enter);
    view_set_exit_callback(view, view_spectrum_analyzer_chart_exit);

    //Config
    instance->variable_item_list = variable_item_list_alloc();
    view = variable_item_list_get_view(instance->variable_item_list);
    view_set_previous_callback(view, spectrum_analyzer_previous_callback);
    spectrum_analyzer_config_items_init(instance);
    view_dispatcher_add_view(instance->view_dispatcher, SpectrumAnalyzerViewConfig, view);

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

    //init timer
    instance->timer = osTimerNew(spectrum_analyzer_update_spectrum, 
    osTimerPeriodic, instance, NULL);

    return instance;
}

void spectrum_analyzer_update_spectrum(void* context){
    SpectrumAnalyzer* instance = context;

    gui_update(instance->gui);
}

void spectrum_analyzer_free(SpectrumAnalyzer* instance) {
    //free timer
    osTimerDelete(instance->timer);

    //free worker
    spectrum_analyzer_worker_free(instance->worker);

    //free menu
    view_dispatcher_remove_view(instance->view_dispatcher, SpectrumAnalyzerViewMenu);
    submenu_free(instance->menu);

    //free chart
    view_dispatcher_remove_view(instance->view_dispatcher, SpectrumAnalyzerViewChart);
    view_free(instance->view_spectrum_analyzer_chart);

    //free settings
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
