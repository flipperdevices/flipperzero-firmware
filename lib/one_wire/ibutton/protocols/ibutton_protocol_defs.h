#pragma once

typedef enum {
    iButtonProtocolMin,
    /* 1-Wire protocols */
    iButtonProtocolOneWireMin = iButtonProtocolMin,
    iButtonProtocolDS1990 = iButtonProtocolOneWireMin,
    iButtonProtocolDS1992,
    iButtonProtocolDS1996,
    /* Add new 1-Wire protocols here */
    /* Default catch-all 1-Wire protocol */
    iButtonProtocolDSGeneric,
    iButtonProtocolOneWireMax = iButtonProtocolDSGeneric,
    /* Miscellaneous comparator-based protocols */
    iButtonProtocolMiscMin,
    iButtonProtocolCyfral = iButtonProtocolMiscMin,
    iButtonProtocolMetakom,
    /* Add new comparator protocols here */
    iButtonProtocolMax
} iButtonProtocol;
