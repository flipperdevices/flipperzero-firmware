/**
MIT License

Copyright (c) 2023 Vojtech Suk (https://github.com/sukvojte)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <gui/canvas.h>
#include <gui/modules/widget.h>
#include <storage/storage.h>
#include <wch_swio_flasher_icons.h>
#include "../utils.h"
#include "../wch_swio_flasher.h"
#include "wch_swio_flasher_scene.h"

#define TAG "WCH_ScFl"

typedef enum {
    SubMenu_Action_ChipInfo = 0,
    SubMenu_Action_OpenFile,
    SubMenu_Action_EraseChip,
    SubMenu_Action_WriteChip,
    SubMenu_ActionCount,
} SubMenu_Action;

static void wch_swio_flasher_scene_flash_on_action_done(void* context, ViewFlasher_Action action) {
    WchSwioFlasherApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, action);
}

static void select_file_to_write(WchSwioFlasherApp* app) {
    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, ".bin", &I_bin10x10);
    browser_options.base_path = ANY_PATH("wch_firmare");
    browser_options.skip_assets = true;

    DialogsApp* dialogs = furi_record_open(RECORD_DIALOGS);
    bool res = dialog_file_browser_show(
        dialogs, app->views.file_path, app->views.file_path, &browser_options);

    if(res) {
        FURI_LOG_D(TAG, "selected bin is %s", furi_string_get_cstr(app->views.file_path));

        wch_swio_flasher_store_defaults(app);
        //view_dispatcher_switch_to_view(app->view_dispatcher, WchSwioFlasherViewFlasher);
        WchSwioFlasher_Ch32vFlipperFlasher_write_chip(
            app->services.flasher, (char*)furi_string_get_cstr(app->views.file_path));
    }

    furi_record_close(RECORD_DIALOGS);
}

static void wch_swio_flasher_scene_flash_submenu_callback(void* context, uint32_t index) {
    furi_assert(context);
    WchSwioFlasherApp* app = context;

    switch(index) {
    case SubMenu_Action_ChipInfo:
        WchSwioFlasher_Ch32vFlipperFlasher_chip_info(app->services.flasher);
        break;
    case SubMenu_Action_EraseChip:
        WchSwioFlasher_Ch32vFlipperFlasher_erase_chip(app->services.flasher);
        break;
    case SubMenu_Action_WriteChip:
        select_file_to_write(app);
        break;
    default:
        break;
    }
}

void wch_swio_flasher_scene_flash_on_enter(void* context) {
    WchSwioFlasherApp* app = context;

    Submenu* submenu = app->views.submenu;
    submenu_set_header(submenu, "Flash CH32Vxx");
    submenu_add_item(
        submenu,
        "Get chip info",
        SubMenu_Action_ChipInfo,
        wch_swio_flasher_scene_flash_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "Erase chip",
        SubMenu_Action_EraseChip,
        wch_swio_flasher_scene_flash_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "Write chip",
        SubMenu_Action_WriteChip,
        wch_swio_flasher_scene_flash_submenu_callback,
        app);

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(app->scene_manager, WchSwioFlasherSceneFlash));

    WchSwioFlasher_Ch32vFlipperFlasher_attach(app->services.flasher);
    WchSwioFlasher_Ch32vFlipperFlasher_event_callback(
        app->services.flasher, wch_swio_flasher_scene_flash_on_action_done, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, WchSwioFlasherViewSubmenu);
}

static char* flasher_err_to_text(WchSwioFlasher_CFF_ResultStatus status) {
    switch(status) {
    case VWchSwioFlasher_CFF_ChipNotConnected:
        return "not connected";
    case WchSwioFlasher_CFF_UnableToOpenFile:
        return "unable to open file";
    default:
        return "<unk>";
    }
}

static char* error_action_to_str(ViewFlasher_Action action) {
    switch(action) {
    case WchSwioFlasher_CFF_ChipInfoCompleted:
        return "Unable to get\nchip info!";
    case WchSwioFlasher_CFF_EraseChipCompleted:
        return "Unable erase\nchip!";
    case WchSwioFlasher_CFF_WriteChipCompleted:
        return "Unable write\nchip!";
    default:
        return "";
    }
}

static void show_error_dialog(
    WchSwioFlasherApp* app,
    ViewFlasher_Action action,
    WchSwioFlasher_CFF_ResultStatus status) {
    UNUSED(app);
    DialogsApp* dialogs = furi_record_open(RECORD_DIALOGS);
    DialogMessage* message = dialog_message_alloc();

    dialog_message_set_header(message, "Error", 64, 0, AlignCenter, AlignTop);

    FuriString* error_message = furi_string_alloc();
    furi_string_cat_printf(
        error_message, "%s\n'%s'", error_action_to_str(action), flasher_err_to_text(status));

    dialog_message_set_text(
        message, furi_string_get_cstr(error_message), 64, 30, AlignCenter, AlignCenter);

    //dialog_message_set_icon(message, &I_SDQuestion_35x43, 5, 6);
    dialog_message_set_buttons(message, "Back", NULL, NULL);
    dialog_message_show(dialogs, message);
    dialog_message_free(message);
    furi_string_free(error_message);
    furi_record_close(RECORD_DIALOGS);
}

static void show_chip_info_dialog(WchSwioFlasherApp* app, WchSwioFlasher_CFF_ChipInfo* data) {
    UNUSED(app);
    DialogsApp* dialogs = furi_record_open(RECORD_DIALOGS);
    DialogMessage* message = dialog_message_alloc();

    dialog_message_set_header(message, "Chip info", 64, 0, AlignCenter, AlignTop);

    FuriString* text = furi_string_alloc();
    furi_string_cat_printf(
        text,
        "Flash size: %lu kB\nESIG0: " FMT_4HEX "\nESIG1: " FMT_4HEX "\nESIG2: " FMT_4HEX,
        data->flash_size,
        _UI(data->esig_uniid[0]),
        _UI(data->esig_uniid[1]),
        _UI(data->esig_uniid[2]));

    dialog_message_set_text(message, furi_string_get_cstr(text), 2, 15, AlignLeft, AlignTop);

    //dialog_message_set_icon(message, &I_SDQuestion_35x43, 5, 6);
    dialog_message_set_buttons(message, NULL, NULL, "Ok");
    dialog_message_show(dialogs, message);
    dialog_message_free(message);
    furi_string_free(text);
    furi_record_close(RECORD_DIALOGS);
}

static void show_ok_dialog(WchSwioFlasherApp* app, const char* text) {
    UNUSED(app);
    DialogsApp* dialogs = furi_record_open(RECORD_DIALOGS);
    DialogMessage* message = dialog_message_alloc();

    dialog_message_set_header(message, "Success", 64, 0, AlignCenter, AlignTop);
    dialog_message_set_text(message, text, 64, 31, AlignCenter, AlignCenter);
    dialog_message_set_buttons(message, NULL, NULL, "Ok");
    dialog_message_show(dialogs, message);
    dialog_message_free(message);

    furi_record_close(RECORD_DIALOGS);
}

static void handle_chip_info_completed(WchSwioFlasherApp* app) {
    WchSwioFlasher_CFF_ChipInfo data;
    WchSwioFlasher_Ch32vFlipperFlasher_chip_info_data(app->services.flasher, &data);
    FURI_LOG_D(TAG, "chip_info finish %d", data.status);

    switch(data.status) {
    case VWchSwioFlasher_CFF_Ok: {
        show_chip_info_dialog(app, &data);
        break;
    }
    case VWchSwioFlasher_CFF_NoData:
        // TODO throw error
        break;
    default:
        show_error_dialog(app, WchSwioFlasher_CFF_ChipInfoCompleted, data.status);
        break;
    }
}

static void handle_erase_chip_completed(WchSwioFlasherApp* app) {
    WchSwioFlasher_CFF_EraseChip data;
    WchSwioFlasher_Ch32vFlipperFlasher_erase_chip_data(app->services.flasher, &data);
    FURI_LOG_D(TAG, "erase_chip finish %d", data.status);

    switch(data.status) {
    case VWchSwioFlasher_CFF_Ok: {
        show_ok_dialog(app, "Chip erased");
        break;
    }
    case VWchSwioFlasher_CFF_NoData:
        // TODO throw error
        break;
    default:
        show_error_dialog(app, WchSwioFlasher_CFF_EraseChipCompleted, data.status);
        break;
    }
}

static void handle_write_chip_completed(WchSwioFlasherApp* app) {
    WchSwioFlasher_CFF_EraseChip data;
    WchSwioFlasher_Ch32vFlipperFlasher_erase_chip_data(app->services.flasher, &data);
    FURI_LOG_D(TAG, "write_chip finish %d", data.status);

    switch(data.status) {
    case VWchSwioFlasher_CFF_Ok: {
        show_ok_dialog(app, "Chip programmed");
        break;
    }
    case VWchSwioFlasher_CFF_NoData:
        // TODO throw error
        break;
    default:
        show_error_dialog(app, WchSwioFlasher_CFF_WriteChipCompleted, data.status);
        break;
    }
}

bool wch_swio_flasher_scene_flash_on_event(void* context, SceneManagerEvent event) {
    WchSwioFlasherApp* app = context;
    UNUSED(app);
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case WchSwioFlasher_CFF_ChipInfoCompleted:
            handle_chip_info_completed(app);
            break;
        case WchSwioFlasher_CFF_EraseChipCompleted:
            handle_erase_chip_completed(app);
            break;
        case WchSwioFlasher_CFF_WriteChipCompleted:
            handle_write_chip_completed(app);
            break;
        default:
            break;
        }
        consumed = true;
    }

    return consumed;
}

void wch_swio_flasher_scene_flash_on_exit(void* context) {
    WchSwioFlasherApp* app = context;

    WchSwioFlasher_Ch32vFlipperFlasher_detach(app->services.flasher);

    submenu_reset(app->views.submenu);

    //widget_reset(app->views.widget);
}
