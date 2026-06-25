#include "mf_ultralight_ndef.h"

#include <string.h>
#include <furi.h>
#include <nfc/protocols/mf_ultralight/mf_ultralight.h>

// NTAG21x parameters per NXP datasheets.
typedef struct {
    MfUltralightType ul_type;
    const char* name;
    uint8_t total_pages; // including config pages
    uint8_t cc_size; // value of CC byte 2 (data area / 8)
    uint8_t storage_size; // GET_VERSION storage_size byte
    uint16_t user_capacity_bytes; // usable bytes for the NDEF TLV blob
} NtagParams;

static const NtagParams ntag_params[] = {
    [NdefNtagTypeNTAG213] = {MfUltralightTypeNTAG213, "NTAG213", 45, 0x12, 0x0F, 144},
    [NdefNtagTypeNTAG215] = {MfUltralightTypeNTAG215, "NTAG215", 135, 0x3E, 0x11, 504},
    [NdefNtagTypeNTAG216] = {MfUltralightTypeNTAG216, "NTAG216", 231, 0x6D, 0x13, 888},
};

// NTAG21x GET_VERSION response (fixed prefix; storage_size differs per variant).
static const uint8_t ntag21x_version_prefix[] = {
    0x00, // header
    0x04, // vendor (NXP)
    0x04, // type (NTAG)
    0x02, // subtype
    0x01, // major version
    0x00, // minor version
    // storage_size at byte 6 — filled per variant
    // protocol_type at byte 7
};

const char* mf_ultralight_ndef_ntag_name(NdefNtagType ntag) {
    if(ntag > NdefNtagTypeNTAG216) return "?";
    return ntag_params[ntag].name;
}

size_t mf_ultralight_ndef_user_capacity(NdefNtagType ntag) {
    if(ntag > NdefNtagTypeNTAG216) return 0;
    return ntag_params[ntag].user_capacity_bytes;
}

bool mf_ultralight_ndef_fill_device(
    NfcDevice* nfc_device,
    NdefNtagType ntag,
    const uint8_t* ndef_blob,
    size_t ndef_size) {
    furi_assert(nfc_device);
    furi_assert(ndef_blob);
    if(ntag > NdefNtagTypeNTAG216) return false;
    const NtagParams* p = &ntag_params[ntag];
    if(ndef_size > p->user_capacity_bytes) return false;

    MfUltralightData* data = mf_ultralight_alloc();
    data->type = p->ul_type;
    data->pages_total = p->total_pages;
    data->pages_read = p->total_pages;

    // GET_VERSION
    data->version.header = ntag21x_version_prefix[0];
    data->version.vendor_id = ntag21x_version_prefix[1];
    data->version.prod_type = ntag21x_version_prefix[2];
    data->version.prod_subtype = ntag21x_version_prefix[3];
    data->version.prod_ver_major = ntag21x_version_prefix[4];
    data->version.prod_ver_minor = ntag21x_version_prefix[5];
    data->version.storage_size = p->storage_size;
    data->version.protocol_type = 0x03;

    // ISO14443-3A identification: NTAGs respond with ATQA=0x0044, SAK=0x00, 7-byte UID.
    Iso14443_3aData* iso = data->iso14443_3a_data;
    iso->atqa[0] = 0x44;
    iso->atqa[1] = 0x00;
    iso->sak = 0x00;
    iso->uid_len = 7;
    // Arbitrary but plausible UID — pages 0-2 are read-only on real cards and the
    // poller never writes to them anyway, so the values here don't reach the tag.
    static const uint8_t default_uid[7] = {0x04, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};
    memcpy(iso->uid, default_uid, 7);

    // Mirror the UID into pages 0-1 + BCC bytes (so render/debug paths see sane data).
    // Page 0: UID0..UID2 + BCC0 (CT XOR UID0 XOR UID1 XOR UID2)
    data->page[0].data[0] = default_uid[0];
    data->page[0].data[1] = default_uid[1];
    data->page[0].data[2] = default_uid[2];
    data->page[0].data[3] = 0x88 ^ default_uid[0] ^ default_uid[1] ^ default_uid[2];
    // Page 1: UID3..UID6
    data->page[1].data[0] = default_uid[3];
    data->page[1].data[1] = default_uid[4];
    data->page[1].data[2] = default_uid[5];
    data->page[1].data[3] = default_uid[6];
    // Page 2: BCC1, internal, lock0, lock1 — keep locks zero (poller refuses to write
    // if static-lock bits are set).
    data->page[2].data[0] = default_uid[3] ^ default_uid[4] ^ default_uid[5] ^ default_uid[6];
    data->page[2].data[1] = 0x48; // Internal byte (NTAG default)
    data->page[2].data[2] = 0x00;
    data->page[2].data[3] = 0x00;

    // Page 3: Capability Container (E1 10 <size/8> 00)
    data->page[3].data[0] = 0xE1;
    data->page[3].data[1] = 0x10;
    data->page[3].data[2] = p->cc_size;
    data->page[3].data[3] = 0x00;

    // Pages 4..config_page-1 = user memory. Clear, then write NDEF TLV blob.
    uint8_t config_page = p->total_pages - 4;
    for(uint8_t pg = 4; pg < config_page; pg++) {
        memset(data->page[pg].data, 0, 4);
    }
    // Copy NDEF blob into user memory, page-aligned starting at page 4.
    for(size_t i = 0; i < ndef_size; i++) {
        size_t byte_offset = i;
        uint8_t pg = 4 + (byte_offset / 4);
        uint8_t off = byte_offset % 4;
        data->page[pg].data[off] = ndef_blob[i];
    }

    // Config pages (replicate NXP factory defaults so the poller's lock/dynlock
    // checks pass and the auth handler can use the default password).
    // Page config_page+0: STRG_MOD_EN | RFUI | RFUI | AUTH0
    data->page[config_page].data[0] = 0x04;
    data->page[config_page].data[1] = 0x00;
    data->page[config_page].data[2] = 0x00;
    data->page[config_page].data[3] = 0xFF; // AUTH0 = no password protection
    // Page config_page+1: ACCESS | VCTID | RFUI | RFUI
    data->page[config_page + 1].data[0] = 0x00;
    data->page[config_page + 1].data[1] = 0x05;
    data->page[config_page + 1].data[2] = 0x00;
    data->page[config_page + 1].data[3] = 0x00;
    // Page config_page+2: PWD (default 0xFFFFFFFF)
    memset(data->page[config_page + 2].data, 0xFF, 4);
    // Page config_page+3: PACK (default 0x0000) + RFUI
    data->page[config_page + 3].data[0] = 0x00;
    data->page[config_page + 3].data[1] = 0x00;
    data->page[config_page + 3].data[2] = 0x00;
    data->page[config_page + 3].data[3] = 0x00;

    // Dynamic lock byte page sits just before the config page block. Leave zero
    // so the poller's pre-write check passes.
    if(config_page > 0) {
        data->page[config_page - 1].data[0] = 0x00;
        data->page[config_page - 1].data[1] = 0x00;
        data->page[config_page - 1].data[2] = 0x00;
        data->page[config_page - 1].data[3] = 0xBD; // default tearing flag pattern
    }

    nfc_device_set_data(nfc_device, NfcProtocolMfUltralight, data);
    mf_ultralight_free(data);
    return true;
}
