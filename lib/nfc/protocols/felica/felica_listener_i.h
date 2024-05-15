#include "felica_listener.h"

#include <nfc/protocols/nfc_generic_event.h>

typedef enum {
    Felica_ListenerStateIdle,
    Felica_ListenerStateActivated,
} FelicaListenerState;

typedef struct {
    uint8_t length;
    FelicaCommandHeader header;
} FelicaListenerGenericRequest;

typedef struct {
    uint8_t length;
    FelicaCommandHeader header;
    FelicaBlockListElement list[];
} FelicaListenerRequest;

typedef FelicaListenerRequest FelicaListenerReadRequest;
typedef FelicaListenerRequest FelicaListenerWriteRequest;

typedef struct {
    FelicaBlockData blocks[2];
} FelicaListenerWriteBlockData;

struct FelicaListener {
    Nfc* nfc;
    FelicaData* data;
    FelicaListenerState state;
    FelicaAuthentication auth;
    FelicaBlockData mc_shadow;
    bool rc_written;
    ///TODO: replace bools below woth one single bool operation_needs_mac
    bool write_with_mac;
    bool read_with_mac;
    BitBuffer* tx_buffer;
    BitBuffer* rx_buffer;

    NfcGenericEvent generic_event;
    NfcGenericCallback callback;
    void* context;
};

void felica_wcnt_increment(FelicaData* data);
bool felica_wcnt_check_warning_boundary(const FelicaData* data);
bool felica_wcnt_check_error_boundary(const FelicaData* data);
void felica_wcnt_post_process(FelicaData* data);