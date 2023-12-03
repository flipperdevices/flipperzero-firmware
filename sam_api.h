#pragma once

#include <nfc/helpers/iso13239_crc.h>

#include "seader_i.h"
#include "seader_credential.h"
#include "seader_bridge.h"
#include "seader_worker.h"

#include <Payload.h>

NfcCommand seader_worker_card_detect(
    Seader* seader,
    uint8_t sak,
    uint8_t* atqa,
    const uint8_t* uid,
    uint8_t uid_len,
    uint8_t* ats,
    uint8_t ats_len);

bool seader_process_success_response_i(
    Seader* seader,
    uint8_t* apdu,
    size_t len,
    bool online,
    SeaderPollerContainer* spc);
