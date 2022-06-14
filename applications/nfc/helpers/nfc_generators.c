#include <furi_hal_random.h>
#include "nfc_generators.h"

#define NXP_MANUFACTURER_ID (0x04)

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

static const NfcGenerator mf_ul_generator = {
    "Mifare Ultralight",
    nfc_generate_mf_ul_orig,
    NfcSceneReadMifareUlSuccess};

const NfcGenerator* nfc_generators[] = {
    &mf_ul_generator,
    NULL,
};
