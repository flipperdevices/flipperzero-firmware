#include "mf_classic_poller_data_formatter.h"
#include "../iso14443_3a/iso14443_3a_poller_data_formatter_i.h"
#include "../nfc_hal/nfc_hal_formatter.h"

#include <nfc/protocols/mf_classic/mf_classic_poller_history_data.h>

#define TAG "MfcPollF"

static const char* poller_states[MfClassicPollerStateNum] = {
    [MfClassicPollerStateDetectType] = "Detect Type",
    [MfClassicPollerStateStart] = "Start",

    // Write states
    [MfClassicPollerStateRequestSectorTrailer] = "Req SecTr",
    [MfClassicPollerStateCheckWriteConditions] = "Check WrConds",
    [MfClassicPollerStateReadBlock] = "Read Block",
    [MfClassicPollerStateWriteBlock] = "Write Block",
    [MfClassicPollerStateWriteValueBlock] = "Write Value Block",

    // Read states
    [MfClassicPollerStateRequestReadSector] = "Req Read Sector",
    [MfClassicPollerStateReadSectorBlocks] = "Read Sector Block",

    // Dict attack states
    [MfClassicPollerStateNextSector] = "Next Sector",
    [MfClassicPollerStateAnalyzeBackdoor] = "Analyze Backdoor",
    [MfClassicPollerStateBackdoorReadSector] = "Backdoor Read Sector",
    [MfClassicPollerStateRequestKey] = "Request Key",
    [MfClassicPollerStateReadSector] = "Read Sector",
    [MfClassicPollerStateAuthKeyA] = "Auth Key A",
    [MfClassicPollerStateAuthKeyB] = "Auth Key B",
    [MfClassicPollerStateKeyReuseStart] = "Key Reuse Start",
    [MfClassicPollerStateKeyReuseStartNoOffset] = "Key Reuse Start No Off",
    [MfClassicPollerStateKeyReuseAuthKeyA] = "Reuse Auth Key A",
    [MfClassicPollerStateKeyReuseAuthKeyB] = "Reuse Auth Key B",
    [MfClassicPollerStateKeyReuseReadSector] = "Reuse Read Sector",
    [MfClassicPollerStateSuccess] = "Success",
    [MfClassicPollerStateFail] = "Fail",

    //Enhanced dictionary attack states
    [MfClassicPollerStateNestedAnalyzePRNG] = "Nst Analyze PRNG",
    [MfClassicPollerStateNestedCalibrate] = "Nst Calibrate",
    [MfClassicPollerStateNestedCollectNt] = "Nst Collect NT",
    [MfClassicPollerStateNestedController] = "Nst Controller",
    [MfClassicPollerStateNestedCollectNtEnc] = "Nst Collect NTenc",
    [MfClassicPollerStateNestedDictAttack] = "Nested Dict Attack",
    [MfClassicPollerStateNestedLog] = "Nested Log",
};

static void mf_classic_poller_data_format(
    const NfcPacket* request,
    const NfcHistoryData* data,
    FuriString* output) {
    const MfClassicPollerHistoryData* mf_classic_data = data;
    UNUSED(request);

    const char* event_text = iso14443_3a_poller_data_format_event_type(mf_classic_data->event);
    const char* state_text = poller_states[mf_classic_data->state];
    const char* result_text = nfc_hal_data_format_nfc_command(mf_classic_data->command);
    furi_string_printf(output, "E=%s, MFC: %s, R=%s", event_text, state_text, result_text);
}

static bool mf_classic_poller_decrypt_payload(
    const NfcPacket* packet,
    NfcHistoryData* data,
    NfcPacket* output) {
    bool result = false;

    MfClassicPollerHistoryData* mf_classic_data = data;

    do {
        if(!mf_classic_data->encrypted || !mf_classic_data->mode_ctx.read_ctx.auth_passed) {
            break;
        }

        Crypto1* crypto = &mf_classic_data->crypto;
        for(size_t i = 0; i < packet->data_size; i++) {
            output->data[i] = crypto1_byte(crypto, 0, 0) ^ packet->data[i];
        }

        result = true;
    } while(false);

    return result;
}

static NfcHistoryCrcStatus mf_classic_poller_get_crc_status(const NfcHistoryData* data) {
    const MfClassicPollerHistoryData* mf_classic_data = data;
    return mf_classic_data->crc_ok ? NfcHistoryCrcOk : NfcHistoryCrcBad;
}

const NfcProtocolFormatterBase mf_classic_poller_data_formatter = {
    .format_history = mf_classic_poller_data_format,
    .decrypt = mf_classic_poller_decrypt_payload,
    .get_crc_status = mf_classic_poller_get_crc_status,
};
