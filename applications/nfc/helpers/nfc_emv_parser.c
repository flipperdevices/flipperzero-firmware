#include "nfc_emv_parser.h"

#include <file-worker.h>
#include <lib/toolbox/path.h>
#include <lib/toolbox/hex.h>

bool nfc_emv_parser_get_aid_name(uint8_t* aid, uint8_t aid_len, string_t aid_name) {
    bool result = false;
    string_t key;
    string_init(key);
    for(uint8_t i = 0; i < aid_len; i++) {
        string_cat_printf(key, "%02X", aid[i]);
    }
    FileWorker* file_worker = file_worker_alloc(true);

    do {
        if(!file_worker_open(file_worker, "/ext/nfc/emv/aid.nfc", FSAM_READ, FSOM_OPEN_EXISTING)) {
            break;
        }
        if(!file_worker_get_value_from_key(file_worker, key, ' ', aid_name)) {
            break;
        }
        result = true;
    } while(0);

    file_worker_close(file_worker);
    file_worker_free(file_worker);
    string_clear(key);
    return result;
}
