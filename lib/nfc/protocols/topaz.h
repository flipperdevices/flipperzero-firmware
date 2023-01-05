#pragma once

#include <furi_hal_nfc.h>

#define TOPAZ_MAX_SIZE (512)
#define TOPAZ_UID_FULL_LENGTH (7)

#define TOPAZ_CMD_RID (0x78)
#define TOPAZ_CMD_RALL (0x00)
#define TOPAZ_CMD_READ (0x01)
#define TOPAZ_CMD_WRITE_E (0x53)
#define TOPAZ_CMD_WRITE_NE (0x1A)
#define TOPAZ_CMD_RSEG (0x10)
#define TOPAZ_CMD_READ8 (0x02)
#define TOPAZ_CMD_WRITE_E8 (0x54)
#define TOPAZ_CMD_WRITE_NE8 (0x1B)

#define TOPAZ_96_HR0 (0x11)
#define TOPAZ_96_SIZE (96)
#define TOPAZ_512_HR0 (0x12)
#define TOPAZ_512_SIZE (512)

typedef enum {
    TopazTypeUnknown,
    TopazType96,
    TopazType512,
} TopazType;

typedef struct {
    TopazType type;
    size_t size;
    uint8_t hr[2];
    uint8_t data[TOPAZ_MAX_SIZE];
} TopazData;

bool topaz_check_card_type(uint8_t ATQA0, uint8_t ATQA1);
TopazType topaz_get_type_from_hr0(uint8_t hr0);
size_t topaz_get_size_by_type(TopazType type);
bool topaz_read_card(FuriHalNfcTxRxContext* tx_rx, TopazData* data, uint8_t* uid);
