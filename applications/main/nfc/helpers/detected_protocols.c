#include <furi.h>
#include <lib/nfc/protocols/nfc_protocol.h>

typedef struct {
    uint32_t protocols_detected_num;
    NfcProtocol protocols_detected[NfcProtocolNum];
    uint32_t selected_idx;
} NfcDetectedProtocols;

NfcDetectedProtocols* nfc_detected_protocols_alloc(void) {
    NfcDetectedProtocols* instance = malloc(sizeof(NfcDetectedProtocols));

    instance->protocols_detected_num = 0;
    instance->selected_idx = 0;

    return instance;
}

void nfc_detected_protocols_free(NfcDetectedProtocols* instance) {
    furi_assert(instance);

    free(instance);
}

void nfc_detected_protocols_reset(NfcDetectedProtocols* instance) {
    furi_assert(instance);

    instance->protocols_detected_num = 0;
    memset(instance->protocols_detected, 0, sizeof(instance->protocols_detected));
    instance->selected_idx = 0;
}

void nfc_detected_protocols_select(NfcDetectedProtocols* instance, uint32_t idx) {
    furi_assert(instance);

    instance->selected_idx = idx;
}

void nfc_detected_protocols_set(
    NfcDetectedProtocols* instance,
    const NfcProtocol* types,
    uint32_t count) {
    furi_assert(instance);
    furi_assert(types);
    furi_assert(count < NfcProtocolNum);

    memcpy(instance->protocols_detected, types, count);
    instance->protocols_detected_num = count;
    instance->selected_idx = 0;
}

uint32_t nfc_detected_protocols_get_num(NfcDetectedProtocols* instance) {
    furi_assert(instance);

    return instance->protocols_detected_num;
}

NfcProtocol nfc_detected_protocols_get_protocol(NfcDetectedProtocols* instance, uint32_t idx) {
    furi_assert(instance);
    furi_assert(idx < instance->protocols_detected_num);

    return instance->protocols_detected[idx];
}

void nfc_detected_protocols_fill(NfcDetectedProtocols* instance) {
    furi_assert(instance);

    instance->protocols_detected_num = NfcProtocolNum;
    for(uint32_t i = 0; i < NfcProtocolNum; i++) {
        instance->protocols_detected[i] = i;
    }
    instance->selected_idx = 0;
}

NfcProtocol nfc_detected_protocols_get_selected(NfcDetectedProtocols* instance) {
    furi_assert(instance);

    return instance->protocols_detected[instance->selected_idx];
}