#include "nfc_rpc_i.h"

#include "assets/compiled/nfca.pb.h"
#include <nfc/protocols/iso14443_3a/iso14443_3a_poller_sync_api.h>

#define TAG "NfcRpcIso14443_3a"

static PB_Nfca_Error nfc_rpc_nfca_process_error(Iso14443_3aError error) {
    PB_Nfca_Error ret = PB_Nfca_Error_None;
    switch(error) {
    case Iso14443_3aErrorNone:
        ret = PB_Nfca_Error_None;
        break;
    case Iso14443_3aErrorNotPresent:
        ret = PB_Nfca_Error_NotPresent;
        break;
    case Iso14443_3aErrorColResFailed:
        ret = PB_Nfca_Error_ColResFailed;
        break;
    case Iso14443_3aErrorBufferOverflow:
        ret = PB_Nfca_Error_BufferOverflow;
        break;
    case Iso14443_3aErrorCommunication:
        ret = PB_Nfca_Error_Communication;
        break;
    case Iso14443_3aErrorFieldOff:
        ret = PB_Nfca_Error_FieldOff;
        break;
    case Iso14443_3aErrorWrongCrc:
        ret = PB_Nfca_Error_WrongCrc;
        break;
    case Iso14443_3aErrorTimeout:
        ret = PB_Nfca_Error_Timeout;
        break;

    default:
        ret = PB_Nfca_Error_Timeout;
        break;
    }

    return ret;
}

static void nfc_rpc_nfca_read(Nfc_Main* cmd, void* context) {
    furi_assert(cmd);
    furi_assert(context);

    NfcRpc* instance = context;
    PB_Nfca_ReadResponse pb_nfca_read_resp = PB_Nfca_ReadResponse_init_default;

    Iso14443_3aData iso14443_3a_data = {};
    Iso14443_3aError error = iso14443_3a_poller_read(instance->nfc, &iso14443_3a_data);

    cmd->command_status = Nfc_CommandStatus_OK;
    cmd->which_content = Nfc_Main_nfca_read_resp_tag;
    pb_nfca_read_resp.error = nfc_rpc_nfca_process_error(error);
    if(pb_nfca_read_resp.error == PB_Nfca_Error_None) {
        pb_nfca_read_resp.uid_len = iso14443_3a_data.uid_len;
        memcpy(pb_nfca_read_resp.uid.bytes, iso14443_3a_data.uid, iso14443_3a_data.uid_len);
        pb_nfca_read_resp.uid.size = iso14443_3a_data.uid_len;
        memcpy(pb_nfca_read_resp.sak.bytes, &iso14443_3a_data.sak, sizeof(iso14443_3a_data.sak));
        pb_nfca_read_resp.sak.size = sizeof(iso14443_3a_data.sak);
        memcpy(pb_nfca_read_resp.atqa.bytes, iso14443_3a_data.atqa, sizeof(iso14443_3a_data.atqa));
        pb_nfca_read_resp.atqa.size = sizeof(iso14443_3a_data.atqa);
    }
    cmd->content.nfca_read_resp = pb_nfca_read_resp;
}

static void nfc_rpc_nfca_emulate_start(Nfc_Main* cmd, void* context) {
    furi_assert(cmd);
    furi_assert(context);

    NfcRpc* instance = context;
    PB_Nfca_EmulateStartResponse pb_nfca_emulate_start_resp =
        PB_Nfca_EmulateStartResponse_init_default;
    cmd->command_status = Nfc_CommandStatus_OK;
    cmd->which_content = Nfc_Main_nfca_emulate_start_resp_tag;
    if(instance->listener == NULL) {
        Iso14443_3aData iso14443_3a_data = {};
        iso14443_3a_data.uid_len = cmd->content.nfca_emulate_start_req.uid_len;
        memcpy(
            iso14443_3a_data.uid,
            cmd->content.nfca_emulate_start_req.uid.bytes,
            iso14443_3a_data.uid_len);
        memcpy(iso14443_3a_data.atqa, cmd->content.nfca_emulate_start_req.atqa.bytes, 2);
        memcpy(&iso14443_3a_data.sak, cmd->content.nfca_emulate_start_req.sak.bytes, 1);

        instance->listener =
            nfc_listener_alloc(instance->nfc, NfcProtocolIso14443_3a, &iso14443_3a_data);
        nfc_listener_start(instance->listener, NULL, NULL);
        pb_nfca_emulate_start_resp.error = PB_Nfca_Error_None;
    } else {
        // TODO add Busy error
        pb_nfca_emulate_start_resp.error = PB_Nfca_Error_NotPresent;
    }
    cmd->content.nfca_emulate_start_resp = pb_nfca_emulate_start_resp;
}

static void nfc_rpc_nfca_emulate_stop(Nfc_Main* cmd, void* context) {
    furi_assert(cmd);
    furi_assert(context);

    NfcRpc* instance = context;
    PB_Nfca_EmulateStopResponse pb_nfca_emulate_stop_resp =
        PB_Nfca_EmulateStopResponse_init_default;
    cmd->command_status = Nfc_CommandStatus_OK;
    cmd->which_content = Nfc_Main_nfca_emulate_stop_resp_tag;
    if(instance->listener) {
        nfc_listener_stop(instance->listener);
        nfc_listener_free(instance->listener);
        instance->listener = NULL;
        pb_nfca_emulate_stop_resp.error = PB_Nfca_Error_None;
    } else {
        // TODO add Busy error
        pb_nfca_emulate_stop_resp.error = PB_Nfca_Error_NotPresent;
    }
    cmd->content.nfca_emulate_stop_resp = pb_nfca_emulate_stop_resp;
}

void nfc_rpc_nfca_alloc(void* context) {
    furi_assert(context);

    NfcRpc* instance = context;
    nfc_rpc_add_handler(instance, Nfc_Main_nfca_read_req_tag, nfc_rpc_nfca_read);
    nfc_rpc_add_handler(instance, Nfc_Main_nfca_emulate_start_req_tag, nfc_rpc_nfca_emulate_start);
    nfc_rpc_add_handler(instance, Nfc_Main_nfca_emulate_stop_req_tag, nfc_rpc_nfca_emulate_stop);
}
