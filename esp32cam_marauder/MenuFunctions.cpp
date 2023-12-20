#include "MenuFunctions.h"
#include "lang_var.h"
//#include "icons.h"

#ifdef HAS_SCREEN

extern const unsigned char menu_icons[][66];
PROGMEM lv_obj_t * slider_label;
PROGMEM lv_obj_t * ta1;
PROGMEM lv_obj_t * ta2;
PROGMEM lv_obj_t * save_name;

MenuFunctions::MenuFunctions()
{
}

// LVGL Stuff
/* Interrupt driven periodic handler */

#ifdef HAS_ILI9341
  uint8_t MenuFunctions::updateTouch(uint16_t *x, uint16_t *y, uint16_t threshold) {
    if (!display_obj.headless_mode)
      return display_obj.tft.getTouch(x, y, threshold);
    else
      return !display_obj.headless_mode;
  }

  void MenuFunctions::lv_tick_handler()
  {
    lv_tick_inc(LVGL_TICK_PERIOD);
  }
  
  /* Display flushing */
  void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
  {
    extern Display display_obj;
    uint16_t c;
  
    display_obj.tft.startWrite();
    display_obj.tft.setAddrWindow(area->x1, area->y1, (area->x2 - area->x1 + 1), (area->y2 - area->y1 + 1));
    for (int y = area->y1; y <= area->y2; y++) {
      for (int x = area->x1; x <= area->x2; x++) {
        c = color_p->full;
        display_obj.tft.writeColor(c, 1);
        color_p++;
      }
    }
    display_obj.tft.endWrite();
    lv_disp_flush_ready(disp);
  }
  
  
  bool my_touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data)
  {
    extern Display display_obj;
    
    uint16_t touchX, touchY;
  
    bool touched = display_obj.tft.getTouch(&touchX, &touchY, 600);
  
    if(!touched)
    {
      return false;
    }
  
    if(touchX>WIDTH_1 || touchY > HEIGHT_1)
    {
      Serial.println("Y or y outside of expected parameters..");
      Serial.print("y:");
      Serial.print(touchX);
      Serial.print(" x:");
      Serial.print(touchY);
    }
    else
    {
  
      data->state = touched ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL; 
       
      data->point.x = touchX;
      data->point.y = touchY;
  
    }
  
    return false;
  }
  
  void MenuFunctions::initLVGL() {
    tick.attach_ms(LVGL_TICK_PERIOD, lv_tick_handler);
    
    lv_init();
  
    lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);
  
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = WIDTH_1;
    disp_drv.ver_res = HEIGHT_1;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);
  
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);             
    indev_drv.type = LV_INDEV_TYPE_POINTER;    
    indev_drv.read_cb = my_touchpad_read;      
    lv_indev_drv_register(&indev_drv);         
  }
  
  
  void MenuFunctions::deinitLVGL() {
    Serial.println(F("Deinit LVGL"));
    //lv_deinit();
  }
  
  
  // Event handler for settings drop down menus
  void setting_dropdown_cb(lv_obj_t * obj, lv_event_t event) {

  }

  // GFX Function to build a list showing all Stations scanned
  void MenuFunctions::addStationGFX(){
    extern LinkedList<Station>* stations;
    extern LinkedList<AccessPoint>* access_points;
    extern WiFiScan wifi_scan_obj;
  
    lv_obj_t * list1 = lv_list_create(lv_scr_act(), NULL);
    lv_obj_set_size(list1, 160, 200);
    lv_obj_set_width(list1, LV_HOR_RES);
    lv_obj_align(list1, NULL, LV_ALIGN_CENTER, 0, 0);
  
    lv_obj_t * list_btn;
  
    lv_obj_t * label;
  
    list_btn = lv_list_add_btn(list1, LV_SYMBOL_CLOSE, text09);
    lv_obj_set_event_cb(list_btn, station_list_cb);

    char addr[] = "00:00:00:00:00:00";
    for (int x = 0; x < access_points->size(); x++) {
      AccessPoint cur_ap = access_points->get(x);

      // Add non clickable button for AP
      String full_label = "AP: " + cur_ap.essid;
      char buf[full_label.length() + 1] = {};
      full_label.toCharArray(buf, full_label.length() + 1);
      list_btn = lv_list_add_btn(list1, NULL, buf);
      lv_btn_set_checkable(list_btn, false);
      
      int cur_ap_sta_len = access_points->get(x).stations->size();
      for (int y = 0; y < cur_ap_sta_len; y++) {
        Station cur_sta = stations->get(cur_ap.stations->get(y));
        // Convert uint8_t MAC to char array
        wifi_scan_obj.getMAC(addr, cur_sta.mac, 0);
        
        list_btn = lv_list_add_btn(list1, LV_SYMBOL_WIFI, addr);
        lv_btn_set_checkable(list_btn, true);
        lv_obj_set_event_cb(list_btn, station_list_cb);
    
        if (cur_sta.selected)
          lv_btn_toggle(list_btn);
      }
    }
  }

  // Function to work with list of Stations
  void station_list_cb(lv_obj_t * btn, lv_event_t event) {
    extern LinkedList<Station>* stations;
    extern MenuFunctions menu_function_obj;
    extern WiFiScan wifi_scan_obj;
  
    String btn_text = lv_list_get_btn_text(btn);
    String display_string = "";
    char addr[] = "00:00:00:00:00:00";
    
    if (event == LV_EVENT_CLICKED) {
      if (btn_text != text09) {
      }
      else {
        Serial.println("Exiting...");
        lv_obj_del_async(lv_obj_get_parent(lv_obj_get_parent(btn)));
  
        for (int i = 0; i < stations->size(); i++) {
          if (stations->get(i).selected) {
            wifi_scan_obj.getMAC(addr, stations->get(i).mac, 0);
            Serial.print("Selected: ");
            Serial.println(addr);
          }
        }
  
        printf("LV_EVENT_CANCEL\n");
        menu_function_obj.deinitLVGL();
        wifi_scan_obj.StartScan(WIFI_SCAN_OFF);
        display_obj.exit_draw = true; // set everything back to normal
      }
    }
    
    if (event == LV_EVENT_VALUE_CHANGED) {     
      if (lv_btn_get_state(btn) == LV_BTN_STATE_CHECKED_RELEASED) {
        for (int i = 0; i < stations->size(); i++) {
          wifi_scan_obj.getMAC(addr, stations->get(i).mac, 0); 
          if (strcmp(addr, btn_text.c_str()) == 0) {
            Serial.print("Adding Station: ");
            Serial.println(addr);
            Station sta = stations->get(i);
            sta.selected = true;
            stations->set(i, sta);
          }
        }
      }
      else {
        for (int i = 0; i < stations->size(); i++) {
          wifi_scan_obj.getMAC(addr, stations->get(i).mac, 0); 
          if (strcmp(addr, btn_text.c_str()) == 0) {
            Serial.print("Removing Station: ");
            Serial.println(addr);
            Station sta = stations->get(i);
            sta.selected = false;
            stations->set(i, sta);
          }
        }
      }
    }
  }

  // GFX Function to build a list showing all EP HTML Files
  void MenuFunctions::selectEPHTMLGFX() {
    extern EvilPortal evil_portal_obj;
  
    lv_obj_t * list1 = lv_list_create(lv_scr_act(), NULL);
    lv_obj_set_size(list1, 160, 200);
    lv_obj_set_width(list1, LV_HOR_RES);
    lv_obj_align(list1, NULL, LV_ALIGN_CENTER, 0, 0);
  
    lv_obj_t * list_btn;
  
    lv_obj_t * label;
  
    list_btn = lv_list_add_btn(list1, LV_SYMBOL_CLOSE, text09);
    lv_obj_set_event_cb(list_btn, html_list_cb);
  
    for (int i = 1; i < evil_portal_obj.html_files->size(); i++) {
      char buf[evil_portal_obj.html_files->get(i).length() + 1] = {};
      evil_portal_obj.html_files->get(i).toCharArray(buf, evil_portal_obj.html_files->get(i).length() + 1);
      
      list_btn = lv_list_add_btn(list1, LV_SYMBOL_FILE, buf);
      lv_btn_set_checkable(list_btn, true);
      lv_obj_set_event_cb(list_btn, html_list_cb);
  
      if (i == evil_portal_obj.selected_html_index)
        lv_btn_toggle(list_btn);
    }
  }

  void html_list_cb(lv_obj_t * btn, lv_event_t event) {
    extern EvilPortal evil_portal_obj;
    extern MenuFunctions menu_function_obj;
  
    String btn_text = lv_list_get_btn_text(btn);
    String display_string = "";
    
    if (event == LV_EVENT_CLICKED) {
      if (btn_text != text09) {
      }
      else {
        Serial.println("Exiting...");
        lv_obj_del_async(lv_obj_get_parent(lv_obj_get_parent(btn)));
  
        for (int i = 1; i < evil_portal_obj.html_files->size(); i++) {
          if (i == evil_portal_obj.selected_html_index) {
            Serial.println("Selected: " + (String)evil_portal_obj.html_files->get(i));
          }
        }
  
        printf("LV_EVENT_CANCEL\n");
        menu_function_obj.deinitLVGL();
        wifi_scan_obj.StartScan(WIFI_SCAN_OFF);
        display_obj.exit_draw = true; // set everything back to normal
      }
    }
    
    if (event == LV_EVENT_VALUE_CHANGED) {      
      if (lv_btn_get_state(btn) == LV_BTN_STATE_CHECKED_RELEASED) {
        for (int i = 1; i < evil_portal_obj.html_files->size(); i++) {
          if (evil_portal_obj.html_files->get(i) == btn_text) {
            Serial.println("Setting HTML: " + (String)evil_portal_obj.html_files->get(i));
            evil_portal_obj.selected_html_index = i;
            evil_portal_obj.target_html_name = (String)evil_portal_obj.html_files->get(i);
          }
        }

        // Deselect buttons that were previously selected
        lv_obj_t * list = lv_obj_get_parent(btn);

        lv_obj_t * next_btn = lv_obj_get_child(list, NULL);
        while (next_btn != NULL) {
          if (next_btn != btn) {
            lv_btn_set_state(next_btn, LV_BTN_STATE_RELEASED);
          }
          next_btn = lv_obj_get_child(list, next_btn);
        }
      }
    }
  }
  
  // GFX Function to build a list showing all APs scanned
  void MenuFunctions::addAPGFX(){
    extern LinkedList<AccessPoint>* access_points;
  
    lv_obj_t * list1 = lv_list_create(lv_scr_act(), NULL);
    lv_obj_set_size(list1, 160, 200);
    lv_obj_set_width(list1, LV_HOR_RES);
    lv_obj_align(list1, NULL, LV_ALIGN_CENTER, 0, 0);
  
    lv_obj_t * list_btn;
  
    lv_obj_t * label;
  
    list_btn = lv_list_add_btn(list1, LV_SYMBOL_CLOSE, text09);
    lv_obj_set_event_cb(list_btn, ap_list_cb);
  
    for (int i = 0; i < access_points->size(); i++) {
      char buf[access_points->get(i).essid.length() + 1] = {};
      access_points->get(i).essid.toCharArray(buf, access_points->get(i).essid.length() + 1);
      
      list_btn = lv_list_add_btn(list1, LV_SYMBOL_WIFI, buf);
      lv_btn_set_checkable(list_btn, true);
      lv_obj_set_event_cb(list_btn, ap_list_cb);
  
      if (access_points->get(i).selected)
        lv_btn_toggle(list_btn);
    }
  }
  
  
  
  void ap_list_cb(lv_obj_t * btn, lv_event_t event) {
    extern LinkedList<AccessPoint>* access_points;
    extern MenuFunctions menu_function_obj;
  
    String btn_text = lv_list_get_btn_text(btn);
    String display_string = "";
    
    if (event == LV_EVENT_CLICKED) {
      if (btn_text != text09) {
      }
      else {
        Serial.println("Exiting...");
        lv_obj_del_async(lv_obj_get_parent(lv_obj_get_parent(btn)));
  
        for (int i = 0; i < access_points->size(); i++) {
          if (access_points->get(i).selected) {
            Serial.println("Selected: " + (String)access_points->get(i).essid);
          }
        }
  
        printf("LV_EVENT_CANCEL\n");
        menu_function_obj.deinitLVGL();
        wifi_scan_obj.StartScan(WIFI_SCAN_OFF);
        display_obj.exit_draw = true; // set everything back to normal
      }
    }
    
    if (event == LV_EVENT_VALUE_CHANGED) {      
      if (lv_btn_get_state(btn) == LV_BTN_STATE_CHECKED_RELEASED) {
        for (int i = 0; i < access_points->size(); i++) {
          if (access_points->get(i).essid == btn_text) {
            Serial.println("Adding AP: " + (String)access_points->get(i).essid);
            AccessPoint ap = access_points->get(i);
            ap.selected = true;
            access_points->set(i, ap);
          }
        }
      }
      else {
        for (int i = 0; i < access_points->size(); i++) {
          if (access_points->get(i).essid == btn_text) {
            Serial.println("Removing AP: " + (String)access_points->get(i).essid);
            AccessPoint ap = access_points->get(i);
            ap.selected = false;
            access_points->set(i, ap);
          }
        }
      }
    }
  }
  
  void MenuFunctions::addSSIDGFX(){
    extern LinkedList<ssid>* ssids;
    
    String display_string = "";
    // Create a keyboard and apply the styles
    kb = lv_keyboard_create(lv_scr_act(), NULL);
    lv_obj_set_size(kb, LV_HOR_RES, LV_VER_RES / 2);
    lv_obj_set_event_cb(kb, add_ssid_keyboard_event_cb);
  
    // Create one text area
    // Store all SSIDs
    ta1 = lv_textarea_create(lv_scr_act(), NULL);
    lv_textarea_set_one_line(ta1, false);
    lv_obj_set_width(ta1, LV_HOR_RES);
    lv_obj_set_height(ta1, (LV_VER_RES / 2) - 35);
    lv_obj_set_pos(ta1, 5, 20);
    lv_textarea_set_cursor_hidden(ta1, true);
    lv_obj_align(ta1, NULL, LV_ALIGN_IN_TOP_MID, NULL, NULL);
    lv_textarea_set_placeholder_text(ta1, text_table1[0]);
  
    // Create second text area
    // Add SSIDs
    ta2 = lv_textarea_create(lv_scr_act(), ta1);
    lv_textarea_set_cursor_hidden(ta2, false);
    lv_textarea_set_one_line(ta2, true);
    lv_obj_align(ta2, NULL, LV_ALIGN_IN_TOP_MID, NULL, (LV_VER_RES / 2) - 35);
    lv_textarea_set_text(ta2, "");
    lv_textarea_set_placeholder_text(ta2, text_table1[1]);
  
    // After generating text areas, add text to first text box
    for (int i = 0; i < ssids->size(); i++)
      display_string.concat((String)ssids->get(i).essid + "\n");
      
    lv_textarea_set_text(ta1, display_string.c_str());
  
    // Focus it on one of the text areas to start
    lv_keyboard_set_textarea(kb, ta2);
    lv_keyboard_set_cursor_manage(kb, true);
    
  }
  
  // Keyboard callback dedicated to joining wifi
  void add_ssid_keyboard_event_cb(lv_obj_t * keyboard, lv_event_t event){
    extern Display display_obj;
    extern MenuFunctions menu_function_obj;
    extern WiFiScan wifi_scan_obj;
    extern LinkedList<ssid>* ssids;
    
    lv_keyboard_def_event_cb(kb, event);
  
    // User has applied text box
    if(event == LV_EVENT_APPLY){
      String display_string = "";
      printf("LV_EVENT_APPLY\n");
  
      // Get text from SSID text box
      String ta2_text = lv_textarea_get_text(ta2);
      Serial.println(ta2_text);
  
      // Add text box text to list of SSIDs
      wifi_scan_obj.addSSID(ta2_text);
  
      // Update large text box with ssid
      for (int i = 0; i < ssids->size(); i++)
        display_string.concat((String)ssids->get(i).essid + "\n");
      lv_textarea_set_text(ta1, display_string.c_str());
  
      lv_textarea_set_text(ta2, "");
    }else if(event == LV_EVENT_CANCEL){
      printf("LV_EVENT_CANCEL\n");
      menu_function_obj.deinitLVGL();
      display_obj.exit_draw = true; // set everything back to normal
    }
  }
  
  
  void ta_event_cb(lv_obj_t * ta, lv_event_t event)
  {
    if(event == LV_EVENT_CLICKED) {
      if(kb != NULL)
        lv_keyboard_set_textarea(kb, ta);
    }
  
  }

#endif
//// END LV_ARDUINO STUFF

void MenuFunctions::buttonNotSelected(uint8_t b, int8_t x) {
  if (x == -1)
    x = b;
  display_obj.tft.setFreeFont(NULL);
  display_obj.key[b].drawButton(false, current_menu->list->get(x).name);
}

void MenuFunctions::buttonSelected(uint8_t b, int8_t x) {
  if (x == -1)
    x = b;
  display_obj.tft.setFreeFont(NULL);
  display_obj.key[b].drawButton(true, current_menu->list->get(x).name);
}

// Function to check menu input
void MenuFunctions::main(uint32_t currentTime)
{
  // Some function exited and we need to go back to normal
  if (display_obj.exit_draw) {
    wifi_scan_obj.currentScanMode = WIFI_SCAN_OFF;
    display_obj.exit_draw = false;
    this->orientDisplay();
  }
  if ((wifi_scan_obj.currentScanMode == WIFI_SCAN_OFF) ||
      (wifi_scan_obj.currentScanMode == OTA_UPDATE) ||
      (wifi_scan_obj.currentScanMode == ESP_UPDATE) ||
      (wifi_scan_obj.currentScanMode == SHOW_INFO) ||
      (wifi_scan_obj.currentScanMode == WIFI_SCAN_GPS_DATA) ||
      (wifi_scan_obj.currentScanMode == WIFI_SCAN_GPS_NMEA)) {
    if (wifi_scan_obj.orient_display) {
      this->orientDisplay();
      wifi_scan_obj.orient_display = false;
    }
    /*#ifdef HAS_ILI9341
      if ((wifi_scan_obj.currentScanMode != LV_JOIN_WIFI) &&
          (wifi_scan_obj.currentScanMode != LV_ADD_SSID))
        display_obj.updateBanner(current_menu->name);
    #endif*/
  }

  if (currentTime != 0) {
    if (currentTime - initTime >= BANNER_TIME) {
      this->initTime = millis();
      if ((wifi_scan_obj.currentScanMode != LV_JOIN_WIFI) &&
          (wifi_scan_obj.currentScanMode != LV_ADD_SSID))
        this->updateStatusBar();
        //#if defined(MARAUDER_MINI) || defined(MARAUDER_M5STICKC)
        //  display_obj.updateBanner(current_menu->name);
        //#endif
    }
  }


  boolean pressed = false;
  // This is code from bodmer's keypad example
  uint16_t t_x = 0, t_y = 0; // To store the touch coordinates

  // Get the display buffer out of the way
  if ((wifi_scan_obj.currentScanMode != WIFI_SCAN_OFF ) &&
      (wifi_scan_obj.currentScanMode != WIFI_ATTACK_BEACON_SPAM) &&
      (wifi_scan_obj.currentScanMode != WIFI_ATTACK_AP_SPAM) &&
      (wifi_scan_obj.currentScanMode != WIFI_ATTACK_AUTH) &&
      (wifi_scan_obj.currentScanMode != WIFI_ATTACK_DEAUTH) &&
      (wifi_scan_obj.currentScanMode != WIFI_ATTACK_DEAUTH_MANUAL) &&
      (wifi_scan_obj.currentScanMode != WIFI_ATTACK_DEAUTH_TARGETED) &&
      (wifi_scan_obj.currentScanMode != WIFI_ATTACK_MIMIC) &&
      (wifi_scan_obj.currentScanMode != WIFI_ATTACK_RICK_ROLL))
    display_obj.displayBuffer();


  // Pressed will be set true is there is a valid touch on the screen
  int pre_getTouch = millis();

  // getTouch causes a 10ms delay which makes beacon spam less effective
  #ifdef HAS_ILI9341
    pressed = this->updateTouch(&t_x, &t_y);
  #endif


  // This is if there are scans/attacks going on
  #ifdef HAS_ILI9341
    if ((wifi_scan_obj.currentScanMode != WIFI_SCAN_OFF) &&
        (pressed) &&
        (wifi_scan_obj.currentScanMode != OTA_UPDATE) &&
        (wifi_scan_obj.currentScanMode != ESP_UPDATE) &&
        (wifi_scan_obj.currentScanMode != SHOW_INFO) &&
        (wifi_scan_obj.currentScanMode != WIFI_SCAN_GPS_DATA) &&
        (wifi_scan_obj.currentScanMode != WIFI_SCAN_GPS_NMEA))
    {
      // Stop the current scan
      if ((wifi_scan_obj.currentScanMode == WIFI_SCAN_PROBE) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_STATION_WAR_DRIVE) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_RAW_CAPTURE) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_STATION) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_AP) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_WAR_DRIVE) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_EVIL_PORTAL) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_SIG_STREN) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_TARGET_AP) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_TARGET_AP_FULL) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_PWN) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_ESPRESSIF) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_ALL) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_DEAUTH) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_BEACON_SPAM) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_AP_SPAM) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_AUTH) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_DEAUTH) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_DEAUTH_MANUAL) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_DEAUTH_TARGETED) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_MIMIC) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_RICK_ROLL) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_BEACON_LIST) ||
          (wifi_scan_obj.currentScanMode == BT_SCAN_ALL) ||
          (wifi_scan_obj.currentScanMode == BT_ATTACK_SOUR_APPLE) ||
          (wifi_scan_obj.currentScanMode == BT_ATTACK_SWIFTPAIR_SPAM) ||
          (wifi_scan_obj.currentScanMode == BT_ATTACK_SPAM_ALL) ||
          (wifi_scan_obj.currentScanMode == BT_ATTACK_SAMSUNG_SPAM) ||
          (wifi_scan_obj.currentScanMode == BT_ATTACK_GOOGLE_SPAM) ||
          (wifi_scan_obj.currentScanMode == BT_SCAN_WAR_DRIVE) ||
          (wifi_scan_obj.currentScanMode == BT_SCAN_WAR_DRIVE_CONT) ||
          (wifi_scan_obj.currentScanMode == BT_SCAN_SKIMMERS))
      {
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
  #endif

  #ifdef HAS_BUTTONS

    bool c_btn_press = c_btn.justPressed();
    
    if ((c_btn_press) &&
        (wifi_scan_obj.currentScanMode != WIFI_SCAN_OFF) &&
        (wifi_scan_obj.currentScanMode != OTA_UPDATE) &&
        (wifi_scan_obj.currentScanMode != ESP_UPDATE) &&
        (wifi_scan_obj.currentScanMode != SHOW_INFO) &&
        (wifi_scan_obj.currentScanMode != WIFI_SCAN_GPS_DATA) &&
        (wifi_scan_obj.currentScanMode != WIFI_SCAN_GPS_NMEA))
    {
      // Stop the current scan
      if ((wifi_scan_obj.currentScanMode == WIFI_SCAN_PROBE) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_STATION_WAR_DRIVE) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_RAW_CAPTURE) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_STATION) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_AP) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_WAR_DRIVE) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_EVIL_PORTAL) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_SIG_STREN) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_TARGET_AP) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_TARGET_AP_FULL) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_PWN) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_ESPRESSIF) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_ALL) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_DEAUTH) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_BEACON_SPAM) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_AP_SPAM) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_AUTH) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_DEAUTH) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_DEAUTH_MANUAL) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_DEAUTH_TARGETED) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_MIMIC) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_RICK_ROLL) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_BEACON_LIST) ||
          (wifi_scan_obj.currentScanMode == BT_SCAN_ALL) ||
          (wifi_scan_obj.currentScanMode == BT_ATTACK_SOUR_APPLE) ||
          (wifi_scan_obj.currentScanMode == BT_ATTACK_SWIFTPAIR_SPAM) ||
          (wifi_scan_obj.currentScanMode == BT_ATTACK_SPAM_ALL) ||
          (wifi_scan_obj.currentScanMode == BT_ATTACK_SAMSUNG_SPAM) ||
          (wifi_scan_obj.currentScanMode == BT_ATTACK_GOOGLE_SPAM) ||
          (wifi_scan_obj.currentScanMode == BT_SCAN_WAR_DRIVE) ||
          (wifi_scan_obj.currentScanMode == BT_SCAN_WAR_DRIVE_CONT) ||
          (wifi_scan_obj.currentScanMode == BT_SCAN_SKIMMERS) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_EAPOL) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_ACTIVE_EAPOL) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_ACTIVE_LIST_EAPOL) ||
          (wifi_scan_obj.currentScanMode == WIFI_PACKET_MONITOR))
      {
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

  #endif


  // Check if any key coordinate boxes contain the touch coordinates
  // This is for when on a menu
  #ifdef HAS_ILI9341
    if ((wifi_scan_obj.currentScanMode != WIFI_ATTACK_BEACON_SPAM) &&
        (wifi_scan_obj.currentScanMode != WIFI_ATTACK_AP_SPAM) &&
        (wifi_scan_obj.currentScanMode != WIFI_ATTACK_AUTH) &&
        (wifi_scan_obj.currentScanMode != WIFI_ATTACK_DEAUTH) &&
        (wifi_scan_obj.currentScanMode != WIFI_ATTACK_DEAUTH_MANUAL) &&
        (wifi_scan_obj.currentScanMode != WIFI_ATTACK_DEAUTH_TARGETED) &&
        (wifi_scan_obj.currentScanMode != WIFI_ATTACK_MIMIC) &&
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
          display_obj.key[b].drawButton(true, current_menu->list->get(b).name);
          if (current_menu->list->get(b).name != text09)
            display_obj.tft.drawXBitmap(0,
                                        KEY_Y + b * (KEY_H + KEY_SPACING_Y) - (ICON_H / 2),
                                        menu_icons[current_menu->list->get(b).icon],
                                        ICON_W,
                                        ICON_H,
                                        current_menu->list->get(b).color,
                                        TFT_BLACK);
        }
  
        // If button was just release, execute the button's function
        if ((display_obj.key[b].justReleased()) && (!pressed))
        {
          display_obj.key[b].drawButton(false, current_menu->list->get(b).name);
          current_menu->list->get(b).callable();
        }
        // This
        else if ((display_obj.key[b].justReleased()) && (pressed)) {
          display_obj.key[b].drawButton(false, current_menu->list->get(b).name);
          if (current_menu->list->get(b).name != text09)
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
  #endif

  #ifdef HAS_BUTTONS
    #if !(defined(MARAUDER_V6) || defined(MARAUDER_V6_1))
      #ifndef MARAUDER_M5STICKC
        if (u_btn.justPressed()){
          if ((wifi_scan_obj.currentScanMode == WIFI_SCAN_OFF) ||
              (wifi_scan_obj.currentScanMode == OTA_UPDATE)) {
            if (current_menu->selected > 0) {
              current_menu->selected--;
              // Page up
              if (current_menu->selected < this->menu_start_index) {
                this->buildButtons(current_menu, current_menu->selected);
                this->displayCurrentMenu(current_menu->selected);
              }
              this->buttonSelected(current_menu->selected - this->menu_start_index, current_menu->selected);
              if (!current_menu->list->get(current_menu->selected + 1).selected)
                this->buttonNotSelected(current_menu->selected + 1 - this->menu_start_index, current_menu->selected + 1);
            }
            // Loop to end
            else {
              current_menu->selected = current_menu->list->size() - 1;
              if (current_menu->selected >= BUTTON_SCREEN_LIMIT) {
                this->buildButtons(current_menu, current_menu->selected + 1 - BUTTON_SCREEN_LIMIT);
                this->displayCurrentMenu(current_menu->selected + 1 - BUTTON_SCREEN_LIMIT);
              }
              this->buttonSelected(current_menu->selected, current_menu->selected);
              if (!current_menu->list->get(0).selected)
                this->buttonNotSelected(0, this->menu_start_index);
            }
          }
          else if ((wifi_scan_obj.currentScanMode == WIFI_PACKET_MONITOR) ||
                  (wifi_scan_obj.currentScanMode == WIFI_SCAN_EAPOL)) {
            if (wifi_scan_obj.set_channel < 14)
              wifi_scan_obj.changeChannel(wifi_scan_obj.set_channel + 1);
            else
              wifi_scan_obj.changeChannel(1);
          }
        }
      #endif
      if (d_btn.justPressed()){
        if ((wifi_scan_obj.currentScanMode == WIFI_SCAN_OFF) ||
            (wifi_scan_obj.currentScanMode == OTA_UPDATE)) {
          if (current_menu->selected < current_menu->list->size() - 1) {
            current_menu->selected++;
            this->buttonSelected(current_menu->selected - this->menu_start_index, current_menu->selected);
            if (!current_menu->list->get(current_menu->selected - 1).selected)
              this->buttonNotSelected(current_menu->selected - 1 - this->menu_start_index, current_menu->selected - 1);
            // Page down
            if (current_menu->selected - this->menu_start_index >= BUTTON_SCREEN_LIMIT) {
              this->buildButtons(current_menu, current_menu->selected + 1 - BUTTON_SCREEN_LIMIT);
              this->displayCurrentMenu(current_menu->selected + 1 - BUTTON_SCREEN_LIMIT);
            }
          }
          // Loop to beginning
          else {
            if (current_menu->selected >= BUTTON_SCREEN_LIMIT) {
              this->buildButtons(current_menu);
              this->displayCurrentMenu();
            }
            current_menu->selected = 0;
            this->buttonSelected(current_menu->selected);
            if (!current_menu->list->get(current_menu->list->size() - 1).selected)
              this->buttonNotSelected(current_menu->list->size() - 1);
          }
        }
        else if ((wifi_scan_obj.currentScanMode == WIFI_PACKET_MONITOR) ||
                (wifi_scan_obj.currentScanMode == WIFI_SCAN_EAPOL)) {
          if (wifi_scan_obj.set_channel > 1)
            wifi_scan_obj.changeChannel(wifi_scan_obj.set_channel - 1);
          else
            wifi_scan_obj.changeChannel(14);
        }
      }
      if(c_btn_press){
        current_menu->list->get(current_menu->selected).callable();
      }
    #endif
  #endif
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
  display_obj.tft.setTextSize(1);
  
  #if defined(MARAUDER_MINI) || defined(MARAUDER_M5STICKC)
    display_obj.tft.setFreeFont(NULL);
  #endif
  
  uint16_t the_color; 

  // GPS Stuff
  #ifdef HAS_GPS
    if (gps_obj.getGpsModuleStatus()) {
      if (gps_obj.getFixStatus())
        the_color = TFT_GREEN;
      else
        the_color = TFT_RED;
        
      #ifdef HAS_ILI9341
        display_obj.tft.drawXBitmap(4,
                                    0,
                                    menu_icons[STATUS_GPS],
                                    16,
                                    16,
                                    STATUSBAR_COLOR,
                                    the_color);
        display_obj.tft.setTextColor(TFT_WHITE, STATUSBAR_COLOR);

        display_obj.tft.drawString(gps_obj.getNumSatsString(), 22, 0, 2);
      #elif defined(HAS_SCREEN)
        display_obj.tft.setTextColor(the_color, STATUSBAR_COLOR);
        display_obj.tft.drawString("GPS", 0, 0, 1);
      #endif
    }
  #endif

  display_obj.tft.setTextColor(TFT_WHITE, STATUSBAR_COLOR);

  // WiFi Channel Stuff
  if (wifi_scan_obj.set_channel != wifi_scan_obj.old_channel) {
    wifi_scan_obj.old_channel = wifi_scan_obj.set_channel;
    display_obj.tft.fillRect(50, 0, TFT_WIDTH * 0.21, STATUS_BAR_WIDTH, STATUSBAR_COLOR);
    #ifdef HAS_ILI9341
      display_obj.tft.drawString("CH: " + (String)wifi_scan_obj.set_channel, 50, 0, 2);
    #endif

    #if defined(MARAUDER_MINI) || defined(MARAUDER_M5STICKC)
      display_obj.tft.drawString("CH: " + (String)wifi_scan_obj.set_channel, TFT_WIDTH/4, 0, 1);
    #endif
  }

  // RAM Stuff
  wifi_scan_obj.freeRAM();
  if (wifi_scan_obj.free_ram != wifi_scan_obj.old_free_ram) {
    wifi_scan_obj.old_free_ram = wifi_scan_obj.free_ram;
    display_obj.tft.fillRect(100, 0, 60, STATUS_BAR_WIDTH, STATUSBAR_COLOR);
    #ifdef HAS_ILI9341
      display_obj.tft.drawString((String)wifi_scan_obj.free_ram + "B", 100, 0, 2);
    #endif

    #if defined(MARAUDER_MINI) || defined(MARAUDER_M5STICKC)
      display_obj.tft.drawString((String)wifi_scan_obj.free_ram + "B", TFT_WIDTH/1.75, 0, 1);
    #endif
  }

  // Draw battery info
  MenuFunctions::battery(false);


  // Draw SD info
  #ifndef WRITE_PACKETS_SERIAL
    if (sd_obj.supported)
      the_color = TFT_GREEN;
    else
      the_color = TFT_RED;
  #else
    the_color = TFT_RED;
  #endif

  #ifdef HAS_ILI9341
    display_obj.tft.drawXBitmap(170,
                                0,
                                menu_icons[STATUS_SD],
                                16,
                                16,
                                STATUSBAR_COLOR,
                                the_color);
  #endif

  #if defined(MARAUDER_MINI) || defined(MARAUDER_M5STICKC)
    display_obj.tft.setTextColor(the_color, STATUSBAR_COLOR);
    display_obj.tft.drawString("SD", TFT_WIDTH - 12, 0, 1);
  #endif
}

void MenuFunctions::drawStatusBar()
{
  display_obj.tft.setTextSize(1);
  #if defined(MARAUDER_MINI) || defined(MARAUDER_M5STICKC)
    display_obj.tft.setFreeFont(NULL);
  #endif
  display_obj.tft.fillRect(0, 0, 240, STATUS_BAR_WIDTH, STATUSBAR_COLOR);
  display_obj.tft.setTextColor(TFT_WHITE, STATUSBAR_COLOR);

  uint16_t the_color;

  // GPS Stuff
  #ifdef HAS_GPS
    if (gps_obj.getGpsModuleStatus()) {
      if (gps_obj.getFixStatus())
        the_color = TFT_GREEN;
      else
        the_color = TFT_RED;
        
      #ifdef HAS_ILI9341
        display_obj.tft.drawXBitmap(4,
                                    0,
                                    menu_icons[STATUS_GPS],
                                    16,
                                    16,
                                    STATUSBAR_COLOR,
                                    the_color);
        display_obj.tft.setTextColor(TFT_WHITE, STATUSBAR_COLOR);

        display_obj.tft.drawString(gps_obj.getNumSatsString(), 22, 0, 2);
      #endif
    }
  #endif

  display_obj.tft.setTextColor(TFT_WHITE, STATUSBAR_COLOR);


  // WiFi Channel Stuff
  wifi_scan_obj.old_channel = wifi_scan_obj.set_channel;
  display_obj.tft.fillRect(50, 0, 50, STATUS_BAR_WIDTH, STATUSBAR_COLOR);
  #ifdef HAS_ILI9341
    display_obj.tft.drawString("CH: " + (String)wifi_scan_obj.set_channel, 50, 0, 2);
  #endif

  #if defined(MARAUDER_MINI) || defined(MARAUDER_M5STICKC)
    display_obj.tft.drawString("CH: " + (String)wifi_scan_obj.set_channel, TFT_WIDTH/4, 0, 1);
  #endif

  // RAM Stuff
  wifi_scan_obj.freeRAM();
  wifi_scan_obj.old_free_ram = wifi_scan_obj.free_ram;
  display_obj.tft.fillRect(100, 0, 60, STATUS_BAR_WIDTH, STATUSBAR_COLOR);
  #ifdef HAS_ILI9341
    display_obj.tft.drawString((String)wifi_scan_obj.free_ram + "B", 100, 0, 2);
  #endif

  #if defined(MARAUDER_MINI) || defined(MARAUDER_M5STICKC)
    display_obj.tft.drawString((String)wifi_scan_obj.free_ram + "B", TFT_WIDTH/1.75, 0, 1);
  #endif


  MenuFunctions::battery2(true);

  // Draw SD info
  #ifndef WRITE_PACKETS_SERIAL
    if (sd_obj.supported)
      the_color = TFT_GREEN;
    else
      the_color = TFT_RED;
  #else
    the_color = TFT_RED;
  #endif

  #ifdef HAS_ILI9341
    display_obj.tft.drawXBitmap(170,
                                0,
                                menu_icons[STATUS_SD],
                                16,
                                16,
                                STATUSBAR_COLOR,
                                the_color);
  #endif

  #if defined(MARAUDER_MINI) || defined(MARAUDER_M5STICKC)
    display_obj.tft.setTextColor(the_color, STATUSBAR_COLOR);
    display_obj.tft.drawString("SD", TFT_WIDTH - 12, 0, 1);
  #endif
}

void MenuFunctions::orientDisplay()
{
  display_obj.tft.init();

  display_obj.tft.setRotation(0); // Portrait

  display_obj.tft.setCursor(0, 0);

  #ifdef HAS_ILI9341
    #ifdef TFT_SHIELD
      uint16_t calData[5] = { 275, 3494, 361, 3528, 4 }; // tft.setRotation(0); // Portrait with TFT Shield
    #else if defined(TFT_DIY)
      uint16_t calData[5] = { 339, 3470, 237, 3438, 2 }; // tft.setRotation(0); // Portrait with DIY TFT
    #endif

    display_obj.tft.setTouch(calData);
  #endif

  changeMenu(current_menu);
}

void MenuFunctions::runBoolSetting(String key) {
  display_obj.tftDrawRedOnOffButton();
}

String MenuFunctions::callSetting(String key) {
  specSettingMenu.name = key;
  
  String setting_type = settings_obj.getSettingType(key);

  if (setting_type == "bool") {
    return "bool";
  }
}

void MenuFunctions::displaySetting(String key, Menu* menu, int index) {
  specSettingMenu.name = key;

  bool setting_value = settings_obj.loadSetting<bool>(key);

  // Make a local copy of menu node
  MenuNode node = menu->list->get(index);

  display_obj.tft.setTextWrap(false);
  display_obj.tft.setFreeFont(NULL);
  display_obj.tft.setCursor(0, 100);
  display_obj.tft.setTextSize(1);

  // Set local copy value
  if (!setting_value) {
    display_obj.tft.setTextColor(TFT_RED);
    display_obj.tft.println(F(text_table1[4]));
    node.selected = false;
  }
  else {
    display_obj.tft.setTextColor(TFT_GREEN);
    display_obj.tft.println(F(text_table1[5]));
    node.selected = true;
  }

  // Put local copy back into menu
  menu->list->set(index, node);
    
}

// Function to build the menus
void MenuFunctions::RunSetup()
{
  extern LinkedList<AccessPoint>* access_points;
  
  #ifdef HAS_ILI9341
    this->initLVGL();
  #endif
   
  // root menu stuff
  mainMenu.list = new LinkedList<MenuNode>(); // Get list in first menu ready

  // Main menu stuff
  wifiMenu.list = new LinkedList<MenuNode>(); // Get list in second menu ready
  bluetoothMenu.list = new LinkedList<MenuNode>(); // Get list in third menu ready
  deviceMenu.list = new LinkedList<MenuNode>();
  #ifdef HAS_GPS
    if (gps_obj.getGpsModuleStatus()) {
      gpsInfoMenu.list = new LinkedList<MenuNode>();
    }
  #endif

  // Device menu stuff
  failedUpdateMenu.list = new LinkedList<MenuNode>();
  whichUpdateMenu.list = new LinkedList<MenuNode>();
  confirmMenu.list = new LinkedList<MenuNode>();
  updateMenu.list = new LinkedList<MenuNode>();
  settingsMenu.list = new LinkedList<MenuNode>();
  specSettingMenu.list = new LinkedList<MenuNode>();
  infoMenu.list = new LinkedList<MenuNode>();
  languageMenu.list = new LinkedList<MenuNode>();

  // WiFi menu stuff
  wifiSnifferMenu.list = new LinkedList<MenuNode>();
  wifiAttackMenu.list = new LinkedList<MenuNode>();
  wifiGeneralMenu.list = new LinkedList<MenuNode>();
  wifiAPMenu.list = new LinkedList<MenuNode>();

  // WiFi HTML menu stuff
  htmlMenu.list = new LinkedList<MenuNode>();

  // Bluetooth menu stuff
  bluetoothSnifferMenu.list = new LinkedList<MenuNode>();
  bluetoothAttackMenu.list = new LinkedList<MenuNode>();

  // Settings stuff
  generateSSIDsMenu.list = new LinkedList<MenuNode>();
  clearSSIDsMenu.list = new LinkedList<MenuNode>();
  clearAPsMenu.list = new LinkedList<MenuNode>();

  // Work menu names
  mainMenu.name = text_table1[6];
  wifiMenu.name = text_table1[7];
  deviceMenu.name = text_table1[9];
  failedUpdateMenu.name = text_table1[11];
  whichUpdateMenu.name = text_table1[12];
  confirmMenu.name = text_table1[13];
  updateMenu.name = text_table1[15];
  languageMenu.name = text_table1[16]; 
  infoMenu.name = text_table1[17];
  settingsMenu.name = text_table1[18];
  bluetoothMenu.name = text_table1[19];
  wifiSnifferMenu.name = text_table1[20];
  wifiAttackMenu.name = text_table1[21];
  wifiGeneralMenu.name = text_table1[22];
  bluetoothSnifferMenu.name = text_table1[23];
  bluetoothAttackMenu.name = "Bluetooth Attacks";
  generateSSIDsMenu.name = text_table1[27];
  clearSSIDsMenu.name = text_table1[28];
  clearAPsMenu.name = text_table1[29];
  wifiAPMenu.name = "Access Points";
  #ifdef HAS_GPS
    gpsInfoMenu.name = "GPS Data";
  #endif  
  htmlMenu.name = "EP HTML List";

  // Build Main Menu
  mainMenu.parentMenu = NULL;
  this->addNodes(&mainMenu, text_table1[7], TFT_GREEN, NULL, WIFI, [this]() {
    this->changeMenu(&wifiMenu);
  });
  this->addNodes(&mainMenu, text_table1[19], TFT_CYAN, NULL, BLUETOOTH, [this]() {
    this->changeMenu(&bluetoothMenu);
  });
  this->addNodes(&mainMenu, text_table1[9], TFT_BLUE, NULL, DEVICE, [this]() {
    this->changeMenu(&deviceMenu);
  });
  this->addNodes(&mainMenu, text_table1[30], TFT_LIGHTGREY, NULL, REBOOT, []() {
    ESP.restart();
  });

  // Build WiFi Menu
  wifiMenu.parentMenu = &mainMenu; // Main Menu is second menu parent
  this->addNodes(&wifiMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    this->changeMenu(wifiMenu.parentMenu);
  });
  this->addNodes(&wifiMenu, text_table1[31], TFT_YELLOW, NULL, SNIFFERS, [this]() {
    this->changeMenu(&wifiSnifferMenu);
  });
  this->addNodes(&wifiMenu, text_table1[32], TFT_RED, NULL, ATTACKS, [this]() {
    this->changeMenu(&wifiAttackMenu);
  });
  this->addNodes(&wifiMenu, text_table1[33], TFT_PURPLE, NULL, GENERAL_APPS, [this]() {
    this->changeMenu(&wifiGeneralMenu);
  });

  // Build WiFi sniffer Menu
  wifiSnifferMenu.parentMenu = &wifiMenu; // Main Menu is second menu parent
  this->addNodes(&wifiSnifferMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    this->changeMenu(wifiSnifferMenu.parentMenu);
  });
  this->addNodes(&wifiSnifferMenu, text_table1[42], TFT_CYAN, NULL, PROBE_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_PROBE, TFT_CYAN);
  });
  this->addNodes(&wifiSnifferMenu, text_table1[43], TFT_MAGENTA, NULL, BEACON_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_AP, TFT_MAGENTA);
  });
  this->addNodes(&wifiSnifferMenu, text_table1[44], TFT_RED, NULL, DEAUTH_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_DEAUTH, TFT_RED);
  });
  #ifdef HAS_ILI9341
    this->addNodes(&wifiSnifferMenu, text_table1[46], TFT_VIOLET, NULL, EAPOL, [this]() {
      wifi_scan_obj.StartScan(WIFI_SCAN_EAPOL, TFT_VIOLET);
    });
    this->addNodes(&wifiSnifferMenu, text_table1[45], TFT_BLUE, NULL, PACKET_MONITOR, [this]() {
      wifi_scan_obj.StartScan(WIFI_PACKET_MONITOR, TFT_BLUE);
    });
  #else
    this->addNodes(&wifiSnifferMenu, text_table1[46], TFT_VIOLET, NULL, EAPOL, [this]() {
      display_obj.clearScreen();
      this->drawStatusBar();
      wifi_scan_obj.StartScan(WIFI_SCAN_EAPOL, TFT_VIOLET);
    });
    this->addNodes(&wifiSnifferMenu, text_table1[45], TFT_BLUE, NULL, PACKET_MONITOR, [this]() {
      display_obj.clearScreen();
      this->drawStatusBar();
      wifi_scan_obj.StartScan(WIFI_PACKET_MONITOR, TFT_BLUE);
    });
  #endif
  #ifndef HAS_ILI9341
    this->addNodes(&wifiSnifferMenu, text_table1[47], TFT_RED, NULL, PWNAGOTCHI, [this]() {
      display_obj.clearScreen();
      this->drawStatusBar();
      wifi_scan_obj.StartScan(WIFI_SCAN_PWN, TFT_RED);
    });
  #endif
  this->addNodes(&wifiSnifferMenu, text_table1[49], TFT_MAGENTA, NULL, BEACON_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_TARGET_AP, TFT_MAGENTA);
  });
  this->addNodes(&wifiSnifferMenu, text_table1[58], TFT_WHITE, NULL, PACKET_MONITOR, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_RAW_CAPTURE, TFT_WHITE);
  });
  this->addNodes(&wifiSnifferMenu, text_table1[59], TFT_ORANGE, NULL, PACKET_MONITOR, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_STATION, TFT_WHITE);
  });
  #ifdef HAS_ILI9341
    this->addNodes(&wifiSnifferMenu, "Signal Monitor", TFT_CYAN, NULL, PACKET_MONITOR, [this]() {
      display_obj.clearScreen();
      this->drawStatusBar();
      wifi_scan_obj.StartScan(WIFI_SCAN_SIG_STREN, TFT_CYAN);
    });
  #endif
  #ifdef HAS_GPS
    if (gps_obj.getGpsModuleStatus()) {
      this->addNodes(&wifiSnifferMenu, "Wardrive", TFT_GREEN, NULL, BEACON_SNIFF, [this]() {
        display_obj.clearScreen();
        this->drawStatusBar();
        wifi_scan_obj.StartScan(WIFI_SCAN_WAR_DRIVE, TFT_GREEN);
      });
    }
  #endif
  #ifdef HAS_GPS
    if (gps_obj.getGpsModuleStatus()) {
      this->addNodes(&wifiSnifferMenu, "Station Wardrive", TFT_ORANGE, NULL, PROBE_SNIFF, [this]() {
        display_obj.clearScreen();
        this->drawStatusBar();
        wifi_scan_obj.StartScan(WIFI_SCAN_STATION_WAR_DRIVE, TFT_ORANGE);
      });
    }
  #endif

  // Build WiFi attack menu
  wifiAttackMenu.parentMenu = &wifiMenu; // Main Menu is second menu parent
  this->addNodes(&wifiAttackMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    this->changeMenu(wifiAttackMenu.parentMenu);
  });
  this->addNodes(&wifiAttackMenu, text_table1[50], TFT_RED, NULL, BEACON_LIST, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_ATTACK_BEACON_LIST, TFT_RED);
  });
  this->addNodes(&wifiAttackMenu, text_table1[51], TFT_ORANGE, NULL, BEACON_SPAM, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_ATTACK_BEACON_SPAM, TFT_ORANGE);
  });
  this->addNodes(&wifiAttackMenu, text_table1[52], TFT_YELLOW, NULL, RICK_ROLL, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_ATTACK_RICK_ROLL, TFT_YELLOW);
  });
  this->addNodes(&wifiAttackMenu, text_table1[53], TFT_RED, NULL, PROBE_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_ATTACK_AUTH, TFT_RED);
  });
  this->addNodes(&wifiAttackMenu, "Evil Portal", TFT_ORANGE, NULL, BEACON_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_EVIL_PORTAL, TFT_ORANGE);
  });
  this->addNodes(&wifiAttackMenu, text_table1[54], TFT_RED, NULL, DEAUTH_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_ATTACK_DEAUTH, TFT_RED);
  });
  this->addNodes(&wifiAttackMenu, text_table1[57], TFT_MAGENTA, NULL, BEACON_LIST, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_ATTACK_AP_SPAM, TFT_MAGENTA);
  });
  this->addNodes(&wifiAttackMenu, text_table1[62], TFT_RED, NULL, DEAUTH_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_ATTACK_DEAUTH_TARGETED, TFT_ORANGE);
  });

  // Build WiFi General menu
  wifiGeneralMenu.parentMenu = &wifiMenu;
  this->addNodes(&wifiGeneralMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    this->changeMenu(wifiGeneralMenu.parentMenu);
  });
  this->addNodes(&wifiGeneralMenu, text_table1[27], TFT_SKYBLUE, NULL, GENERATE, [this]() {
    this->changeMenu(&generateSSIDsMenu);
    wifi_scan_obj.RunGenerateSSIDs();
  });
  #ifdef HAS_ILI9341
    this->addNodes(&wifiGeneralMenu, text_table1[1], TFT_NAVY, NULL, KEYBOARD_ICO, [this](){
      display_obj.clearScreen(); 
      wifi_scan_obj.StartScan(LV_ADD_SSID, TFT_YELLOW); 
      addSSIDGFX();
    });
  #endif
  this->addNodes(&wifiGeneralMenu, text_table1[28], TFT_SILVER, NULL, CLEAR_ICO, [this]() {
    this->changeMenu(&clearSSIDsMenu);
    wifi_scan_obj.RunClearSSIDs();
  });
  this->addNodes(&wifiGeneralMenu, text_table1[29], TFT_DARKGREY, NULL, CLEAR_ICO, [this]() {
    this->changeMenu(&clearAPsMenu);
    wifi_scan_obj.RunClearAPs();
  });
  this->addNodes(&wifiGeneralMenu, text_table1[60], TFT_BLUE, NULL, CLEAR_ICO, [this]() {
    this->changeMenu(&clearAPsMenu);
    wifi_scan_obj.RunClearStations();
  });
  #ifdef HAS_ILI9341
    // Select APs on OG
    this->addNodes(&wifiGeneralMenu, text_table1[56], TFT_NAVY, NULL, KEYBOARD_ICO, [this](){
      display_obj.clearScreen(); 
      wifi_scan_obj.currentScanMode = LV_ADD_SSID; 
      wifi_scan_obj.StartScan(LV_ADD_SSID, TFT_RED);  
      addAPGFX();
    });
    this->addNodes(&wifiGeneralMenu, text_table1[61], TFT_LIGHTGREY, NULL, KEYBOARD_ICO, [this](){
      display_obj.clearScreen(); 
      wifi_scan_obj.currentScanMode = LV_ADD_SSID; 
      wifi_scan_obj.StartScan(LV_ADD_SSID, TFT_RED);  
      addStationGFX();
    });
    this->addNodes(&wifiGeneralMenu, "Select EP HTML File", TFT_CYAN, NULL, KEYBOARD_ICO, [this](){
      display_obj.clearScreen(); 
      wifi_scan_obj.currentScanMode = LV_ADD_SSID; 
      wifi_scan_obj.StartScan(LV_ADD_SSID, TFT_RED);  
      selectEPHTMLGFX();
    });
  #else
    this->addNodes(&wifiGeneralMenu, "Select EP HTML File", TFT_CYAN, NULL, KEYBOARD_ICO, [this](){
      this->changeMenu(&htmlMenu);
      #ifdef HAS_BUTTONS
        #if !(defined(MARAUDER_V6) || defined(MARAUDER_V6_1))
          while(true) {
            if (d_btn.justPressed()) {
              if (evil_portal_obj.selected_html_index > 0)
                evil_portal_obj.selected_html_index--;
              else
                evil_portal_obj.selected_html_index = evil_portal_obj.html_files->size() - 1;

              //Serial.println("Setting button text as " + evil_portal_obj.html_files->get(evil_portal_obj.selected_html_index));
              this->htmlMenu.list->set(0, MenuNode{evil_portal_obj.html_files->get(evil_portal_obj.selected_html_index), false, TFT_CYAN, 0, NULL, true, NULL});
              this->buildButtons(&htmlMenu);
              this->displayCurrentMenu();
            }
            #ifndef MARAUDER_M5STICKC
              if (u_btn.justPressed()) {
                if (evil_portal_obj.selected_html_index < evil_portal_obj.html_files->size() - 1)
                  evil_portal_obj.selected_html_index++;
                else
                  evil_portal_obj.selected_html_index = 0;

                //Serial.println("Setting button text as " + evil_portal_obj.html_files->get(evil_portal_obj.selected_html_index));
                this->htmlMenu.list->set(0, MenuNode{evil_portal_obj.html_files->get(evil_portal_obj.selected_html_index), false, TFT_CYAN, 0, NULL, true, NULL});
                this->buildButtons(&htmlMenu, 0, evil_portal_obj.html_files->get(evil_portal_obj.selected_html_index));
                this->displayCurrentMenu();
              }
            #endif
            if (c_btn.justPressed()) {
              if (evil_portal_obj.html_files->get(evil_portal_obj.selected_html_index) != "Back") {
                evil_portal_obj.target_html_name = evil_portal_obj.html_files->get(evil_portal_obj.selected_html_index);
                Serial.println("Set Evil Portal HTML as " + evil_portal_obj.target_html_name);
                evil_portal_obj.using_serial_html = false;
              }
              this->changeMenu(htmlMenu.parentMenu);
              break;
            }
          }
        #endif
      #endif
    });

    htmlMenu.parentMenu = &wifiGeneralMenu;
    this->addNodes(&htmlMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
      this->changeMenu(htmlMenu.parentMenu);
    });

    /*int loopLimit = min(evil_portal_obj.html_files->size(), BUTTON_ARRAY_LEN);

    for (int i = 0; i < loopLimit - 1; i++) {
      this->addNodes(&htmlMenu, evil_portal_obj.html_files->get(i), TFT_CYAN, NULL, 0, [this, i]() {
        evil_portal_obj.target_html_name = (String)evil_portal_obj.html_files->get(i);
        Serial.println("Set Evil Portal HTML as " + evil_portal_obj.target_html_name);
        evil_portal_obj.using_serial_html = false;
        this->changeMenu(htmlMenu.parentMenu);
      });
    }*/

    // Select APs on Mini
    this->addNodes(&wifiGeneralMenu, text_table1[56], TFT_NAVY, NULL, KEYBOARD_ICO, [this](){
      wifiAPMenu.list->clear();
        this->addNodes(&wifiAPMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
        this->changeMenu(wifiAPMenu.parentMenu);
      });
      int menu_limit;
      if (access_points->size() <= BUTTON_ARRAY_LEN)
        menu_limit = access_points->size();
      else
        menu_limit = BUTTON_ARRAY_LEN;
      for (int i = 0; i < menu_limit - 1; i++) {
        this->addNodes(&wifiAPMenu, access_points->get(i).essid, TFT_CYAN, NULL, KEYBOARD_ICO, [this, i](){
        AccessPoint new_ap = access_points->get(i);
        new_ap.selected = !access_points->get(i).selected;

        // Change selection status of menu node
        MenuNode new_node = current_menu->list->get(i + 1);
        new_node.selected = !current_menu->list->get(i + 1).selected;
        current_menu->list->set(i + 1, new_node);

        // Change selection status of button key
        if (new_ap.selected) {
          this->buttonSelected(i + 1);
        } else {
          this->buttonNotSelected(i + 1);
        }
        access_points->set(i, new_ap);
        }, access_points->get(i).selected);
      }
      this->changeMenu(&wifiAPMenu);
    });

    wifiAPMenu.parentMenu = &wifiGeneralMenu;
    this->addNodes(&wifiAPMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
      this->changeMenu(wifiAPMenu.parentMenu);
    });
  #endif

  // Build generate ssids menu
  generateSSIDsMenu.parentMenu = &wifiGeneralMenu;
  this->addNodes(&generateSSIDsMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    this->changeMenu(generateSSIDsMenu.parentMenu);
  });

  // Build clear ssids menu
  clearSSIDsMenu.parentMenu = &wifiGeneralMenu;
  this->addNodes(&clearSSIDsMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    this->changeMenu(clearSSIDsMenu.parentMenu);
  });
  clearAPsMenu.parentMenu = &wifiGeneralMenu;
  this->addNodes(&clearAPsMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    this->changeMenu(clearAPsMenu.parentMenu);
  });


  // Build Bluetooth Menu
  bluetoothMenu.parentMenu = &mainMenu; // Second Menu is third menu parent
  this->addNodes(&bluetoothMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    this->changeMenu(bluetoothMenu.parentMenu);
  });
  this->addNodes(&bluetoothMenu, text_table1[31], TFT_YELLOW, NULL, SNIFFERS, [this]() {
    this->changeMenu(&bluetoothSnifferMenu);
  });
  this->addNodes(&bluetoothMenu, "Bluetooth Attacks", TFT_RED, NULL, ATTACKS, [this]() {
    this->changeMenu(&bluetoothAttackMenu);
  });

  // Build bluetooth sniffer Menu
  bluetoothSnifferMenu.parentMenu = &bluetoothMenu; // Second Menu is third menu parent
  this->addNodes(&bluetoothSnifferMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    this->changeMenu(bluetoothSnifferMenu.parentMenu);
  });
  this->addNodes(&bluetoothSnifferMenu, text_table1[34], TFT_GREEN, NULL, BLUETOOTH_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(BT_SCAN_ALL, TFT_GREEN);
  });
  #ifdef HAS_GPS
    if (gps_obj.getGpsModuleStatus()) {
      this->addNodes(&bluetoothSnifferMenu, "BT Wardrive", TFT_CYAN, NULL, BLUETOOTH_SNIFF, [this]() {
        display_obj.clearScreen();
        this->drawStatusBar();
        wifi_scan_obj.StartScan(BT_SCAN_WAR_DRIVE, TFT_GREEN);
      });
      this->addNodes(&bluetoothSnifferMenu, "BT Wardrive Continuous", TFT_RED, NULL, REBOOT, [this]() {
        display_obj.clearScreen();
        this->drawStatusBar();
        wifi_scan_obj.StartScan(BT_SCAN_WAR_DRIVE_CONT, TFT_GREEN);
      });
    }
  #endif
  this->addNodes(&bluetoothSnifferMenu, text_table1[35], TFT_MAGENTA, NULL, CC_SKIMMERS, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(BT_SCAN_SKIMMERS, TFT_MAGENTA);
  });

  // Bluetooth Attack menu
  bluetoothAttackMenu.parentMenu = &bluetoothMenu; // Second Menu is third menu parent
  this->addNodes(&bluetoothAttackMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    this->changeMenu(bluetoothAttackMenu.parentMenu);
  });
  this->addNodes(&bluetoothAttackMenu, "Sour Apple", TFT_GREEN, NULL, DEAUTH_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(BT_ATTACK_SOUR_APPLE, TFT_GREEN);
  });
  this->addNodes(&bluetoothAttackMenu, "Swiftpair Spam", TFT_CYAN, NULL, KEYBOARD_ICO, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(BT_ATTACK_SWIFTPAIR_SPAM, TFT_CYAN);
  });
  this->addNodes(&bluetoothAttackMenu, "Samsung BLE Spam", TFT_RED, NULL, GENERAL_APPS, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(BT_ATTACK_SAMSUNG_SPAM, TFT_RED);
  });
  this->addNodes(&bluetoothAttackMenu, "Google BLE Spam", TFT_PURPLE, NULL, LANGUAGE, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(BT_ATTACK_GOOGLE_SPAM, TFT_RED);
  });
  this->addNodes(&bluetoothAttackMenu, "BLE Spam All", TFT_MAGENTA, NULL, DEAUTH_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(BT_ATTACK_SPAM_ALL, TFT_MAGENTA);
  });

  // Device menu
  deviceMenu.parentMenu = &mainMenu;
  this->addNodes(&deviceMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    this->changeMenu(deviceMenu.parentMenu);
  });
  this->addNodes(&deviceMenu, text_table1[15], TFT_ORANGE, NULL, UPDATE, [this]() {
    wifi_scan_obj.currentScanMode = OTA_UPDATE;
    this->changeMenu(&whichUpdateMenu);
  });

  this->addNodes(&deviceMenu, text_table1[16], TFT_GREEN, NULL, LANGUAGE, [this]() {

    wifi_scan_obj.currentScanMode = SHOW_INFO;
    this->changeMenu(&languageMenu);   
  });
  this->addNodes(&deviceMenu, text_table1[17], TFT_WHITE, NULL, DEVICE_INFO, [this]() {
    wifi_scan_obj.currentScanMode = SHOW_INFO;
    this->changeMenu(&infoMenu);
    wifi_scan_obj.RunInfo();
  });
  this->addNodes(&deviceMenu, text08, TFT_NAVY, NULL, KEYBOARD_ICO, [this]() {
    this->changeMenu(&settingsMenu);
  });
  // GPS Menu
  #ifdef HAS_GPS
    if (gps_obj.getGpsModuleStatus()) {
      this->addNodes(&deviceMenu, "GPS Data", TFT_RED, NULL, GPS_MENU, [this]() {
        wifi_scan_obj.currentScanMode = WIFI_SCAN_GPS_DATA;
        this->changeMenu(&gpsInfoMenu);
        wifi_scan_obj.StartScan(WIFI_SCAN_GPS_DATA, TFT_CYAN);
      });

      this->addNodes(&deviceMenu, "NMEA Stream", TFT_ORANGE, NULL, GPS_MENU, [this]() {
        wifi_scan_obj.currentScanMode = WIFI_SCAN_GPS_NMEA;
        this->changeMenu(&gpsInfoMenu);
        wifi_scan_obj.StartScan(WIFI_SCAN_GPS_NMEA, TFT_ORANGE);
      });

      // GPS Info Menu
      gpsInfoMenu.parentMenu = &deviceMenu;
      this->addNodes(&gpsInfoMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
        wifi_scan_obj.currentScanMode = WIFI_SCAN_OFF;
        this->changeMenu(gpsInfoMenu.parentMenu);
      }); 
    }
  #endif

  // Settings menu
  // Device menu
  settingsMenu.parentMenu = &deviceMenu;
  this->addNodes(&settingsMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    changeMenu(settingsMenu.parentMenu);
  });
  for (int i = 0; i < settings_obj.getNumberSettings(); i++) {
    if (this->callSetting(settings_obj.setting_index_to_name(i)) == "bool")
      this->addNodes(&settingsMenu, settings_obj.setting_index_to_name(i), TFT_LIGHTGREY, NULL, 0, [this, i]() {
      settings_obj.toggleSetting(settings_obj.setting_index_to_name(i));
      this->changeMenu(&specSettingMenu);
      this->displaySetting(settings_obj.setting_index_to_name(i), &settingsMenu, i + 1);
    }, settings_obj.loadSetting<bool>(settings_obj.setting_index_to_name(i)));
  }

  // Specific setting menu
  specSettingMenu.parentMenu = &settingsMenu;
  addNodes(&specSettingMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    this->changeMenu(specSettingMenu.parentMenu);
  });
 
  // Select update
  whichUpdateMenu.parentMenu = &deviceMenu;
  this->addNodes(&whichUpdateMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    wifi_scan_obj.currentScanMode = WIFI_SCAN_OFF;
    this->changeMenu(whichUpdateMenu.parentMenu);
  });
  #ifndef WRITE_PACKETS_SERIAL
    if (sd_obj.supported) addNodes(&whichUpdateMenu, text_table1[40], TFT_MAGENTA, NULL, SD_UPDATE, [this]() {
      wifi_scan_obj.currentScanMode = OTA_UPDATE;
      this->changeMenu(&confirmMenu);
    });
  #endif

  // Confirm SD update menu
  confirmMenu.parentMenu = &whichUpdateMenu;
  this->addNodes(&confirmMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    this->changeMenu(confirmMenu.parentMenu);
  });
  this->addNodes(&confirmMenu, text14, TFT_ORANGE, NULL, UPDATE, [this]() {
    wifi_scan_obj.currentScanMode = OTA_UPDATE;
    this->changeMenu(&failedUpdateMenu);
    sd_obj.runUpdate();
  });

  // Web Update
  updateMenu.parentMenu = &deviceMenu;

  // Failed update menu
  failedUpdateMenu.parentMenu = &whichUpdateMenu;
  this->addNodes(&failedUpdateMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    wifi_scan_obj.currentScanMode = WIFI_SCAN_OFF;
    this->changeMenu(failedUpdateMenu.parentMenu);
  });

  // Device info menu
  infoMenu.parentMenu = &deviceMenu;
  this->addNodes(&infoMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    wifi_scan_obj.currentScanMode = WIFI_SCAN_OFF;
    this->changeMenu(infoMenu.parentMenu);
  });
  //language info menu
  languageMenu.parentMenu = &deviceMenu;
    this->addNodes(&languageMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
      wifi_scan_obj.currentScanMode = WIFI_SCAN_OFF;
      this->changeMenu(infoMenu.parentMenu);
    });
  // Set the current menu to the mainMenu
  this->changeMenu(&mainMenu);

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
  for (uint8_t i = 0; i < menu->list->size(); i++)
  {
    // Depending on layer, indent
    for (uint8_t x = 0; x < layer * 4; x++)
      Serial.print(" ");
    Serial.print("Node: ");
    Serial.println(menu->list->get(i).name);
  }
  Serial.println();
}


// Function to add MenuNodes to a menu
void MenuFunctions::addNodes(Menu * menu, String name, uint16_t color, Menu * child, int place, std::function<void()> callable, bool selected, String command)
{
  TFT_eSPI_Button new_button;
  menu->list->add(MenuNode{name, false, color, place, &new_button, selected, callable});
  //menu->list->add(MenuNode{name, false, color, place, selected, callable});
}

void MenuFunctions::buildButtons(Menu * menu, int starting_index, String button_name)
{
  if (menu->list != NULL)
  {
    this->menu_start_index = starting_index;
    for (uint8_t i = 0; i < menu->list->size(); i++)
    {
      TFT_eSPI_Button new_button;
      char buf[menu->list->get(starting_index + i).name.length() + 1] = {};
      if (button_name != "")
        menu->list->get(starting_index + i).name.toCharArray(buf, menu->list->get(starting_index + i).name.length() + 1);
      else
        button_name.toCharArray(buf, button_name.length() + 1);
      display_obj.key[i].initButton(&display_obj.tft,
                                    KEY_X + 0 * (KEY_W + KEY_SPACING_X),
                                    KEY_Y + i * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                                    KEY_W,
                                    KEY_H,
                                    TFT_BLACK, // Outline
                                    TFT_BLACK, // Fill
                                    menu->list->get(starting_index + i).color, // Text
                                    buf,
                                    KEY_TEXTSIZE);

      display_obj.key[i].setLabelDatum(BUTTON_PADDING - (KEY_W / 2), 2, ML_DATUM);
    }
  }
}


void MenuFunctions::displayCurrentMenu(uint8_t start_index)
{
  //Serial.println(F("Displaying current menu..."));
  display_obj.clearScreen();
  display_obj.updateBanner(current_menu->name);
  display_obj.tft.setTextColor(TFT_LIGHTGREY, TFT_DARKGREY);
  this->drawStatusBar();

  if (current_menu->list != NULL)
  {
    #ifdef HAS_ILI9341
      display_obj.tft.setFreeFont(MENU_FONT);
    #endif

    #if defined(MARAUDER_MINI) || defined(MARAUDER_M5STICKC)
      display_obj.tft.setFreeFont(NULL);
      display_obj.tft.setTextSize(1);
    #endif
    for (uint8_t i = start_index; i < current_menu->list->size(); i++)
    {
      #ifdef HAS_ILI9341
        if (!current_menu->list->get(i).selected)
          display_obj.key[i].drawButton(false, current_menu->list->get(i).name);
        else
          display_obj.key[i].drawButton(true, current_menu->list->get(i).name);
        
        if (current_menu->list->get(i).name != text09)
          display_obj.tft.drawXBitmap(0,
                                      KEY_Y + i * (KEY_H + KEY_SPACING_Y) - (ICON_H / 2),
                                      menu_icons[current_menu->list->get(i).icon],
                                      ICON_W,
                                      ICON_H,
                                      TFT_BLACK,
                                      current_menu->list->get(i).color);

      #endif

      #if defined(MARAUDER_MINI) || defined(MARAUDER_M5STICKC)
        if ((current_menu->selected == i) || (current_menu->list->get(i).selected))
          display_obj.key[i - start_index].drawButton(true, current_menu->list->get(i).name);
        else 
          display_obj.key[i - start_index].drawButton(false, current_menu->list->get(i).name);
      #endif
    }
    display_obj.tft.setFreeFont(NULL);
  }
}

#endif
