#pragma once

#include "nfc_rpc.h"

#include "furi.h"
#include "furi_hal.h"
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <rpc/rpc_app.h>

#include <lib/nanopb/pb_decode.h>
#include <lib/nanopb/pb_encode.h>

#include "assets/compiled/main.pb.h"

typedef void (*NfcRpcHandler)(Nfc_Main* cmd, void* context);

void nfc_rpc_add_handler(NfcRpc* instance, pb_size_t message_tag, NfcRpcHandler handler);

void nfc_rpc_nfca_alloc(void* context);
void nfc_rpc_mf_ultralight_alloc(void* context);
