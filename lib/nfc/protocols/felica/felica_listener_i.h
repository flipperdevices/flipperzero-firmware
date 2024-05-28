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
    FelicaListenerGenericRequest base;
    FelicaBlockListElement list[];
} FelicaListenerRequest;

typedef FelicaListenerRequest FelicaListenerReadRequest;
typedef FelicaListenerRequest FelicaListenerWriteRequest;

typedef struct {
    FelicaBlockData blocks[2];
} FelicaListenerWriteBlockData;

typedef void (*FelicaCommandWriteBlockHandler)(
    FelicaListener* instance,
    const uint8_t block_number,
    const FelicaBlockData* data_block);

typedef void (*FelicaCommanReadBlockHandler)(
    FelicaListener* instance,
    const uint8_t block_number,
    const uint8_t resp_data_index,
    FelicaListenerReadCommandResponse* response);

struct FelicaListener {
    Nfc* nfc;
    FelicaData* data;
    FelicaListenerState state;
    FelicaAuthentication auth;
    FelicaBlockData mc_shadow;

    uint8_t block_list_size;
    uint8_t requested_blocks[4];
    uint8_t mac_calc_start;
    bool rc_written;

    BitBuffer* tx_buffer;
    BitBuffer* rx_buffer;

    NfcGenericEvent generic_event;
    NfcGenericCallback callback;
    void* context;
};

void felica_listener_reset(FelicaListener* instance);
void felica_wcnt_increment(FelicaData* data);
//void felica_wcnt_post_process(FelicaData* data);
bool felica_listener_check_idm(const FelicaListener* instance, const FelicaIDm* request_idm);
bool felica_listener_check_block_list_size(
    FelicaListener* instance,
    FelicaListenerGenericRequest* request);
const FelicaBlockListElement* felica_listener_block_list_item_get_first(
    FelicaListener* instance,
    const FelicaListenerRequest* request);
const FelicaBlockListElement* felica_listener_block_list_item_get_next(
    FelicaListener* instance,
    const FelicaBlockListElement* item);

//uint8_t felica_listener_get_block_index(uint8_t number);
//bool felica_block_exists(uint8_t number);
//bool felica_get_mc_bit(const FelicaListener* instance, uint8_t byte_index, uint8_t bit_number);
/* bool felica_block_requires_auth(
    const FelicaListener* instance,
    uint8_t command,
    uint8_t block_number);
 */
//bool felica_block_is_readonly(const FelicaListener* instance, uint8_t block_number);
//bool felica_block_requires_mac(const FelicaListener* instance, uint8_t block_number);
const FelicaListenerWriteBlockData* felica_listener_get_write_request_data_pointer(
    const FelicaListenerGenericRequest* const generic_request);

bool felica_listener_validate_write_request_and_set_sf(
    FelicaListener* instance,
    const FelicaListenerWriteRequest* const request,
    const FelicaListenerWriteBlockData* const data,
    FelicaListenerWriteCommandResponse* response);

bool felica_listener_validate_read_request_and_set_sf(
    FelicaListener* instance,
    const FelicaListenerReadRequest* const request,
    FelicaCommandResponseHeader* resp_header);

FelicaCommanReadBlockHandler felica_listener_get_read_block_handler(const uint8_t block_number);
FelicaCommandWriteBlockHandler felica_listener_get_write_block_handler(const uint8_t block_number);

FelicaError
    felica_listener_frame_exchange(const FelicaListener* instance, const BitBuffer* tx_buffer);