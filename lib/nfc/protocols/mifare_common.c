#include "mifare_common.h"
#include "furi_hal_nfc.h"

MifareType mifare_common_get_type(FuriHalNfcADevData* data) {
    uint8_t ATQA0 = data->atqa[0];
    uint8_t ATQA1 = data->atqa[1];
    uint8_t SAK = data->sak;
    MifareType type = MifareTypeUnknown;

    if((ATQA0 == 0x44) && (ATQA1 == 0x00) && (SAK == 0x00)) {
        type = MifareTypeUltralight;
    } else if(
        ((ATQA0 == 0x44 || ATQA0 == 0x04) && (SAK == 0x08 || SAK == 0x88 || SAK == 0x09)) ||
        ((ATQA0 == 0x42 || ATQA0 == 0x02) && (SAK == 0x18)) ||
        ((ATQA0 == 0x01) && (ATQA1 == 0x0F) && (SAK == 0x01))) {
        type = MifareTypeClassic;
    } else if(ATQA0 == 0x44 && ATQA1 == 0x03 && SAK == 0x20) {
        type = MifareTypeDesfire;
    }

    return type;
}
