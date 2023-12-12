#include <furi.h>

#include <nfc/nfc.h>
#include <nfc/nfc_common.h>
#include <nfc/nfc_device.h>
#include <nfc/nfc_device_i.h>
#include <nfc/nfc_listener.h>
#include <nfc/nfc_poller.h>
#include <nfc/nfc_scanner.h>

#include <nfc/protocols/iso14443_3a/iso14443_3a.h>

#include <storage/storage.h>
#include <toolbox/stream/stream.h>
#include <toolbox/stream/file_stream.h>

// Define log tag
#define TAG "NfcPlaylist"

// Application entry point
int32_t nfc_playlist_main(void* p) {
   // Mark argument as unused
   UNUSED(p);
   // Just a little debug toggle
   const bool DEBUG = false;

   Storage* storage = furi_record_open(RECORD_STORAGE);
   Stream* stream = file_stream_alloc(storage);
   FuriString* line = furi_string_alloc();
   NfcDevice* nfcDevice = nfc_device_alloc();
   Nfc* nfc = nfc_alloc();

   // Read file
   if(file_stream_open(stream, APP_DATA_PATH("playlistTest.txt"), FSAM_READ, FSOM_OPEN_EXISTING)) {
      while(stream_read_line(stream, line)) {
         // Store file location
         const char* fileLocation = strcat("/ext/nfc/",furi_string_get_cstr(line));

         // Load file
         if (nfc_device_load(nfcDevice, fileLocation)) {

            if (DEBUG) {FURI_LOG_I(TAG, "Loaded file");}

            // Get protocol
            const NfcProtocol protocol = nfc_device_get_protocol(nfcDevice);
            // Get listern
            NfcListener* mfu_listener = nfc_listener_alloc(nfc, protocol, nfc_device_get_data(nfcDevice, protocol));
            // Start listener
            nfc_listener_start(mfu_listener, NULL, NULL);

            // Worst timer ever
            int counter = 0;
            while(true) {
               furi_delay_ms(50);
               counter++;
               if (counter == 100) {
                  break;
               }
            }

            // Stop listener && free
            nfc_listener_stop(mfu_listener);
            nfc_listener_free(mfu_listener);
            
         } else {
            if (DEBUG) {FURI_LOG_E(TAG, "Failed to load file");}
         }
         // output file location
         if (DEBUG) {FURI_LOG_I(TAG, "%s", fileLocation);}
         // clear instance
         nfc_device_clear(nfcDevice);
      }
   } else {
      if (DEBUG) {FURI_LOG_E(TAG, "Failed to open file");}
   }

   // Free/close resources
   furi_string_free(line);
   file_stream_close(stream);
   stream_free(stream);
   nfc_device_free(nfcDevice);
   nfc_free(nfc);

   // Close storage
   furi_record_close(RECORD_STORAGE);

   return 0;
}