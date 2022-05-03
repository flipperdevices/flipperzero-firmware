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
    
  if (input == "scanap") {
    display_obj.clearScreen();
    menu_function_obj.drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_TARGET_AP, TFT_MAGENTA);
  }
}
