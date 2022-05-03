#include "CommandLine.h"

CommandLine::CommandLine() {
}

void CommandLine::RunSetup() {
  
}

String CommandLine::getSerialInput() {
  String input = "";

  if (Serial.available() > 0)
    input = Serial.readStringUntil('\n');

  return input;
}

void CommandLine::main(uint32_t currentTime) {
   this->parseCommand(this->getSerialInput());
}

void CommandLine::parseCommand(String input) {
  if (input != "")
    Serial.println("#" + input);

  if (input == "stopscan") {
    wifi_scan_obj.StartScan(WIFI_SCAN_OFF);

    // If we don't do this, the text and button coordinates will be off
    display_obj.tft.init();

    // Take us back to the menu
    menu_function_obj.changeMenu(menu_function_obj.current_menu);
  }
    
  else if (input == "scanap") {
    display_obj.clearScreen();
    menu_function_obj.drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_TARGET_AP, TFT_MAGENTA);
  }

  else if (input == "clearap") {
    wifi_scan_obj.RunClearAPs();
  }
}
