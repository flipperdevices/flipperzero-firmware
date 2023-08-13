#include "util.h"

void utilShowDialog(
    const char* paramTitle,
    const char* paramMessage,
    const char* paramConfirmButtonText) {
    DialogsApp* newDialogsApp = furi_record_open(RECORD_DIALOGS);
    DialogMessage* newDialogMessage = dialog_message_alloc();
    dialog_message_set_header(newDialogMessage, paramTitle, 64, 3, AlignCenter, AlignTop);
    dialog_message_set_text(newDialogMessage, paramMessage, 64, 32, AlignCenter, AlignCenter);
    dialog_message_set_buttons(newDialogMessage, NULL, paramConfirmButtonText, NULL);
    dialog_message_set_icon(newDialogMessage, NULL, 0, 10);
    dialog_message_show(newDialogsApp, newDialogMessage);
    dialog_message_free(newDialogMessage);
    furi_record_close(RECORD_DIALOGS);
    return;
}
