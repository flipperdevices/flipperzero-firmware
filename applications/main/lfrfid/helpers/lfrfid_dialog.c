#include "../lfrfid_i.h"
#include <dialogs/dialogs.h>
#include "lfrfid_dialog.h"

void lfrfid_alert_dialog(void* context, char* text) {
    LfRfid* app = context;

    DialogMessage* message = dialog_message_alloc();
    dialog_message_set_header(message, "Alert!", 0, 0, AlignLeft, AlignTop);
    dialog_message_set_buttons(message, NULL, NULL, NULL);
    dialog_message_set_text(
        message, text, SCREEN_WIDTH_CENTER, SCREEN_HEIGHT_CENTER, AlignCenter, AlignCenter);
    DialogMessageButton dialog_result = dialog_message_show(app->dialogs, message);
    dialog_message_free(message);
    if(dialog_result == DialogMessageButtonRight)
        ;
    return;
}

DialogMessageButton lfrfid_message_dialog(
    void* context,
    char* header,
    char* text,
    char* button_left,
    char* button_center,
    char* button_right) {
    LfRfid* app = context;

    DialogMessage* message = dialog_message_alloc();
    dialog_message_set_header(message, header, 0, 0, AlignLeft, AlignTop);
    dialog_message_set_buttons(message, button_left, button_center, button_right);
    dialog_message_set_text(
        message, text, SCREEN_WIDTH_CENTER, SCREEN_HEIGHT_CENTER, AlignCenter, AlignCenter);
    DialogMessageButton dialog_result = dialog_message_show(app->dialogs, message);
    dialog_message_free(message);

    return dialog_result;
}
