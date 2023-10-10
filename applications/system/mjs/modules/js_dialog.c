#include <core/common_defines.h>
#include "js_modules.h"
#include <dialogs/dialogs.h>

static bool js_dialog_parse_params(struct mjs* mjs, const char** hdr, const char** msg) {
    size_t num_args = mjs_nargs(mjs);
    if(num_args != 2) {
        return false;
    }
    mjs_val_t header_obj = mjs_arg(mjs, 0);
    mjs_val_t msg_obj = mjs_arg(mjs, 1);
    if((!mjs_is_string(header_obj)) || (!mjs_is_string(msg_obj))) {
        return false;
    }

    size_t arg_len = 0;
    *hdr = mjs_get_string(mjs, &header_obj, &arg_len);
    if(arg_len == 0) {
        *hdr = NULL;
    }

    *msg = mjs_get_string(mjs, &msg_obj, &arg_len);
    if(arg_len == 0) {
        *msg = NULL;
    }

    return true;
}

static void js_dialog_message(struct mjs* mjs) {
    const char* dialog_header = NULL;
    const char* dialog_msg = NULL;
    if(!js_dialog_parse_params(mjs, &dialog_header, &dialog_msg)) {
        mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "");
        mjs_return(mjs, MJS_UNDEFINED);
        return;
    }
    DialogsApp* dialogs = furi_record_open(RECORD_DIALOGS);
    DialogMessage* message = dialog_message_alloc();
    dialog_message_set_buttons(message, NULL, "OK", NULL);
    if(dialog_header) {
        dialog_message_set_header(message, dialog_header, 64, 3, AlignCenter, AlignTop);
    }
    if(dialog_msg) {
        dialog_message_set_text(message, dialog_msg, 64, 26, AlignCenter, AlignTop);
    }
    dialog_message_show(dialogs, message);
    dialog_message_free(message);
    furi_record_close(RECORD_DIALOGS);
    mjs_return(mjs, MJS_UNDEFINED);
}

void* js_dialog_create(struct mjs* mjs, mjs_val_t* object) {
    mjs_val_t dialog_obj = mjs_mk_object(mjs);
    mjs_set(mjs, dialog_obj, "message", ~0, MFS_MK_FN(js_dialog_message));
    *object = dialog_obj;

    return (void*)1;
}
