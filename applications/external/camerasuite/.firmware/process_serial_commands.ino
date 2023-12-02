#include "process_serial_commands.h"

void process_serial_commands() {
    if (Serial.available() > 0) {
        char input = Serial.read();
        sensor_t* cam = esp_camera_sensor_get();

        switch (input) {
        case '>': // Toggle dithering.
            camera_model.isDitheringEnabled = !camera_model.isDitheringEnabled;
            break;
        case 'i': // Turn invert off.
            camera_model.isInvertEnabled = false;
            break;
        case 'I': // Turn invert on.
            camera_model.isInvertEnabled = true;
            break;
        case 'b': // Remove brightness.
            cam->set_contrast(cam, cam->status.brightness - 1);
            break;
        case 'B': // Add brightness.
            cam->set_contrast(cam, cam->status.brightness + 1);
            break;
        case 'c': // Remove contrast.
            cam->set_contrast(cam, cam->status.contrast - 1);
            break;
        case 'C': // Add contrast.
            cam->set_contrast(cam, cam->status.contrast + 1);
            break;
        case 'f': // Turn the flash off.
            toggle_flash_off();
            break;
        case 'F': // Turn the flash on.
            toggle_flash_on();
            break;
        case 'P': // Save image to the onboard SD card.
            // @todo - Future feature.
            // save_picture_to_sd_card();
            break;
        case 'M': // Toggle Mirror.
            cam->set_hmirror(cam, !cam->status.hmirror);
            break;
        case 's': // Stop stream.
            camera_model.isStreamEnabled = false;
            break;
        case 'S': // Start stream.
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
