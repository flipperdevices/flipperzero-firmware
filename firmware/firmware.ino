#include "firmware.h"

void setup() {
    // Begin serial communication.
    Serial.begin(230400);

    // Set initial camera configs for serial streaming.
    set_camera_config_defaults(CAMERA_FUNCTION_SERIAL);

    // Set initial camera model for serial streaming.
    set_camera_model_defaults(CAMERA_FUNCTION_SERIAL);

    // Initialize the camera.
    initialize_camera();

    // Set initial camera settings for serial streaming.
    set_camera_defaults(CAMERA_FUNCTION_SERIAL);
}

// Main loop of the program.
void loop() {
    process_serial_input();
    
    if (camera_model.isStreamToSerialEnabled) {
        // Stream the camera output to serial.
        streamToSerial();
        return;
    }
    
    if (camera_model.isStreamToWiFiEnabled) {
        // Stream the camera output to WiFi.
        streamToWiFi();
        return;
    }

    // No camera function is enabled, turn the flash off if on.
    if (camera_model.isFlashEnabled) {
        turn_flash_off();
    }
}

void streamToSerial() {
    camera_fb_t* frame_buffer = esp_camera_fb_get();
    if (frame_buffer) {
        // Process the image and output to serial.
        process_image_to_serial(frame_buffer);
        // Return the frame buffer back to the camera driver.
        esp_camera_fb_return(frame_buffer);
        delay(50);
    }
}

void streamToWiFi() {
    // @todo - Implement.
    turn_flash_on();
    delay(50);
}
