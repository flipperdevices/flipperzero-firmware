#include <furi.h>

#include <nfc/nfc.h>
#include <nfc/nfc_common.h>
#include <nfc/nfc_device.h>
#include <nfc/nfc_device_i.h>
#include <nfc/nfc_listener.h>

#include <storage/storage.h>
#include <toolbox/stream/stream.h>
#include <toolbox/stream/file_stream.h>

#include <nfc_playlist_worker.h>

// Define log tag
#define TAG "NfcPlaylist"

// Application entry point
int32_t nfc_playlist_main(void* p) {
   // Mark argument as unused
   UNUSED(p);
   // Just a little debug toggle
   const bool DEBUG = true;
   // open/alloc resources
   Storage* storage = furi_record_open(RECORD_STORAGE);
   Stream* stream = file_stream_alloc(storage);
   FuriString* line = furi_string_alloc();
   NfcPlaylistWorker* nfc_playlist_worker = nfc_playlist_worker_alloc();
   // Read file
   if(file_stream_open(stream, APP_DATA_PATH("playlist.txt"), FSAM_READ, FSOM_OPEN_EXISTING)) {

      // read the file line by line and print the text 
      while(stream_read_line(stream, line)) {
         if (DEBUG) {FURI_LOG_I(TAG, "Read line: %s", furi_string_get_cstr(line));}
         nfc_playlist_worker_set_nfc_data(nfc_playlist_worker, (char*)furi_string_get_cstr(line));
         nfc_playlist_worker_start(nfc_playlist_worker);
         furi_delay_ms(1500);
         nfc_playlist_worker_stop(nfc_playlist_worker);
         furi_string_reset(line);
      }
   } else {
      if (DEBUG) {FURI_LOG_E(TAG, "Failed to open file");}
   }
   // Free/close resources
   furi_string_free(line);
   file_stream_close(stream);
   stream_free(stream);
   nfc_playlist_worker_free(nfc_playlist_worker);

   // Close storage
   furi_record_close(RECORD_STORAGE);

   return 0;
}