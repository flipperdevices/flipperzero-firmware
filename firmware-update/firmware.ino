#include <esp_camera.h>

#include "camera_model.h"
#include "initialize.h"
#include "process_image.h"
#include "serial_commands.h"

camera_config_t config;

// Entry point of the program.
void setup()
{
    // Set up the camera model.
    CameraModel *model = CameraModel::getInstance();

    // Set up the model defaults.
    model->setIsDitheringDisabled(false);
    model->setIsInverted(false);
    model->setIsFlashEnabled(false);
    model->setIsStreamEnabled(true);
    model->setDitherAlgorithm(FLOYD_STEINBERG);

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

    // Begin serial communication.
    Serial.begin(230400); // 115200

    // Initialize the camera.
    initialize(&config);
}

// Main loop of the program.
void loop()
{
    // Get the camera model reference.
    CameraModel *model = CameraModel::getInstance();
    if (model->getIsStreamEnabled())
    {
        camera_fb_t *frame_buffer = esp_camera_fb_get();
        if (frame_buffer)
        {
            process_image(frame_buffer);
            // Return the frame buffer back to the camera driver.
            esp_camera_fb_return(frame_buffer);
        }
        delay(25);
    }
    serial_commands();
}
