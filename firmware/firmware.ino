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
        // Process the camera image and output to serial.
        stream_to_serial();
        delay(50);
        return;
    }
    
    if (camera_model.isStreamToWiFiEnabled) {
        // Stream the camera output to WiFi.
        streamToWiFi();
        delay(50);
        return;
    }

    // No camera function is enabled, turn the flash off if on.
    if (camera_model.isFlashEnabled) {
        turn_flash_off();
    }
    
    delay(50);
}
