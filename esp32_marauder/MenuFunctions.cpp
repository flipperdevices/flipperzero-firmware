#include "MenuFunctions.h"
//#include "icons.h"

extern const unsigned char menu_icons[][66];

MenuFunctions::MenuFunctions()
{
}

// Function to check menu input
void MenuFunctions::main(uint32_t currentTime)
{
  if (display_obj.exit_draw) {
    display_obj.exit_draw = false;
    changeMenu(current_menu);
  }
  if ((wifi_scan_obj.currentScanMode == WIFI_SCAN_OFF) ||
      (wifi_scan_obj.currentScanMode == OTA_UPDATE) ||
      (wifi_scan_obj.currentScanMode == SHOW_INFO)) {
    if (wifi_scan_obj.orient_display) {
      this->orientDisplay();
      wifi_scan_obj.orient_display = false;
    }
    //if ((display_obj.current_banner_pos <= 0) || (display_obj.current_banner_pos == SCREEN_WIDTH))
    //{
    //  this->drawStatusBar();
    //}
    display_obj.updateBanner(current_menu->name);
  }

  if (currentTime != 0) {
    if (currentTime - initTime >= 100) {
      this->initTime = millis();
      this->updateStatusBar();
    }
  }

  //this->displayCurrentMenu();

  boolean pressed = false;
  // This is code from bodmer's keypad example
  uint16_t t_x = 0, t_y = 0; // To store the touch coordinates

  // Get the display buffer out of the way
  if ((wifi_scan_obj.currentScanMode != WIFI_SCAN_OFF ) &&
      (wifi_scan_obj.currentScanMode != WIFI_ATTACK_BEACON_SPAM) &&
      (wifi_scan_obj.currentScanMode != WIFI_ATTACK_RICK_ROLL))
    display_obj.displayBuffer();
  //Serial.println(wifi_scan_obj.freeRAM());


  // Pressed will be set true is there is a valid touch on the screen
  int pre_getTouch = millis();

  // getTouch causes a 10ms delay which makes beacon spam less effective
  //if (wifi_scan_obj.currentScanMode == WIFI_SCAN_OFF)
  pressed = display_obj.tft.getTouch(&t_x, &t_y);

  //if (pressed)
  //  Serial.println("Pressed, son");
  //boolean pressed = false;

  //Serial.print("getTouch: ");
  //Serial.print(millis() - pre_getTouch);
  //Serial.println("ms");


  // This is if there are scans/attacks going on
  if ((wifi_scan_obj.currentScanMode != WIFI_SCAN_OFF) &&
      (pressed) &&
      (wifi_scan_obj.currentScanMode != OTA_UPDATE) &&
      (wifi_scan_obj.currentScanMode != SHOW_INFO))
  {
    // Stop the current scan
    if ((wifi_scan_obj.currentScanMode == WIFI_SCAN_PROBE) ||
        (wifi_scan_obj.currentScanMode == WIFI_SCAN_AP) ||
        (wifi_scan_obj.currentScanMode == WIFI_SCAN_PWN) ||
        (wifi_scan_obj.currentScanMode == WIFI_SCAN_ESPRESSIF) ||
        (wifi_scan_obj.currentScanMode == WIFI_SCAN_ALL) ||
        (wifi_scan_obj.currentScanMode == WIFI_SCAN_DEAUTH) ||
        (wifi_scan_obj.currentScanMode == WIFI_ATTACK_BEACON_SPAM) ||
        (wifi_scan_obj.currentScanMode == WIFI_ATTACK_RICK_ROLL) ||
        (wifi_scan_obj.currentScanMode == BT_SCAN_ALL) ||
        (wifi_scan_obj.currentScanMode == BT_SCAN_SKIMMERS))
    {
      Serial.println("Stopping scan...");
      wifi_scan_obj.StartScan(WIFI_SCAN_OFF);

      // If we don't do this, the text and button coordinates will be off
      display_obj.tft.init();

      // Take us back to the menu
      changeMenu(current_menu);
    }

    x = -1;
    y = -1;

    return;
  }

  // Check if any key coordinate boxes contain the touch coordinates
  // This is for when on a menu
  if ((wifi_scan_obj.currentScanMode != WIFI_ATTACK_BEACON_SPAM) &&
      (wifi_scan_obj.currentScanMode != WIFI_ATTACK_RICK_ROLL))
  {
    // Need this to set all keys to false
    for (uint8_t b = 0; b < BUTTON_ARRAY_LEN; b++) {
      if (pressed && display_obj.key[b].contains(t_x, t_y)) {
        display_obj.key[b].press(true);  // tell the button it is pressed
      } else {
        display_obj.key[b].press(false);  // tell the button it is NOT pressed
      }
    }

    // Check if any key has changed state
    for (uint8_t b = 0; b < current_menu->list->size(); b++) {
      display_obj.tft.setFreeFont(MENU_FONT);
      if (display_obj.key[b].justPressed()) {
        //display_obj.key[b].drawButton2(current_menu->list->get(b).name, true);  // draw invert
        //display_obj.key[b].drawButton(ML_DATUM, BUTTON_PADDING, current_menu->list->get(b).name, true);
        display_obj.key[b].drawButton(true, current_menu->list->get(b).name);
        if (current_menu->list->get(b).name != "Back")
          display_obj.tft.drawXBitmap(0,
                                      KEY_Y + b * (KEY_H + KEY_SPACING_Y) - (ICON_H / 2),
                                      menu_icons[current_menu->list->get(b).icon],
                                      ICON_W,
                                      ICON_H,
                                      current_menu->list->get(b).color,
                                      TFT_BLACK);
      }
      //else if (pressed)
      //  display_obj.key[b].drawButton(false, current_menu->list->get(b).name);

      // If button was just release, execute the button's function
      if ((display_obj.key[b].justReleased()) && (!pressed))
      {
        //display_obj.key[b].drawButton2(current_menu->list->get(b).name);     // draw normal
        //display_obj.key[b].drawButton(ML_DATUM, BUTTON_PADDING, current_menu->list->get(b).name);
        display_obj.key[b].drawButton(false, current_menu->list->get(b).name);
        current_menu->list->get(b).callable();
      }
      // This
      else if ((display_obj.key[b].justReleased()) && (pressed)) {
        display_obj.key[b].drawButton(false, current_menu->list->get(b).name);
        if (current_menu->list->get(b).name != "Back")
          display_obj.tft.drawXBitmap(0,
                                      KEY_Y + b * (KEY_H + KEY_SPACING_Y) - (ICON_H / 2),
                                      menu_icons[current_menu->list->get(b).icon],
                                      ICON_W,
                                      ICON_H,
                                      TFT_BLACK,
                                      current_menu->list->get(b).color);
      }

      display_obj.tft.setFreeFont(NULL);
    }
  }
  x = -1;
  y = -1;
}

#if BATTERY_ANALOG_ON == 1
byte battery_analog_array[10];
byte battery_count = 0;
byte battery_analog_last = 101;
#define BATTERY_CHECK 50
uint16_t battery_analog = 0;
void MenuFunctions::battery(bool initial)
{
  if (BATTERY_ANALOG_ON) {
    uint8_t n = 0;
    byte battery_analog_sample[10];
    byte deviation;
    if (battery_count == BATTERY_CHECK - 5)  digitalWrite(BATTERY_PIN, HIGH);
    else if (battery_count == 5) digitalWrite(BATTERY_PIN, LOW);
    if (battery_count == 0) {
      battery_analog = 0;
      for (n = 9; n > 0; n--)battery_analog_array[n] = battery_analog_array[n - 1];
      for (n = 0; n < 10; n++) {
        battery_analog_sample[n] = map((analogRead(ANALOG_PIN) * 5), 2400, 4200, 0, 100);
        if (battery_analog_sample[n] > 100) battery_analog_sample[n] = 100;
        else if (battery_analog_sample[n] < 0) battery_analog_sample[n] = 0;
        battery_analog += battery_analog_sample[n];
      }
      battery_analog = battery_analog / 10;
      for (n = 0; n < 10; n++) {
        deviation = abs(battery_analog - battery_analog_sample[n]);
        if (deviation >= 10) battery_analog_sample[n] = battery_analog;
      }
      battery_analog = 0;
      for (n = 0; n < 10; n++) battery_analog += battery_analog_sample[n];
      battery_analog = battery_analog / 10;
      battery_analog_array[0] = battery_analog;
      if (battery_analog_array[9] > 0 ) {
        battery_analog = 0;
        for (n = 0; n < 10; n++) battery_analog += battery_analog_array[n];
        battery_analog = battery_analog / 10;
      }
      battery_count ++;
    }
    else if (battery_count < BATTERY_CHECK) battery_count++;
    else if (battery_count >= BATTERY_CHECK) battery_count = 0;

    if (battery_analog_last != battery_analog) {
      battery_analog_last = battery_analog;
      MenuFunctions::battery2();
    }
  }
}
void MenuFunctions::battery2(bool initial)
{
  uint16_t the_color;
  if ( digitalRead(CHARGING_PIN) == 1) the_color = TFT_BLUE;
  else if (battery_analog < 20) the_color = TFT_RED;
  else if (battery_analog < 40)  the_color = TFT_YELLOW;
  else the_color = TFT_GREEN;

  display_obj.tft.setTextColor(the_color, STATUSBAR_COLOR);
  display_obj.tft.fillRect(186, 0, 50, STATUS_BAR_WIDTH, STATUSBAR_COLOR);
  display_obj.tft.drawXBitmap(186,
                              0,
                              menu_icons[STATUS_BAT],
                              16,
                              16,
                              STATUSBAR_COLOR,
                              the_color);
  display_obj.tft.drawString((String) battery_analog + "%", 204, 0, 2);
}
#else
void MenuFunctions::battery(bool initial)
{
  uint16_t the_color;
  if (battery_obj.i2c_supported)
  {
    // Could use int compare maybe idk
    if (((String)battery_obj.battery_level != "25") && ((String)battery_obj.battery_level != "0"))
      the_color = TFT_GREEN;
    else
      the_color = TFT_RED;

    if ((battery_obj.battery_level != battery_obj.old_level) || (initial)) {
      battery_obj.old_level = battery_obj.battery_level;
      display_obj.tft.fillRect(204, 0, SCREEN_WIDTH, STATUS_BAR_WIDTH, STATUSBAR_COLOR);
      display_obj.tft.setCursor(0, 1);
      display_obj.tft.drawXBitmap(186,
                                  0,
                                  menu_icons[STATUS_BAT],
                                  16,
                                  16,
                                  STATUSBAR_COLOR,
                                  the_color);
      display_obj.tft.drawString((String)battery_obj.battery_level + "%", 204, 0, 2);
    }
  }
}
void MenuFunctions::battery2(bool initial)
{
  MenuFunctions::battery(initial);
}
#endif

void MenuFunctions::updateStatusBar()
{
  uint16_t the_color; 

  // Draw temp info
  if (temp_obj.current_temp < 70)
    the_color = TFT_GREEN;
  else if ((temp_obj.current_temp >= 70) && (temp_obj.current_temp < 80))
    the_color = TFT_YELLOW;
  else if ((temp_obj.current_temp >= 80) && (temp_obj.current_temp < 90))
    the_color = TFT_ORANGE;
  else if ((temp_obj.current_temp >= 90) && (temp_obj.current_temp < 100))
    the_color = TFT_RED;
  else
    the_color = TFT_MAROON;

  display_obj.tft.setTextColor(the_color, STATUSBAR_COLOR);
  if (temp_obj.current_temp != temp_obj.old_temp) {
    temp_obj.old_temp = temp_obj.current_temp;
    display_obj.tft.fillRect(0, 0, 50, STATUS_BAR_WIDTH, STATUSBAR_COLOR);
    display_obj.tft.drawString((String)temp_obj.current_temp + " C", 4, 0, 2);
  }
  display_obj.tft.setTextColor(TFT_WHITE, STATUSBAR_COLOR);

  // WiFi Channel Stuff
  if (wifi_scan_obj.set_channel != wifi_scan_obj.old_channel) {
    wifi_scan_obj.old_channel = wifi_scan_obj.set_channel;
    display_obj.tft.fillRect(50, 0, 50, STATUS_BAR_WIDTH, STATUSBAR_COLOR);
    display_obj.tft.drawString("CH: " + (String)wifi_scan_obj.set_channel, 50, 0, 2);
  }

  // RAM Stuff
  wifi_scan_obj.freeRAM();
  if (wifi_scan_obj.free_ram != wifi_scan_obj.old_free_ram) {
    wifi_scan_obj.old_free_ram = wifi_scan_obj.free_ram;
    display_obj.tft.fillRect(100, 0, 60, STATUS_BAR_WIDTH, STATUSBAR_COLOR);
    display_obj.tft.drawString((String)wifi_scan_obj.free_ram + "B", 100, 0, 2);
  }

  // Draw battery info
  MenuFunctions::battery(false);

  // Draw SD info
  if (sd_obj.supported)
    the_color = TFT_GREEN;
  else
    the_color = TFT_RED;

  display_obj.tft.drawXBitmap(170,
                              0,
                              menu_icons[STATUS_SD],
                              16,
                              16,
                              STATUSBAR_COLOR,
                              the_color);
  //display_obj.tft.print((String)battery_obj.battery_level + "%");
}

void MenuFunctions::drawStatusBar()
{
  display_obj.tft.fillRect(0, 0, 240, STATUS_BAR_WIDTH, STATUSBAR_COLOR);
  //display_obj.tft.fillRect(0, STATUS_BAR_WIDTH + 1, 240, 1, TFT_DARKGREY);
  display_obj.tft.setTextColor(TFT_WHITE, STATUSBAR_COLOR);
  //display_obj.tft.setTextSize(2);

  uint16_t the_color;

  // Draw temp info
  if (temp_obj.current_temp < 70)
    the_color = TFT_GREEN;
  else if ((temp_obj.current_temp >= 70) && (temp_obj.current_temp < 80))
    the_color = TFT_YELLOW;
  else if ((temp_obj.current_temp >= 80) && (temp_obj.current_temp < 90))
    the_color = TFT_ORANGE;
  else if ((temp_obj.current_temp >= 90) && (temp_obj.current_temp < 100))
    the_color = TFT_RED;
  else
    the_color = TFT_MAROON;

  display_obj.tft.setTextColor(the_color, STATUSBAR_COLOR);
  temp_obj.old_temp = temp_obj.current_temp;
  display_obj.tft.fillRect(0, 0, 50, STATUS_BAR_WIDTH, STATUSBAR_COLOR);
  display_obj.tft.drawString((String)temp_obj.current_temp + " C", 4, 0, 2);
  display_obj.tft.setTextColor(TFT_WHITE, STATUSBAR_COLOR);


  // WiFi Channel Stuff
  wifi_scan_obj.old_channel = wifi_scan_obj.set_channel;
  display_obj.tft.fillRect(50, 0, 50, STATUS_BAR_WIDTH, STATUSBAR_COLOR);
  display_obj.tft.drawString("CH: " + (String)wifi_scan_obj.set_channel, 50, 0, 2);

  // RAM Stuff
  wifi_scan_obj.freeRAM();
  wifi_scan_obj.old_free_ram = wifi_scan_obj.free_ram;
  display_obj.tft.fillRect(100, 0, 60, STATUS_BAR_WIDTH, STATUSBAR_COLOR);
  display_obj.tft.drawString((String)wifi_scan_obj.free_ram + "B", 100, 0, 2);


  MenuFunctions::battery2(true);

  // Draw SD info
  if (sd_obj.supported)
    the_color = TFT_GREEN;
  else
    the_color = TFT_RED;

  display_obj.tft.drawXBitmap(170,
                              0,
                              menu_icons[STATUS_SD],
                              16,
                              16,
                              STATUSBAR_COLOR,
                              the_color);
  //display_obj.tft.print((String)battery_obj.battery_level + "%");
}

void MenuFunctions::orientDisplay()
{
  display_obj.tft.init();

  display_obj.tft.setRotation(0); // Portrait

  display_obj.tft.setCursor(0, 0);

  //uint16_t calData[5] = { 275, 3494, 361, 3528, 4 }; // tft.setRotation(0); // Portrait
  //uint16_t calData[5] = { 339, 3470, 237, 3438, 2 }; // tft.setRotation(0); // Portrait with DIY TFT

#ifdef TFT_SHIELD
  uint16_t calData[5] = { 275, 3494, 361, 3528, 4 }; // tft.setRotation(0); // Portrait with TFT Shield
  Serial.println("Using TFT Shield");
#else if defined(TFT_DIY)
  uint16_t calData[5] = { 339, 3470, 237, 3438, 2 }; // tft.setRotation(0); // Portrait with DIY TFT
  Serial.println("Using TFT DIY");
#endif

  display_obj.tft.setTouch(calData);

  //display_obj.clearScreen();

  changeMenu(current_menu);
}


// Function to build the menus
void MenuFunctions::RunSetup()
{
  // root menu stuff
  mainMenu.list = new LinkedList<MenuNode>(); // Get list in first menu ready

  // Main menu stuff
  wifiMenu.list = new LinkedList<MenuNode>(); // Get list in second menu ready
  bluetoothMenu.list = new LinkedList<MenuNode>(); // Get list in third menu ready
  generalMenu.list = new LinkedList<MenuNode>();
  deviceMenu.list = new LinkedList<MenuNode>();

  // Device menu stuff
  failedUpdateMenu.list = new LinkedList<MenuNode>();
  whichUpdateMenu.list = new LinkedList<MenuNode>();
  confirmMenu.list = new LinkedList<MenuNode>();
  updateMenu.list = new LinkedList<MenuNode>();
  infoMenu.list = new LinkedList<MenuNode>();

  // WiFi menu stuff
  wifiSnifferMenu.list = new LinkedList<MenuNode>();
  wifiScannerMenu.list = new LinkedList<MenuNode>();
  wifiAttackMenu.list = new LinkedList<MenuNode>();

  // Bluetooth menu stuff
  bluetoothSnifferMenu.list = new LinkedList<MenuNode>();
  bluetoothScannerMenu.list = new LinkedList<MenuNode>();

  // Work menu names
  mainMenu.name = " ESP32 Marauder ";
  wifiMenu.name = " WiFi ";
  deviceMenu.name = " Device ";
  generalMenu.name = " General Apps ";
  failedUpdateMenu.name = " Updating... ";
  whichUpdateMenu.name = "Select Method ";
  confirmMenu.name = " Confirm Update ";
  updateMenu.name = " Update Firmware ";
  infoMenu.name = " Device Info ";
  bluetoothMenu.name = " Bluetooth ";
  wifiSnifferMenu.name = " WiFi Sniffers ";
  wifiScannerMenu.name = " WiFi Scanners";
  wifiAttackMenu.name = " WiFi Attacks ";
  bluetoothSnifferMenu.name = " Bluetooth Sniffers ";
  bluetoothScannerMenu.name = " Bluetooth Scanners ";

  // Build Main Menu
  mainMenu.parentMenu = NULL;
  addNodes(&mainMenu, "WiFi", TFT_GREEN, NULL, WIFI, [this]() {
    changeMenu(&wifiMenu);
  });
  addNodes(&mainMenu, "Bluetooth", TFT_CYAN, NULL, BLUETOOTH, [this]() {
    changeMenu(&bluetoothMenu);
  });
  addNodes(&mainMenu, "General Apps", TFT_MAGENTA, NULL, GENERAL_APPS, [this]() {
    changeMenu(&generalMenu);
  });
  addNodes(&mainMenu, "Device", TFT_BLUE, NULL, DEVICE, [this]() {
    changeMenu(&deviceMenu);
  });
  addNodes(&mainMenu, "Reboot", TFT_LIGHTGREY, NULL, REBOOT, []() {
    ESP.restart();
  });

  // Build WiFi Menu
  wifiMenu.parentMenu = &mainMenu; // Main Menu is second menu parent
  addNodes(&wifiMenu, "Back", TFT_LIGHTGREY, NULL, 0, [this]() {
    changeMenu(wifiMenu.parentMenu);
  });
  addNodes(&wifiMenu, "Sniffers", TFT_YELLOW, NULL, SNIFFERS, [this]() {
    changeMenu(&wifiSnifferMenu);
  });
  addNodes(&wifiMenu, "Scanners", TFT_ORANGE, NULL, SCANNERS, [this]() {
    changeMenu(&wifiScannerMenu);
  });
  addNodes(&wifiMenu, "Attacks", TFT_RED, NULL, ATTACKS, [this]() {
    changeMenu(&wifiAttackMenu);
  });

  // Build WiFi sniffer Menu
  wifiSnifferMenu.parentMenu = &wifiMenu; // Main Menu is second menu parent
  addNodes(&wifiSnifferMenu, "Back", TFT_LIGHTGREY, NULL, 0, [this]() {
    changeMenu(wifiSnifferMenu.parentMenu);
  });
  addNodes(&wifiSnifferMenu, "Probe Request Sniff", TFT_CYAN, NULL, PROBE_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_PROBE, TFT_CYAN);
  });
  addNodes(&wifiSnifferMenu, "Beacon Sniff", TFT_MAGENTA, NULL, BEACON_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_AP, TFT_MAGENTA);
  });
  addNodes(&wifiSnifferMenu, "Deauth Sniff", TFT_RED, NULL, DEAUTH_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_DEAUTH, TFT_RED);
  });

  // Build WiFi scanner Menu
  wifiScannerMenu.parentMenu = &wifiMenu; // Main Menu is second menu parent
  addNodes(&wifiScannerMenu, "Back", TFT_LIGHTGREY, NULL, 0, [this]() {
    changeMenu(wifiScannerMenu.parentMenu);
  });
  addNodes(&wifiScannerMenu, "Packet Monitor", TFT_BLUE, NULL, PACKET_MONITOR, [this]() {
    wifi_scan_obj.StartScan(WIFI_PACKET_MONITOR, TFT_BLUE);
  });
  addNodes(&wifiScannerMenu, "EAPOL/PMKID Scan", TFT_VIOLET, NULL, EAPOL, [this]() {
    wifi_scan_obj.StartScan(WIFI_SCAN_EAPOL, TFT_VIOLET);
  });
  addNodes(&wifiScannerMenu, "Detect Pwnagotchi", TFT_RED, NULL, PWNAGOTCHI, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_PWN, TFT_RED);
  });
  addNodes(&wifiScannerMenu, "Detect Espressif", TFT_ORANGE, NULL, ESPRESSIF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_ESPRESSIF, TFT_ORANGE);
  });

  // Build WiFi attack menu
  wifiAttackMenu.parentMenu = &wifiMenu; // Main Menu is second menu parent
  addNodes(&wifiAttackMenu, "Back", TFT_LIGHTGREY, NULL, 0, [this]() {
    changeMenu(wifiAttackMenu.parentMenu);
  });
  addNodes(&wifiAttackMenu, "Beacon Spam Random", TFT_ORANGE, NULL, BEACON_SPAM, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_ATTACK_BEACON_SPAM, TFT_ORANGE);
  });
  addNodes(&wifiAttackMenu, "Rick Roll Beacon", TFT_YELLOW, NULL, RICK_ROLL, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_ATTACK_RICK_ROLL, TFT_YELLOW);
  });

  // Build Bluetooth Menu
  bluetoothMenu.parentMenu = &mainMenu; // Second Menu is third menu parent
  addNodes(&bluetoothMenu, "Back", TFT_LIGHTGREY, NULL, 0, [this]() {
    changeMenu(bluetoothMenu.parentMenu);
  });
  addNodes(&bluetoothMenu, "Sniffers", TFT_YELLOW, NULL, SNIFFERS, [this]() {
    changeMenu(&bluetoothSnifferMenu);
  });
  addNodes(&bluetoothMenu, "Scanners", TFT_ORANGE, NULL, SCANNERS, [this]() {
    changeMenu(&bluetoothScannerMenu);
  });

  // Build bluetooth sniffer Menu
  bluetoothSnifferMenu.parentMenu = &bluetoothMenu; // Second Menu is third menu parent
  addNodes(&bluetoothSnifferMenu, "Back", TFT_LIGHTGREY, NULL, 0, [this]() {
    changeMenu(bluetoothSnifferMenu.parentMenu);
  });
  addNodes(&bluetoothSnifferMenu, "Bluetooth Sniffer", TFT_GREEN, NULL, BLUETOOTH_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(BT_SCAN_ALL, TFT_GREEN);
  });

  // Build bluetooth scanner Menu
  bluetoothScannerMenu.parentMenu = &bluetoothMenu; // Second Menu is third menu parent
  addNodes(&bluetoothScannerMenu, "Back", TFT_LIGHTGREY, NULL, 0, [this]() {
    changeMenu(bluetoothScannerMenu.parentMenu);
  });
  addNodes(&bluetoothScannerMenu, "Detect Card Skimmers", TFT_MAGENTA, NULL, CC_SKIMMERS, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(BT_SCAN_SKIMMERS, TFT_MAGENTA);
  });

  // General apps menu
  generalMenu.parentMenu = &mainMenu;
  addNodes(&generalMenu, "Back", TFT_LIGHTGREY, NULL, 0, [this]() {
    display_obj.draw_tft = false;
    changeMenu(generalMenu.parentMenu);
  });
  addNodes(&generalMenu, "Draw", TFT_WHITE, NULL, DRAW, [this]() {
    display_obj.clearScreen();
    display_obj.setupDraw();
    display_obj.draw_tft = true;
  });

  // Device menu
  deviceMenu.parentMenu = &mainMenu;
  addNodes(&deviceMenu, "Back", TFT_LIGHTGREY, NULL, 0, [this]() {
    changeMenu(deviceMenu.parentMenu);
  });
  //addNodes(&deviceMenu, "Update Firmware", TFT_ORANGE, NULL, UPDATE, [this](){wifi_scan_obj.currentScanMode = OTA_UPDATE; changeMenu(&updateMenu); web_obj.setupOTAupdate();});
  addNodes(&deviceMenu, "Update Firmware", TFT_ORANGE, NULL, UPDATE, [this]() {
    wifi_scan_obj.currentScanMode = OTA_UPDATE;
    changeMenu(&whichUpdateMenu);
  });
  addNodes(&deviceMenu, "Device Info", TFT_WHITE, NULL, DEVICE_INFO, [this]() {
    wifi_scan_obj.currentScanMode = SHOW_INFO;
    changeMenu(&infoMenu);
    wifi_scan_obj.RunInfo();
  });
  //addNodes(&deviceMenu, "Join WiFi", TFT_YELLOW, NULL, SNIFFERS, [this](){display_obj.clearScreen(); wifi_scan_obj.currentScanMode = LV_JOIN_WIFI; wifi_scan_obj.StartScan(LV_JOIN_WIFI, TFT_YELLOW);});

  // Select update
  whichUpdateMenu.parentMenu = &deviceMenu;
  addNodes(&whichUpdateMenu, "Back", TFT_LIGHTGREY, NULL, 0, [this]() {
    changeMenu(whichUpdateMenu.parentMenu);
  });
  addNodes(&whichUpdateMenu, "Web Update", TFT_GREEN, NULL, WEB_UPDATE, [this]() {
    wifi_scan_obj.currentScanMode = OTA_UPDATE;
    changeMenu(&updateMenu);
    web_obj.setupOTAupdate();
  });
  if (sd_obj.supported) addNodes(&whichUpdateMenu, "SD Update", TFT_MAGENTA, NULL, SD_UPDATE, [this]() {
    wifi_scan_obj.currentScanMode = OTA_UPDATE;
    changeMenu(&confirmMenu);
  });

  // Confirm SD update menu
  confirmMenu.parentMenu = &whichUpdateMenu;
  addNodes(&confirmMenu, "Back", TFT_LIGHTGREY, NULL, 0, [this]() {
    changeMenu(confirmMenu.parentMenu);
  });
  //addNodes(&confirmMenu, "Yes", TFT_ORANGE, NULL, UPDATE, [this](){wifi_scan_obj.currentScanMode = OTA_UPDATE; changeMenu(&updateMenu); sd_obj.runUpdate();});
  addNodes(&confirmMenu, "Yes", TFT_ORANGE, NULL, UPDATE, [this]() {
    wifi_scan_obj.currentScanMode = OTA_UPDATE;
    changeMenu(&failedUpdateMenu);
    sd_obj.runUpdate();
  });

  // Web Update
  updateMenu.parentMenu = &deviceMenu;
  addNodes(&updateMenu, "Back", TFT_LIGHTGREY, NULL, 0, [this]() {
    wifi_scan_obj.currentScanMode = WIFI_SCAN_OFF;
    changeMenu(updateMenu.parentMenu);
    WiFi.softAPdisconnect(true);
    web_obj.shutdownServer();
  });
  addNodes(&updateMenu, "Back", TFT_LIGHTGREY, NULL, 0, [this](){wifi_scan_obj.currentScanMode = WIFI_SCAN_OFF; changeMenu(updateMenu.parentMenu);});

  // Failed update menu
  failedUpdateMenu.parentMenu = &whichUpdateMenu;
  addNodes(&failedUpdateMenu, "Back", TFT_LIGHTGREY, NULL, 0, [this]() {
    wifi_scan_obj.currentScanMode = WIFI_SCAN_OFF;
    changeMenu(failedUpdateMenu.parentMenu);
  });

  // Device info menu
  infoMenu.parentMenu = &deviceMenu;
  addNodes(&infoMenu, "Back", TFT_LIGHTGREY, NULL, 0, [this]() {
    wifi_scan_obj.currentScanMode = WIFI_SCAN_OFF;
    changeMenu(infoMenu.parentMenu);
  });

  // Set the current menu to the mainMenu
  changeMenu(&mainMenu);

  this->initTime = millis();
}

// Function to change menu
void MenuFunctions::changeMenu(Menu * menu)
{
  display_obj.initScrollValues();
  display_obj.setupScrollArea(TOP_FIXED_AREA, BOT_FIXED_AREA);
  display_obj.tft.init();
  current_menu = menu;

  buildButtons(menu);

  displayCurrentMenu();
}

// Function to show all MenuNodes in a Menu
void MenuFunctions::showMenuList(Menu * menu, int layer)
{
  // Iterate through all of the menu nodes in the menu
  for (int i = 0; i < menu->list->size(); i++)
  {
    // Depending on layer, indent
    for (int x = 0; x < layer * 4; x++)
      Serial.print(" ");
    Serial.print("Node: ");
    Serial.println(menu->list->get(i).name);

    // If the current menu node points to another menu, list that menu
    //if (menu->list->get(i).childMenu != NULL)
    //  showMenuList(menu->list->get(i).childMenu, layer+1);
  }
  Serial.println();
}


// Function to add MenuNodes to a menu
void MenuFunctions::addNodes(Menu * menu, String name, uint16_t color, Menu * child, int place, std::function<void()> callable)
{
  TFT_eSPI_Button new_button;
  menu->list->add(MenuNode{name, color, place, &new_button, callable});
  //strcpy(menu->list->get(-1).icon, bluetooth_icon);
}

void MenuFunctions::buildButtons(Menu * menu)
{
  Serial.println("Bulding buttons...");
  if (menu->list != NULL)
  {
    //for (int i = 0; i < sizeof(key); i++)
    //  key[i] = NULL;
    for (int i = 0; i < menu->list->size(); i++)
    {
      TFT_eSPI_Button new_button;
      char buf[menu->list->get(i).name.length() + 1] = {};
      menu->list->get(i).name.toCharArray(buf, menu->list->get(i).name.length() + 1);
      display_obj.key[i].initButton(&display_obj.tft,
                                    KEY_X + 0 * (KEY_W + KEY_SPACING_X),
                                    KEY_Y + i * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                                    KEY_W,
                                    KEY_H,
                                    TFT_BLACK, // Outline
                                    TFT_BLACK, // Fill
                                    menu->list->get(i).color, // Text
                                    buf,
                                    KEY_TEXTSIZE);

      display_obj.key[i].setLabelDatum(BUTTON_PADDING - (KEY_W / 2), 2, ML_DATUM);
    }
  }
}


void MenuFunctions::displayCurrentMenu()
{
  Serial.println("Displaying current menu...");
  display_obj.clearScreen();
  display_obj.tft.setTextColor(TFT_LIGHTGREY, TFT_DARKGREY);
  this->drawStatusBar();
  //display_obj.tft.fillRect(0,0,240,16, TFT_DARKGREY);
  //display_obj.tft.drawCentreString(" ESP32 Marauder ",120,0,2);
  //Serial.println("Getting size...");
  //char buf[&current_menu->parentMenu->name.length() + 1] = {};
  //Serial.println("Got size...");
  //current_menu->parentMenu->name.toCharArray(buf, current_menu->parentMenu->name.length() + 1);
  //String current_name = &current_menu->parentMenu->name;
  //Serial.println("gottem");
  //display_obj.tft.drawCentreString(current_menu->name,120,0,2);
  if (current_menu->list != NULL)
  {
    display_obj.tft.setFreeFont(MENU_FONT);
    for (int i = 0; i < current_menu->list->size(); i++)
    {
      //display_obj.key[i].drawButton2(current_menu->list->get(i).name);
      //display_obj.key[i].drawButton(ML_DATUM, BUTTON_PADDING, current_menu->list->get(i).name);
      //display_obj.key[i].drawButton(true);
      display_obj.key[i].drawButton(false, current_menu->list->get(i).name);

      if (current_menu->list->get(i).name != "Back")
        display_obj.tft.drawXBitmap(0,
                                    KEY_Y + i * (KEY_H + KEY_SPACING_Y) - (ICON_H / 2),
                                    menu_icons[current_menu->list->get(i).icon],
                                    ICON_W,
                                    ICON_H,
                                    TFT_BLACK,
                                    current_menu->list->get(i).color);
    }
    display_obj.tft.setFreeFont(NULL);
  }
}
