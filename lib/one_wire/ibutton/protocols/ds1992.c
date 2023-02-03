#include "ds1992.h"

#include <core/core_defines.h>

#include "dallas_common.h"

#define DS1992_FAMILY_CODE 0x08U
#define DS1992_USER_DATA_SIZE 128U

static bool dallas_ds1992_read_sram(OneWireHost*, uint8_t*);

const iButtonProtocolBase ibutton_protocol_ds1992 = {
    .family_code = DS1992_FAMILY_CODE,
    .rom_size = DALLAS_COMMON_ROM_SIZE,
    .user_data_size = DS1992_USER_DATA_SIZE,

    .read_rom = dallas_common_read_rom,
    .read_user_data = dallas_ds1992_read_sram,
};

bool dallas_ds1992_read_sram(OneWireHost* host, uint8_t* data) {
    UNUSED(host);
    UNUSED(data);
    return true;
}
