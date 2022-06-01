#include "CommandLine.h"

CommandLine::CommandLine() {
}

void CommandLine::RunSetup() {
  Serial.println(this->ascii_art);

  Serial.println(F("\n\n--------------------------------\n"));
  Serial.println(F("         ESP32 Marauder      \n"));
  Serial.println("            " + version_number + "\n");
  Serial.println(F("       By: justcallmekoko\n"));
  Serial.println(F("--------------------------------\n\n"));
  
  Serial.print("> ");
}

String CommandLine::getSerialInput() {
  String input = "";

  if (Serial.available() > 0)
    input = Serial.readStringUntil('\n');

  input.trim();
  return input;
}

void CommandLine::main(uint32_t currentTime) {
  String input = this->getSerialInput();

  this->runCommand(input);

  if (input != "")
    Serial.print("> ");
}

LinkedList<String> CommandLine::parseCommand(String input, char* delim) {
  LinkedList<String> cmd_args;
  
  if (input != "") {
    
    //char delim[] = " ";

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

  LinkedList<String> cmd_args = this->parseCommand(input, " ");

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

  else if (cmd_args.get(0) == REBOOT_CMD) {
    ESP.restart();
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

    //// WiFi attack commands
    // attack
    if (cmd_args.get(0) == ATTACK_CMD) {
      int attack_type_switch = this->argSearch(&cmd_args, "-t"); // Required
  
      if (attack_type_switch == -1) {
        Serial.println("You must specify an attack type");
        return;
      }
      else {
        String attack_type = cmd_args.get(attack_type_switch + 1);
  
        // Branch on attack type
        if (attack_type == ATTACK_TYPE_DEAUTH) {
          #ifdef HAS_SCREEN
            display_obj.clearScreen();
            menu_function_obj.drawStatusBar();
          #endif
          wifi_scan_obj.StartScan(WIFI_ATTACK_DEAUTH, TFT_RED);
        }
        else {
          Serial.println("Attack type not properly defined");
          return;
        }
      }
    }
  }


  //// WiFi aux commands

  // List access points
  if (cmd_args.get(0) == LIST_AP_CMD) {
    for (int i = 0; i < access_points->size(); i++) {
      if (access_points->get(i).selected)
        Serial.println("[" + (String)i + "] " + access_points->get(i).essid + " (selected)");
      else
        Serial.println("[" + (String)i + "] " + access_points->get(i).essid);
    }
  }
  // Select access points or stations
  else if (cmd_args.get(0) == SEL_CMD) {
    // Get switches
    int ap_sw = this->argSearch(&cmd_args, "-a");
    int st_sw = this->argSearch(&cmd_args, "-s");

    // Access points
    if (ap_sw != -1) {
      // Get list of indices
      LinkedList<String> ap_index = this->parseCommand(cmd_args.get(ap_sw + 1), ",");

      // Mark APs as selected
      for (int i = 0; i < ap_index.size(); i++) {
        int index = ap_index.get(i).toInt();
        if (access_points->get(index).selected) {
          // Unselect "selected" ap
          AccessPoint new_ap = access_points->get(index);
          new_ap.selected = false;
          access_points->set(index, new_ap);
        }
        else {
          // Select "unselected" ap
          AccessPoint new_ap = access_points->get(index);
          new_ap.selected = true;
          access_points->set(index, new_ap);
        }
      }
    }
    // Stations
    else if (st_sw != -1) {
      
    }
  }
}
