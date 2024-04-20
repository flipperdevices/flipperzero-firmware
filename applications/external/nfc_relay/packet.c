#include "packet.h"
#include <stdlib.h>
#include <furi.h>

NfcRelayPacket* packet_alloc_empty(NfcRelayPacketType typ) {
    return packet_alloc(typ, 0);
}

NfcRelayPacket* packet_alloc(NfcRelayPacketType typ, size_t len) {
    NfcRelayPacket* packet = malloc(sizeof(NfcRelayPacket) + len);
    packet->typ = typ;
    packet->len = len;
    return packet;
}

NfcRelayPacket*
    packet_setdata(NfcRelayPacket* packet, size_t offset, size_t len, const void* data) {
    memcpy(packet->buf + offset, data, len);
    return packet;
}

NfcRelayPacket* packet_alloc_data(NfcRelayPacketType typ, size_t len, const void* data) {
    NfcRelayPacket* packet = packet_alloc(typ, len);
    packet_setdata(packet, 0, len, data);
    return packet;
}

SerializedIso14443_4a* iso14443_4a_serialize(const Iso14443_4aData* data) {
    size_t total_len =
        sizeof(SerializedIso14443_4a) + simple_array_get_count(data->ats_data.t1_tk);
    SerializedIso14443_4a* s = malloc(total_len);
    memcpy(s->buf_3aData, data->iso14443_3a_data, sizeof(Iso14443_3aData));
    memcpy(s->buf_4aAtsData1, &data->ats_data, sizeof(Iso14443_4aAtsData) - sizeof(SimpleArray*));
    s->len_t1_tk = simple_array_get_count(data->ats_data.t1_tk);
    simple_array_cget_data(data->ats_data.t1_tk);
    memcpy(&s->buf_t1_tk, simple_array_cget_data(data->ats_data.t1_tk), s->len_t1_tk);
    return s;
}

void iso14443_4a_deserialize(Iso14443_4aData* obj, const SerializedIso14443_4a* serialized) {
    iso14443_3a_copy(obj->iso14443_3a_data, (Iso14443_3aData*)serialized->buf_3aData);
    memcpy(
        &obj->ats_data,
        serialized->buf_4aAtsData1,
        sizeof(Iso14443_4aAtsData) - sizeof(SimpleArray*));

    if(serialized->len_t1_tk > 0) {
        simple_array_init(obj->ats_data.t1_tk, serialized->len_t1_tk);
        memcpy(
            simple_array_get_data(obj->ats_data.t1_tk),
            serialized->buf_t1_tk,
            serialized->len_t1_tk);
    }
    return;
}