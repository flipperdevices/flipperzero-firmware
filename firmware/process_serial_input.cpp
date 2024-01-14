#include "process_serial_input.h"

void process_serial_input() {
  if (Serial.available() > 0) {
    char input = Serial.read();
    sensor_t *cam = esp_camera_sensor_get();

    switch (input) {
    case 'b':
      lower_brightness();
      break;
    case 'B':
      add_brightness();
      break;
    case 'c':
      lower_contrast();
      break;
    case 'C':
      add_contrast();
      break;
    case 'd':
      camera_model.isDitheringEnabled = false;
      break;
    case 'D':
      camera_model.isDitheringEnabled = true;
      break;
    case 'f':
      turn_flash_off();
      break;
    case 'F':
      turn_flash_on();
      break;
    case 'i':
      camera_model.isInvertEnabled = false;
      break;
    case 'I':
      camera_model.isInvertEnabled = true;
      break;
    case 'P':
      // save_picture_to_sd_card(); // @todo
      break;
    case 's': // Stop serial stream.
      camera_model.isStreamToSerialEnabled = false;
      break;
    case 'S': // Start serial stream.
      camera_model.isStreamToWiFiEnabled = false;
      set_camera_config_defaults(CAMERA_FUNCTION_SERIAL);
      set_camera_model_defaults(CAMERA_FUNCTION_SERIAL);
      set_camera_defaults(CAMERA_FUNCTION_SERIAL);
      camera_model.isStreamToSerialEnabled = true;
      break;
    case 'w': // Stop wifi stream.
      camera_model.isStreamToWiFiEnabled = false;
      break;
    case 'W': // Start wifi stream.
      camera_model.isStreamToSerialEnabled = false;
      set_camera_config_defaults(CAMERA_FUNCTION_WIFI);
      set_camera_model_defaults(CAMERA_FUNCTION_WIFI);
      set_camera_defaults(CAMERA_FUNCTION_WIFI);
      camera_model.isStreamToWiFiEnabled = true;
      break;
    case '0':
      camera_model.ditherAlgorithm = FLOYD_STEINBERG;
      break;
    case '1':
      camera_model.ditherAlgorithm = JARVIS_JUDICE_NINKE;
      break;
    case '2':
      camera_model.ditherAlgorithm = STUCKI;
      break;
    default: // Do nothing.
      break;
    }
  }
}
