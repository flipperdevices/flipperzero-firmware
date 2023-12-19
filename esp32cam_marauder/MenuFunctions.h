#pragma once

#ifndef MenuFunctions_h
#define MenuFunctions_h

#include "configs.h"

#ifdef HAS_SCREEN

#define BATTERY_ANALOG_ON 0

#include "WiFiScan.h"
#include "BatteryInterface.h"
#include "SDInterface.h"
#include "settings.h"

#ifdef HAS_BUTTONS
  #include <SwitchLib.h>
  extern SwitchLib u_btn;
  extern SwitchLib d_btn;
  extern SwitchLib l_btn;
  extern SwitchLib r_btn;
  extern SwitchLib c_btn;
#endif

extern WiFiScan wifi_scan_obj;
extern SDInterface sd_obj;
extern BatteryInterface battery_obj;
extern Settings settings_obj;

#define FLASH_BUTTON 0

#if BATTERY_ANALOG_ON == 1
#define BATTERY_PIN 13
#define ANALOG_PIN 34
#define CHARGING_PIN 27
#endif

// Icon definitions
#define ATTACKS 0
#define BEACON_SNIFF 1
#define BLUETOOTH 2
#define BLUETOOTH_SNIFF 3
#define DEAUTH_SNIFF 4
#define DRAW 5
#define PACKET_MONITOR 6
#define PROBE_SNIFF 7
#define SCANNERS 8
#define CC_SKIMMERS 9
#define SNIFFERS 10
#define WIFI 11
#define BEACON_SPAM 12
#define RICK_ROLL 13
#define REBOOT 14
#define GENERAL_APPS 15
#define UPDATE 16
#define DEVICE 17
#define DEVICE_INFO 18
#define SD_UPDATE 19
#define WEB_UPDATE 20
#define EAPOL 21
#define STATUS_BAT 22
#define STATUS_SD 23
#define PWNAGOTCHI 24
#define SHUTDOWN 25
#define BEACON_LIST 26
#define GENERATE 27
#define CLEAR_ICO 28
#define KEYBOARD_ICO 29
#define JOIN_WIFI 30
#define LANGUAGE 31
#define STATUS_GPS 32
#define GPS_MENU 33

PROGMEM void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
PROGMEM bool my_touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data);

PROGMEM static lv_disp_buf_t disp_buf;
PROGMEM static lv_color_t buf[LV_HOR_RES_MAX * 10];

PROGMEM static void ta_event_cb(lv_obj_t * ta, lv_event_t event);
PROGMEM static void add_ssid_keyboard_event_cb(lv_obj_t * keyboard, lv_event_t event);
PROGMEM static void html_list_cb(lv_obj_t * btn, lv_event_t event);
PROGMEM static void ap_list_cb(lv_obj_t * btn, lv_event_t event);
PROGMEM static void station_list_cb(lv_obj_t * btn, lv_event_t event);
PROGMEM static void setting_dropdown_cb(lv_obj_t * btn, lv_event_t event);

// lvgl stuff
PROGMEM static lv_obj_t *kb;
PROGMEM static lv_obj_t * save_as_kb;

struct Menu;

// Individual Nodes of a menu

struct MenuNode {
  String name;
  bool command;
  uint16_t color;
  uint8_t icon;
  TFT_eSPI_Button* button;
  bool selected;
  std::function<void()> callable;
};

// Full Menus
struct Menu {
  String name;
  LinkedList<MenuNode>* list;
  Menu                * parentMenu;
  uint8_t               selected = 0;
};


class MenuFunctions
{
  private:

    String u_result = "";

    uint32_t initTime = 0;
    uint8_t menu_start_index = 0;


    // Main menu stuff
    Menu mainMenu;

    Menu wifiMenu;
    Menu bluetoothMenu;
    Menu badusbMenu;
    Menu deviceMenu;

    // Device menu stuff
    Menu whichUpdateMenu;
    Menu failedUpdateMenu;
    Menu confirmMenu;
    Menu updateMenu;
    Menu settingsMenu;
    Menu specSettingMenu;
    Menu infoMenu;
    Menu languageMenu;

    // WiFi menu stuff
    Menu wifiSnifferMenu;
    Menu wifiAttackMenu;
    Menu wifiGeneralMenu;
    Menu wifiAPMenu;

    // WiFi General Menu
    Menu htmlMenu;

    // Bluetooth menu stuff
    Menu bluetoothSnifferMenu;
    Menu bluetoothAttackMenu;

    // Settings things menus
    Menu generateSSIDsMenu;

    static void lv_tick_handler();

    // Menu icons



    void addNodes(Menu* menu, String name, uint16_t color, Menu* child, int place, std::function<void()> callable, bool selected = false, String command = "");
    void updateStatusBar();
    void battery(bool initial = false);
    void battery2(bool initial = false);
    void showMenuList(Menu* menu, int layer);
    String callSetting(String key);
    void runBoolSetting(String ley);
    void displaySetting(String key, Menu* menu, int index);
    void buttonSelected(uint8_t b, int8_t x = -1);
    void buttonNotSelected(uint8_t b, int8_t x = -1);

    uint8_t updateTouch(uint16_t *x, uint16_t *y, uint16_t threshold = 600);

  public:
    MenuFunctions();

    Menu* current_menu;
    Menu clearSSIDsMenu;
    Menu clearAPsMenu;

    #ifdef HAS_GPS
      // GPS Menu
      Menu gpsInfoMenu;
    #endif

    Ticker tick;

    uint16_t x = -1, y = -1;
    boolean pressed = false;

    String loaded_file = "";

    void initLVGL();
    void deinitLVGL();
    void selectEPHTMLGFX();
    void addSSIDGFX();
    void addAPGFX();
    void addStationGFX();
    void buildButtons(Menu* menu, int starting_index = 0, String button_name = "");
    void changeMenu(Menu* menu);
    void drawStatusBar();
    void displayCurrentMenu(uint8_t start_index = 0);
    void main(uint32_t currentTime);
    void RunSetup();
    void orientDisplay();
};


#endif
#endif
