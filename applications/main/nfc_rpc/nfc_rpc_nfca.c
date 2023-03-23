#include "nfc_rpc_i.h"

#include "assets/compiled/nfca.pb.h"
#include <lib/nfc/protocols/nfca_poller.h>

#define TAG "NfcRpcNfca"

static PB_Nfca_Error nfc_rpc_nfca_process_error(NfcaError error) {
    PB_Nfca_Error ret = PB_Nfca_Error_None;
    switch(error) {
    case NfcaErrorNone:
        ret = PB_Nfca_Error_None;
        break;
    case NfcaErrorNotPresent:
        ret = PB_Nfca_Error_NotPresent;
        break;
    case NfcaErrorColResFailed:
        ret = PB_Nfca_Error_ColResFailed;
        break;
    case NfcaErrorBufferOverflow:
        ret = PB_Nfca_Error_BufferOverflow;
        break;
    case NfcaErrorCommunication:
        ret = PB_Nfca_Error_Communication;
        break;
    case NfcaErrorFieldOff:
        ret = PB_Nfca_Error_FieldOff;
        break;
    case NfcaErrorWrongCrc:
        ret = PB_Nfca_Error_WrongCrc;
        break;
    case NfcaErrorTimeout:
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
    PB_Nfca_ReadResponse pb_nfca_read_resp = PB_Nfca_ReadResponse_init_default;

    NfcaData nfca_data = {};
    NfcaPoller* nfca_poller = nfca_poller_alloc();
    NfcaError error = nfca_poller_activate(nfca_poller, &nfca_data);

    cmd->command_status = Nfc_CommandStatus_OK;
    cmd->which_content = Nfc_Main_nfca_read_resp_tag;
    pb_nfca_read_resp.error = nfc_rpc_nfca_process_error(error);
    if(pb_nfca_read_resp.error == PB_Nfca_Error_None) {
        pb_nfca_read_resp.uid_len = nfca_data.uid_len;
        memcpy(pb_nfca_read_resp.uid.bytes, nfca_data.uid, nfca_data.uid_len);
        pb_nfca_read_resp.uid.size = nfca_data.uid_len;
        memcpy(pb_nfca_read_resp.sak.bytes, &nfca_data.sak, sizeof(nfca_data.sak));
        pb_nfca_read_resp.sak.size = sizeof(nfca_data.sak);
        memcpy(pb_nfca_read_resp.atqa.bytes, nfca_data.atqa, sizeof(nfca_data.atqa));
        pb_nfca_read_resp.atqa.size = sizeof(nfca_data.atqa);
    }
    cmd->content.nfca_read_resp = pb_nfca_read_resp;

    nfca_poller_free(nfca_poller);
}

void nfc_rpc_nfca_alloc(void* context) {
    furi_assert(context);

    NfcRpc* instance = context;
    nfc_rpc_add_handler(instance, Nfc_Main_nfca_read_req_tag, nfc_rpc_nfca_read);
}
