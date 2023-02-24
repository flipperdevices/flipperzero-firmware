#include "ibutton_protocol_dallas_defs.h"

#include "ds1990.h"
#include "ds1992.h"
#include "ds1996.h"
#include "ds_generic.h"

const iButtonProtocolDallasBase* ibutton_protocols_dallas[] = {
    [iButtonProtocolDS1990] = &ibutton_protocol_ds1990,
    [iButtonProtocolDS1992] = &ibutton_protocol_ds1992,
    [iButtonProtocolDS1996] = &ibutton_protocol_ds1996,
    /* Add new 1-Wire protocols here */

    /* Default catch-all 1-Wire protocol */
    [iButtonProtocolDSGeneric] = &ibutton_protocol_ds_generic,
};
