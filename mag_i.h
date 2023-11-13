#pragma once

#include "mag_device.h"
//#include "helpers/mag_helpers.h"
#include "helpers/mag_text_input.h"
#include "helpers/mag_types.h"

#include <furi.h>
#include <furi_hal.h>
#include <furi/core/log.h>
#include <furi_hal_gpio.h>
#include <furi_hal_resources.h>

#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <notification/notification_messages.h>

#include <gui/modules/submenu.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/popup.h>
#include <gui/modules/loading.h>
#include <gui/modules/text_input.h>
#include <gui/modules/widget.h>
#include <gui/modules/variable_item_list.h>

#include <dialogs/dialogs.h>
#include <storage/storage.h>
#include <flipper_format/flipper_format.h>

#include <toolbox/path.h>
#include <toolbox/value_index.h>

#include "scenes/mag_scene.h"
#include "scenes/mag_scene_read.h"

#define MAG_TEXT_STORE_SIZE 150

enum MagCustomEvent {
    MagEventNext = 100,
    MagEventExit,
    MagEventPopupClosed,
};

typedef struct {
    MagTxState tx;
    MagTrackState track;
    MagReverseState reverse;
    uint32_t us_clock;
    uint32_t us_interpacket;
} MagSetting;


typedef struct {
    ViewDispatcher* view_dispatcher;
    Gui* gui;
    NotificationApp* notifications;
    SceneManager* scene_manager;
    Storage* storage;
    DialogsApp* dialogs;
    MagDevice* mag_dev;

    char text_store[MAG_TEXT_STORE_SIZE + 1];
    FuriString* file_path;
    FuriString* file_name;

    MagSetting* setting;

    // Common views
    Submenu* submenu;
    DialogEx* dialog_ex;
    Popup* popup;
    Loading* loading;
    TextInput* text_input;
    Widget* widget;
    VariableItemList* variable_item_list;

    // Custom views
    Mag_TextInput* mag_text_input;

    // UART
    FuriThread* uart_rx_thread;
    FuriStreamBuffer* uart_rx_stream;
    uint8_t uart_rx_buf[UART_RX_BUF_SIZE + 1];
    void (*handle_rx_data_cb)(uint8_t* buf, size_t len, void* context);
    
    char uart_text_input_store[UART_TERMINAL_TEXT_INPUT_STORE_SIZE + 1];
    FuriString* uart_text_box_store;
    size_t uart_text_box_store_strlen;
    // UART_TextInput* text_input;
} Mag;

void mag_text_store_set(Mag* mag, const char* text, ...);

void mag_text_store_clear(Mag* mag);

void mag_show_loading_popup(void* context, bool show);

void mag_make_app_folder(Mag* mag);

void mag_popup_timeout_callback(void* context);

void mag_widget_callback(GuiButtonType result, InputType type, void* context);

void mag_text_input_callback(void* context);
