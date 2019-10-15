#include "MenuFunctions.h"


MenuFunctions::MenuFunctions()
{
  //Serial.println("Initializing Menu Object...");
}

// Function to check menu input
void MenuFunctions::main()
{
  // This is code from bodmer's keypad example
  uint16_t t_x = 0, t_y = 0; // To store the touch coordinates

  // Get the display buffer out of the way
  if (wifi_scan_obj.currentScanMode != WIFI_SCAN_OFF)
    display_obj.displayBuffer();
  //else
  //  display_obj.displayBuffer(true);

  //Serial.println("Cycle");

  // Pressed will be set true is there is a valid touch on the screen
  //vTaskDelay(10 / portTICK_PERIOD_MS);
  boolean pressed = display_obj.tft.getTouch(&t_x, &t_y);
  //boolean pressed = false;

  
  // This is if there are scans going on
  if ((wifi_scan_obj.currentScanMode != WIFI_SCAN_OFF) && (pressed))
  //if ((wifi_scan_obj.currentScanMode != WIFI_SCAN_OFF) && (x != -1) && (y != -1))
  {  
    // Stop the current scan
    if ((wifi_scan_obj.currentScanMode == WIFI_SCAN_PROBE) ||
    (wifi_scan_obj.currentScanMode == WIFI_SCAN_AP) ||
    (wifi_scan_obj.currentScanMode == WIFI_SCAN_ST) ||
    (wifi_scan_obj.currentScanMode == WIFI_SCAN_ALL) || 
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
  
  // / Check if any key coordinate boxes contain the touch coordinates
  for (uint8_t b = 0; b < BUTTON_ARRAY_LEN; b++) {
    if (pressed && key[b].contains(t_x, t_y)) {
      key[b].press(true);  // tell the button it is pressed
    } else {
      key[b].press(false);  // tell the button it is NOT pressed
    }
  }

  // Check if any key has changed state
  for (uint8_t b = 0; b < BUTTON_ARRAY_LEN; b++) {
    display_obj.tft.setFreeFont(MENU_FONT);
    if (key[b].justPressed()) {
      key[b].drawButton2(current_menu->list->get(b).name, true);  // draw invert
    }

    // If button was just release, execute the button's function
    if (key[b].justReleased())
    {
      key[b].drawButton2(current_menu->list->get(b).name);     // draw normal
      //Serial.print("Executing button -> ");
      //Serial.println(current_menu->list->get(b).name);
      current_menu->list->get(b).callable();
    }
    display_obj.tft.setFreeFont(NULL);
  }

  // This is if there are any scans or sniffs going on
  if(digitalRead(FLASH_BUTTON) == 0)
  {
    // Stop the current scan
    if ((wifi_scan_obj.currentScanMode == WIFI_SCAN_PROBE) ||
    (wifi_scan_obj.currentScanMode == WIFI_SCAN_AP) ||
    (wifi_scan_obj.currentScanMode == WIFI_SCAN_ST) ||
    (wifi_scan_obj.currentScanMode == WIFI_SCAN_ALL) ||
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
  }
  
  //else
  //{
  //  display_obj.displayBuffer();
  //}
  x = -1;
  y = -1;
}


void MenuFunctions::handlePress(boolean pressed, uint16_t t_x, uint16_t t_y)
{
  if ((wifi_scan_obj.currentScanMode != WIFI_SCAN_OFF) && (pressed))
  {  
    // Stop the current scan
    if ((wifi_scan_obj.currentScanMode == WIFI_SCAN_PROBE) ||
    (wifi_scan_obj.currentScanMode == WIFI_SCAN_AP) ||
    (wifi_scan_obj.currentScanMode == WIFI_SCAN_ST) ||
    (wifi_scan_obj.currentScanMode == WIFI_SCAN_ALL) || 
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

    return;
  }
  
  // / Check if any key coordinate boxes contain the touch coordinates
  for (uint8_t b = 0; b < BUTTON_ARRAY_LEN; b++) {
    if (pressed && key[b].contains(t_x, t_y)) {
      key[b].press(true);  // tell the button it is pressed
    } else {
      key[b].press(false);  // tell the button it is NOT pressed
    }
  }

  // Check if any key has changed state
  for (uint8_t b = 0; b < BUTTON_ARRAY_LEN; b++) {
    display_obj.tft.setFreeFont(MENU_FONT);
    if (key[b].justPressed()) {
      key[b].drawButton2(current_menu->list->get(b).name, true);  // draw invert
    }

    // If button was just release, execute the button's function
    if (key[b].justReleased())
    {
      key[b].drawButton2(current_menu->list->get(b).name);     // draw normal
      //Serial.print("Executing button -> ");
      //Serial.println(current_menu->list->get(b).name);
      current_menu->list->get(b).callable();
    }
    display_obj.tft.setFreeFont(NULL);
  }

  // This is if there are any scans or sniffs going on
  if(digitalRead(FLASH_BUTTON) == 0)
  {
    // Stop the current scan
    if ((wifi_scan_obj.currentScanMode == WIFI_SCAN_PROBE) ||
    (wifi_scan_obj.currentScanMode == WIFI_SCAN_AP) ||
    (wifi_scan_obj.currentScanMode == WIFI_SCAN_ST) ||
    (wifi_scan_obj.currentScanMode == WIFI_SCAN_ALL) ||
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
  }
}

// Function to build the menus
void MenuFunctions::RunSetup()
{
  mainMenu.list = new SimpleList<MenuNode>(); // Get list in first menu ready
  wifiMenu.list = new SimpleList<MenuNode>(); // Get list in second menu ready
  bluetoothMenu.list = new SimpleList<MenuNode>(); // Get list in third menu ready

  // Build Main Menu
  mainMenu.parentMenu = NULL;
  addNodes(&mainMenu, "WiFi", TFT_GREEN, NULL, 0, [this](){changeMenu(&wifiMenu);});
  addNodes(&mainMenu, "Bluetooth", TFT_CYAN, NULL, 1, [this](){changeMenu(&bluetoothMenu);});
  addNodes(&mainMenu, "Pee", TFT_MAGENTA, NULL, 2, NULL);

  // Build WiFi Menu
  wifiMenu.parentMenu = &mainMenu; // Main Menu is second menu parent
  addNodes(&wifiMenu, "Back", TFT_RED, NULL, 0, [this](){changeMenu(wifiMenu.parentMenu);});
  addNodes(&wifiMenu, "Probe Request Sniff", TFT_MAGENTA, NULL, 1, [this](){wifi_scan_obj.StartScan(WIFI_SCAN_PROBE);});
  addNodes(&wifiMenu, "Beacon Sniff", TFT_BLUE, NULL, 2, [this](){wifi_scan_obj.StartScan(WIFI_SCAN_AP);});

  // Build Bluetooth Menu
  bluetoothMenu.parentMenu = &mainMenu; // Second Menu is third menu parent
  addNodes(&bluetoothMenu, "Back", TFT_RED, NULL, 0, [this](){changeMenu(bluetoothMenu.parentMenu);});
  addNodes(&bluetoothMenu, "Bluetooth Sniffer", TFT_GREEN, NULL, 1, [this](){wifi_scan_obj.StartScan(BT_SCAN_ALL);});
  addNodes(&bluetoothMenu, "Detect Card Skimmers", TFT_MAGENTA, NULL, 2, [this](){wifi_scan_obj.StartScan(BT_SCAN_SKIMMERS);});


  // Set the current menu to the mainMenu
  changeMenu(&mainMenu);

  // Show the current menu
  //Serial.println("\n\nCurrent Menu Tree:");
  //Serial.println("-------------------------------");
  //showMenuList(current_menu, 0);
  //xTaskCreate(&MenuFunctions::getPresses, "getPresses", 2048, NULL, 5, NULL);
}

// Function to change menu
void MenuFunctions::changeMenu(Menu* menu)
{  
  //display_obj.clearScreen();
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
    if (menu->list->get(i).childMenu != NULL)
      showMenuList(menu->list->get(i).childMenu, layer+1);
  }
  Serial.println();
}


// Function to add MenuNodes to a menu
void MenuFunctions::addNodes(Menu* menu, String name, uint16_t color, Menu* child, int place, std::function<void()> callable)
{
  TFT_eSPI_Button new_button;
  menu->list->add(MenuNode{name, color, child, &new_button, callable});
}

void MenuFunctions::buildButtons(Menu* menu)
{
  Serial.println("Bulding buttons...");
  //display_obj.clearScreen();
  if (menu->list != NULL)
  {
    for (int i = 0; i < menu->list->size(); i++)
    {
      TFT_eSPI_Button new_button;
      //Serial.print("Building button -> ");
      //Serial.println(menu->list->get(i).name);
      char buf[menu->list->get(i).name.length() + 1] = {};
      menu->list->get(i).name.toCharArray(buf, menu->list->get(i).name.length() + 1);
      //for (int x = 0; x < sizeof(buf); x++)
      //  Serial.print(buf[x]);
      //Serial.println();
      key[i].initButton(&display_obj.tft,
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
  display_obj.tft.fillRect(0,0,240,16, TFT_DARKGREY);
  display_obj.tft.drawCentreString(" ESP32 Marauder ",120,0,2);
  if (current_menu->list != NULL)
  {
    display_obj.tft.setFreeFont(MENU_FONT);
    for (int i = 0; i < current_menu->list->size(); i++)
    {
      key[i].drawButton2(current_menu->list->get(i).name);
    }
    display_obj.tft.setFreeFont(NULL);
  }
}
