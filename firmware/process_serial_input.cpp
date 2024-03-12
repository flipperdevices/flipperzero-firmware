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
      set_dithering(false);
      break;
    case 'D':
      set_dithering(true);
      break;
    case 'f':
      turn_flash_off();
      break;
    case 'F':
      turn_flash_on();
      break;
    case 'i':
      set_inverted(false);
      break;
    case 'I':
      set_inverted(true);
      break;
    case 's':
      stop_serial_stream();
      break;
    case 'S':
      start_serial_stream();
      break;
    case '0':
      set_dithering_algorithm(FLOYD_STEINBERG);
      break;
    case '1':
      set_dithering_algorithm(JARVIS_JUDICE_NINKE);
      break;
    case '2':
      set_dithering_algorithm(STUCKI);
      break;
    default:
      break;
    }
  }
}
