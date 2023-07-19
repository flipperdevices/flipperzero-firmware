#pragma once

#include "../subbrute_protocols.h"
#include "radio_device_loader.h"

typedef enum {
    SubBruteWorkerStateIDLE,
    SubBruteWorkerStateReady,
    SubBruteWorkerStateTx,
    SubBruteWorkerStateFinished
} SubBruteWorkerState;

typedef void (*SubBruteWorkerCallback)(void* context, SubBruteWorkerState state);

typedef struct SubBruteWorker SubBruteWorker;

SubBruteWorker* subbrute_worker_alloc(const SubGhzDevice* radio_device);
void subbrute_worker_free(SubBruteWorker* instance);
uint64_t subbrute_worker_get_step(SubBruteWorker* instance);
bool subbrute_worker_set_step(SubBruteWorker* instance, uint64_t step);
bool subbrute_worker_is_running(SubBruteWorker* instance);
bool subbrute_worker_init_default_attack(
    SubBruteWorker* instance,
    SubBruteAttacks attack_type,
    uint64_t step,
    const SubBruteProtocol* protocol,
    uint8_t extra_repeats);
bool subbrute_worker_init_file_attack(
    SubBruteWorker* instance,
    uint64_t step,
    uint8_t load_index,
    uint64_t file_key,
    SubBruteProtocol* protocol,
    uint8_t extra_repeats,
    bool two_bytes);
bool subbrute_worker_start(SubBruteWorker* instance);
void subbrute_worker_stop(SubBruteWorker* instance);
bool subbrute_worker_transmit_current_key(SubBruteWorker* instance, uint64_t step);
bool subbrute_worker_can_manual_transmit(SubBruteWorker* instance);
void subbrute_worker_set_callback(
    SubBruteWorker* instance,
    SubBruteWorkerCallback callback,
    void* context);

uint8_t subbrute_worker_get_timeout(SubBruteWorker* instance);
void subbrute_worker_set_timeout(SubBruteWorker* instance, uint8_t timeout);
uint8_t subbrute_worker_get_repeats(SubBruteWorker* instance);
void subbrute_worker_set_repeats(SubBruteWorker* instance, uint8_t repeats);
uint32_t subbrute_worker_get_te(SubBruteWorker* instance);
void subbrute_worker_set_te(SubBruteWorker* instance, uint32_t te);

// void subbrute_worker_timeout_inc(SubBruteWorker* instance);

// void subbrute_worker_timeout_dec(SubBruteWorker* instance);

bool subbrute_worker_is_tx_allowed(SubBruteWorker* instance, uint32_t value);