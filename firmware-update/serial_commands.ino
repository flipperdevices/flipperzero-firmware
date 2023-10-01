#include "serial_commands.h"

void serial_commands(CameraModel *model)
{
    if (Serial.available() > 0)
    {
        char input = Serial.read();
        sensor_t *cam = esp_camera_sensor_get();

        switch (input)
        {
        case '>': // Toggle dithering.
            model->isDitheringDisabled = !model->isDitheringDisabled;
            break;
        case '<': // Toggle invert.
            model->isInverted = !model->isInverted;
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
            model->isFlashEnabled = false;
            pinMode(FLASH_GPIO_NUM, OUTPUT);
            digitalWrite(FLASH_GPIO_NUM, LOW);
            break;
        case 'F': // Turn the flash on.
            model->isFlashEnabled = true;
            pinMode(FLASH_GPIO_NUM, OUTPUT);
            digitalWrite(FLASH_GPIO_NUM, HIGH);
            break;
        case 'P': // Save image to the onboard SD card.
            // @todo - Future feature.
            // save_picture();
            break;
        case 'M': // Toggle Mirror.
            cam->set_hmirror(cam, !cam->status.hmirror);
            break;
        case 's': // Stop stream.
            model->isStreamEnabled = false;
            break;
        case 'S': // Start stream.
            model->isStreamEnabled = true;
            break;
        case '0': // Use Floyd Steinberg dithering.
            model->ditherAlgorithm = FLOYD_STEINBERG;
            break;
        case '1': // Use Jarvis Judice dithering.
            model->ditherAlgorithm = JARVIS_JUDICE_NINKE;
            break;
        case '2': // Use Stucki dithering.
            model->ditherAlgorithm = STUCKI;
            break;
        default:
            // Do nothing.
            break;
        }
    }
}
