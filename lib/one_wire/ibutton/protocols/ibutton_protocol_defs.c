#include "ibutton_protocol_defs.h"

#include "dallas/ds1990.h"
#include "dallas/ds1992.h"
#include "dallas/ds1996.h"
#include "dallas/ds_generic.h"

const iButtonProtocolBase* ibutton_protocols[] = {
    /* 1-Wire protocols */
    [iButtonProtocolDS1990] = &ibutton_protocol_ds1990,
    [iButtonProtocolDS1992] = &ibutton_protocol_ds1992,
    [iButtonProtocolDS1996] = &ibutton_protocol_ds1996,
    /* Add new 1-Wire protocols here */
    /* Default catch-all 1-Wire protocol */
    [iButtonProtocolDSGeneric] = &ibutton_protocol_ds_generic,
};
