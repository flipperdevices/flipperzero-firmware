#include "comm.h"

#define TAG "COMM"

Comm* comm_alloc(NfcRelayConfig* config) {
    Comm* comm = malloc(sizeof(Comm));
    comm->config = config;
    return comm;
}

void comm_init(Comm* comm) {
    if(NfcRelayModeUart == comm->config->mode) {
        comm->uart_worker = uart_worker_alloc(comm->config->uart_config);
        uart_worker_start(comm->uart_worker);
    } // TODO: else
}

void comm_deinit(Comm* comm) {
    if(NfcRelayModeUart == comm->config->mode) {
        uart_worker_stop(comm->uart_worker);
    }
}

NfcRelayPacket* comm_read_packet(Comm* comm) {
    if(NfcRelayModeUart == comm->config->mode) {
        return uart_worker_pop_packet(comm->uart_worker);
    }
    FURI_LOG_E(TAG, "MODE NOT IMPLEMENT WHEN READ PAKCET");
    return NULL;
}
bool comm_write_packet(Comm* comm, NfcRelayPacket* packet) {
    if(NfcRelayModeUart == comm->config->mode) {
        uart_worker_tx_packet(comm->uart_worker, packet);
        return true;
    }
    FURI_LOG_E(TAG, "MODE NOT IMPLEMENT WHEN WRITE PAKCET");
    return false;
}

void comm_free(Comm* comm) {
    if(NfcRelayModeUart == comm->config->mode) {
        uart_worker_free(comm->uart_worker);
    }
    free(comm);
}

void comm_send_no_data(Comm* comm, NfcRelayPacketType typ) {
    NfcRelayPacket* packet;
    packet = packet_no_data_alloc(typ);
    comm_write_packet(comm, packet);
    free(packet);
}

void comm_send_pingpong(Comm* comm, NfcRelayPacketType typ, NfcRelayRole who) {
    NfcRelayPacket* packet;
    packet = packet_alloc(typ, 1, &who);
    comm_write_packet(comm, packet);
    free(packet);
}

NfcRelayPacket* comm_wait_packet(Comm* comm, NfcRelayPacketType target_typ) {
    NfcRelayPacket* recv_packet = NULL;
    recv_packet = comm_read_packet(comm);
    if(!recv_packet) return NULL;
    if(recv_packet->typ == target_typ) {
        FURI_LOG_D(TAG, "Wait %d type succ, len: %d", target_typ, recv_packet->len);
        return recv_packet;
    } else {
        FURI_LOG_D(TAG, "Wait %d type fail, recv: %d", target_typ, recv_packet->typ);
    }
    return NULL;
}

bool comm_wait_pong(Comm* comm, NfcRelayRole role, NfcRelayRole selfRole) {
    NfcRelayPacket* recv_packet;
    recv_packet = comm_read_packet(comm);
    if(!recv_packet) {
        return false;
    }
    if(recv_packet->typ == NfcRelayPacketPing && recv_packet->len == 1 &&
       recv_packet->buf[0] == role) {
        FURI_LOG_D(TAG, "Recv Ping, Send Pong");
        comm_send_pingpong(comm, NfcRelayPacketPong, selfRole);
        free(recv_packet);
        return true;
    } else if(
        recv_packet->typ == NfcRelayPacketPong && recv_packet->len == 1 &&
        recv_packet->buf[0] == role) {
        FURI_LOG_D(TAG, "Recv Pong, Continue");
        free(recv_packet);
        return true;
    } else {
        FURI_LOG_D(
            TAG, "Another End's role is Wrong or recv something else when waiting ping/pong");
        free(recv_packet);
        return false;
    }
}