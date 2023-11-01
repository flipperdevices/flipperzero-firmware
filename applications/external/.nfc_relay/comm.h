#pragma once
#include <stdint.h>
#include "packet.h"
#include "nfc_relay.h"
#include "uart_worker.h"

typedef struct Comm {
    NfcRelayConfig* config;
    UartWorker* uart_worker;
} Comm;

Comm* comm_alloc();
void comm_init(Comm* comm);
void comm_deinit(Comm* comm);
NfcRelayPacket* comm_read_packet(Comm* comm);
bool comm_write_packet(Comm* comm, NfcRelayPacket* packet);
void comm_free(Comm* comm);
NfcRelayPacket* comm_wait_packet(Comm* comm, NfcRelayPacketType target_typ);
void comm_send_no_data(Comm* comm, NfcRelayPacketType typ);
void comm_send_pingpong(Comm* comm, NfcRelayPacketType typ, NfcRelayRole role);
bool comm_wait_pong(Comm* comm, NfcRelayRole role, NfcRelayRole selfRole);
