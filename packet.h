#pragma once
#include <furi.h>
#include <nfc/protocols/iso14443_4a/iso14443_4a.h>

typedef enum {
    NfcRelayPacketPing = 0x60,
    NfcRelayPacketPong,
    NfcRelayPacketNfcDevData,
    NfcRelayPacketApduReq,
    NfcRelayPacketApduResp,
    NfcRelayPacketErr,
} NfcRelayPacketType;

typedef enum {
    NfcRelayAsReader,
    NfcRelayAsCard,
} NfcRelayRole;

typedef struct NfcRelayPacket {
    NfcRelayPacketType typ;
    uint8_t len;
    uint8_t buf[];
} __attribute__((packed, aligned(1))) NfcRelayPacket;

typedef struct SerializedIso14443_4a {
    uint8_t buf_3aData[sizeof(Iso14443_3aData)];
    uint8_t buf_4aAtsData1[sizeof(Iso14443_4aAtsData) - sizeof(SimpleArray*)];
    size_t len_t1_tk;
    uint8_t buf_t1_tk[];
} __attribute__((packed, aligned(1))) SerializedIso14443_4a;

NfcRelayPacket* packet_alloc_empty(NfcRelayPacketType typ);
NfcRelayPacket* packet_alloc(NfcRelayPacketType typ, size_t len);
NfcRelayPacket*
    packet_setdata(NfcRelayPacket* packet, size_t offset, size_t len, const void* data);
NfcRelayPacket* packet_alloc_data(NfcRelayPacketType typ, size_t len, const void* data);
SerializedIso14443_4a* iso14443_4a_serialize(const Iso14443_4aData* data);
void iso14443_4a_deserialize(Iso14443_4aData* obj, const SerializedIso14443_4a* serialized);