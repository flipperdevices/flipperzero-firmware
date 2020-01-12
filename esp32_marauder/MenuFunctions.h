#ifndef MenuFunctions_h
#define MenuFunctions_h

#include "WiFiScan.h"
#include "Display.h"

extern Display display_obj;
extern WiFiScan wifi_scan_obj;

// Keypad start position, key sizes and spacing
#define KEY_X 120 // Centre of key
#define KEY_Y 50
#define KEY_W 240 // Width and height
#define KEY_H 22
#define KEY_SPACING_X 0 // X and Y gap
#define KEY_SPACING_Y 1
#define KEY_TEXTSIZE 1   // Font size multiplier
#define BUTTON_ARRAY_LEN 5

#define FLASH_BUTTON 0


struct Menu;

// Individual Nodes of a menu

struct MenuNode {
    String name;
    uint16_t color;
    Menu *childMenu;
    TFT_eSPI_Button* button;
    std::function<void()> callable; // Make a function that changes menu to a child menu
};

// Full Menus
struct Menu {
    String name;
    SimpleList<MenuNode>* list;
    Menu                * parentMenu;
    uint8_t               selected;
};


class MenuFunctions
{
  private:    
    Menu* current_menu;

    // Main menu stuff
    Menu mainMenu;
    
    Menu wifiMenu;
    Menu bluetoothMenu;
    Menu generalMenu;

    // WiFi menu stuff
    Menu wifiSnifferMenu;
    Menu wifiScannerMenu;
    Menu wifiAttackMenu;

    // Bluetooth menu stuff
    Menu bluetoothSnifferMenu;
    Menu bluetoothScannerMenu;

    TFT_eSPI_Button key[BUTTON_ARRAY_LEN];
    
    void addNodes(Menu* menu, String name, uint16_t color, Menu* child, int place, std::function<void()> callable);
    void showMenuList(Menu* menu, int layer);

  public:    
    MenuFunctions();

    uint16_t x = -1, y = -1;
    boolean pressed = false;

    void buildButtons(Menu* menu);
    void changeMenu(Menu* menu);
    void displayCurrentMenu();
    void main();
    void RunSetup();
};



#endif
