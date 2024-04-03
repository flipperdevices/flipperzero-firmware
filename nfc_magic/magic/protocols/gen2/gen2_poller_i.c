#include "gen2_poller_i.h"
#include <nfc/helpers/iso14443_crc.h>

#include <bit_lib/bit_lib.h>
#include "furi_hal_random.h"

#include <furi/furi.h>

#define TAG "GEN2_I"

MfClassicError mf_classic_process_error(Iso14443_3aError error) {
    MfClassicError ret = MfClassicErrorNone;

    switch(error) {
    case Iso14443_3aErrorNone:
        ret = MfClassicErrorNone;
        break;
    case Iso14443_3aErrorNotPresent:
        ret = MfClassicErrorNotPresent;
        break;
    case Iso14443_3aErrorColResFailed:
    case Iso14443_3aErrorCommunication:
    case Iso14443_3aErrorWrongCrc:
        ret = MfClassicErrorProtocol;
        break;
    case Iso14443_3aErrorTimeout:
        ret = MfClassicErrorTimeout;
        break;
    default:
        ret = MfClassicErrorProtocol;
        break;
    }
    return ret;
}

Gen2PollerError gen2_poller_process_iso3_error(Iso14443_3aError error) {
    Gen2PollerError ret = Gen2PollerErrorNone;

    switch(error) {
    case Iso14443_3aErrorNone:
        ret = Gen2PollerErrorNone;
        break;
    case Iso14443_3aErrorNotPresent:
        ret = Gen2PollerErrorNotPresent;
        break;
    case Iso14443_3aErrorWrongCrc:
        ret = Gen2PollerErrorProtocol;
        break;
    case Iso14443_3aErrorTimeout:
        ret = Gen2PollerErrorTimeout;
        break;
    default:
        ret = Gen2PollerErrorProtocol;
        break;
    }
    return ret;
}

Gen2PollerError gen2_poller_process_mifare_classic_error(MfClassicError error) {
    Gen2PollerError ret = Gen2PollerErrorNone;

    switch(error) {
    case MfClassicErrorNone:
        ret = Gen2PollerErrorNone;
        break;
    case MfClassicErrorNotPresent:
        ret = Gen2PollerErrorNotPresent;
        break;
    case MfClassicErrorProtocol:
        ret = Gen2PollerErrorProtocol;
        break;
    case MfClassicErrorAuth:
        ret = Gen2PollerErrorAuth;
        break;
    case MfClassicErrorTimeout:
        ret = Gen2PollerErrorTimeout;
        break;
    default:
        ret = Gen2PollerErrorProtocol;
        break;
    }

    return ret;
}

static Gen2PollerError gen2_poller_get_nt_common(
    Gen2Poller* instance,
    uint8_t block_num,
    MfClassicKeyType key_type,
    MfClassicNt* nt,
    bool is_nested) {
    MfClassicError ret = MfClassicErrorNone;
    Iso14443_3aError error = Iso14443_3aErrorNone;

    do {
        uint8_t auth_type = (key_type == MfClassicKeyTypeB) ? MF_CLASSIC_CMD_AUTH_KEY_B :
                                                              MF_CLASSIC_CMD_AUTH_KEY_A;
        uint8_t auth_cmd[2] = {auth_type, block_num};
        bit_buffer_copy_bytes(instance->tx_plain_buffer, auth_cmd, sizeof(auth_cmd));

        if(is_nested) {
            iso14443_crc_append(Iso14443CrcTypeA, instance->tx_plain_buffer);
            crypto1_encrypt(
                instance->crypto, NULL, instance->tx_plain_buffer, instance->tx_encrypted_buffer);
            error = iso14443_3a_poller_txrx_custom_parity(
                instance->iso3_poller,
                instance->tx_encrypted_buffer,
                instance->rx_plain_buffer, // NT gets decrypted by mf_classic_async_auth
                GEN2_POLLER_MAX_FWT);
            if(error != Iso14443_3aErrorNone) {
                ret = mf_classic_process_error(error);
                break;
            }
        } else {
            FURI_LOG_D(TAG, "Plain auth cmd");
            error = iso14443_3a_poller_send_standard_frame(
                instance->iso3_poller,
                instance->tx_plain_buffer,
                instance->rx_plain_buffer,
                GEN2_POLLER_MAX_FWT);
            if(error != Iso14443_3aErrorWrongCrc) {
                ret = mf_classic_process_error(error);
                break;
            }
        }
        if(bit_buffer_get_size_bytes(instance->rx_plain_buffer) != sizeof(MfClassicNt)) {
            ret = MfClassicErrorProtocol;
            break;
        }

        if(nt) {
            bit_buffer_write_bytes(instance->rx_plain_buffer, nt->data, sizeof(MfClassicNt));
        }
    } while(false);

    return gen2_poller_process_mifare_classic_error(ret);
}

Gen2PollerError gen2_poller_get_nt(
    Gen2Poller* instance,
    uint8_t block_num,
    MfClassicKeyType key_type,
    MfClassicNt* nt) {
    return gen2_poller_get_nt_common(instance, block_num, key_type, nt, false);
}

Gen2PollerError gen2_poller_get_nt_nested(
    Gen2Poller* instance,
    uint8_t block_num,
    MfClassicKeyType key_type,
    MfClassicNt* nt) {
    return gen2_poller_get_nt_common(instance, block_num, key_type, nt, true);
}

static Gen2PollerError gen2_poller_auth_common(
    Gen2Poller* instance,
    uint8_t block_num,
    MfClassicKey* key,
    MfClassicKeyType key_type,
    MfClassicAuthContext* data,
    bool is_nested) {
    Gen2PollerError ret = Gen2PollerErrorNone;
    Iso14443_3aError error = Iso14443_3aErrorNone;

    do {
        iso14443_3a_copy(instance->data->iso14443_3a_data, nfc_poller_get_data(instance->poller));

        MfClassicNt nt = {};
        if(is_nested) {
            ret = gen2_poller_get_nt_nested(instance, block_num, key_type, &nt);
        } else {
            ret = gen2_poller_get_nt(instance, block_num, key_type, &nt);
        }
        if(ret != Gen2PollerErrorNone) break;
        if(data) {
            data->nt = nt;
        }

        uint32_t cuid = iso14443_3a_get_cuid(instance->data->iso14443_3a_data);
        uint64_t key_num = bit_lib_bytes_to_num_be(key->data, sizeof(MfClassicKey));
        MfClassicNr nr = {};
        furi_hal_random_fill_buf(nr.data, sizeof(MfClassicNr));

        crypto1_encrypt_reader_nonce(
            instance->crypto,
            key_num,
            cuid,
            nt.data,
            nr.data,
            instance->tx_encrypted_buffer,
            is_nested);
        error = iso14443_3a_poller_txrx_custom_parity(
            instance->iso3_poller,
            instance->tx_encrypted_buffer,
            instance->rx_encrypted_buffer,
            GEN2_POLLER_MAX_FWT);

        if(error != Iso14443_3aErrorNone) {
            ret = gen2_poller_process_iso3_error(error);
            break;
        }
        if(bit_buffer_get_size_bytes(instance->rx_encrypted_buffer) != 4) {
            ret = Gen2PollerErrorAuth;
        }

        crypto1_word(instance->crypto, 0, 0);
        instance->auth_state = Gen2AuthStatePassed;

        if(data) {
            data->nr = nr;
            const uint8_t* nr_ar = bit_buffer_get_data(instance->tx_encrypted_buffer);
            memcpy(data->ar.data, &nr_ar[4], sizeof(MfClassicAr));
            bit_buffer_write_bytes(
                instance->rx_encrypted_buffer, data->at.data, sizeof(MfClassicAt));
        }
    } while(false);

    if(ret != Gen2PollerErrorNone) {
        iso14443_3a_poller_halt(instance->iso3_poller);
    }

    return ret;
}

Gen2PollerError gen2_poller_auth(
    Gen2Poller* instance,
    uint8_t block_num,
    MfClassicKey* key,
    MfClassicKeyType key_type,
    MfClassicAuthContext* data) {
    return gen2_poller_auth_common(instance, block_num, key, key_type, data, false);
}

Gen2PollerError gen2_poller_halt(Gen2Poller* instance) {
    Gen2PollerError ret = Gen2PollerErrorNone;
    Iso14443_3aError error = Iso14443_3aErrorNone;

    do {
        uint8_t halt_cmd[2] = {MF_CLASSIC_CMD_HALT_MSB, MF_CLASSIC_CMD_HALT_LSB};
        bit_buffer_copy_bytes(instance->tx_plain_buffer, halt_cmd, sizeof(halt_cmd));
        iso14443_crc_append(Iso14443CrcTypeA, instance->tx_plain_buffer);

        if(instance->auth_state == Gen2AuthStatePassed) {
            // Send an encrypted halt command
            crypto1_encrypt(
                instance->crypto, NULL, instance->tx_plain_buffer, instance->tx_encrypted_buffer);
            FURI_LOG_D(TAG, "Send enc halt");
            error = iso14443_3a_poller_txrx_custom_parity(
                instance->iso3_poller,
                instance->tx_encrypted_buffer,
                instance->rx_encrypted_buffer,
                GEN2_POLLER_MAX_FWT);
        }

        if(error != Iso14443_3aErrorNone) {
            FURI_LOG_D(TAG, "Enc halt error");
            // Do not break because we still need to halt the iso3 poller
        }

        // Send a regular halt command to halt the iso3 poller
        FURI_LOG_D(TAG, "Send reg halt");
        error = iso14443_3a_poller_halt(instance->iso3_poller);

        if(error != Iso14443_3aErrorTimeout) {
            FURI_LOG_D(TAG, "Reg halt error");
            // Do not break as well becaue the first halt command might have worked
            // and the card didn't respond because it was already halted
        }

        crypto1_reset(instance->crypto);
        instance->auth_state = Gen2AuthStateIdle;
    } while(false);

    return ret;
}

Gen2PollerError
    gen2_poller_write_block(Gen2Poller* instance, uint8_t block_num, const MfClassicBlock* data) {
    Gen2PollerError ret = Gen2PollerErrorNone;
    Iso14443_3aError error = Iso14443_3aErrorNone;

    do {
        uint8_t write_block_cmd[2] = {MF_CLASSIC_CMD_WRITE_BLOCK, block_num};
        bit_buffer_copy_bytes(instance->tx_plain_buffer, write_block_cmd, sizeof(write_block_cmd));
        iso14443_crc_append(Iso14443CrcTypeA, instance->tx_plain_buffer);

        crypto1_encrypt(
            instance->crypto, NULL, instance->tx_plain_buffer, instance->tx_encrypted_buffer);

        error = iso14443_3a_poller_txrx_custom_parity(
            instance->iso3_poller,
            instance->tx_encrypted_buffer,
            instance->rx_encrypted_buffer,
            GEN2_POLLER_MAX_FWT);
        if(error != Iso14443_3aErrorNone) {
            ret = gen2_poller_process_iso3_error(error);
            break;
        }
        if(bit_buffer_get_size(instance->rx_encrypted_buffer) != 4) {
            ret = Gen2PollerErrorProtocol;
            break;
        }

        crypto1_decrypt(
            instance->crypto, instance->rx_encrypted_buffer, instance->rx_plain_buffer);

        if(bit_buffer_get_byte(instance->rx_plain_buffer, 0) != MF_CLASSIC_CMD_ACK) {
            FURI_LOG_D(TAG, "NACK received");
            ret = Gen2PollerErrorProtocol;
            break;
        }

        bit_buffer_copy_bytes(instance->tx_plain_buffer, data->data, sizeof(MfClassicBlock));
        iso14443_crc_append(Iso14443CrcTypeA, instance->tx_plain_buffer);

        crypto1_encrypt(
            instance->crypto, NULL, instance->tx_plain_buffer, instance->tx_encrypted_buffer);

        error = iso14443_3a_poller_txrx_custom_parity(
            instance->iso3_poller,
            instance->tx_encrypted_buffer,
            instance->rx_encrypted_buffer,
            GEN2_POLLER_MAX_FWT);
        if(error != Iso14443_3aErrorNone) {
            ret = gen2_poller_process_iso3_error(error);
            break;
        }
        if(bit_buffer_get_size(instance->rx_encrypted_buffer) != 4) {
            ret = Gen2PollerErrorProtocol;
            break;
        }

        crypto1_decrypt(
            instance->crypto, instance->rx_encrypted_buffer, instance->rx_plain_buffer);

        if(bit_buffer_get_byte(instance->rx_plain_buffer, 0) != MF_CLASSIC_CMD_ACK) {
            FURI_LOG_D(TAG, "NACK received");
            ret = Gen2PollerErrorProtocol;
            break;
        }
    } while(false);

    return ret;
}

bool gen2_poller_can_write_block(const MfClassicData* target_data, uint8_t block_num) {
    furi_assert(target_data);

    bool can_write = true;

    if(block_num == 0 && target_data->iso14443_3a_data->uid_len == 7) {
        // 7-byte UID gen2 cards are not supported yet, need further testing
        can_write = false;
    }

    if(mf_classic_is_sector_trailer(block_num)) {
        can_write = gen2_poller_can_write_sector_trailer(target_data, block_num).all_problems == 0;
    } else {
        can_write = gen2_poller_can_write_data_block(target_data, block_num).all_problems == 0;
    }

    return can_write;
}

Gen2PollerWriteProblems
    gen2_poller_can_write_data_block(const MfClassicData* target_data, uint8_t block_num) {
    // Check whether it's possible to write the block
    furi_assert(target_data);

    // Check rules:
    // 1. Check if block is read
    // 2. Check if we have any of the keys
    // 3. For each key, check if we can write the block
    // 3.1. If none of the keys can write the block, check whether access conditions can be reset to allow writing
    // 3.2 If the above conditions are not met, return an error code

    Gen2PollerWriteProblems can_write = {0};

    bool has_key_a = mf_classic_is_key_found(
        target_data, mf_classic_get_sector_by_block(block_num), MfClassicKeyTypeA);
    bool has_key_b = mf_classic_is_key_found(
        target_data, mf_classic_get_sector_by_block(block_num), MfClassicKeyTypeB);

    if(!has_key_a && !has_key_b) {
        can_write.missing_target_keys = true;
    }
    if(!gen2_is_allowed_access(
           target_data, block_num, MfClassicKeyTypeA, MfClassicActionDataWrite) &&
       !gen2_is_allowed_access(
           target_data, block_num, MfClassicKeyTypeB, MfClassicActionDataWrite)) {
        if(!gen2_can_reset_access_conditions(target_data, block_num)) {
            can_write.locked_access_bits = true;
        }
    }

    return can_write;
}

Gen2PollerWriteProblems
    gen2_poller_can_write_sector_trailer(const MfClassicData* target_data, uint8_t block_num) {
    // Check whether it's possible to write the sector trailer
    furi_assert(target_data);

    // Check rules:
    // 1. Check if block is read
    // 2. Check if we have any of the keys
    // 3. For each key, check if we can write the block
    // 3.1 Check that at least one of the keys can write Key A
    // 3.1.1 If none of the keys can write Key A, check whether access conditions can be reset to allow writing
    // 3.2 Check that at least one of the keys can write the Access Conditions
    // 3.3 Check that at least one of the keys can write Key B
    // 3.3.1 If none of the keys can write Key B, check whether access conditions can be reset to allow writing
    // 3.4 If any of the above conditions are not met, return an error code

    Gen2PollerWriteProblems can_write = {0};

    bool has_key_a = mf_classic_is_key_found(
        target_data, mf_classic_get_sector_by_block(block_num), MfClassicKeyTypeA);
    bool has_key_b = mf_classic_is_key_found(
        target_data, mf_classic_get_sector_by_block(block_num), MfClassicKeyTypeB);

    if(!has_key_a && !has_key_b) {
        can_write.missing_target_keys = true;
    }
    if(!gen2_is_allowed_access(
           target_data, block_num, MfClassicKeyTypeA, MfClassicActionKeyAWrite) &&
       !gen2_is_allowed_access(
           target_data, block_num, MfClassicKeyTypeB, MfClassicActionKeyAWrite)) {
        if(!gen2_can_reset_access_conditions(target_data, block_num)) {
            can_write.locked_access_bits = true;
        }
    }
    if(!gen2_is_allowed_access(target_data, block_num, MfClassicKeyTypeA, MfClassicActionACWrite) &&
       !gen2_is_allowed_access(target_data, block_num, MfClassicKeyTypeB, MfClassicActionACWrite)) {
        can_write.locked_access_bits = true;
    }
    if(!gen2_is_allowed_access(
           target_data, block_num, MfClassicKeyTypeA, MfClassicActionKeyBWrite) &&
       !gen2_is_allowed_access(
           target_data, block_num, MfClassicKeyTypeB, MfClassicActionKeyBWrite)) {
        if(!gen2_can_reset_access_conditions(target_data, block_num)) {
            can_write.locked_access_bits = true;
        }
    }

    return can_write;
}

bool gen2_can_reset_access_conditions(const MfClassicData* target_data, uint8_t block_num) {
    // Check whether it's possible to reset the access conditions
    furi_assert(target_data);

    // Check rules:
    // 1. Check if the sector trailer for this block is read
    // 2. Check if we have any of the keys
    // 3. For each key, check if we can write the access conditions
    // 3.1. If none of the keys can write the access conditions, return false

    bool can_reset = false;

    bool has_key_a = mf_classic_is_key_found(
        target_data, mf_classic_get_sector_by_block(block_num), MfClassicKeyTypeA);
    bool has_key_b = mf_classic_is_key_found(
        target_data, mf_classic_get_sector_by_block(block_num), MfClassicKeyTypeB);
    uint8_t sector_tr_num = mf_classic_get_sector_trailer_num_by_block(block_num);

    if(!mf_classic_is_block_read(target_data, sector_tr_num)) {
        can_reset = false;
        return can_reset;
    }

    if(!has_key_a && !has_key_b) {
        can_reset = false;
        return can_reset;
    }
    if(gen2_is_allowed_access(target_data, block_num, MfClassicKeyTypeA, MfClassicActionACWrite) ||
       gen2_is_allowed_access(target_data, block_num, MfClassicKeyTypeB, MfClassicActionACWrite)) {
        can_reset = true;
    }

    return can_reset;
}

MfClassicKeyType
    gen2_poller_get_key_type_to_write(const MfClassicData* target_data, uint8_t block_num) {
    // Get the key type to use for writing
    // We assume that at least one of the keys can write the block
    furi_assert(target_data);

    MfClassicKeyType key_type = MfClassicKeyTypeA;

    if(gen2_is_allowed_access(
           target_data, block_num, MfClassicKeyTypeA, MfClassicActionDataWrite)) {
        key_type = MfClassicKeyTypeA;
    } else if(gen2_is_allowed_access(
                  target_data, block_num, MfClassicKeyTypeB, MfClassicActionDataWrite)) {
        key_type = MfClassicKeyTypeB;
    }

    return key_type;
}

static bool gen2_is_allowed_access_sector_trailer(
    const MfClassicData* data,
    uint8_t block_num,
    MfClassicKeyType key_type,
    MfClassicAction action) {
    uint8_t sector_num = mf_classic_get_sector_by_block(block_num);
    MfClassicSectorTrailer* sec_tr = mf_classic_get_sector_trailer_by_sector(data, sector_num);
    uint8_t* access_bits_arr = sec_tr->access_bits.data;
    uint8_t AC = ((access_bits_arr[1] >> 5) & 0x04) | ((access_bits_arr[2] >> 2) & 0x02) |
                 ((access_bits_arr[2] >> 7) & 0x01);
    FURI_LOG_T("NFC", "AC: %02X", AC);

    switch(action) {
    case MfClassicActionKeyARead: {
        return false;
    }
    case MfClassicActionKeyAWrite:
    case MfClassicActionKeyBWrite: {
        return (
            (key_type == MfClassicKeyTypeA && (AC == 0x00 || AC == 0x01)) ||
            (key_type == MfClassicKeyTypeB &&
             (AC == 0x00 || AC == 0x04 || AC == 0x03 || AC == 0x01)));
    }
    case MfClassicActionKeyBRead: {
        return (key_type == MfClassicKeyTypeA && (AC == 0x00 || AC == 0x02 || AC == 0x01)) ||
               (key_type == MfClassicKeyTypeB && (AC == 0x00 || AC == 0x02 || AC == 0x01));
    }
    case MfClassicActionACRead: {
        return ((key_type == MfClassicKeyTypeA) || (key_type == MfClassicKeyTypeB));
    }
    case MfClassicActionACWrite: {
        return (
            (key_type == MfClassicKeyTypeA && (AC == 0x01)) ||
            (key_type == MfClassicKeyTypeB && (AC == 0x01 || AC == 0x03 || AC == 0x05)));
    }
    default:
        return false;
    }
    return true;
}

bool gen2_is_allowed_access_data_block(
    MfClassicSectorTrailer* sec_tr,
    uint8_t block_num,
    MfClassicKeyType key_type,
    MfClassicAction action) {
    // Same as mf_classic_is_allowed_access_data_block but with sector 0 allowed
    furi_assert(sec_tr);

    uint8_t* access_bits_arr = sec_tr->access_bits.data;

    uint8_t sector_block = 0;
    if(block_num <= 128) {
        sector_block = block_num & 0x03;
    } else {
        sector_block = (block_num & 0x0f) / 5;
    }

    uint8_t AC;
    switch(sector_block) {
    case 0x00: {
        AC = ((access_bits_arr[1] >> 2) & 0x04) | ((access_bits_arr[2] << 1) & 0x02) |
             ((access_bits_arr[2] >> 4) & 0x01);
        break;
    }
    case 0x01: {
        AC = ((access_bits_arr[1] >> 3) & 0x04) | ((access_bits_arr[2] >> 0) & 0x02) |
             ((access_bits_arr[2] >> 5) & 0x01);
        break;
    }
    case 0x02: {
        AC = ((access_bits_arr[1] >> 4) & 0x04) | ((access_bits_arr[2] >> 1) & 0x02) |
             ((access_bits_arr[2] >> 6) & 0x01);
        break;
    }
    default:
        return false;
    }

    switch(action) {
    case MfClassicActionDataRead: {
        return (
            (key_type == MfClassicKeyTypeA && !(AC == 0x03 || AC == 0x05 || AC == 0x07)) ||
            (key_type == MfClassicKeyTypeB && !(AC == 0x07)));
    }
    case MfClassicActionDataWrite: {
        return (
            (key_type == MfClassicKeyTypeA && (AC == 0x00)) ||
            (key_type == MfClassicKeyTypeB &&
             (AC == 0x00 || AC == 0x04 || AC == 0x06 || AC == 0x03)));
    }
    case MfClassicActionDataInc: {
        return (
            (key_type == MfClassicKeyTypeA && (AC == 0x00)) ||
            (key_type == MfClassicKeyTypeB && (AC == 0x00 || AC == 0x06)));
    }
    case MfClassicActionDataDec: {
        return (
            (key_type == MfClassicKeyTypeA && (AC == 0x00 || AC == 0x06 || AC == 0x01)) ||
            (key_type == MfClassicKeyTypeB && (AC == 0x00 || AC == 0x06 || AC == 0x01)));
    }
    default:
        return false;
    }

    return false;
}

bool gen2_is_allowed_access(
    const MfClassicData* data,
    uint8_t block_num,
    MfClassicKeyType key_type,
    MfClassicAction action) {
    // Same as mf_classic_is_allowed_access but with sector 0 allowed
    furi_assert(data);

    bool access_allowed = false;
    if(mf_classic_is_sector_trailer(block_num)) {
        access_allowed = gen2_is_allowed_access_sector_trailer(data, block_num, key_type, action);
    } else {
        uint8_t sector_num = mf_classic_get_sector_by_block(block_num);
        MfClassicSectorTrailer* sec_tr = mf_classic_get_sector_trailer_by_sector(data, sector_num);
        access_allowed = gen2_is_allowed_access_data_block(sec_tr, block_num, key_type, action);
    }

    return access_allowed;
}