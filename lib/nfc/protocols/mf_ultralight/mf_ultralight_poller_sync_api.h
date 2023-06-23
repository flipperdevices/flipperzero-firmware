#pragma once

#include "mf_ultralight.h"

#ifdef __cplusplus
extern "C" {
#endif

MfUltralightError mf_ultralight_poller_read_page(
    MfUltralightPoller* instance,
    uint16_t page,
    MfUltralightPage* data);

MfUltralightError mf_ultralight_poller_read_page_new(
    NfcPollerManager* poller_manager,
    uint16_t page,
    MfUltralightPage* data);

MfUltralightError mf_ultralight_poller_write_page(
    MfUltralightPoller* instance,
    uint16_t page,
    MfUltralightPage* data);

MfUltralightError
    mf_ultralight_poller_read_version(MfUltralightPoller* instance, MfUltralightVersion* data);

MfUltralightError
    mf_ultralight_poller_read_signature(MfUltralightPoller* instance, MfUltralightSignature* data);

MfUltralightError mf_ultralight_poller_read_counter(
    MfUltralightPoller* instance,
    uint8_t counter_num,
    MfUltralightCounter* data);

MfUltralightError mf_ultralight_poller_read_tearing_flag(
    MfUltralightPoller* instance,
    uint8_t flag_num,
    MfUltralightTearingFlag* data);

MfUltralightError
    mf_ultralight_poller_read_card(MfUltralightPoller* instance, MfUltralightData* data);

#ifdef __cplusplus
}
#endif
