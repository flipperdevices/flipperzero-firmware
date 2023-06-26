#include "../lfrfid_i.h"
#include <dialogs/dialogs.h>

#define LFRFID_PAGE0_MAX_SIZE 8
#define LFRFID_PAGE1_MAX_SIZE 4

void lfrfid_scene_write_block_t5577_confirm_on_enter(void* context) {
    LfRfid* app = context;
    //SceneManager* scene_manager = app->scene_manager;

    DialogMessage* message = dialog_message_alloc();
    dialog_message_set_header(message, "T5577 write data", 0, 0, AlignLeft, AlignTop);
    dialog_message_set_buttons(message, "0", NULL, "1");
    dialog_message_set_text(
        message,
        "Select page...",
        SCREEN_WIDTH_CENTER,
        SCREEN_HEIGHT_CENTER,
        AlignCenter,
        AlignCenter);
    DialogMessageButton dialog_result = dialog_message_show(app->dialogs, message);
    dialog_message_free(message);
    if(dialog_result == DialogMessageButtonRight)
        app->write_page = 1;
    else
        app->write_page = 0;

    message = dialog_message_alloc();
    uint8_t max_page_size = (app->write_page) ? LFRFID_PAGE1_MAX_SIZE : LFRFID_PAGE0_MAX_SIZE;
    dialog_message_set_header(message, "T5577 write data", 0, 0, AlignLeft, AlignTop);
    uint8_t left_box = app->write_block % max_page_size;
    //char str1[4];
    char* str1 = malloc(4);
    //snprintf(str1, sizeof(str1), "%d", left_box);
    snprintf(str1, 4, "%d", left_box);
    uint8_t center_box = (left_box + 1) % max_page_size;
    //char str2[4];
    char* str2 = malloc(4);
    //snprintf(str2, sizeof(str2), "%d", center_box);
    snprintf(str2, 4, "%d", center_box);
    dialog_message_set_buttons(message, str1, str2, "other...");
    dialog_message_set_text(
        message,
        "Select block...",
        SCREEN_WIDTH_CENTER,
        SCREEN_HEIGHT_CENTER,
        AlignCenter,
        AlignCenter);
    dialog_result = dialog_message_show(app->dialogs, message);
    while(true) {
        if(dialog_result == DialogMessageButtonLeft) {
            app->write_block = left_box;
            break;
        } else if(dialog_result == DialogMessageButtonCenter) {
            app->write_block = center_box;
            break;
        } else {
            left_box += 2;
            left_box %= max_page_size;
            //snprintf(str1, sizeof(str1), "%d", left_box);
            snprintf(str1, 4, "%d", left_box);
            center_box += 2;
            center_box %= max_page_size;
            //snprintf(str2, sizeof(str2), "%d", center_box);
            snprintf(str2, 4, "%d", center_box);
            dialog_message_set_buttons(message, str1, str2, "other...");
            // fix needed?
            dialog_result = dialog_message_show(app->dialogs, message);
        }
    }
    free(str1);
    free(str2);
    dialog_message_free(message);

    message = dialog_message_alloc();
    dialog_message_set_header(message, "T5577 write settings", 0, 0, AlignLeft, AlignTop);
    dialog_message_set_buttons(message, "No", NULL, "Yes");
    dialog_message_set_text(
        message,
        " Set block read-only? If unsure answer no.",
        SCREEN_WIDTH_CENTER,
        SCREEN_HEIGHT_CENTER,
        AlignCenter,
        AlignCenter);
    dialog_result = dialog_message_show(app->dialogs, message);
    dialog_message_free(message);
    if(dialog_result == DialogMessageButtonRight)
        app->extra_options |= LfRfidWriteBlockLockBit;
    else
        app->extra_options &= ~LfRfidWriteBlockLockBit;

    Widget* widget = app->widget;

    widget_add_button_element(widget, GuiButtonTypeLeft, "Exit", lfrfid_widget_callback, app);
    widget_add_button_element(widget, GuiButtonTypeRight, "Start", lfrfid_widget_callback, app);
    widget_add_string_multiline_element(
        widget, 64, 22, AlignCenter, AlignBottom, FontPrimary, "Apply tag to\nFlipper's back");
    widget_add_string_multiline_element(
        widget,
        64,
        45,
        AlignCenter,
        AlignBottom,
        FontSecondary,
        "And don't move it\nwhile process is running");

    view_dispatcher_switch_to_view(app->view_dispatcher, LfRfidViewWidget);
}

bool lfrfid_scene_write_block_t5577_confirm_on_event(void* context, SceneManagerEvent event) {
    LfRfid* app = context;
    SceneManager* scene_manager = app->scene_manager;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeBack) {
        consumed = true; // Ignore Back button presses
    } else if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        if(event.event == GuiButtonTypeLeft) {
            scene_manager_search_and_switch_to_previous_scene(
                scene_manager, LfRfidSceneExtraActions);
        } else if(event.event == GuiButtonTypeRight)
            scene_manager_next_scene(scene_manager, LfRfidSceneWriteBlockT5577);
    }

    return consumed;
}

void lfrfid_scene_write_block_t5577_confirm_on_exit(void* context) {
    LfRfid* app = context;
    widget_reset(app->widget);
}
