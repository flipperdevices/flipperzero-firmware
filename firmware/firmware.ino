#include "firmware.h"

void setup() {
    // Begin serial communication.
    Serial.begin(230400); // 115200

    // Initialize the camera model.
    initialize_camera_model();

    // Initialize the camera configuration.
    initialize_camera_config();

    // Initialize the camera.
    initialize_camera();
}

// Main loop of the program.
void loop() {
    if (camera_model.isStreamEnabled) {
        camera_fb_t* frame_buffer = esp_camera_fb_get();
        if (frame_buffer) {
            process_image(frame_buffer);
            // Return the frame buffer back to the camera driver.
            esp_camera_fb_return(frame_buffer);
        }
        delay(50);
    }
    handle_flash_state();
    process_serial_commands();
}
