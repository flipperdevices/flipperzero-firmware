#include <furi/furi.h>

#define TAG "NfcParser"

int32_t nfc_parser_app(void* p) {
    UNUSED(p);

    FURI_LOG_I(TAG, "Hi");
    if(p) {
        FURI_LOG_I(TAG, "ARG: %s", (char*)p);
    }

    return 0;
}
