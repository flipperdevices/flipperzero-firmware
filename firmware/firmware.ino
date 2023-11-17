#include "firmware.h"

void setup() {
    // Begin serial communication.
    Serial.begin(230400);

    // Set the camera configuration defaults.
    set_camera_config_defaults();

    // Set the camera model defaults values.
    set_camera_model_defaults();

    // Initialize the camera.
    initialize_camera();

    // Set the camera to the default settings.
    set_camera_defaults();
}

// Main loop of the program.
void loop() {
    process_serial_commands();

    if (camera_model.isStreamEnabled) {
        camera_fb_t* frame_buffer = esp_camera_fb_get();
        if (frame_buffer) {
            process_image(frame_buffer);
            // Return the frame buffer back to the camera driver.
            esp_camera_fb_return(frame_buffer);
            delay(50);
        }
    }
}
