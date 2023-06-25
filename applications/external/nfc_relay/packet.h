#pragma once
#include <stdint.h>

typedef enum {
    NfcRelayPacketPing = 0x60,
    NfcRelayPacketPong,
    NfcRelayPacketNfcDevData,
    NfcRelayPacketApduReq,
    NfcRelayPacketApduResp,
    NfcRelayPacketErr,
} __attribute__((__packed__)) NfcRelayPacketType;

typedef enum {
    NfcRelayAsReader,
    NfcRelayAsCard,
} __attribute__((__packed__)) NfcRelayRole;

typedef struct NfcRelayPacket {
    NfcRelayPacketType typ;
    uint8_t len;
    uint8_t buf[];
} __attribute__((aligned(1))) NfcRelayPacket;

NfcRelayPacket* packet_no_data_alloc(NfcRelayPacketType typ);
NfcRelayPacket* packet_alloc(NfcRelayPacketType typ, uint8_t len, void* data);