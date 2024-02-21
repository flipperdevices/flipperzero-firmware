#include "nfc_playlist_worker.h"

NfcPlaylistWorker* nfc_playlist_worker_alloc() {
   NfcPlaylistWorker* nfc_playlist_worker = malloc(sizeof(NfcPlaylistWorker));

   nfc_playlist_worker->thread = furi_thread_alloc_ex("NfcPlaylistWorker", 8192, nfc_playlist_worker_task, nfc_playlist_worker);
   nfc_playlist_worker->state = NfcPlaylistWorkerState_Stopped;

   nfc_playlist_worker->nfc = nfc_alloc();
   nfc_playlist_worker->nfc_device = nfc_device_alloc();

   return nfc_playlist_worker;
}

void nfc_playlist_worker_free(NfcPlaylistWorker* nfc_playlist_worker) {
   furi_assert(nfc_playlist_worker);
   furi_thread_free(nfc_playlist_worker->thread);

   nfc_free(nfc_playlist_worker->nfc);
   nfc_device_free(nfc_playlist_worker->nfc_device);

   free(nfc_playlist_worker);
}

void nfc_playlist_worker_stop(NfcPlaylistWorker* nfc_playlist_worker) {
   furi_assert(nfc_playlist_worker);
   if (nfc_playlist_worker->state != NfcPlaylistWorkerState_Stopped) {
      nfc_playlist_worker->state = NfcPlaylistWorkerState_Stopped;
      furi_thread_join(nfc_playlist_worker->thread);
   }
}

void nfc_playlist_worker_start(NfcPlaylistWorker* nfc_playlist_worker) {
   furi_assert(nfc_playlist_worker);
   nfc_playlist_worker->state = NfcPlaylistWorkerState_Emulating;
   furi_thread_start(nfc_playlist_worker->thread);
}

int32_t nfc_playlist_worker_task(void* context) {
   NfcPlaylistWorker* nfc_playlist_worker = context;

   if (nfc_playlist_worker->state == NfcPlaylistWorkerState_Emulating) {

      nfc_playlist_worker->nfc_listener =
         nfc_listener_alloc(nfc_playlist_worker->nfc,
            nfc_playlist_worker->nfc_protocol,
            nfc_device_get_data(nfc_playlist_worker->nfc_device, nfc_playlist_worker->nfc_protocol)
         );
      nfc_listener_start(nfc_playlist_worker->nfc_listener, NULL, NULL);

      while(nfc_playlist_worker->state == NfcPlaylistWorkerState_Emulating) {
         furi_delay_ms(50);
      }

      nfc_listener_stop(nfc_playlist_worker->nfc_listener);
      nfc_listener_free(nfc_playlist_worker->nfc_listener);
   }

   nfc_playlist_worker->state = NfcPlaylistWorkerState_Stopped;

   return 0;
}

bool nfc_playlist_worker_is_emulating(NfcPlaylistWorker* nfc_playlist_worker) {
   if (nfc_playlist_worker->state == NfcPlaylistWorkerState_Emulating) {
      return true;
   }
   return false;
}

void nfc_playlist_worker_set_nfc_data(NfcPlaylistWorker* nfc_playlist_worker, char* file_path) {
   nfc_device_clear(nfc_playlist_worker->nfc_device);
   nfc_device_load(nfc_playlist_worker->nfc_device, file_path);
   nfc_playlist_worker->nfc_protocol = nfc_device_get_protocol(nfc_playlist_worker->nfc_device);
}

void nfc_playlist_worker_clear_nfc_data(NfcPlaylistWorker* nfc_playlist_worker) {
   nfc_device_clear(nfc_playlist_worker->nfc_device);
}

NfcDeviceData* nfc_playlist_worker_get_nfc_data(NfcPlaylistWorker* nfc_playlist_worker) {
   return nfc_playlist_worker->nfc_data;
}