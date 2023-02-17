#include "ibutton_protocol_defs.h"

#include "ds1990.h"
#include "ds1992.h"
#include "ds1996.h"

const iButtonProtocolBase* ibutton_protocols[] = {
    [iButtonProtocolDS1990] = &ibutton_protocol_ds1990,
    [iButtonProtocolDS1992] = &ibutton_protocol_ds1992,
    [iButtonProtocolDS1996] = &ibutton_protocol_ds1996,
};
