#pragma once

#include <furi.h>
#include "nfc_relay.h"
#include "packet.h"

typedef enum {
    UartWorkerStateReady,
    UartWorkerStateRx,
    UartWorkerStateStop,
} UartWorkerState;

typedef enum {
    UartWorkerStateRxSubReadTyp,
    UartWorkerStateRxSubReadLen,
    UartWorkerStateRxSubReadData,
    UartWorkerStateRxSubReadFinish,
} UartWorkerStateRxSub;

typedef struct UartWorker {
    FuriThread* thread;
    UartWorkerState state;
    UartWorkerStateRxSub rxSubState;
    FuriStreamBuffer* rx_stream;
    NfcRelayPacket* packet;

    NfcRelayConfigUart config;
} UartWorker;

UartWorker* uart_worker_alloc(NfcRelayConfigUart config);

//void uart_worker_change_state(UartWorker* uart_worker, UartWorkerState state);

void uart_worker_free(UartWorker* uart_worker);

void uart_worker_stop(UartWorker* uart_worker);

void uart_worker_start(UartWorker* uart_worker);

int32_t uart_worker_task(void* context);

void uart_worker_tx(UartWorker* uart_worker, uint8_t* data, size_t len);

void uart_worker_tx_packet(UartWorker* uart_worker, NfcRelayPacket* packet);

NfcRelayPacket* uart_worker_pop_packet(UartWorker* uart_worker);
