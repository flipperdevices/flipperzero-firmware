#pragma once

#include "spectrum_analyzer_worker.h"

#include <furi-hal.h>
#include <furi.h>

#include <gui/gui_i.h>
#include <gui/canvas_i.h>
#include <gui/modules/menu.h>

#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/variable-item-list.h>

#include <stdint.h>

#define TAG "Spectrum Analyzer"

/* Application views */
typedef enum {
    SpectrumAnalyzerViewMenu,///Configurations menu (appears on boot)
    SpectrumAnalyzerViewChart,///Chart that displays values with selected setup
    SpectrumAnalyzerViewConfig///Variable list configuration
} SpectrumAnalyzerView;

/* Application Instance */
typedef struct {
    Gui* gui;
    Submenu* menu;
    ViewDispatcher* view_dispatcher;
    View* view_spectrum_analyzer_chart;
    VariableItemList* variable_item_list;
    osTimerId_t timer;///need to updateGUI on tick

    uint32_t base_freq;
    uint16_t step;
    bool user_gay;

    SpectrumAnalyzerWorker* worker;///worker updates RSSI
} SpectrumAnalyzer;

SpectrumAnalyzer* spectrum_analyzer_alloc();//init

void spectrum_analyzer_free();//termination

void spectrum_analyzer_config_items_init(SpectrumAnalyzer* instance);

uint32_t spectrum_analyzer_exit_callback(void* context);

uint32_t spectrum_analyzer_previous_callback(void* context);

void spectrum_analyzer_menu_callback(void* context, uint32_t index);

void spectrum_analyzer_config_apply(SpectrumAnalyzer* instance);

void spectrum_analyzer_config_set_base_width(VariableItem* item);

void spectrum_analyzer_config_set_user_gay(VariableItem* item);

void spectrum_analyzer_config_set_step(VariableItem* item);

void spectrum_analyzer_update_spectrum(void* context);