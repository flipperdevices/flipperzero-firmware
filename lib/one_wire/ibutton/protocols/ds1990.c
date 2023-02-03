#include "ds1990.h"

#include "dallas_common.h"

#define DS1990_FAMILY_CODE 0x01U
#define DS1990_USER_DATA_SIZE 0U

const iButtonProtocolBase ibutton_protocol_ds1990 = {
    .family_code = DS1990_FAMILY_CODE,
    .rom_size = DALLAS_COMMON_ROM_SIZE,
    .user_data_size = DS1990_USER_DATA_SIZE,

    .read_rom = dallas_common_read_rom,
    .read_user_data = NULL,
};
