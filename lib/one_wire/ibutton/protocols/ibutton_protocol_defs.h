#pragma once

typedef enum {
    /* 1-Wire protocols */
    iButtonProtocolDS1990,
    iButtonProtocolDS1992,
    iButtonProtocolDS1996,
    /* Add new 1-Wire protocols here */
    /* Default catch-all 1-Wire protocol */
    iButtonProtocolDSGeneric,
    /* Miscellaneous comparator-based protocols */
    // iButtonProtocolCyfral,
    // iButtonProtocolMetakom,
    /* Add new comparator protocols here */
    iButtonProtocolMax
} iButtonProtocol;
