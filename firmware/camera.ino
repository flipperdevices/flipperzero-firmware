#include "camera.h"

void initialize_camera() {
    // Initialize camera.
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK) {
        // If initialization fails, log here for debugging.
        return;
    }

    turn_flash_off();
}

void reset_camera() {
    turn_flash_off();

    // Get the camera sensor reference.
    sensor_t* cam = esp_camera_sensor_get();

    cam->set_contrast(cam, 0);    // Set initial contrast.
    cam->set_brightness(cam, 0);  // Set initial brightness.
    cam->set_saturation(cam, 0);  // Set initial saturation.
    cam->set_sharpness(cam, 0);   // Set initial sharpness.
    cam->set_vflip(cam, false);   // Set initial vertical flip.
    cam->set_hmirror(cam, false); // Set initial horizontal mirror.
}

void turn_flash_off() {
    pinMode(FLASH_GPIO_NUM, OUTPUT);
    digitalWrite(FLASH_GPIO_NUM, LOW);
    camera_model.isFlashEnabled = false;
}

void turn_flash_on() {
    pinMode(FLASH_GPIO_NUM, OUTPUT);
    digitalWrite(FLASH_GPIO_NUM, HIGH);
    camera_model.isFlashEnabled = true;
}

void handle_flash_state() {
    // If the flash state ever gets out of sync with the camera model, fix it.
    if (!camera_model.isFlashEnabled) {
        int flashState = digitalRead(FLASH_GPIO_NUM);
        if (flashState == HIGH) {
            turn_flash_off();
        }
    }
}

void add_contrast() {
    sensor_t* cam = esp_camera_sensor_get();
    cam->set_contrast(cam, cam->status.contrast + 1);
}

void lower_contrast() {
    sensor_t* cam = esp_camera_sensor_get();
    cam->set_contrast(cam, cam->status.contrast - 1);
}

void add_brightness() {
    sensor_t* cam = esp_camera_sensor_get();
    cam->set_brightness(cam, cam->status.brightness + 1);
}

void lower_brightness() {
    sensor_t* cam = esp_camera_sensor_get();
    cam->set_brightness(cam, cam->status.brightness - 1);
}

void set_hmirror(bool is_hmirror_enabled) {
    sensor_t* cam = esp_camera_sensor_get();
    cam->set_hmirror(cam, is_hmirror_enabled);
}
