#include "firmware.h"

void setup() {
  camera_model.isStreamToSerialEnabled = false;
  
  // Begin serial communication.
  Serial.begin(230400);

  // Set initial camera configs for serial streaming.
  set_camera_config_defaults();

  // Set initial camera model for serial streaming.
  set_camera_model_defaults();

  // Initialize the camera.
  initialize_camera();

  // Set initial camera settings for serial streaming.
  set_camera_defaults();
}

// Main loop of the program.
void loop() {
  process_serial_input();
  
  if (camera_model.isStreamToSerialEnabled) {
    // Process the camera image and output to serial.
    stream_to_serial();
  } else if (camera_model.isFlashEnabled) {
    // Not currently streaming, turn the flash off if it's enabled.
    turn_flash_off();
  }
  
  delay(50);
}
