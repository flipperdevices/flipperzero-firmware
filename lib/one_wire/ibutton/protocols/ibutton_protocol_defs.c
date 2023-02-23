#include "ibutton_protocol_defs_i.h"

#include "ds1990.h"
#include "ds1992.h"
#include "ds1996.h"
#include "ds_generic.h"

const iButtonProtocolBase* ibutton_protocols[] = {
    [iButtonProtocolDS1990] = &ibutton_protocol_ds1990,
    [iButtonProtocolDS1992] = &ibutton_protocol_ds1992,
    [iButtonProtocolDS1996] = &ibutton_protocol_ds1996,
    [iButtonProtocolDSGeneric] = &ibutton_protocol_ds_generic,
};
