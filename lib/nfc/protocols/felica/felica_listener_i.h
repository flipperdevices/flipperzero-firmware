#include "felica_listener.h"

#include <nfc/protocols/nfc_generic_event.h>

typedef enum {
    Felica_ListenerStateIdle,
    Felica_ListenerStateActivated,
} FelicaListenerState;

typedef struct {
    uint8_t length;
    FelicaCommandHeader header;
    FelicaBlockListElement list[];
} FelicaListenerRequest;

struct FelicaListener {
    Nfc* nfc;
    FelicaData* data;
    FelicaListenerState state;
    bool rc_written;

    BitBuffer* tx_buffer;
    BitBuffer* rx_buffer;

    NfcGenericEvent generic_event;
    NfcGenericCallback callback;
    void* context;
};