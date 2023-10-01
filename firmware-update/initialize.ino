#include "initialize.h"

void initialize(CameraModel *model)
{
    // Set up the model defaults.
    model->isDitheringDisabled = false;
    model->isInverted = false;
    model->isFlashEnabled = false;
    model->isStreamEnabled = true;
    model->ditherAlgorithm = FLOYD_STEINBERG;

    // Set initial camera configurations for grayscale.
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_GRAYSCALE;
    config.frame_size = FRAMESIZE_QQVGA;
    config.fb_count = 1;

    // Initialize camera.
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        return;
    }

    // Check if the flash is already on, if it is turn it off.
    if (digitalRead(FLASH_GPIO_NUM) == HIGH)
    {
        pinMode(FLASH_GPIO_NUM, OUTPUT);
        digitalWrite(FLASH_GPIO_NUM, LOW);
        model->isFlashEnabled = false;
    }

    // Set global reference to the camera.
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
