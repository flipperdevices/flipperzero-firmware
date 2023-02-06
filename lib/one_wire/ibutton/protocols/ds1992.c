#include "ds1992.h"

#include <core/core_defines.h>

#include "dallas_common.h"

#define DS1992_FAMILY_CODE 0x08U
#define DS1992_USER_DATA_SIZE 128U

static bool dallas_ds1992_read(OneWireHost* host, void* protocol_data);

const iButtonProtocolBase ibutton_protocol_ds1992 = {
    .family_code = DS1992_FAMILY_CODE,
    .data_size = sizeof(DallasCommonRomData) + DS1992_USER_DATA_SIZE,

    .read = dallas_ds1992_read,
    .render_data = NULL,
    .render_brief_data = NULL,
};

bool dallas_ds1992_read(OneWireHost* host, iButtonProtocolData* protocol_data) {
    UNUSED(host);
    UNUSED(protocol_data);
    return true;
}
