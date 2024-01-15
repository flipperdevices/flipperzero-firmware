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
  } else if (camera_model.isStreamToWiFiEnabled) {
    // Stream the camera output to WiFi.
    stream_to_wifi();
  } else if (camera_model.isFlashEnabled) {
    // Not currently streaming, turn the flash off if it's enabled.
    turn_flash_off();
  }
  
  delay(50);
}
