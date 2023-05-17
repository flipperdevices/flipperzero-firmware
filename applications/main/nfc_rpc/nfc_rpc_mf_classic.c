#include "nfc_rpc_i.h"

#include "assets/compiled/mf_classic.pb.h"

#define TAG "NfcRpcMfClassic"

static PB_MfClassic_Error nfc_rpc_mf_classic_process_error(MfClassicError error) {
    PB_MfClassic_Error ret = PB_MfClassic_Error_None;

    switch(error) {
    case MfClassicErrorNone:
        ret = PB_MfClassic_Error_None;
        break;
    case MfClassicErrorNotPresent:
        ret = PB_MfClassic_Error_NotPresent;
        break;
    case MfClassicErrorProtocol:
        ret = PB_MfClassic_Error_Protocol;
        break;
    case MfClassicErrorAuth:
        ret = PB_MfClassic_Error_Auth;
        break;
    case MfClassicErrorTimeout:
        ret = PB_MfClassic_Error_Timeout;
        break;

    default:
        ret = PB_MfClassic_Error_Timeout;
        break;
    }

    return ret;
}

static void nfc_rpc_mf_classic_auth(Nfc_Main* cmd, void* context) {
    furi_assert(cmd);
    furi_assert(context);

    NfcRpc* instance = context;
    PB_MfClassic_AuthResponse pb_mf_classic_auth_resp = PB_MfClassic_AuthResponse_init_default;

    PB_MfClassic_AuthRequest* req = &cmd->content.mf_classic_auth_req;
    MfClassicAuthContext auth_context = {};
    MfClassicKey key = {};
    memcpy(key.data, req->key.bytes, sizeof(MfClassicKey));
    MfClassicKeyType key_type =
        (req->key_type == PB_MfClassic_KeyType_KeyTypeB) ? MfClassicKeyTypeB : MfClassicKeyTypeA;
    MfClassicError error = mf_classic_poller_auth(
        instance->mf_classic_poller, req->block, &key, key_type, &auth_context);

    cmd->command_status = Nfc_CommandStatus_OK;
    cmd->which_content = Nfc_Main_mf_classic_auth_resp_tag;

    pb_mf_classic_auth_resp.error = nfc_rpc_mf_classic_process_error(error);
    if(pb_mf_classic_auth_resp.error == PB_MfClassic_Error_None) {
        pb_mf_classic_auth_resp.block = auth_context.block_num;
        memcpy(pb_mf_classic_auth_resp.key.bytes, auth_context.key.data, sizeof(MfClassicKey));
        pb_mf_classic_auth_resp.key.size = sizeof(MfClassicKey);
        pb_mf_classic_auth_resp.key_type = (auth_context.key_type == MfClassicKeyTypeB) ?
                                               PB_MfClassic_KeyType_KeyTypeB :
                                               PB_MfClassic_KeyType_KeyTypeA;
        memcpy(pb_mf_classic_auth_resp.nt.bytes, auth_context.nt.data, sizeof(MfClassicNt));
        pb_mf_classic_auth_resp.nt.size = sizeof(MfClassicNt);
        memcpy(pb_mf_classic_auth_resp.nr.bytes, auth_context.nr.data, sizeof(MfClassicNr));
        pb_mf_classic_auth_resp.nr.size = sizeof(MfClassicNr);
        memcpy(pb_mf_classic_auth_resp.ar.bytes, auth_context.ar.data, sizeof(MfClassicAr));
        pb_mf_classic_auth_resp.ar.size = sizeof(MfClassicAr);
        memcpy(pb_mf_classic_auth_resp.at.bytes, auth_context.at.data, sizeof(MfClassicAt));
        pb_mf_classic_auth_resp.at.size = sizeof(MfClassicAt);
    }
    cmd->content.mf_classic_auth_resp = pb_mf_classic_auth_resp;
}

static void nfc_rpc_mf_classic_read_block(Nfc_Main* cmd, void* context) {
    furi_assert(cmd);
    furi_assert(context);

    NfcRpc* instance = context;
    PB_MfClassic_ReadBlockResponse pb_mf_classic_read_block_resp =
        PB_MfClassic_ReadBlockResponse_init_default;

    PB_MfClassic_ReadBlockRequest* req = &cmd->content.mf_classic_read_block_req;
    MfClassicKey key = {};
    memcpy(key.data, req->key.bytes, sizeof(MfClassicKey));
    MfClassicKeyType key_type =
        (req->key_type == PB_MfClassic_KeyType_KeyTypeB) ? MfClassicKeyTypeB : MfClassicKeyTypeA;
    MfClassicBlock block = {};
    MfClassicError error = mf_classic_poller_read_block(
        instance->mf_classic_poller, req->block, &key, key_type, &block);

    cmd->command_status = Nfc_CommandStatus_OK;
    cmd->which_content = Nfc_Main_mf_classic_read_block_resp_tag;

    pb_mf_classic_read_block_resp.error = nfc_rpc_mf_classic_process_error(error);
    if(pb_mf_classic_read_block_resp.error == PB_MfClassic_Error_None) {
        memcpy(pb_mf_classic_read_block_resp.data.bytes, block.data, sizeof(MfClassicBlock));
        pb_mf_classic_read_block_resp.data.size = sizeof(MfClassicBlock);
    }
    cmd->content.mf_classic_read_block_resp = pb_mf_classic_read_block_resp;
}

void nfc_rpc_mf_classic_alloc(void* context) {
    furi_assert(context);

    NfcRpc* instance = context;
    nfc_rpc_add_handler(instance, Nfc_Main_mf_classic_auth_req_tag, nfc_rpc_mf_classic_auth);
    nfc_rpc_add_handler(
        instance, Nfc_Main_mf_classic_read_block_req_tag, nfc_rpc_mf_classic_read_block);
}
