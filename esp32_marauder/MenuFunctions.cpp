#include "MenuFunctions.h"
//#include "icons.h"

PROGMEM const unsigned char menu_icons[][66] = {
    {0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x37, 0xFF, 0xFF, 0x3A, // Attack: 0
    0xFF, 0xFF, 0x39, 0xFF, 0xBF, 0x3C, 0xFF, 0x7F, 0x3E, 0xFF, 0x2F, 0x3F, 
    0xFF, 0x9F, 0x3F, 0xFF, 0xCB, 0x3F, 0xFF, 0xE7, 0x3F, 0xCF, 0xF2, 0x3F, 
    0xDF, 0xF9, 0x3F, 0xBF, 0xFC, 0x3F, 0x3F, 0xFF, 0x3F, 0x7F, 0xFE, 0x3F, 
    0xDF, 0xF9, 0x3F, 0xCF, 0xFB, 0x3F, 0xE7, 0xFF, 0x3F, 0xF3, 0xFF, 0x3F, 
    0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3F},
    {0xBF, 0xDD, 0x3E, 0xDF, 0xBE, 0x3D, 0x5F, 0x6B, 0x3D, 0x5F, 0x5D, 0x3D, // Beacon sniff: 1
    0x5F, 0x55, 0x3D, 0xFF, 0xF7, 0x3F, 0xFF, 0xF7, 0x3F, 0xFF, 0xEB, 0x3F, 
    0xFF, 0xEB, 0x3F, 0xFF, 0xEB, 0x3F, 0xFF, 0xDD, 0x3F, 0xFF, 0xD5, 0x3F, 
    0xFF, 0xC9, 0x3F, 0xFF, 0xDD, 0x3F, 0xFF, 0xC9, 0x3F, 0xFF, 0xB6, 0x3F, 
    0xFF, 0xAA, 0x3F, 0xFF, 0x9C, 0x3F, 0xFF, 0xAA, 0x3F, 0x7F, 0x77, 0x3F, 
    0x7F, 0x49, 0x3F, 0x7F, 0x3E, 0x3F},
    {0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3F, 0xFF, 0xFB, 0x3F, // Bluetooth: 2
    0xFF, 0xF3, 0x3F, 0xFF, 0xEB, 0x3F, 0xFF, 0xDB, 0x3F, 0x7F, 0xBB, 0x3F, 
    0xFF, 0xDA, 0x3F, 0xFF, 0xE9, 0x3F, 0xFF, 0xF3, 0x3F, 0xFF, 0xF3, 0x3F, 
    0xFF, 0xE9, 0x3F, 0xFF, 0xDA, 0x3F, 0x7F, 0xBB, 0x3F, 0xFF, 0xDB, 0x3F, 
    0xFF, 0xEB, 0x3F, 0xFF, 0xF3, 0x3F, 0xFF, 0xFB, 0x3F, 0xFF, 0xFF, 0x3F, 
    0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3F},
    {0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3F, 0xCF, 0xFB, 0x3F, // Bluetooth Sniff: 3
    0xF7, 0xF9, 0x3F, 0xDB, 0xFA, 0x3F, 0x6B, 0xFB, 0x3F, 0xAB, 0xDB, 0x3F, 
    0x6B, 0xEB, 0x3F, 0xDB, 0xF2, 0x3F, 0xF7, 0x79, 0x3E, 0xCF, 0xF3, 0x3D, 
    0xFF, 0x69, 0x3B, 0xFF, 0xDA, 0x3A, 0x7F, 0xBB, 0x3A, 0xFF, 0xDB, 0x3A, 
    0xFF, 0x6B, 0x3B, 0xFF, 0xF3, 0x3D, 0xFF, 0x7B, 0x3E, 0xFF, 0xFF, 0x3F, 
    0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3F},
    {0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3F, 0xFF, 0xE1, 0x3F, 0x7F, 0x80, 0x3F, // Deauth sniff: 4
    0x3F, 0x00, 0x3F, 0x1F, 0x00, 0x3E, 0x1F, 0x00, 0x3E, 0x1F, 0x00, 0x3E, 
    0x1F, 0x00, 0x3E, 0x9F, 0x73, 0x3E, 0xDF, 0xF3, 0x3E, 0x9F, 0x65, 0x3E, 
    0x1F, 0x0C, 0x3E, 0x3F, 0x0C, 0x3F, 0x3F, 0x00, 0x3F, 0x3F, 0x25, 0x3F, 
    0x7F, 0x8C, 0x3F, 0x7F, 0x80, 0x3F, 0xFF, 0xC0, 0x3F, 0xFF, 0xFF, 0x3F, 
    0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3F},
    {0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3D, 0xFF, 0xFF, 0x38, // Draw: 5
    0xFF, 0x7F, 0x3D, 0xFF, 0xBF, 0x3E, 0xFF, 0x5F, 0x3F, 0xFF, 0xAF, 0x3F, 
    0xFF, 0xD7, 0x3F, 0xFF, 0xEB, 0x3F, 0xFF, 0xF5, 0x3F, 0xFF, 0xFA, 0x3F, 
    0x7F, 0xFD, 0x3F, 0xBF, 0xFE, 0x3F, 0x3F, 0xFF, 0x3F, 0xDF, 0xFF, 0x3F, 
    0x1F, 0x00, 0x00, 0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3F, 
    0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3F},
    {0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3F, 0xFF, 0xBF, 0x3F, // Packet monitor: 6
    0xFF, 0xBF, 0x3F, 0xFF, 0xBF, 0x3F, 0xFF, 0xBF, 0x3F, 0xFF, 0x5E, 0x3F, 
    0xFF, 0x5E, 0x3F, 0x7F, 0x5D, 0x3F, 0x20, 0xED, 0x00, 0xAF, 0xEB, 0x3F, 
    0xDF, 0xEB, 0x3F, 0xDF, 0xEB, 0x3F, 0xFF, 0xF7, 0x3F, 0xFF, 0xF7, 0x3F, 
    0xFF, 0xF7, 0x3F, 0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3F, 
    0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3F},
    {0xFF, 0xDB, 0x3F, 0xFF, 0xED, 0x3F, 0xFF, 0xB5, 0x3F, 0xFF, 0xD5, 0x3F, // Probe sniff: 7
    0xFF, 0x55, 0x3E, 0xFF, 0xF5, 0x3D, 0xFF, 0xED, 0x3D, 0xFF, 0x7B, 0x3E, 
    0xFF, 0xBF, 0x3F, 0xFF, 0xCF, 0x3F, 0xFF, 0xD7, 0x3F, 0xFF, 0xEB, 0x3F, 
    0xFF, 0xF5, 0x3F, 0xFF, 0xF8, 0x3F, 0x7F, 0xFC, 0x3F, 0x3F, 0xFE, 0x3F, 
    0x1F, 0xFF, 0x3F, 0x8F, 0xFF, 0x3F, 0xCF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3F, 
    0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3F},
    {0xFF, 0xFF, 0x3F, 0xFF, 0xC0, 0x3F, 0x3F, 0x3F, 0x3F, 0xDF, 0xFF, 0x3E, // Scanners: 8
    0xEF, 0xEF, 0x3D, 0xF7, 0xFF, 0x39, 0xFB, 0xE1, 0x36, 0xDB, 0x5E, 0x37, 
    0x7D, 0xBF, 0x2F, 0xBD, 0x5E, 0x2F, 0xBD, 0x63, 0x2F, 0xBD, 0x73, 0x2F, 
    0xBD, 0x7F, 0x2F, 0x7D, 0xBF, 0x2F, 0xFB, 0xDE, 0x37, 0xFB, 0xE1, 0x37, 
    0xF7, 0xFF, 0x3B, 0xEF, 0xFF, 0x3D, 0xDF, 0xFF, 0x3E, 0x3F, 0x3F, 0x3F, 
    0xFF, 0xC0, 0x3F, 0xFF, 0xFF, 0x3F},
    {0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3F, 0x1F, 0x60, 0x3F, 0x0F, 0xC0, 0x3E, // CC Skimmers: 9
    0xEF, 0xDF, 0x3C, 0xEF, 0xDF, 0x38, 0xEF, 0xDF, 0x3A, 0xEF, 0xDF, 0x39, 
    0xEF, 0xDF, 0x3B, 0xEF, 0xDF, 0x3B, 0x0F, 0xC0, 0x3B, 0x0F, 0x80, 0x3B, 
    0x0F, 0x40, 0x3B, 0x0F, 0x40, 0x3B, 0x0F, 0x40, 0x3B, 0x0F, 0x40, 0x3B, 
    0x0F, 0xC0, 0x3C, 0x0F, 0xC0, 0x3F, 0x07, 0x80, 0x3F, 0xFF, 0xFF, 0x3F, 
    0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3F},
    {0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3F, 0xFF, 0xFB, 0x3F, 0xFF, 0xFB, 0x3F, // Sniffers: 10
    0xFF, 0xF3, 0x3F, 0xFF, 0xE7, 0x3F, 0xFF, 0xCF, 0x3F, 0xFF, 0x3F, 0x3F, 
    0xFF, 0x7F, 0x3E, 0xFF, 0xFF, 0x3C, 0xFF, 0xFF, 0x3D, 0xFF, 0xFF, 0x39, 
    0xFF, 0xFF, 0x3B, 0x3F, 0xF8, 0x3B, 0x9F, 0xFB, 0x3B, 0xDF, 0xFF, 0x39, 
    0x5F, 0xFC, 0x3C, 0x3F, 0x03, 0x3E, 0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3F, 
    0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3F},
    {0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3F, // WiFi: 11
    0x7F, 0x80, 0x3F, 0x9F, 0x7F, 0x3E, 0xE7, 0xFF, 0x39, 0xFB, 0xFF, 0x37, 
    0xFF, 0xC0, 0x3F, 0x1F, 0x3F, 0x3E, 0xEF, 0xFF, 0x3D, 0xFF, 0xFF, 0x3F, 
    0xFF, 0xE1, 0x3F, 0x7F, 0x9E, 0x3F, 0xBF, 0x7F, 0x3F, 0xFF, 0xFF, 0x3F, 
    0xFF, 0xF3, 0x3F, 0xFF, 0xF3, 0x3F, 0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3F, 
    0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0x3F}};

MenuFunctions::MenuFunctions()
{
}

// Function to check menu input
void MenuFunctions::main()
{
  if (wifi_scan_obj.currentScanMode == WIFI_SCAN_OFF) {
    if (wifi_scan_obj.orient_display) {
      this->orientDisplay();
      wifi_scan_obj.orient_display = false;
    }
    display_obj.updateBanner(current_menu->name);
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


  // Pressed will be set true is there is a valid touch on the screen
  int pre_getTouch = millis();

  // getTouch causes a 10ms delay which makes beacon spam less effective
  //if (wifi_scan_obj.currentScanMode == WIFI_SCAN_OFF)
  pressed = display_obj.tft.getTouch(&t_x, &t_y);
  //boolean pressed = false;

  //Serial.print("getTouch: ");
  //Serial.print(millis() - pre_getTouch);
  //Serial.println("ms");

  
  // This is if there are scans/attacks going on
  if ((wifi_scan_obj.currentScanMode != WIFI_SCAN_OFF) && (pressed))
  {  
    // Stop the current scan
    if ((wifi_scan_obj.currentScanMode == WIFI_SCAN_PROBE) ||
    (wifi_scan_obj.currentScanMode == WIFI_SCAN_AP) ||
    (wifi_scan_obj.currentScanMode == WIFI_SCAN_ST) ||
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
        display_obj.key[b].drawButton2(current_menu->list->get(b).name, true);  // draw invert
      }
  
      // If button was just release, execute the button's function
      if (display_obj.key[b].justReleased())
      {
        display_obj.key[b].drawButton2(current_menu->list->get(b).name);     // draw normal
        current_menu->list->get(b).callable();
      }
      display_obj.tft.setFreeFont(NULL);
    }
  }
  x = -1;
  y = -1;
}

void MenuFunctions::orientDisplay()
{
  display_obj.tft.init();
  
  display_obj.tft.setRotation(0); // Portrait

  display_obj.tft.setCursor(0, 0);

  uint16_t calData[5] = { 275, 3494, 361, 3528, 4 }; // tft.setRotation(0); // Portrait
  
  display_obj.tft.setTouch(calData);

  //display_obj.clearScreen();

  changeMenu(current_menu);
}


// Function to build the menus
void MenuFunctions::RunSetup()
{
  // root menu stuff
  mainMenu.list = new SimpleList<MenuNode>(); // Get list in first menu ready

  // Main menu stuff
  wifiMenu.list = new SimpleList<MenuNode>(); // Get list in second menu ready
  bluetoothMenu.list = new SimpleList<MenuNode>(); // Get list in third menu ready
  generalMenu.list = new SimpleList<MenuNode>();

  // WiFi menu stuff
  wifiSnifferMenu.list = new SimpleList<MenuNode>();
  wifiScannerMenu.list = new SimpleList<MenuNode>();
  wifiAttackMenu.list = new SimpleList<MenuNode>();

  // Bluetooth menu stuff
  bluetoothSnifferMenu.list = new SimpleList<MenuNode>();
  bluetoothScannerMenu.list = new SimpleList<MenuNode>();

  // Work menu names
  mainMenu.name = " ESP32 Marauder ";
  wifiMenu.name = " WiFi ";
  generalMenu.name = " General Apps ";
  bluetoothMenu.name = " Bluetooth ";
  wifiSnifferMenu.name = " WiFi Sniffers ";
  wifiScannerMenu.name = " WiFi Scanners";
  wifiAttackMenu.name = " WiFi Attacks ";
  bluetoothSnifferMenu.name = " Bluetooth Sniffers ";
  bluetoothScannerMenu.name = " Bluetooth Scanners ";

  // Build Main Menu
  mainMenu.parentMenu = NULL;
  addNodes(&mainMenu, "WiFi", TFT_GREEN, NULL, WIFI, [this](){changeMenu(&wifiMenu);});
  addNodes(&mainMenu, "Bluetooth", TFT_CYAN, NULL, BLUETOOTH, [this](){changeMenu(&bluetoothMenu);});
  addNodes(&mainMenu, "General Apps", TFT_MAGENTA, NULL, ATTACKS, [this](){changeMenu(&generalMenu);});
  addNodes(&mainMenu, "Reboot", TFT_LIGHTGREY, NULL, SCANNERS, [](){ESP.restart();});

  // Build WiFi Menu
  wifiMenu.parentMenu = &mainMenu; // Main Menu is second menu parent
  addNodes(&wifiMenu, "Back", TFT_LIGHTGREY, NULL, 0, [this](){changeMenu(wifiMenu.parentMenu);});
  addNodes(&wifiMenu, "Sniffers", TFT_YELLOW, NULL, SNIFFERS, [this](){changeMenu(&wifiSnifferMenu);});
  addNodes(&wifiMenu, "Scanners", TFT_ORANGE, NULL, SCANNERS, [this](){changeMenu(&wifiScannerMenu);});
  addNodes(&wifiMenu, "Attacks", TFT_RED, NULL, ATTACKS, [this](){changeMenu(&wifiAttackMenu);});

  // Build WiFi sniffer Menu
  wifiSnifferMenu.parentMenu = &wifiMenu; // Main Menu is second menu parent
  addNodes(&wifiSnifferMenu, "Back", TFT_LIGHTGREY, NULL, 0, [this](){changeMenu(wifiSnifferMenu.parentMenu);});
  addNodes(&wifiSnifferMenu, "Probe Request Sniff", TFT_CYAN, NULL, PROBE_SNIFF, [this](){wifi_scan_obj.StartScan(WIFI_SCAN_PROBE, TFT_CYAN);});
  addNodes(&wifiSnifferMenu, "Beacon Sniff", TFT_MAGENTA, NULL, BEACON_SNIFF, [this](){wifi_scan_obj.StartScan(WIFI_SCAN_AP, TFT_MAGENTA);});
  addNodes(&wifiSnifferMenu, "Deauth Sniff", TFT_RED, NULL, DEAUTH_SNIFF, [this](){wifi_scan_obj.StartScan(WIFI_SCAN_DEAUTH, TFT_RED);});

  // Build WiFi scanner Menu
  wifiScannerMenu.parentMenu = &wifiMenu; // Main Menu is second menu parent
  addNodes(&wifiScannerMenu, "Back", TFT_LIGHTGREY, NULL, 0, [this](){changeMenu(wifiScannerMenu.parentMenu);});
  addNodes(&wifiScannerMenu, "Packet Monitor", TFT_BLUE, NULL, PACKET_MONITOR, [this](){wifi_scan_obj.StartScan(WIFI_PACKET_MONITOR, TFT_BLUE);});

  // Build WiFi attack menu
  wifiAttackMenu.parentMenu = &wifiMenu; // Main Menu is second menu parent
  addNodes(&wifiAttackMenu, "Back", TFT_LIGHTGREY, NULL, 0, [this](){changeMenu(wifiAttackMenu.parentMenu);});
  addNodes(&wifiAttackMenu, "Beacon Spam Random", TFT_ORANGE, NULL, ATTACKS, [this](){wifi_scan_obj.StartScan(WIFI_ATTACK_BEACON_SPAM, TFT_ORANGE);});
  addNodes(&wifiAttackMenu, "Rick Roll Beacon", TFT_YELLOW, NULL, ATTACKS, [this](){wifi_scan_obj.StartScan(WIFI_ATTACK_RICK_ROLL, TFT_YELLOW);});
  
  // Build Bluetooth Menu
  bluetoothMenu.parentMenu = &mainMenu; // Second Menu is third menu parent
  addNodes(&bluetoothMenu, "Back", TFT_LIGHTGREY, NULL, 0, [this](){changeMenu(bluetoothMenu.parentMenu);});
  addNodes(&bluetoothMenu, "Sniffers", TFT_YELLOW, NULL, SNIFFERS, [this](){changeMenu(&bluetoothSnifferMenu);});
  addNodes(&bluetoothMenu, "Scanners", TFT_ORANGE, NULL, SCANNERS, [this](){changeMenu(&bluetoothScannerMenu);});

  // Build bluetooth sniffer Menu
  bluetoothSnifferMenu.parentMenu = &bluetoothMenu; // Second Menu is third menu parent
  addNodes(&bluetoothSnifferMenu, "Back", TFT_LIGHTGREY, NULL, 0, [this](){changeMenu(bluetoothSnifferMenu.parentMenu);});
  addNodes(&bluetoothSnifferMenu, "Bluetooth Sniffer", TFT_GREEN, NULL, BLUETOOTH_SNIFF, [this](){wifi_scan_obj.StartScan(BT_SCAN_ALL, TFT_GREEN);});

  // Build bluetooth scanner Menu
  bluetoothScannerMenu.parentMenu = &bluetoothMenu; // Second Menu is third menu parent
  addNodes(&bluetoothScannerMenu, "Back", TFT_LIGHTGREY, NULL, 0, [this](){changeMenu(bluetoothScannerMenu.parentMenu);});
  addNodes(&bluetoothScannerMenu, "Detect Card Skimmers", TFT_MAGENTA, NULL, CC_SKIMMERS, [this](){wifi_scan_obj.StartScan(BT_SCAN_SKIMMERS, TFT_MAGENTA);});

  generalMenu.parentMenu = &mainMenu;
  addNodes(&generalMenu, "Back", TFT_LIGHTGREY, NULL, 0, [this](){display_obj.draw_tft = false; changeMenu(generalMenu.parentMenu);});
  addNodes(&generalMenu, "Draw", TFT_WHITE, NULL, DRAW, [this](){display_obj.clearScreen(); display_obj.draw_tft = true;});


  // Set the current menu to the mainMenu
  changeMenu(&mainMenu);
}

// Function to change menu
void MenuFunctions::changeMenu(Menu* menu)
{  
  display_obj.initScrollValues();
  display_obj.setupScrollArea(TOP_FIXED_AREA, BOT_FIXED_AREA);
  display_obj.tft.init();
  current_menu = menu;

  buildButtons(menu);

  displayCurrentMenu();
}

// Function to show all MenuNodes in a Menu
void MenuFunctions::showMenuList(Menu* menu, int layer)
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
void MenuFunctions::addNodes(Menu* menu, String name, uint16_t color, Menu* child, int place, std::function<void()> callable)
{
  TFT_eSPI_Button new_button;
  menu->list->add(MenuNode{name, color, place, &new_button, callable});
  //strcpy(menu->list->get(-1).icon, bluetooth_icon);
}

void MenuFunctions::buildButtons(Menu* menu)
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
    }
  }
}


void MenuFunctions::displayCurrentMenu()
{
  Serial.println("Displaying current menu...");
  display_obj.clearScreen();
  display_obj.tft.setTextColor(TFT_LIGHTGREY, TFT_DARKGREY);
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
      display_obj.key[i].drawButton2(current_menu->list->get(i).name);
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
