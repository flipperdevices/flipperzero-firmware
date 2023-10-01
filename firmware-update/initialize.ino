#include "initialize.h"

void initialize(camera_config_t *config) {
    // Initialize camera.
    esp_err_t err = esp_camera_init(config);
    if (err != ESP_OK) {
        return;
    }

    // Get the camera model reference.
    CameraModel *model = CameraModel::getInstance();

    // Check if the flash is already on, if it is turn it off.
    if (model->getIsFlashEnabled()) {
        pinMode(FLASH_GPIO_NUM, OUTPUT);
        digitalWrite(FLASH_GPIO_NUM, LOW);
        model->setIsFlashEnabled(false);
    }

    // Get the camera sensor reference.
    sensor_t *cam = esp_camera_sensor_get();

    // Set up the frame buffer reference.
    camera_fb_t *frame_buffer = esp_camera_fb_get();

    // Set initial brightness.
    cam->set_brightness(cam, 0);

    // Set initial contrast.
    cam->set_contrast(cam, 0);

    // Set initial rotation.
    cam->set_vflip(cam, true);
    cam->set_hmirror(cam, true);

    // Set initial saturation.
    cam->set_saturation(cam, 0);

    // Set initial sharpness.
    cam->set_sharpness(cam, 0);
}
