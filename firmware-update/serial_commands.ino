#include "serial_commands.h"

void serial_commands() {
    if (Serial.available() > 0) {
        // Get the camera model reference.
        CameraModel *model = CameraModel::getInstance();

        char input = Serial.read();
        sensor_t *cam = esp_camera_sensor_get();

        switch (input) {
            case '>':  // Toggle dithering.
                model->setIsDitheringDisabled(!model->getIsDitheringDisabled());
                break;
            case '<':  // Toggle invert.
                model->setIsInverted(!model->getIsInverted());
                break;
            case 'b':  // Remove brightness.
                cam->set_contrast(cam, cam->status.brightness - 1);
                break;
            case 'B':  // Add brightness.
                cam->set_contrast(cam, cam->status.brightness + 1);
                break;
            case 'c':  // Remove contrast.
                cam->set_contrast(cam, cam->status.contrast - 1);
                break;
            case 'C':  // Add contrast.
                cam->set_contrast(cam, cam->status.contrast + 1);
                break;
            case 'f':  // Turn the flash off.
                pinMode(FLASH_GPIO_NUM, OUTPUT);
                digitalWrite(FLASH_GPIO_NUM, LOW);
                model->setIsFlashEnabled(false);
                break;
            case 'F':  // Turn the flash on.
                pinMode(FLASH_GPIO_NUM, OUTPUT);
                digitalWrite(FLASH_GPIO_NUM, HIGH);
                model->setIsFlashEnabled(true);
                break;
            case 'P':  // Save image to the onboard SD card.
                // @todo - Future feature.
                // save_picture();
                break;
            case 'M':  // Toggle Mirror.
                cam->set_hmirror(cam, !cam->status.hmirror);
                break;
            case 's':  // Stop stream.
                model->setIsStreamEnabled(false);
                break;
            case 'S':  // Start stream.
                model->setIsStreamEnabled(true);
                break;
            case '0':  // Use Floyd Steinberg dithering.
                model->setDitherAlgorithm(FLOYD_STEINBERG);
                break;
            case '1':  // Use Jarvis Judice dithering.
                model->setDitherAlgorithm(JARVIS_JUDICE_NINKE);
                break;
            case '2':  // Use Stucki dithering.
                model->setDitherAlgorithm(STUCKI);
                break;
            default:
                // Do nothing.
                break;
        }
    }
}
