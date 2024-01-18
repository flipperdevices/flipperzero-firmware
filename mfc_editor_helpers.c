#include "mfc_editor_app_i.h"

uint8_t mfc_editor_calculate_uid_bcc(uint8_t* uid, uint8_t uid_len) {
    furi_check(uid_len > 0);

    uint8_t bcc = uid[0];
    for(int i = 1; i < uid_len; i++) {
        bcc ^= uid[i];
    }

    return bcc;
}
