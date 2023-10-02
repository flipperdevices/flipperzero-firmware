#include "camera.h"

void initialize_camera() {
    // Initialize camera.
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK) {
        return;
    }

    // Check if the flash is already on, if it is turn it off.
    if (camera_model.isFlashEnabled) {
        toggle_flash_off();
    }

    // Get the camera sensor reference.
    sensor_t* cam = esp_camera_sensor_get();

    cam->set_contrast(cam, 0); // Set initial contrast.
    cam->set_vflip(cam, true); // Set initial vertical flip.

    // cam->set_hmirror(cam, false); // Set initial horizontal mirror.
    // cam->set_brightness(cam, 0);  // Set initial brightness.
    // cam->set_saturation(cam, 0); // Set initial saturation.
    // cam->set_sharpness(cam, 0); // Set initial sharpness.
}

void toggle_flash_off() {
    pinMode(FLASH_GPIO_NUM, OUTPUT);
    digitalWrite(FLASH_GPIO_NUM, LOW);
    camera_model.isFlashEnabled = false;
}

void toggle_flash_on() {
    pinMode(FLASH_GPIO_NUM, OUTPUT);
    digitalWrite(FLASH_GPIO_NUM, HIGH);
    camera_model.isFlashEnabled = true;
}

void handle_flash_state() {
    // If the flash state ever gets out of sync with the camera model, fix it.
    if (!camera_model.isFlashEnabled) {
        int flashState = digitalRead(FLASH_GPIO_NUM);
        if (flashState == HIGH) {
            toggle_flash_off();
        }
    }
}
