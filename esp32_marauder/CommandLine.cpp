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
  String input = this->getSerialInput();

  this->runCommand(input);
}

LinkedList<String> CommandLine::parseCommand(String input) {
  LinkedList<String> cmd_args;
  
  if (input != "") {
    
    char delim[] = " ";

    char fancy[input.length() + 1] = {};
    input.toCharArray(fancy, input.length() + 1);
        
    char* ptr = strtok(fancy, delim);
  
    while (ptr != NULL) {
      cmd_args.add(String(ptr));
  
      ptr = strtok(NULL, delim);
    }

    //memset(fancy, 0, sizeof(fancy));

    // Display the segmented cmd
    //for (int i = 0; i < cmd_args.size(); i++) {
    //  Serial.println(cmd_args.get(i));
    //}
  }

  return cmd_args;
}

int CommandLine::argSearch(LinkedList<String>* cmd_args_list, String key) {
  for (int i = 0; i < cmd_args_list->size(); i++) {
    if (cmd_args_list->get(i) == key)
      return i;
  }

  return -1;
}

void CommandLine::runCommand(String input) {
  if (input != "")
    Serial.println("#" + input);
  else
    return;

  LinkedList<String> cmd_args = this->parseCommand(input);

  //// Admin commands

  // Stop Scan
  if (cmd_args.get(0) == STOPSCAN_CMD) {
    wifi_scan_obj.StartScan(WIFI_SCAN_OFF);

    // If we don't do this, the text and button coordinates will be off
    #ifdef HAS_SCREEN
      display_obj.tft.init();
      menu_function_obj.changeMenu(menu_function_obj.current_menu);
    #endif
  }
  // Channel command
  else if (cmd_args.get(0) == CH_CMD) {
    // Search for channel set arg
    int ch_set = this->argSearch(&cmd_args, "-s");
    
    if (cmd_args.size() == 1) {
      Serial.println("Current channel: " + (String)wifi_scan_obj.set_channel);
    }
    else if (ch_set != -1) {
      wifi_scan_obj.set_channel = cmd_args.get(ch_set + 1).toInt();
      wifi_scan_obj.changeChannel();
      Serial.println("Set channel: " + (String)wifi_scan_obj.set_channel);
    }
  }
  // Clear APs
  else if (cmd_args.get(0) == CLEARAP_CMD) {
    wifi_scan_obj.RunClearAPs();
  }

  //// WiFi Scan commands
  if (!wifi_scan_obj.scanning()) {

    // AP Scan
    if (cmd_args.get(0) == SCANAP_CMD) {
      #ifdef HAS_SCREEN
        display_obj.clearScreen();
        menu_function_obj.drawStatusBar();
      #endif
      wifi_scan_obj.StartScan(WIFI_SCAN_TARGET_AP, TFT_MAGENTA);
    }
    // Beacon sniff
    else if (cmd_args.get(0) == SNIFF_BEACON_CMD) {
      #ifdef HAS_SCREEN
        display_obj.clearScreen();
        menu_function_obj.drawStatusBar();
      #endif
      wifi_scan_obj.StartScan(WIFI_SCAN_AP, TFT_MAGENTA);
    }
    // Deauth sniff
    else if (cmd_args.get(0) == SNIFF_DEAUTH_CMD) {
      #ifdef HAS_SCREEN
        display_obj.clearScreen();
        menu_function_obj.drawStatusBar();
      #endif
      wifi_scan_obj.StartScan(WIFI_SCAN_DEAUTH, TFT_RED);
    }
    // PMKID sniff
    else if (cmd_args.get(0) == SNIFF_PMKID_CMD) {
      wifi_scan_obj.StartScan(WIFI_SCAN_EAPOL, TFT_VIOLET);
    }
  }
}
