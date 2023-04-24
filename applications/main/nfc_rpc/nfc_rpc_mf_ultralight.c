#include "nfc_rpc_i.h"

#include "assets/compiled/mf_ultralight.pb.h"
#include <lib/nfc/protocols/mf_ultralight/mf_ultralight_poller.h>

#define TAG "NfcRpcMfUltralight"

static PB_MfUltralight_Error nfc_rpc_mf_ultralight_process_error(MfUltralightError error) {
    PB_MfUltralight_Error ret = PB_MfUltralight_Error_None;

    switch(error) {
    case MfUltralightErrorNone:
        ret = PB_MfUltralight_Error_None;
        break;
    case MfUltralightErrorNotPresent:
        ret = PB_MfUltralight_Error_NotPresent;
        break;
    case MfUltralightErrorProtocol:
        ret = PB_MfUltralight_Error_Protocol;
        break;
    case MfUltralightErrorAuth:
        ret = PB_MfUltralight_Error_Auth;
        break;
    case MfUltralightErrorTimeout:
        ret = PB_MfUltralight_Error_Timeout;
        break;

    default:
        ret = PB_MfUltralight_Error_Timeout;
        break;
    }

    return ret;
}

static void nfc_rpc_mf_ultralight_read_page(Nfc_Main* cmd, void* context) {
    furi_assert(cmd);
    furi_assert(context);

    NfcRpc* instance = context;
    PB_MfUltralight_ReadPageResponse pb_mf_ul_read_page_resp =
        PB_MfUltralight_ReadPageResponse_init_default;

    MfUltralightPage page = {};
    MfUltralightError error = mf_ultralight_poller_read_page(
        instance->mf_ul_poller, cmd->content.mf_ultralight_read_page_req.page, &page);

    cmd->command_status = Nfc_CommandStatus_OK;
    cmd->which_content = Nfc_Main_mf_ultralight_read_page_resp_tag;
    pb_mf_ul_read_page_resp.error = nfc_rpc_mf_ultralight_process_error(error);
    if(pb_mf_ul_read_page_resp.error == PB_MfUltralight_Error_None) {
        pb_mf_ul_read_page_resp.page = cmd->content.mf_ultralight_read_page_req.page;
        memcpy(pb_mf_ul_read_page_resp.data.bytes, &page, sizeof(MfUltralightPage));
        pb_mf_ul_read_page_resp.data.size = sizeof(MfUltralightPage);
    }
    cmd->content.mf_ultralight_read_page_resp = pb_mf_ul_read_page_resp;
}

static void nfc_rpc_mf_ultralight_write_page(Nfc_Main* cmd, void* context) {
    furi_assert(cmd);
    furi_assert(context);

    NfcRpc* instance = context;
    PB_MfUltralight_WritePageResponse pb_mf_ul_write_page_resp =
        PB_MfUltralight_WritePageResponse_init_default;

    MfUltralightPage data = {};
    memcpy(&data, cmd->content.mf_ultralight_write_page_req.data.bytes, sizeof(MfUltralightPage));
    uint16_t page = cmd->content.mf_ultralight_write_page_req.page;
    MfUltralightError error = mf_ultralight_poller_write_page(instance->mf_ul_poller, page, &data);

    cmd->which_content = Nfc_Main_mf_ultralight_write_page_resp_tag;
    cmd->command_status = Nfc_CommandStatus_OK;
    pb_mf_ul_write_page_resp.error = nfc_rpc_mf_ultralight_process_error(error);
    cmd->content.mf_ultralight_write_page_resp = pb_mf_ul_write_page_resp;

    mf_ultralight_poller_reset(instance->mf_ul_poller);
}

static void nfc_rpc_mf_ultralight_read_version(Nfc_Main* cmd, void* context) {
    furi_assert(cmd);
    furi_assert(context);

    NfcRpc* instance = context;
    PB_MfUltralight_ReadVersionResponse pb_mf_ul_version =
        PB_MfUltralight_ReadVersionResponse_init_default;

    MfUltralightVersion data = {};
    MfUltralightError error = mf_ultralight_poller_read_version(instance->mf_ul_poller, &data);

    cmd->command_status = Nfc_CommandStatus_OK;
    cmd->which_content = Nfc_Main_mf_ultralight_read_version_resp_tag;
    pb_mf_ul_version.error = nfc_rpc_mf_ultralight_process_error(error);
    if(pb_mf_ul_version.error == PB_MfUltralight_Error_None) {
        pb_mf_ul_version.header = data.header;
        pb_mf_ul_version.prod_subtype = data.prod_subtype;
        pb_mf_ul_version.prod_type = data.prod_type;
        pb_mf_ul_version.prod_ver_major = data.prod_ver_major;
        pb_mf_ul_version.prod_ver_minor = data.prod_ver_minor;
        pb_mf_ul_version.protocol_type = data.protocol_type;
        pb_mf_ul_version.storage_size = data.storage_size;
        pb_mf_ul_version.vendor_id = data.vendor_id;
    }
    cmd->content.mf_ultralight_read_version_resp = pb_mf_ul_version;
}

static void nfc_rpc_mf_ultralight_read_signature(Nfc_Main* cmd, void* context) {
    furi_assert(cmd);
    furi_assert(context);

    NfcRpc* instance = context;
    PB_MfUltralight_ReadSignatureResponse pb_mf_ul_signature =
        PB_MfUltralight_ReadSignatureResponse_init_default;

    MfUltralightSignature data = {};
    MfUltralightError error = mf_ultralight_poller_read_signature(instance->mf_ul_poller, &data);

    cmd->command_status = Nfc_CommandStatus_OK;
    cmd->which_content = Nfc_Main_mf_ultralight_read_signature_resp_tag;
    pb_mf_ul_signature.error = nfc_rpc_mf_ultralight_process_error(error);
    if(pb_mf_ul_signature.error == PB_MfUltralight_Error_None) {
        memcpy(pb_mf_ul_signature.data.bytes, data.data, sizeof(MfUltralightSignature));
        pb_mf_ul_signature.data.size = sizeof(MfUltralightSignature);
    }
    cmd->content.mf_ultralight_read_signature_resp = pb_mf_ul_signature;
}

static void nfc_rpc_mf_ultralight_read_counter(Nfc_Main* cmd, void* context) {
    furi_assert(cmd);
    furi_assert(context);

    NfcRpc* instance = context;
    PB_MfUltralight_ReadCounterResponse pb_mf_ul_read_counter_resp =
        PB_MfUltralight_ReadPageResponse_init_default;

    MfUltralightCounter data = {};
    MfUltralightError error = mf_ultralight_poller_read_counter(
        instance->mf_ul_poller, cmd->content.mf_ultralight_read_counter_req.counter_num, &data);

    cmd->command_status = Nfc_CommandStatus_OK;
    cmd->which_content = Nfc_Main_mf_ultralight_read_counter_resp_tag;
    pb_mf_ul_read_counter_resp.error = nfc_rpc_mf_ultralight_process_error(error);
    if(pb_mf_ul_read_counter_resp.error == PB_MfUltralight_Error_None) {
        pb_mf_ul_read_counter_resp.counter_num =
            cmd->content.mf_ultralight_read_counter_req.counter_num;
        memcpy(pb_mf_ul_read_counter_resp.data.bytes, data.data, sizeof(MfUltralightCounter));
        pb_mf_ul_read_counter_resp.data.size = sizeof(MfUltralightCounter);
    }
    cmd->content.mf_ultralight_read_counter_resp = pb_mf_ul_read_counter_resp;

    mf_ultralight_poller_reset(instance->mf_ul_poller);
}

static void nfc_rpc_mf_ultralight_read_tearing_flag(Nfc_Main* cmd, void* context) {
    furi_assert(cmd);
    furi_assert(context);

    NfcRpc* instance = context;
    PB_MfUltralight_ReadTearingFlagResponse pb_mf_ul_read_tearing_flag_resp =
        PB_MfUltralight_ReadTearingFlagResponse_init_default;

    MfUltralightTearingFlag data = {};
    MfUltralightError error = mf_ultralight_poller_read_tearing_flag(
        instance->mf_ul_poller, cmd->content.mf_ultralight_read_tearing_flag_req.flag_num, &data);

    cmd->command_status = Nfc_CommandStatus_OK;
    cmd->which_content = Nfc_Main_mf_ultralight_read_tearing_flag_resp_tag;
    pb_mf_ul_read_tearing_flag_resp.error = nfc_rpc_mf_ultralight_process_error(error);
    if(pb_mf_ul_read_tearing_flag_resp.error == PB_MfUltralight_Error_None) {
        pb_mf_ul_read_tearing_flag_resp.flag_num =
            cmd->content.mf_ultralight_read_tearing_flag_req.flag_num;
        memcpy(
            pb_mf_ul_read_tearing_flag_resp.data.bytes,
            data.data,
            sizeof(MfUltralightTearingFlag));
        pb_mf_ul_read_tearing_flag_resp.data.size = sizeof(MfUltralightTearingFlag);
    }
    cmd->content.mf_ultralight_read_tearing_flag_resp = pb_mf_ul_read_tearing_flag_resp;
    FURI_LOG_D(
        TAG, "Tearing flag %ld: %02X", pb_mf_ul_read_tearing_flag_resp.flag_num, data.data[0]);

    mf_ultralight_poller_reset(instance->mf_ul_poller);
}

// TODO DELETE!
static void init_mf_ul_data(MfUltralightData* data) {
    NfcaData* nfca_data = &data->nfca_data;
    uint8_t uid[7] = {0x44, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
    uint8_t atqa[2] = {0x44, 0x00};
    nfca_data->uid_len = sizeof(uid);
    memcpy(nfca_data->uid, uid, sizeof(uid));
    memcpy(nfca_data->atqa, atqa, sizeof(atqa));
    nfca_data->sak = 0x00;

    data->type = MfUltralightTypeUnknown;
    MfUltralightVersion version = {
        .header = 1,
        .protocol_type = 228,
    };
    data->version = version;
    MfUltralightSignature sig = {.data = {0x01, 0x02, 0x03}};
    data->signature = sig;
    for(size_t i = 0; i < 16; i++) {
        data->page[i].data[0] = i;
    }
}

void nfc_rpc_mf_ultralight_emulate_start(Nfc_Main* cmd, void* context) {
    furi_assert(cmd);
    furi_assert(context);

    NfcRpc* instance = context;
    PB_MfUltralight_EmulateStartResponse pb_mf_ultralight_emulate_start_resp =
        PB_MfUltralight_EmulateStartResponse_init_default;
    cmd->command_status = Nfc_CommandStatus_OK;
    cmd->which_content = Nfc_Main_mf_ultralight_emulate_start_resp_tag;
    if(instance->mf_ul_listener == NULL) {
        MfUltralightData mf_ul_data = {};
        // TODO initialize data from rpc message
        init_mf_ul_data(&mf_ul_data);
        mf_ultralight_listener_start(instance->mf_ul_listener, &mf_ul_data, NULL, NULL);
        pb_mf_ultralight_emulate_start_resp.error = PB_MfUltralight_Error_None;
    } else {
        // TODO add Busy error
        pb_mf_ultralight_emulate_start_resp.error = PB_MfUltralight_Error_NotPresent;
    }
    cmd->content.mf_ultralight_emulate_start_resp = pb_mf_ultralight_emulate_start_resp;
}

void nfc_rpc_mf_ultralight_emulate_stop(Nfc_Main* cmd, void* context) {
    furi_assert(cmd);
    furi_assert(context);

    NfcRpc* instance = context;
    PB_MfUltralight_EmulateStopResponse pb_mf_ultralight_emulate_stop_resp =
        PB_MfUltralight_EmulateStopResponse_init_default;
    cmd->command_status = Nfc_CommandStatus_OK;
    cmd->which_content = Nfc_Main_mf_ultralight_emulate_stop_resp_tag;
    if(instance->mf_ul_listener) {
        // Stop before free
        mf_ultralight_listener_reset(instance->mf_ul_listener);
        instance->mf_ul_listener = NULL;
        pb_mf_ultralight_emulate_stop_resp.error = PB_MfUltralight_Error_None;
    } else {
        // TODO emulation not started error
        pb_mf_ultralight_emulate_stop_resp.error = PB_MfUltralight_Error_NotPresent;
    }
    cmd->content.mf_ultralight_emulate_stop_resp = pb_mf_ultralight_emulate_stop_resp;
}

void nfc_rpc_mf_ultralight_alloc(void* context) {
    furi_assert(context);

    NfcRpc* instance = context;
    nfc_rpc_add_handler(
        instance, Nfc_Main_mf_ultralight_read_page_req_tag, nfc_rpc_mf_ultralight_read_page);
    nfc_rpc_add_handler(
        instance, Nfc_Main_mf_ultralight_read_version_req_tag, nfc_rpc_mf_ultralight_read_version);
    nfc_rpc_add_handler(
        instance, Nfc_Main_mf_ultralight_write_page_req_tag, nfc_rpc_mf_ultralight_write_page);
    nfc_rpc_add_handler(
        instance,
        Nfc_Main_mf_ultralight_read_signature_req_tag,
        nfc_rpc_mf_ultralight_read_signature);
    nfc_rpc_add_handler(
        instance, Nfc_Main_mf_ultralight_read_counter_req_tag, nfc_rpc_mf_ultralight_read_counter);
    nfc_rpc_add_handler(
        instance,
        Nfc_Main_mf_ultralight_read_tearing_flag_req_tag,
        nfc_rpc_mf_ultralight_read_tearing_flag);
    nfc_rpc_add_handler(
        instance,
        Nfc_Main_mf_ultralight_emulate_start_req_tag,
        nfc_rpc_mf_ultralight_emulate_start);
    nfc_rpc_add_handler(
        instance, Nfc_Main_mf_ultralight_emulate_stop_req_tag, nfc_rpc_mf_ultralight_emulate_stop);
}
