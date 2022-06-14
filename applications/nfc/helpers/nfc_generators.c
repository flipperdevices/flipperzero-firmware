#include <furi_hal_random.h>
#include "nfc_generators.h"

#define NXP_MANUFACTURER_ID (0x04)

static const uint8_t version_bytes_mf0ulx1[] = {0x00, 0x04, 0x03, 0x00, 0x01, 0x00, 0x00, 0x03};

static void nfc_generate_common_start(NfcDeviceData* data) {
    nfc_device_data_clear(data);
}

static void nfc_generate_mf_ul_uid(uint8_t* uid) {
    uid[0] = NXP_MANUFACTURER_ID;
    furi_hal_random_fill_buf(&uid[1], 6);
    // I'm not sure how this is generated, but the upper nybble always seems to be 8
    uid[6] &= 0x0F;
    uid[6] |= 0x80;
}

static void nfc_generate_mf_ul_common(NfcDeviceData* data) {
    data->nfc_data.type = FuriHalNfcTypeA;
    data->nfc_data.interface = FuriHalNfcInterfaceRf;
    data->nfc_data.uid_len = 7;
    nfc_generate_mf_ul_uid(data->nfc_data.uid);
    data->nfc_data.atqa[0] = 0x44;
    data->nfc_data.atqa[1] = 0x00;
    data->nfc_data.sak = 0x00;
    data->protocol = NfcDeviceProtocolMifareUl;
}

static void nfc_generate_calc_bcc(uint8_t* uid, uint8_t* bcc0, uint8_t* bcc1) {
    *bcc0 = 0x88 ^ uid[0] ^ uid[1] ^ uid[2];
    *bcc1 = uid[3] ^ uid[4] ^ uid[5] ^ uid[6];
}

static void nfc_generate_mf_ul_copy_uid_with_bcc(NfcDeviceData* data) {
    MfUltralightData* mful = &data->mf_ul_data;
    memcpy(mful->data, data->nfc_data.uid, 3);
    memcpy(&mful->data[4], &data->nfc_data.uid[3], 4);
    nfc_generate_calc_bcc(data->nfc_data.uid, &mful->data[3], &mful->data[8]);
}

static void nfc_generate_mf_ul_orig(NfcDeviceData* data) {
    nfc_generate_common_start(data);
    nfc_generate_mf_ul_common(data);

    MfUltralightData* mful = &data->mf_ul_data;
    mful->type = MfUltralightTypeUnknown;
    mful->data_size = 16 * 4;
    nfc_generate_mf_ul_copy_uid_with_bcc(data);
    // TODO: what's internal byte on page 2?
    memset(&mful->data[4 * 4], 0xFF, 4);
}

static void nfc_generate_mf_ul_with_config_common(NfcDeviceData* data, uint8_t num_pages) {
    nfc_generate_common_start(data);
    nfc_generate_mf_ul_common(data);

    MfUltralightData* mful = &data->mf_ul_data;
    mful->data_size = num_pages * 4;
    nfc_generate_mf_ul_copy_uid_with_bcc(data);
    uint16_t config_index = (num_pages - 4) * 4;
    mful->data[config_index] = 0x04; // STRG_MOD_EN
    mful->data[config_index + 3] = 0xFF; // AUTH0
    mful->data[config_index + 5] = 0x05; // VCTID
    memset(&mful->data[config_index + 8], 0xFF, 4); // Default PWD
    if(num_pages > 20) mful->data[config_index - 1] = MF_UL_TEARING_FLAG_DEFAULT;
}

static void nfc_generate_mf_ul_ev1_common(NfcDeviceData* data, uint8_t num_pages) {
    nfc_generate_mf_ul_with_config_common(data, num_pages);
    MfUltralightData* mful = &data->mf_ul_data;
    memcpy(&mful->version, version_bytes_mf0ulx1, sizeof(version_bytes_mf0ulx1));
    for(size_t i = 0; i < 3; ++i) {
        mful->tearing[i] = MF_UL_TEARING_FLAG_DEFAULT;
    }
}

static void nfc_generate_mf_ul_11(NfcDeviceData* data) {
    nfc_generate_mf_ul_ev1_common(data, 20);
    MfUltralightData* mful = &data->mf_ul_data;
    mful->type = MfUltralightTypeUL11;
    mful->version.prod_subtype = 0x01;
    mful->version.storage_size = 0x0B;
    mful->data[16 * 4] = 0x00; // Low capacitance version does not have STRG_MOD_EN
}

static void nfc_generate_mf_ul_h11(NfcDeviceData* data) {
    nfc_generate_mf_ul_ev1_common(data, 20);
    MfUltralightData* mful = &data->mf_ul_data;
    mful->type = MfUltralightTypeUL11;
    mful->version.prod_subtype = 0x02;
    mful->version.storage_size = 0x0B;
}

static void nfc_generate_mf_ul_21(NfcDeviceData* data) {
    nfc_generate_mf_ul_ev1_common(data, 41);
    MfUltralightData* mful = &data->mf_ul_data;
    mful->type = MfUltralightTypeUL11;
    mful->version.prod_subtype = 0x01;
    mful->version.storage_size = 0x0E;
    mful->data[37 * 4] = 0x00; // Low capacitance version does not have STRG_MOD_EN
}

static void nfc_generate_mf_ul_h21(NfcDeviceData* data) {
    nfc_generate_mf_ul_ev1_common(data, 41);
    MfUltralightData* mful = &data->mf_ul_data;
    mful->type = MfUltralightTypeUL11;
    mful->version.prod_subtype = 0x02;
    mful->version.storage_size = 0x0E;
}

static const NfcGenerator mf_ul_generator = {
    "Mifare Ultralight",
    nfc_generate_mf_ul_orig,
    NfcSceneReadMifareUlSuccess};

static const NfcGenerator mf_ul_11_generator = {
    "Mifare Ultralight EV1 11",
    nfc_generate_mf_ul_11,
    NfcSceneReadMifareUlSuccess};

static const NfcGenerator mf_ul_h11_generator = {
    "Mifare Ultralight EV1 H11",
    nfc_generate_mf_ul_h11,
    NfcSceneReadMifareUlSuccess};

static const NfcGenerator mf_ul_21_generator = {
    "Mifare Ultralight EV1 21",
    nfc_generate_mf_ul_21,
    NfcSceneReadMifareUlSuccess};

static const NfcGenerator mf_ul_h21_generator = {
    "Mifare Ultralight EV1 H21",
    nfc_generate_mf_ul_h21,
    NfcSceneReadMifareUlSuccess};

const NfcGenerator* nfc_generators[] = {
    &mf_ul_generator,
    &mf_ul_11_generator,
    &mf_ul_h11_generator,
    &mf_ul_21_generator,
    &mf_ul_h21_generator,
    NULL,
};
