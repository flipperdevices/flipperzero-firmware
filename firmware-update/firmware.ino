#include "firmware.h"

CameraModel model;

// Entry point of the program.
void setup()
{
    Serial.begin(115200); // Previously 230400, 115200 seems more stable.
    initialize(&model);
}

// Main loop of the program.
void loop()
{
    if (model.isStreamEnabled)
    {
        camera_fb_t *frame_buffer = esp_camera_fb_get();
        if (frame_buffer)
        {
            process_image(frame_buffer, &model);
            // Return the frame buffer back to the camera driver.
            esp_camera_fb_return(frame_buffer);
        }
        delay(25);
    }
    serial_commands(&model);
}
