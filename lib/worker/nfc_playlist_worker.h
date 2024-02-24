#pragma once
#include <furi.h>
#include <furi_hal.h>
#include <nfc/nfc.h>
#include <nfc/nfc_device.h>
#include <nfc/nfc_listener.h>

typedef enum NfcPlaylistWorkerState {
   NfcPlaylistWorkerState_Emulating,
   NfcPlaylistWorkerState_Stopped
} NfcPlaylistWorkerState;

typedef struct NfcPlaylistWorker {
   FuriThread* thread;
   NfcPlaylistWorkerState state;
   NfcListener* nfc_listener;
   NfcDevice* nfc_device;
   NfcProtocol nfc_protocol;
   NfcDeviceData* nfc_data;
   Nfc* nfc;
} NfcPlaylistWorker;

NfcPlaylistWorker* nfc_playlist_worker_alloc();
void nfc_playlist_worker_free(NfcPlaylistWorker* nfc_playlist_worker);
void nfc_playlist_worker_stop(NfcPlaylistWorker* nfc_playlist_worker);
void nfc_playlist_worker_start(NfcPlaylistWorker* nfc_playlist_worker);

int32_t nfc_playlist_worker_task(void* context);

bool nfc_playlist_worker_is_emulating(NfcPlaylistWorker* nfc_playlist_worker);
void nfc_playlist_worker_set_nfc_data(NfcPlaylistWorker* nfc_playlist_worker, char* file_path);
void nfc_playlist_worker_clear_nfc_data(NfcPlaylistWorker* nfc_playlist_worker);
NfcDeviceData* nfc_playlist_worker_get_nfc_data(NfcPlaylistWorker* nfc_playlist_worker);