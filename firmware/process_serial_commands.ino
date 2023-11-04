#include "process_serial_commands.h"

void process_serial_commands() {
    if (Serial.available() > 0) {
        char input = Serial.read();
        sensor_t* cam = esp_camera_sensor_get();

        switch (input) {
        case '>': // Backward supports `[ESP32] Camera`.
            camera_model.isDitheringEnabled = !camera_model.isDitheringEnabled;
            break;
        case '<': // _Only_ for backwards support on `[ESP32] Camera`.
            camera_model.isInvertEnabled = !camera_model.isInvertEnabled;
            break;
        case 'i':
            camera_model.isInvertEnabled = false;
            break;
        case 'I':
            camera_model.isInvertEnabled = true;
            break;
        case 'b': // Backward supports `[ESP32] Camera`.
            lower_brightness();
            break;
        case 'B': // Backward supports `[ESP32] Camera`.
            add_brightness();
            break;
        case 'c': // Backward supports `[ESP32] Camera`.
            lower_contrast();
            break;
        case 'C': // Backward supports `[ESP32] Camera`.
            add_contrast();
            break;
        case 'f':
            turn_flash_off();
            break;
        case 'F':
            turn_flash_on();
            break;
        case 'P':
            // save_picture_to_sd_card(); // Staged future feature.
            break;
        case 'M': // _Only_ for backwards support on `[ESP32] Camera`.
            set_hmirror(!cam->status.hmirror);
            break;
        case 's': // Stop stream.
            set_camera_model_defaults();
            set_camera_defaults();
            camera_model.isStreamEnabled = false;
            break;
        case 'S': // Start stream.
            set_camera_model_defaults();
            set_camera_defaults();
            camera_model.isStreamEnabled = true;
            break;
        case '0': // Use Floyd Steinberg dithering.
            camera_model.ditherAlgorithm = FLOYD_STEINBERG;
            break;
        case '1': // Use Jarvis Judice dithering.
            camera_model.ditherAlgorithm = JARVIS_JUDICE_NINKE;
            break;
        case '2': // Use Stucki dithering.
            camera_model.ditherAlgorithm = STUCKI;
            break;
        default:
            // Do nothing.
            break;
        }
    }
}
