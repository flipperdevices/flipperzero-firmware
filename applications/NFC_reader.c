#include <furi.h>
#include <gui/gui.h>
#include <nfc/nfc.h>

// Función de callback para manejar la lectura NFC
void nfc_read_callback(NfcEvent* event, void* context) {
    if(event->type == NfcEventTypeTagDiscovered) {
        // Muestra el UID de la tarjeta detectada en formato hexadecimal
        FURI_LOG_D("NFC Reader", "Tarjeta detectada, UID: %02X %02X %02X %02X",
            event->data.tag_discovered.uid[0],
            event->data.tag_discovered.uid[1],
            event->data.tag_discovered.uid[2],
            event->data.tag_discovered.uid[3]);
    }
}

// Función principal de la aplicación NFC
int32_t nfc_reader_main(void* p) {
    // Abre el acceso a la GUI del Flipper Zero
    Gui* gui = furi_record_open("gui");
    // Inicializa la funcionalidad NFC
    Nfc* nfc = furi_record_open("nfc");

    // Inicia la búsqueda de tarjetas NFC en modo pasivo
    nfc_start_discovery(nfc, NfcPollModePassiveTarget);

    // Bucle que procesa los eventos NFC
    while(1) {
        nfc_process_events(nfc, 1000); // Procesa eventos NFC cada 1 segundo
    }

    // Libera los recursos utilizados
    furi_record_close("nfc");
    furi_record_close("gui");
    return 0;
}