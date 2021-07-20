#include "dialogs-i.h"
#include "dialogs-api-lock.h"
#include <gui/modules/dialog_ex.h>

typedef struct {
    osSemaphoreId_t semaphore;
    DialogMessageButton result;
} DialogsAppMessageContext;

static void dialogs_app_message_back_callback(void* context) {
    furi_assert(context);
    DialogsAppMessageContext* message_context = context;
    message_context->result = DialogMessageButtonBack;
    API_LOCK_UNLOCK(message_context->semaphore);
}

static void dialogs_app_message_callback(DialogExResult result, void* context) {
    furi_assert(context);
    DialogsAppMessageContext* message_context = context;
    switch(result) {
    case DialogExResultLeft:
        message_context->result = DialogMessageButtonLeft;
        break;
    case DialogExResultRight:
        message_context->result = DialogMessageButtonRight;
        break;
    case DialogExResultCenter:
        message_context->result = DialogMessageButtonCenter;
        break;
    }
    API_LOCK_UNLOCK(message_context->semaphore);
}

DialogMessageButton dialogs_app_process_module_message(const DialogsAppMessageDataDialog* data) {
    DialogMessageButton ret = DialogMessageButtonBack;
    Gui* gui = furi_record_open("gui");
    const DialogMessage* message = data->message;
    DialogsAppMessageContext* message_context = furi_alloc(sizeof(DialogsAppMessageContext));
    message_context->semaphore = API_LOCK_INIT_LOCKED();

    ViewHolder* view_holder = view_holder_alloc();
    view_holder_attach_to_gui(view_holder, gui);
    view_holder_set_back_callback(view_holder, dialogs_app_message_back_callback, message_context);

    DialogEx* dialog_ex = dialog_ex_alloc();
    dialog_ex_set_result_callback(dialog_ex, dialogs_app_message_callback);
    dialog_ex_set_context(dialog_ex, message_context);
    dialog_ex_set_header(
        dialog_ex,
        message->header_text,
        message->header_text_x,
        message->header_text_y,
        message->header_horizontal,
        message->header_vertical);
    dialog_ex_set_text(
        dialog_ex,
        message->dialog_text,
        message->dialog_text_x,
        message->dialog_text_y,
        message->dialog_text_horizontal,
        message->dialog_text_vertical);
    dialog_ex_set_icon(dialog_ex, message->icon_x, message->icon_y, message->icon);
    dialog_ex_set_left_button_text(dialog_ex, message->left_button_text);
    dialog_ex_set_center_button_text(dialog_ex, message->center_button_text);
    dialog_ex_set_right_button_text(dialog_ex, message->right_button_text);

    view_holder_set_view(view_holder, dialog_ex_get_view(dialog_ex));
    view_holder_start(view_holder);
    API_LOCK_WAIT_UNTIL_UNLOCK_AND_FREE(message_context->semaphore);

    ret = message_context->result;

    free(message_context);
    view_holder_stop(view_holder);
    view_holder_free(view_holder);
    dialog_ex_free(dialog_ex);
    furi_record_close("gui");

    return ret;
}