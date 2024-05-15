#pragma once

#include "mag_device.h"
#include "mag_state.h"
//#include "helpers/mag_helpers.h"
#include "helpers/mag_text_input.h"
#include "helpers/mag_types.h"

#include <furi.h>
#include <furi_hal.h>
#include <furi/core/log.h>
#include <furi_hal_gpio.h>
#include <furi_hal_resources.h>
#include <expansion/expansion.h>

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

// CFWs have `submenue_add_lockable_item`; OFW doesn't,
// replace with conditional submenu item
#ifdef FW_ORIGIN_Official
#define submenu_add_lockable_item(                                             \
    submenu, label, index, callback, callback_context, locked, locked_message) \
    if(!locked) {                                                              \
        submenu_add_item(submenu, label, index, callback, callback_context)    \
    }
#endif

enum MagCustomEvent {
    MagEventNext = 100,
    MagEventExit,
    MagEventPopupClosed,
    MagEventConfirmDialog,
};

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
    FuriString* args;

    MagState state;

    // Common views
    Submenu* submenu;
    Popup* popup;
    Loading* loading;
    TextInput* text_input;
    Widget* widget;
    VariableItemList* variable_item_list;
    // Custom views
    Mag_TextInput* mag_text_input;

    // UART
    Expansion* expansion;
    FuriThread* uart_rx_thread;
    FuriStreamBuffer* uart_rx_stream;
    uint8_t uart_rx_buf[UART_RX_BUF_SIZE + 1];
    void (*handle_rx_data_cb)(uint8_t* buf, size_t len, void* context);
    FuriHalSerialHandle* serial_handle;

    char uart_text_input_store[UART_TERMINAL_TEXT_INPUT_STORE_SIZE + 1];
    FuriString* uart_text_box_store;
    size_t uart_text_box_store_strlen;
} Mag;

void mag_text_store_set(Mag* mag, const char* text, ...);

void mag_text_store_clear(Mag* mag);

void mag_show_loading_popup(void* context, bool show);

void mag_make_app_folder(Mag* mag);

void mag_migrate_and_copy_files(Mag* mag);

void mag_popup_timeout_callback(void* context);

void mag_widget_callback(GuiButtonType result, InputType type, void* context);

void mag_text_input_callback(void* context);
