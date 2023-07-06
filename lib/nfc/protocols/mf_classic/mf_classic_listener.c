#include "mf_classic_listener_i.h"
#include "mf_classic_listener_defs.h"
#include <lib/nfc/helpers/nfc_util.h>

#include <furi.h>
#include <furi_hal_random.h>

#define TAG "MfClassicListener"

#define MF_CLASSIC_MAX_BUFF_SIZE (64)

typedef MfClassicListenerCommand (
    *MfClassicListenerCommandCallback)(MfClassicListener* instance, BitBuffer* buf);

typedef struct {
    uint8_t cmd;
    size_t cmd_len_bits;
    MfClassicListenerCommandCallback callback;
} MfClassicListenerCmdHandler;

static void mf_classic_listener_prepare_emulation(MfClassicListener* instance) {
    instance->total_block_num = mf_classic_get_total_block_num(instance->data->type);
}

static MfClassicListenerCommand mf_classic_listnener_auth_handler(
    MfClassicListener* instance,
    MfClassicKeyType key_type,
    uint8_t block_num) {
    MfClassicListenerCommand command = MfClassicListenerCommandNack;
    do {
        if(block_num < instance->total_block_num) break;

        uint8_t sector_num = mf_classic_get_sector_by_block(block_num);
        if(!mf_classic_is_key_found(instance->data, sector_num, key_type)) {
            command = MfClassicListenerCommandSilent;
            break;
        }

        MfClassicSectorTrailer* sec_tr =
            mf_classic_get_sector_trailer_by_sector(instance->data, sector_num);
        MfClassicKey* key = (key_type == MfClassicKeyTypeA) ? &sec_tr->key_a : &sec_tr->key_b;
        uint64_t key_num = nfc_util_bytes2num(key->data, sizeof(MfClassicKey));
        uint32_t cuid = iso14443_3a_get_cuid(instance->data->iso14443_3a_data);

        instance->auth_context.key_type = key_type;
        instance->auth_context.block_num = block_num;
        furi_hal_random_fill_buf(instance->auth_context.nt.data, sizeof(MfClassicNt));
        uint32_t nt_num = nfc_util_bytes2num(instance->auth_context.nt.data, sizeof(MfClassicNt));

        crypto1_init(instance->crypto, key_num);
        if(instance->comm_state == MfClassicListenerCommStatePlain) {
            crypto1_word(instance->crypto, nt_num ^ cuid, 0);
            bit_buffer_copy_bytes(
                instance->tx_encrypted_buffer,
                instance->auth_context.nt.data,
                sizeof(MfClassicNt));
            iso14443_3a_listener_tx(instance->iso14443_3a_listener, instance->tx_encrypted_buffer);
            command = MfClassicListenerCommandProcessed;
        } else {
            uint8_t key_stream[4] = {};
            nfc_util_num2bytes(nt_num ^ cuid, sizeof(uint32_t), key_stream);
            bit_buffer_copy_bytes(
                instance->tx_plain_buffer, instance->auth_context.nt.data, sizeof(MfClassicNt));
            crypto1_encrypt(
                instance->crypto,
                key_stream,
                instance->tx_plain_buffer,
                instance->tx_encrypted_buffer);
            iso14443_3a_listener_tx_with_custom_parity(
                instance->iso14443_3a_listener, instance->tx_encrypted_buffer);
            command = MfClassicListenerCommandProcessed;
        }

        instance->auth_state = MfClassicListenerAuthStateStarted;
    } while(false);

    return command;
}

MfClassicListenerCommand
    mf_classic_listener_auth_key_a_handler(MfClassicListener* instance, BitBuffer* buff) {
    MfClassicListenerCommand command = mf_classic_listnener_auth_handler(
        instance, MfClassicKeyTypeA, bit_buffer_get_byte(buff, 1));

    return command;
}

MfClassicListenerCommand
    mf_classic_listener_auth_key_b_handler(MfClassicListener* instance, BitBuffer* buff) {
    MfClassicListenerCommand command = mf_classic_listnener_auth_handler(
        instance, MfClassicKeyTypeB, bit_buffer_get_byte(buff, 1));

    return command;
}

static const MfClassicListenerCmdHandler mf_classic_command[] = {
    {
        .cmd = MF_CLASSIC_CMD_AUTH_KEY_A,
        .cmd_len_bits = 2 * 8,
        .callback = mf_classic_listener_auth_key_a_handler,
    },
    {
        .cmd = MF_CLASSIC_CMD_AUTH_KEY_B,
        .cmd_len_bits = 2 * 8,
        .callback = mf_classic_listener_auth_key_b_handler,
    },
};

MfClassicListener*
    mf_classic_listener_alloc(Iso14443_3aListener* iso14443_3a_listener, MfClassicData* data) {
    MfClassicListener* instance = malloc(sizeof(MfClassicListener));
    instance->iso14443_3a_listener = iso14443_3a_listener;
    instance->data = mf_classic_alloc();
    mf_classic_copy(instance->data, data);
    mf_classic_listener_prepare_emulation(instance);

    instance->crypto = crypto1_alloc();
    instance->tx_plain_buffer = bit_buffer_alloc(MF_CLASSIC_MAX_BUFF_SIZE);
    instance->tx_encrypted_buffer = bit_buffer_alloc(MF_CLASSIC_MAX_BUFF_SIZE);
    instance->rx_plain_buffer = bit_buffer_alloc(MF_CLASSIC_MAX_BUFF_SIZE);

    instance->mfc_event.data = &instance->mfc_event_data;
    instance->generic_event.protocol = NfcProtocolMfClassic;
    instance->generic_event.data = &instance->mfc_event;
    instance->generic_event.instance = instance;

    return instance;
}

void mf_classic_listener_free(MfClassicListener* instance) {
    furi_assert(instance);
    furi_assert(instance->data);
    furi_assert(instance->crypto);
    furi_assert(instance->rx_plain_buffer);
    furi_assert(instance->tx_encrypted_buffer);
    furi_assert(instance->tx_plain_buffer);

    mf_classic_free(instance->data);
    crypto1_free(instance->crypto);
    bit_buffer_free(instance->rx_plain_buffer);
    bit_buffer_free(instance->tx_encrypted_buffer);
    bit_buffer_free(instance->tx_plain_buffer);

    free(instance);
}

void mf_classic_listener_set_callback(
    MfClassicListener* instance,
    NfcGenericCallback callback,
    void* context) {
    furi_assert(instance);

    instance->callback = callback;
    instance->context = context;
}

const MfClassicData* mf_classic_listener_get_data(const MfClassicListener* instance) {
    furi_assert(instance);
    furi_assert(instance->data);

    return instance->data;
}

static void mf_classic_listener_send_short_frame(MfClassicListener* instance, uint8_t data) {
    BitBuffer* tx_buffer = instance->tx_plain_buffer;

    bit_buffer_set_size(instance->tx_plain_buffer, 4);
    bit_buffer_set_byte(instance->tx_plain_buffer, 0, data);
    if(instance->comm_state == MfClassicListenerCommStateEncrypted) {
        crypto1_encrypt(
            instance->crypto, NULL, instance->tx_plain_buffer, instance->tx_encrypted_buffer);
        tx_buffer = instance->tx_encrypted_buffer;
    }

    iso14443_3a_listener_tx(instance->iso14443_3a_listener, tx_buffer);
}

static void mf_classic_listener_reset_state(MfClassicListener* instance) {
    crypto1_reset(instance->crypto);
    instance->comm_state = MfClassicListenerCommStatePlain;
    instance->state = MfClassicListenerStateIdle;
    instance->auth_state = MfClassicListenerAuthStateIdle;
}

NfcCommand mf_classic_listener_run(NfcGenericEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.data);
    furi_assert(event.protocol == NfcProtocolIso14443_3a);

    NfcCommand command = NfcCommandContinue;
    MfClassicListener* instance = context;
    Iso14443_3aListenerEvent* iso3_event = event.data;
    BitBuffer* rx_buffer_plain;

    if((iso3_event->type == Iso14443_3aListenerEventTypeReceivedData) ||
       (iso3_event->type == Iso14443_3aListenerEventTypeReceivedStandardFrame)) {
        if(instance->comm_state == MfClassicListenerCommStateEncrypted) {
            crypto1_decrypt(instance->crypto, iso3_event->data->buffer, instance->rx_plain_buffer);
            rx_buffer_plain = instance->rx_plain_buffer;
        } else {
            crypto1_reset(instance->crypto);
            rx_buffer_plain = iso3_event->data->buffer;
        }

        MfClassicListenerCommand mfc_command = MfClassicListenerCommandNack;
        for(size_t i = 0; i < COUNT_OF(mf_classic_command); i++) {
            if(bit_buffer_get_size(rx_buffer_plain) != mf_classic_command[i].cmd_len_bits)
                continue;
            if(bit_buffer_get_byte(rx_buffer_plain, 0) != mf_classic_command[i].cmd) continue;
            mfc_command = mf_classic_command[i].callback(instance, rx_buffer_plain);
            break;
        }
        if(mfc_command == MfClassicListenerCommandAck) {
            mf_classic_listener_send_short_frame(instance, MF_CLASSIC_CMD_ACK);
        } else if(mfc_command == MfClassicListenerCommandNack) {
            mf_classic_listener_send_short_frame(instance, MF_CLASSIC_CMD_NACK);
            mf_classic_listener_reset_state(instance);
        }
    } else if(iso3_event->type == Iso14443_3aListenerEventTypeHalted) {
        mf_classic_listener_reset_state(instance);
    }

    return command;
}

const NfcListenerBase mf_classic_listener = {
    .alloc = (NfcListenerAlloc)mf_classic_listener_alloc,
    .free = (NfcListenerFree)mf_classic_listener_free,
    .set_callback = (NfcListenerSetCallback)mf_classic_listener_set_callback,
    .get_data = (NfcListenerGetData)mf_classic_listener_get_data,
    .run = (NfcListenerRun)mf_classic_listener_run,
};
