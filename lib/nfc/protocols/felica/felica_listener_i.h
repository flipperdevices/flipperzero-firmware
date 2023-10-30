#include "felica_listener.h"

#include <nfc/protocols/nfc_generic_event.h>

typedef enum {
    Felica_ListenerStateIdle,
    Felica_ListenerStateActivated,
} FelicaListenerState;

struct FelicaListener {
    Nfc* nfc;
    FelicaData* data;
    FelicaListenerState state;

    BitBuffer* tx_buffer;

    NfcGenericEvent generic_event;
    //Iso14443_3aListenerEvent iso14443_3a_event;
    //Iso14443_3aListenerEventData iso14443_3a_event_data;
    NfcGenericCallback callback;
    void* context;
};