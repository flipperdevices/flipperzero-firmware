#include "packet.h"
#include <stdlib.h>
#include <furi.h>

NfcRelayPacket* packet_no_data_alloc(NfcRelayPacketType typ) {
    NfcRelayPacket* packet = malloc(sizeof(NfcRelayPacket));
    packet->typ = typ;
    packet->len = 0;
    return packet;
}

NfcRelayPacket* packet_alloc(NfcRelayPacketType typ, uint8_t len, void* data) {
    NfcRelayPacket* packet = malloc(sizeof(NfcRelayPacket) + len);
    packet->typ = typ;
    packet->len = len;
    memcpy(&packet->buf, data, len);
    return packet;
}