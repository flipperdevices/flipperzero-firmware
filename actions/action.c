
#include "quac.h"
#include "item.h"
#include "action_i.h"

void action_tx(void* context, Item* item, FuriString* error) {
    // FURI_LOG_I(TAG, "action_run: %s : %s", furi_string_get_cstr(item->name), item->ext);

    if(!strcmp(item->ext, ".sub")) {
        action_subghz_tx(context, item->path, error);
    } else if(!strcmp(item->ext, ".ir")) {
        action_ir_tx(context, item->path, error);
    } else if(!strcmp(item->ext, ".rfid")) {
        action_rfid_tx(context, item->path, error);
    } else if(!strcmp(item->ext, ".nfc")) {
        action_nfc_tx(context, item->path, error);
    } else if(!strcmp(item->ext, ".qpl")) {
        action_qpl_tx(context, item->path, error);
    } else {
        FURI_LOG_E(TAG, "Unknown item file type! %s", item->ext);
    }
}
