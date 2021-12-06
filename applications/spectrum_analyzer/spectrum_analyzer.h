#pragma once

#include "spectrum_analyzer_worker.h"
#include "views/spectrum_analyzer_chart.h"

#include <furi-hal.h>
#include <furi.h>

#include <gui/gui_i.h>
#include <gui/canvas_i.h>
#include <gui/modules/menu.h>

#include <gui/view_dispatcher.h>
#include <gui/view_port.h>
#include <gui/view_dispatcher_i.h>
#include <gui/view_port_i.h>
#include <gui/view.h>
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

/* Application */
typedef struct {
    Gui* gui;///ptr to gui
    Submenu* menu;///entry menu
    ViewDispatcher* view_dispatcher;///manages views
    VariableItemList* variable_item_list;///config items
    ViewSpectrumAnalyzerChart* view_spectrum_analyzer_chart;///chart view

    uint32_t base_freq;
    uint16_t step;
    bool user_gay;

    SpectrumAnalyzerWorker* worker;///worker updates RSSI
} SpectrumAnalyzer;

SpectrumAnalyzer* spectrum_analyzer_alloc();//init

void spectrum_analyzer_free();//termination

void spectrum_analyzer_config_items_init(SpectrumAnalyzer* instance);///init config items

uint32_t spectrum_analyzer_exit_callback(void* context);///exit

uint32_t spectrum_analyzer_previous_callback(void* context);///return to main menu

void spectrum_analyzer_menu_callback(void* context, uint32_t index);///switch from menu

void spectrum_analyzer_config_apply(SpectrumAnalyzer* instance);///process user input

void spectrum_analyzer_config_set_base_freq(VariableItem* item);///set base freq

void spectrum_analyzer_config_set_user_gay(VariableItem* item);///set joke setting

void spectrum_analyzer_config_set_step(VariableItem* item);///set step