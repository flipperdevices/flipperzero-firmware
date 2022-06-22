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

#ifndef MARAUDER_MINI
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
  
      //if(data->state == LV_INDEV_STATE_PR) touchpad_get_xy(&last_x, &last_y);
     
      data->point.x = touchX;
      data->point.y = touchY;
  
      //Serial.print("Data x");
      //Serial.println(touchX);
      
      //Serial.print("Data y");
      //Serial.println(touchY);
  
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
  
  void MenuFunctions::writeBadUSB(){
    // Create a keyboard and apply the styles
    kb = lv_keyboard_create(lv_scr_act(), NULL);
    lv_obj_set_size(kb, LV_HOR_RES, LV_VER_RES / 2);
    lv_obj_set_event_cb(kb, write_bad_usb_keyboard_event_cb);
  
    // Create one text area
    // Store all SSIDs
    ta1 = lv_textarea_create(lv_scr_act(), NULL);
    lv_textarea_set_cursor_hidden(ta1, false);
    lv_textarea_set_one_line(ta1, false);
    lv_obj_set_width(ta1, LV_HOR_RES);
    lv_obj_set_height(ta1, (LV_VER_RES / 2) - 35);
    lv_obj_set_pos(ta1, 5, 20);
    lv_textarea_set_cursor_hidden(ta1, true);
    lv_obj_align(ta1, NULL, LV_ALIGN_IN_TOP_MID, NULL, NULL);
    lv_textarea_set_text(ta1, "");
    lv_textarea_set_placeholder_text(ta1, "Ducky script");
  
    if (sd_obj.supported) {
      // Create load button
      lv_obj_t * label;
      lv_obj_t * load_btn = lv_btn_create(lv_scr_act(), NULL);
      lv_obj_set_event_cb(load_btn, load_btn_cb);
      lv_obj_set_height(load_btn, 35);
      lv_obj_set_width(load_btn, LV_HOR_RES / 3);
      lv_obj_align(load_btn, ta1, LV_ALIGN_IN_TOP_RIGHT, NULL, (LV_VER_RES / 2) - 35); // align to text area
      label = lv_label_create(load_btn, NULL);
      lv_label_set_text(label, text05);
    
      // Create Save As button
      lv_obj_t * label2;
      lv_obj_t * save_as_btn = lv_btn_create(lv_scr_act(), NULL);
      lv_obj_set_event_cb(save_as_btn, load_btn_cb);
      lv_obj_set_height(save_as_btn, 35);
      lv_obj_set_width(save_as_btn, LV_HOR_RES / 3);
      lv_obj_align(save_as_btn, ta1, LV_ALIGN_IN_TOP_MID, NULL, (LV_VER_RES / 2) - 35); // align to text area
      label2 = lv_label_create(save_as_btn, NULL);
      lv_label_set_text(label2, text06);
    }
    
    // Focus it on one of the text areas to start
    lv_keyboard_set_textarea(kb, ta1);
    lv_keyboard_set_cursor_manage(kb, true);
  }
  
  // Event handler for settings drop down menus
  void setting_dropdown_cb(lv_obj_t * obj, lv_event_t event) {
    //lv_event_code_t code = lv_event_get_code(event);
    //lv_obj_t * obj = lv_event_get_target(event);
    //lv_obj_t * list1 = lv_obj_get_parent(lv_obj_get_parent(obj));
    //if(event == LV_EVENT_CLICKED) {
    //    LV_LOG_USER("Clicked: %s", lv_list_get_btn_text(list1, obj));
    //}
  }
  
  void settings_list_cb(lv_obj_t * btn, lv_event_t event) {
    extern Settings settings_obj;
    extern MenuFunctions menu_function_obj;
  
    String btn_text = lv_list_get_btn_text(btn);
    String display_string = "";
    
    if (event == LV_EVENT_CLICKED) {
      if (btn_text == text07) {
        Serial.println("Exiting...");
        lv_obj_del_async(lv_obj_get_parent(lv_obj_get_parent(btn)));
  
        printf("LV_EVENT_CANCEL\n");
        Serial.println("Potato");
        //menu_function_obj.deinitLVGL();
        //wifi_scan_obj.StartScan(WIFI_SCAN_OFF);
        //display_obj.exit_draw = true; // set everything back to normal
      }
      else {
        // Build base obj to host buttons
        Serial.println("Creating base object...");
        lv_obj_t * obj;
        obj = lv_obj_create(lv_scr_act(), NULL);
        lv_obj_set_size(obj, LV_HOR_RES, LV_VER_RES);
        
        lv_obj_t * exit_btn;
  
        lv_obj_t * label;
        
        // Build the generic Exit button
        exit_btn = lv_btn_create(obj, NULL);
        lv_obj_set_event_cb(exit_btn, settings_list_cb);
        lv_label_set_text(label, text07);
        //lv_obj_center(label);
  
        label = lv_label_create(exit_btn, NULL);
  
        // Create the type specific device
        if (settings_obj.getSettingType(btn_text) == "bool") {
          lv_obj_t * sw = lv_switch_create(obj, NULL);
          lv_obj_align(sw, NULL, LV_ALIGN_CENTER, 0, 0);
        }
      }
    }
  
    /*
    if (event == LV_EVENT_VALUE_CHANGED) {      
      if (lv_btn_get_state(btn) == LV_BTN_STATE_CHECKED_RELEASED) {
        //Serial.print("Toggle on: ");
        //Serial.println(btn_text);
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
        //Serial.print("Toggle off: ");
        //Serial.println(btn_text);
        for (int i = 0; i < access_points->size(); i++) {
          if (access_points->get(i).essid == btn_text) {
            Serial.println("Removing AP: " + (String)access_points->get(i).essid);
            AccessPoint ap = access_points->get(i);
            ap.selected = false;
            access_points->set(i, ap);
          }
        }
      }
    }*/
  }
  
  void MenuFunctions::displaySettingsGFX(){
    extern Settings settings_obj;
  
    DynamicJsonDocument json(1024); // ArduinoJson v6
  
    if (deserializeJson(json, settings_obj.getSettingsString())) {
      Serial.println("\nCould not parse json");
    }
    
    lv_obj_t * list1 = lv_list_create(lv_scr_act(), NULL);
    lv_obj_set_size(list1, 160, 200);
    lv_obj_set_width(list1, LV_HOR_RES);
    lv_obj_align(list1, NULL, LV_ALIGN_CENTER, 0, 0);
  
    lv_obj_t * list_btn;
  
    lv_obj_t * label;
  
    lv_obj_t * sw;
  
    list_btn = lv_list_add_btn(list1, LV_SYMBOL_CLOSE, text07);
    lv_obj_set_event_cb(list_btn, ap_list_cb);
  
    for (int i = 0; i < json[text09].size(); i++) {
      char buf[json[text08][i]["name"].as<String>().length() + 1] = {};
      json[text08][i]["name"].as<String>().toCharArray(buf, json[text08][i]["name"].as<String>().length() + 1);
      
      list_btn = lv_list_add_btn(list1, LV_SYMBOL_WIFI, buf);
      lv_btn_set_checkable(list_btn, false);
      lv_obj_set_event_cb(list_btn, settings_list_cb);
  
      //lv_list_add_text(list1, buf);
  
      // Create the dropdown menu
      /*lv_obj_t * dd = lv_dropdown_create(list1, NULL);
      lv_dropdown_set_options(dd, "Apple\n"
                                  "Banana\n"
                                  "Orange\n"
                                  "Cherry\n"
                                  "Grape\n"
                                  "Raspberry\n"
                                  "Melon\n"
                                  "Orange\n"
                                  "Lemon\n"
                                  "Nuts");
  
      //lv_obj_align(dd, LV_ALIGN_IN_RIGHT_MID, 0, 20);
      lv_obj_align(dd, NULL, LV_ALIGN_IN_RIGHT_MID, 0, 0);
      lv_obj_set_width(dd, LV_HOR_RES / 3);
      lv_obj_set_event_cb(dd, setting_dropdown_cb);
      //lv_obj_add_event_cb(dd, setting_dropdown_cb, LV_EVENT_ALL, NULL);*/
      
      //if (access_points->get(i).selected)
      //  lv_btn_toggle(list_btn);
  
      //lv_obj_t * btn1 = lv_btn_create(list_btn, NULL);
      //lv_obj_set_event_cb(btn1, ap_list_cb);
      //lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, 0, 0);
      //lv_btn_set_checkable(btn1, true);
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
  
      //lv_obj_t * btn1 = lv_btn_create(list_btn, NULL);
      //lv_obj_set_event_cb(btn1, ap_list_cb);
      //lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, 0, 0);
      //lv_btn_set_checkable(btn1, true);
  
      //label = lv_label_create(btn1, NULL);
      //lv_label_set_text(label, buf);
    }
  }
  
  
  
  void ap_list_cb(lv_obj_t * btn, lv_event_t event) {
    extern LinkedList<AccessPoint>* access_points;
    extern MenuFunctions menu_function_obj;
  
    String btn_text = lv_list_get_btn_text(btn);
    String display_string = "";
    
    if (event == LV_EVENT_CLICKED) {
      if (btn_text != text09) {
        //lv_list_focus_btn(lv_obj_get_parent(lv_obj_get_parent(btn)), btn);
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
        //Serial.print("Toggle on: ");
        //Serial.println(btn_text);
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
        //Serial.print("Toggle off: ");
        //Serial.println(btn_text);
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
  
  void MenuFunctions::joinWiFiGFX(){
  
    // Create one text area
    ta1 = lv_textarea_create(lv_scr_act(), NULL);
    lv_textarea_set_one_line(ta1, true);
    lv_obj_set_width(ta1, LV_HOR_RES / 2 - 20);
    lv_obj_set_pos(ta1, 5, 20);
    //lv_ta_set_cursor_type(ta, LV_CURSOR_BLOCK);
    lv_textarea_set_text(ta1, "");
    lv_obj_set_event_cb(ta1, ta_event_cb);
  
    // Create first label
    lv_obj_t * ssid_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(ssid_label, text_table1[2]);
    lv_obj_align(ssid_label, ta1, LV_ALIGN_OUT_TOP_LEFT, 0, 0);
  
    // Create second text area
    ta2 = lv_textarea_create(lv_scr_act(), ta1);
    //lv_textarea_set_pwd_mode(ta2, true); // This shit makes it so backspace does not work
    //lv_textarea_set_pwd_show_time(ta2, 1000);
    lv_textarea_set_cursor_hidden(ta2, true);
    lv_obj_align(ta2, NULL, LV_ALIGN_IN_TOP_RIGHT, -5, 20);
  
    // Create second label
    lv_obj_t * pw_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(pw_label, text_table1[3]);
    lv_obj_align(pw_label, ta2, LV_ALIGN_OUT_TOP_LEFT, 0, 0);
  
    // Create a keyboard and apply the styles
    kb = lv_keyboard_create(lv_scr_act(), NULL);
    lv_obj_set_size(kb, LV_HOR_RES, LV_VER_RES / 2);
    lv_obj_set_event_cb(kb, join_wifi_keyboard_event_cb);
  
    // Focus it on one of the text areas to start
    lv_keyboard_set_textarea(kb, ta1);
    lv_keyboard_set_cursor_manage(kb, true);
    
  }
  
  // Function to create keyboard for saving file name
  void save_as_keyboard_event_cb(lv_obj_t * keyboard, lv_event_t event) {
    extern MenuFunctions menu_function_obj;
  
    lv_keyboard_def_event_cb(save_as_kb, event);
  
    // User canceled so we will get rid of the keyboard and text box
    if (event == LV_EVENT_CANCEL) {
      lv_obj_del_async(save_as_kb);
      lv_obj_del_async(save_name);
    }
  
    // Save content from ta1 to file name in save_name
    else if(event == LV_EVENT_APPLY){
        String display_string = "";
        printf("LV_EVENT_APPLY\n");
  
        // Get ducky script
        String content = lv_textarea_get_text(ta1);
  
        String target_file_name = "/SCRIPTS/" + (String)lv_textarea_get_text(save_name);
  
        Serial.println("Writing to target file: " + (String)target_file_name);
  
        // Open file with the given name
        File script = SD.open(target_file_name, FILE_WRITE);
  
        if (script) {
          menu_function_obj.loaded_file = target_file_name;
  
          Serial.println("Writing content: ");
          Serial.println(content);
  
          script.print(content);
  
          script.close();
        }
  
        lv_obj_del_async(save_as_kb);
        lv_obj_del_async(save_name);
  
        // Create Save button
        lv_obj_t * save_label;
        lv_obj_t * save_btn = lv_btn_create(lv_scr_act(), NULL);
        lv_obj_set_event_cb(save_btn, load_btn_cb);
        lv_obj_set_height(save_btn, 35);
        lv_obj_set_width(save_btn, LV_HOR_RES / 3);
        lv_obj_align(save_btn, ta1, LV_ALIGN_IN_TOP_LEFT, NULL, (LV_VER_RES / 2) - 35); // align to text area
        save_label = lv_label_create(save_btn, NULL);
        lv_label_set_text(save_label, text13);
    }
  }
  
  
  void test_btn_cb(lv_obj_t * btn, lv_event_t event) {
    extern MenuFunctions menu_function_obj;
    
    if (event == LV_EVENT_CLICKED) {
      String btn_text = lv_list_get_btn_text(btn);
      String display_string = "";
      //printf("Clicked: %s\n", btn_text);
      Serial.print("Clicked: ");
      Serial.println(btn_text);
  
      // Get file content and send to text area
      if (btn_text != text12) {
        File script = SD.open(btn_text);
  
        if (script) {
          while (script.available()) {
            display_string.concat((char)script.read());
          }
          script.close();
  
          Serial.println(display_string);
  
          char buf[display_string.length() + 1] = {};
          display_string.toCharArray(buf, display_string.length() + 1);
          
          lv_textarea_set_text(ta1, buf);
  
          // Create Save button
          lv_obj_t * save_label;
          lv_obj_t * save_btn = lv_btn_create(lv_scr_act(), NULL);
          lv_obj_set_event_cb(save_btn, load_btn_cb);
          lv_obj_set_height(save_btn, 35);
          lv_obj_set_width(save_btn, LV_HOR_RES / 3);
          lv_obj_align(save_btn, ta1, LV_ALIGN_IN_TOP_LEFT, NULL, (LV_VER_RES / 2) - 35); // align to text area
          save_label = lv_label_create(save_btn, NULL);
          lv_label_set_text(save_label, text13);
        }
      }
  
      // Delete the file list obj
      lv_obj_del_async(lv_obj_get_parent(lv_obj_get_parent(btn)));
      menu_function_obj.loaded_file = btn_text;
    }
  }
  
  void load_btn_cb(lv_obj_t * load_btn, lv_event_t event) {
    extern SDInterface sd_obj;
    extern MenuFunctions menu_function_obj;
  
    String btn_text = lv_list_get_btn_text(load_btn);
  
    if (btn_text == "Load") {
      if (event == LV_EVENT_CLICKED)
        Serial.println("Load button pressed");
      else if (event == LV_EVENT_RELEASED) {
        Serial.println("Load button released");
        /*Create a list*/
        lv_obj_t * list1 = lv_list_create(lv_scr_act(), NULL);
        lv_obj_set_size(list1, 160, 200);
        lv_obj_set_width(list1, LV_HOR_RES);
        lv_obj_align(list1, NULL, LV_ALIGN_CENTER, 0, 0);
        //lv_list_set_anim_time(list1, 0);
    
        // Load file names into buttons
        File scripts = SD.open("/SCRIPTS");
    
        // Build list of files from the SD card
        lv_obj_t * list_btn;
  
        list_btn = lv_list_add_btn(list1, LV_SYMBOL_CLOSE, text12);
        lv_obj_set_event_cb(list_btn, test_btn_cb);
    
        while (true) {
          File entity = scripts.openNextFile();
    
          if (!entity)
            break;
    
          if (!entity.isDirectory()) {
            String file_name = entity.name();
    
            // Fancy button text time
            char buf[file_name.length() + 1] = {};
            file_name.toCharArray(buf, file_name.length() + 1);
            
            list_btn = lv_list_add_btn(list1, LV_SYMBOL_FILE, buf);
            lv_obj_set_event_cb(list_btn, test_btn_cb);
          }
    
          entity.close();
        }
    
        scripts.close();
      }
    }
  
    // Save current text bod content to new file
    else if (btn_text == text06) {
      if (event == LV_EVENT_CLICKED)
        Serial.println("Save button pressed");
      else if (event == LV_EVENT_RELEASED) {
        Serial.println("Save button released");
  
        save_name = lv_textarea_create(lv_scr_act(), ta2);
        lv_textarea_set_cursor_hidden(save_name, false);
        lv_textarea_set_one_line(save_name, true);
        lv_obj_align(save_name, NULL, LV_ALIGN_IN_TOP_MID, NULL, (LV_VER_RES / 2) - 35);
        lv_textarea_set_text(save_name, "");
        lv_textarea_set_placeholder_text(save_name, "File Name");
  
        // Create a keyboard and apply the styles
        save_as_kb = lv_keyboard_create(lv_scr_act(), NULL);
        lv_obj_set_size(save_as_kb, LV_HOR_RES, LV_VER_RES / 2);
        lv_obj_set_event_cb(save_as_kb, save_as_keyboard_event_cb);
  
        lv_keyboard_set_textarea(save_as_kb, save_name);
        lv_keyboard_set_cursor_manage(save_as_kb, true);
      }
    }
  
    // Save current text box content to current loaded file
    else if (btn_text == text13) {
      if (event == LV_EVENT_CLICKED)
        Serial.println("Save button pressed");
      else if (event == LV_EVENT_RELEASED) {
        Serial.println("Save button released");
  
        Serial.println("Writing to file: " + (String)menu_function_obj.loaded_file);
  
        File script = SD.open(menu_function_obj.loaded_file, FILE_WRITE);
  
        // Write data to file
        if (script) {
          String content = lv_textarea_get_text(ta1);
  
          Serial.println("Writing content:");
          Serial.println(content);
          Serial.println("to file: " + (String)menu_function_obj.loaded_file);
          script.print(lv_textarea_get_text(ta1));
          script.close();
        }
      }
    }
  }
  
  void write_bad_usb_keyboard_event_cb(lv_obj_t * keyboard, lv_event_t event) {
    extern Display display_obj;
    extern MenuFunctions menu_function_obj;
    extern A32u4Interface a32u4_obj;
    extern WiFiScan wifi_scan_obj;
    
    lv_keyboard_def_event_cb(kb, event);
    if(event == LV_EVENT_APPLY){
        String display_string = "";
        printf("LV_EVENT_APPLY\n");
  
      String ta1_text = lv_textarea_get_text(ta1);
    
      Serial.println(ta1_text);
    
      a32u4_obj.runScript(ta1_text);
    }
    else if(event == LV_EVENT_CANCEL) {
      printf("LV_EVENT_CANCEL\n");
      menu_function_obj.deinitLVGL();
      wifi_scan_obj.StartScan(WIFI_SCAN_OFF);
      display_obj.exit_draw = true; // set everything back to normal
    }
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
      //Serial.println(ta1_text);
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
      //lv_textarea_set_text(lv_keyboard_get_textarea(kb), "");
      menu_function_obj.deinitLVGL();
      //wifi_scan_obj.StartScan(WIFI_SCAN_OFF);
      display_obj.exit_draw = true; // set everything back to normal
    }
  }
  
  // Keyboard callback dedicated to joining wifi
  void join_wifi_keyboard_event_cb(lv_obj_t * keyboard, lv_event_t event){
    extern Display display_obj;
    extern MenuFunctions menu_function_obj;
    extern WiFiScan wifi_scan_obj;
    lv_keyboard_def_event_cb(kb, event);
    if(event == LV_EVENT_APPLY){
      printf("LV_EVENT_APPLY\n");
      //String ta1_text = lv_textarea_get_text(lv_keyboard_get_textarea(kb));
      String ta1_text = lv_textarea_get_text(ta1);
      String ta2_text = lv_textarea_get_text(ta2);
      Serial.println(ta1_text);
      Serial.println(ta2_text);
      wifi_scan_obj.joinWiFi(ta1_text, ta2_text);
    }else if(event == LV_EVENT_CANCEL){
      printf("LV_EVENT_CANCEL\n");
      //lv_textarea_set_text(lv_keyboard_get_textarea(kb), "");
      menu_function_obj.deinitLVGL();
      //wifi_scan_obj.StartScan(WIFI_SCAN_OFF);
      display_obj.exit_draw = true; // set everything back to normal
    }
  }
  
  
  void ta_event_cb(lv_obj_t * ta, lv_event_t event)
  {
    if(event == LV_EVENT_CLICKED) {
      if(kb != NULL)
        lv_keyboard_set_textarea(kb, ta);
    }
  
    //else if(event == LV_EVENT_INSERT) {
    //  const char * str = lv_event_get_data();
    //  if(str[0] == '\n') {
    //    printf("Ready\n");
    //  }
    //}
  }

#endif
//// END LV_ARDUINO STUFF

void MenuFunctions::buttonNotSelected(uint8_t b) {
  display_obj.tft.setFreeFont(NULL);
  display_obj.key[b].drawButton(false, current_menu->list->get(b).name);
}

void MenuFunctions::buttonSelected(uint8_t b) {
  display_obj.tft.setFreeFont(NULL);
  display_obj.key[b].drawButton(true, current_menu->list->get(b).name);
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
      (wifi_scan_obj.currentScanMode == SHOW_INFO)) {
    if (wifi_scan_obj.orient_display) {
      this->orientDisplay();
      wifi_scan_obj.orient_display = false;
    }
    #ifndef MARAUDER_MINI
      if ((wifi_scan_obj.currentScanMode != LV_JOIN_WIFI) &&
          (wifi_scan_obj.currentScanMode != LV_ADD_SSID))
        display_obj.updateBanner(current_menu->name);
    #endif
  }

  if (currentTime != 0) {
    if (currentTime - initTime >= BANNER_TIME) {
      this->initTime = millis();
      if ((wifi_scan_obj.currentScanMode != LV_JOIN_WIFI) &&
          (wifi_scan_obj.currentScanMode != LV_ADD_SSID))
        this->updateStatusBar();
        #ifdef MARAUDER_MINI
          display_obj.updateBanner(current_menu->name);
        #endif
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
      (wifi_scan_obj.currentScanMode != WIFI_ATTACK_MIMIC) &&
      (wifi_scan_obj.currentScanMode != WIFI_ATTACK_RICK_ROLL))
    display_obj.displayBuffer();


  // Pressed will be set true is there is a valid touch on the screen
  int pre_getTouch = millis();

  // getTouch causes a 10ms delay which makes beacon spam less effective
  //if (wifi_scan_obj.currentScanMode == WIFI_SCAN_OFF)
  #ifndef MARAUDER_MINI
    pressed = display_obj.tft.getTouch(&t_x, &t_y);
  #endif


  // This is if there are scans/attacks going on
  #ifndef MARAUDER_MINI
    if ((wifi_scan_obj.currentScanMode != WIFI_SCAN_OFF) &&
        (pressed) &&
        (wifi_scan_obj.currentScanMode != OTA_UPDATE) &&
        (wifi_scan_obj.currentScanMode != ESP_UPDATE) &&
        (wifi_scan_obj.currentScanMode != SHOW_INFO))
    {
      // Stop the current scan
      if ((wifi_scan_obj.currentScanMode == WIFI_SCAN_PROBE) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_AP) ||
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
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_MIMIC) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_RICK_ROLL) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_BEACON_LIST) ||
          (wifi_scan_obj.currentScanMode == BT_SCAN_ALL) ||
          (wifi_scan_obj.currentScanMode == BT_SCAN_SKIMMERS))
      {
        //Serial.println("Stopping scan...");
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

  #ifdef MARAUDER_MINI

    bool c_btn_press = c_btn.justPressed();
    
    if ((c_btn_press) &&
        (wifi_scan_obj.currentScanMode != WIFI_SCAN_OFF) &&
        (wifi_scan_obj.currentScanMode != OTA_UPDATE) &&
        (wifi_scan_obj.currentScanMode != ESP_UPDATE) &&
        (wifi_scan_obj.currentScanMode != SHOW_INFO))
    {
      // Stop the current scan
      if ((wifi_scan_obj.currentScanMode == WIFI_SCAN_PROBE) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_AP) ||
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
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_MIMIC) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_RICK_ROLL) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_BEACON_LIST) ||
          (wifi_scan_obj.currentScanMode == BT_SCAN_ALL) ||
          (wifi_scan_obj.currentScanMode == BT_SCAN_SKIMMERS))
      {
        //Serial.println("Stopping scan...");
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
  #ifndef MARAUDER_MINI
    if ((wifi_scan_obj.currentScanMode != WIFI_ATTACK_BEACON_SPAM) &&
        (wifi_scan_obj.currentScanMode != WIFI_ATTACK_AP_SPAM) &&
        (wifi_scan_obj.currentScanMode != WIFI_ATTACK_AUTH) &&
        (wifi_scan_obj.currentScanMode != WIFI_ATTACK_DEAUTH) &&
        (wifi_scan_obj.currentScanMode != WIFI_ATTACK_MIMIC) &&
        (wifi_scan_obj.currentScanMode != WIFI_ATTACK_RICK_ROLL))
        //(wifi_scan_obj.currentScanMode != WIFI_ATTACK_BEACON_LIST))
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
          if (current_menu->list->get(b).name != text09)
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

  #ifdef MARAUDER_MINI
    if (u_btn.justPressed()){
      if (current_menu->selected > 0) {
        current_menu->selected--;
        this->buttonSelected(current_menu->selected);
        this->buttonNotSelected(current_menu->selected + 1);
      }
    }
    if (d_btn.justPressed()){
      if (current_menu->selected < current_menu->list->size() - 1) {
        current_menu->selected++;
        this->buttonSelected(current_menu->selected);
        this->buttonNotSelected(current_menu->selected - 1);
      }
    }
    if(c_btn_press){
      current_menu->list->get(current_menu->selected).callable();
    }

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
  
  #ifdef MARAUDER_MINI
    display_obj.tft.setFreeFont(NULL);
  #endif
  
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
    #ifndef MARAUDER_MINI
      display_obj.tft.drawString((String)temp_obj.current_temp + " C", 4, 0, 2);
    #endif

    #ifdef MARAUDER_MINI
      display_obj.tft.drawString((String)temp_obj.current_temp + " C", 0, 0, 1);
    #endif
  }
  display_obj.tft.setTextColor(TFT_WHITE, STATUSBAR_COLOR);

  // WiFi Channel Stuff
  if (wifi_scan_obj.set_channel != wifi_scan_obj.old_channel) {
    wifi_scan_obj.old_channel = wifi_scan_obj.set_channel;
    display_obj.tft.fillRect(50, 0, 50, STATUS_BAR_WIDTH, STATUSBAR_COLOR);
    #ifndef MARAUDER_MINI
      display_obj.tft.drawString("CH: " + (String)wifi_scan_obj.set_channel, 50, 0, 2);
    #endif

    #ifdef MARAUDER_MINI
      display_obj.tft.drawString("CH: " + (String)wifi_scan_obj.set_channel, TFT_WIDTH/4, 0, 1);
    #endif
  }

  // RAM Stuff
  wifi_scan_obj.freeRAM();
  if (wifi_scan_obj.free_ram != wifi_scan_obj.old_free_ram) {
    wifi_scan_obj.old_free_ram = wifi_scan_obj.free_ram;
    display_obj.tft.fillRect(100, 0, 60, STATUS_BAR_WIDTH, STATUSBAR_COLOR);
    #ifndef MARAUDER_MINI
      display_obj.tft.drawString((String)wifi_scan_obj.free_ram + "B", 100, 0, 2);
    #endif

    #ifdef MARAUDER_MINI
      display_obj.tft.drawString((String)wifi_scan_obj.free_ram + "B", TFT_WIDTH/1.75, 0, 1);
    #endif
  }

  // Draw battery info
  MenuFunctions::battery(false);

  // Draw SD info
  if (sd_obj.supported)
    the_color = TFT_GREEN;
  else
    the_color = TFT_RED;

  #ifndef MARAUDER_MINI
    display_obj.tft.drawXBitmap(170,
                                0,
                                menu_icons[STATUS_SD],
                                16,
                                16,
                                STATUSBAR_COLOR,
                                the_color);
  #endif

  #ifdef MARAUDER_MINI
    display_obj.tft.setTextColor(the_color, STATUSBAR_COLOR);
    display_obj.tft.drawString("SD", TFT_WIDTH - 12, 0, 1);
  #endif
}

void MenuFunctions::drawStatusBar()
{
  display_obj.tft.setTextSize(1);
  #ifdef MARAUDER_MINI
    display_obj.tft.setFreeFont(NULL);
  #endif
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
  #ifndef MARAUDER_MINI
    display_obj.tft.drawString((String)temp_obj.current_temp + " C", 4, 0, 2);
  #endif

  #ifdef MARAUDER_MINI
    display_obj.tft.drawString((String)temp_obj.current_temp + " C", 0, 0, 1);
  #endif
  display_obj.tft.setTextColor(TFT_WHITE, STATUSBAR_COLOR);


  // WiFi Channel Stuff
  wifi_scan_obj.old_channel = wifi_scan_obj.set_channel;
  display_obj.tft.fillRect(50, 0, 50, STATUS_BAR_WIDTH, STATUSBAR_COLOR);
  #ifndef MARAUDER_MINI
    display_obj.tft.drawString("CH: " + (String)wifi_scan_obj.set_channel, 50, 0, 2);
  #endif

  #ifdef MARAUDER_MINI
    display_obj.tft.drawString("CH: " + (String)wifi_scan_obj.set_channel, TFT_WIDTH/4, 0, 1);
  #endif

  // RAM Stuff
  wifi_scan_obj.freeRAM();
  wifi_scan_obj.old_free_ram = wifi_scan_obj.free_ram;
  display_obj.tft.fillRect(100, 0, 60, STATUS_BAR_WIDTH, STATUSBAR_COLOR);
  #ifndef MARAUDER_MINI
    display_obj.tft.drawString((String)wifi_scan_obj.free_ram + "B", 100, 0, 2);
  #endif

  #ifdef MARAUDER_MINI
    display_obj.tft.drawString((String)wifi_scan_obj.free_ram + "B", TFT_WIDTH/1.75, 0, 1);
  #endif


  MenuFunctions::battery2(true);

  // Draw SD info
  if (sd_obj.supported)
    the_color = TFT_GREEN;
  else
    the_color = TFT_RED;

  #ifndef MARAUDER_MINI
    display_obj.tft.drawXBitmap(170,
                                0,
                                menu_icons[STATUS_SD],
                                16,
                                16,
                                STATUSBAR_COLOR,
                                the_color);
  #endif

  #ifdef MARAUDER_MINI
    display_obj.tft.setTextColor(the_color, STATUSBAR_COLOR);
    display_obj.tft.drawString("SD", TFT_WIDTH - 12, 0, 1);
  #endif
}

void MenuFunctions::orientDisplay()
{
  display_obj.tft.init();

  display_obj.tft.setRotation(0); // Portrait

  display_obj.tft.setCursor(0, 0);

  #ifndef MARAUDER_MINI
    #ifdef TFT_SHIELD
      uint16_t calData[5] = { 275, 3494, 361, 3528, 4 }; // tft.setRotation(0); // Portrait with TFT Shield
      //Serial.println("Using TFT Shield");
    #else if defined(TFT_DIY)
      uint16_t calData[5] = { 339, 3470, 237, 3438, 2 }; // tft.setRotation(0); // Portrait with DIY TFT
      //Serial.println("Using TFT DIY");
    #endif

    display_obj.tft.setTouch(calData);
  #endif

  changeMenu(current_menu);
}

void MenuFunctions::runBoolSetting(String key) {
  //Serial.println("Building bool setting screen...");
  display_obj.tftDrawRedOnOffButton();
  //display_obj.tftDrawGreenOnOffButton();
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
  #ifndef MARAUDER_MINI
    this->initLVGL();
  #endif
   
  // root menu stuff
  mainMenu.list = new LinkedList<MenuNode>(); // Get list in first menu ready

  // Main menu stuff
  wifiMenu.list = new LinkedList<MenuNode>(); // Get list in second menu ready
  bluetoothMenu.list = new LinkedList<MenuNode>(); // Get list in third menu ready
  badusbMenu.list = new LinkedList<MenuNode>();
  generalMenu.list = new LinkedList<MenuNode>();
  deviceMenu.list = new LinkedList<MenuNode>();

  // Device menu stuff
  failedUpdateMenu.list = new LinkedList<MenuNode>();
  whichUpdateMenu.list = new LinkedList<MenuNode>();
  confirmMenu.list = new LinkedList<MenuNode>();
  espUpdateMenu.list = new LinkedList<MenuNode>();
  updateMenu.list = new LinkedList<MenuNode>();
  settingsMenu.list = new LinkedList<MenuNode>();
  specSettingMenu.list = new LinkedList<MenuNode>();
  infoMenu.list = new LinkedList<MenuNode>();
  languageMenu.list = new LinkedList<MenuNode>();

  // WiFi menu stuff
  wifiSnifferMenu.list = new LinkedList<MenuNode>();
  wifiAttackMenu.list = new LinkedList<MenuNode>();
  wifiGeneralMenu.list = new LinkedList<MenuNode>();

  // Bluetooth menu stuff
  bluetoothSnifferMenu.list = new LinkedList<MenuNode>();
  bluetoothGeneralMenu.list = new LinkedList<MenuNode>();

  // Settings stuff
  shutdownWiFiMenu.list = new LinkedList<MenuNode>();
  shutdownBLEMenu.list = new LinkedList<MenuNode>();
  generateSSIDsMenu.list = new LinkedList<MenuNode>();
  clearSSIDsMenu.list = new LinkedList<MenuNode>();
  clearAPsMenu.list = new LinkedList<MenuNode>();

  // Work menu names
  mainMenu.name = text_table1[6];
  wifiMenu.name = text_table1[7];
  badusbMenu.name = text_table1[8];
  deviceMenu.name = text_table1[9];
  generalMenu.name = text_table1[10];
  failedUpdateMenu.name = text_table1[11];
  whichUpdateMenu.name = text_table1[12];
  confirmMenu.name = text_table1[13];
  espUpdateMenu.name = text_table1[14];
  updateMenu.name = text_table1[15];
  languageMenu.name = text_table1[16]; 
  infoMenu.name = text_table1[17];
  settingsMenu.name = text_table1[18];
  bluetoothMenu.name = text_table1[19];
  wifiSnifferMenu.name = text_table1[20];
  wifiAttackMenu.name = text_table1[21];
  wifiGeneralMenu.name = text_table1[22];
  bluetoothSnifferMenu.name = text_table1[23];
  bluetoothGeneralMenu.name = text_table1[24];
  shutdownWiFiMenu.name = text_table1[25];
  shutdownBLEMenu.name = text_table1[26];
  generateSSIDsMenu.name = text_table1[27];
  clearSSIDsMenu.name = text_table1[28];
  clearAPsMenu.name = text_table1[29];
  

  // Build Main Menu
  mainMenu.parentMenu = NULL;
  addNodes(&mainMenu, text_table1[7], TFT_GREEN, NULL, WIFI, [this]() {
    changeMenu(&wifiMenu);
  });
  addNodes(&mainMenu, text_table1[19], TFT_CYAN, NULL, BLUETOOTH, [this]() {
    changeMenu(&bluetoothMenu);
  });
  if (a32u4_obj.supported) addNodes(&mainMenu, text_table1[8], TFT_RED, NULL, BAD_USB_ICO, [this]() {
    changeMenu(&badusbMenu);
  });
  addNodes(&mainMenu, text_table1[10], TFT_MAGENTA, NULL, GENERAL_APPS, [this]() {
    changeMenu(&generalMenu);
  });
  addNodes(&mainMenu, text_table1[9], TFT_BLUE, NULL, DEVICE, [this]() {
    changeMenu(&deviceMenu);
  });
  addNodes(&mainMenu, text_table1[30], TFT_LIGHTGREY, NULL, REBOOT, []() {
    ESP.restart();
  });

  // Build WiFi Menu
  wifiMenu.parentMenu = &mainMenu; // Main Menu is second menu parent
  addNodes(&wifiMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    changeMenu(wifiMenu.parentMenu);
  });
  addNodes(&wifiMenu, text_table1[31], TFT_YELLOW, NULL, SNIFFERS, [this]() {
    changeMenu(&wifiSnifferMenu);
  });
  //addNodes(&wifiMenu, "Scanners", TFT_ORANGE, NULL, SCANNERS, [this]() {
  //  changeMenu(&wifiScannerMenu);
  //});
  addNodes(&wifiMenu, text_table1[32], TFT_RED, NULL, ATTACKS, [this]() {
    changeMenu(&wifiAttackMenu);
  });
  addNodes(&wifiMenu, text_table1[33], TFT_PURPLE, NULL, GENERAL_APPS, [this]() {
    changeMenu(&wifiGeneralMenu);
  });

  // Build WiFi sniffer Menu
  wifiSnifferMenu.parentMenu = &wifiMenu; // Main Menu is second menu parent
  addNodes(&wifiSnifferMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    changeMenu(wifiSnifferMenu.parentMenu);
  });
  addNodes(&wifiSnifferMenu, text_table1[42], TFT_CYAN, NULL, PROBE_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_PROBE, TFT_CYAN);
  });
  addNodes(&wifiSnifferMenu, text_table1[43], TFT_MAGENTA, NULL, BEACON_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_AP, TFT_MAGENTA);
  });
  addNodes(&wifiSnifferMenu, text_table1[44], TFT_RED, NULL, DEAUTH_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_DEAUTH, TFT_RED);
  });
  addNodes(&wifiSnifferMenu, text_table1[45], TFT_BLUE, NULL, PACKET_MONITOR, [this]() {
    wifi_scan_obj.StartScan(WIFI_PACKET_MONITOR, TFT_BLUE);
  });
  addNodes(&wifiSnifferMenu, text_table1[46], TFT_VIOLET, NULL, EAPOL, [this]() {
    wifi_scan_obj.StartScan(WIFI_SCAN_EAPOL, TFT_VIOLET);
  });
  addNodes(&wifiSnifferMenu, text_table1[47], TFT_RED, NULL, PWNAGOTCHI, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_PWN, TFT_RED);
  });
  addNodes(&wifiSnifferMenu, text_table1[48], TFT_ORANGE, NULL, ESPRESSIF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_ESPRESSIF, TFT_ORANGE);
  });
  addNodes(&wifiSnifferMenu, text_table1[49], TFT_MAGENTA, NULL, BEACON_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_TARGET_AP, TFT_MAGENTA);
  });

  // Build WiFi attack menu
  wifiAttackMenu.parentMenu = &wifiMenu; // Main Menu is second menu parent
  addNodes(&wifiAttackMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    changeMenu(wifiAttackMenu.parentMenu);
  });
  addNodes(&wifiAttackMenu, text_table1[50], TFT_RED, NULL, BEACON_LIST, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_ATTACK_BEACON_LIST, TFT_RED);
  });
  addNodes(&wifiAttackMenu, text_table1[51], TFT_ORANGE, NULL, BEACON_SPAM, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_ATTACK_BEACON_SPAM, TFT_ORANGE);
  });
  addNodes(&wifiAttackMenu, text_table1[52], TFT_YELLOW, NULL, RICK_ROLL, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_ATTACK_RICK_ROLL, TFT_YELLOW);
  });
  addNodes(&wifiAttackMenu, text_table1[53], TFT_RED, NULL, PROBE_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_ATTACK_AUTH, TFT_RED);
  });
  addNodes(&wifiAttackMenu, text_table1[54], TFT_RED, NULL, DEAUTH_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_ATTACK_DEAUTH, TFT_RED);
  });
  addNodes(&wifiAttackMenu, text_table1[57], TFT_MAGENTA, NULL, BEACON_LIST, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_ATTACK_AP_SPAM, TFT_MAGENTA);
  });
  //addNodes(&wifiAttackMenu, "AP Mimic Flood", TFT_PURPLE, NULL, DEAUTH_SNIFF, [this]() {
  //  display_obj.clearScreen();
  //  this->drawStatusBar();
  //  wifi_scan_obj.StartScan(WIFI_ATTACK_MIMIC, TFT_PURPLE);
  //});

  // Build WiFi General menu
  wifiGeneralMenu.parentMenu = &wifiMenu;
  addNodes(&wifiGeneralMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    changeMenu(wifiGeneralMenu.parentMenu);
  });
  #ifndef MARAUDER_MINI
    addNodes(&wifiGeneralMenu, text_table1[55], TFT_DARKCYAN, NULL, JOIN_WIFI, [this](){
      display_obj.clearScreen(); 
      wifi_scan_obj.currentScanMode = LV_JOIN_WIFI; 
      wifi_scan_obj.StartScan(LV_JOIN_WIFI, TFT_YELLOW); 
      joinWiFiGFX();
    });
  #endif
  addNodes(&wifiGeneralMenu, text_table1[25], TFT_CYAN, NULL, SHUTDOWN, [this]() {
    changeMenu(&shutdownWiFiMenu);
    wifi_scan_obj.RunShutdownWiFi();
  });
  addNodes(&wifiGeneralMenu, text_table1[27], TFT_SKYBLUE, NULL, GENERATE, [this]() {
    changeMenu(&generateSSIDsMenu);
    wifi_scan_obj.RunGenerateSSIDs();
  });
  #ifndef MARAUDER_MINI
    addNodes(&wifiGeneralMenu, text_table1[1], TFT_NAVY, NULL, KEYBOARD_ICO, [this](){
      display_obj.clearScreen(); 
      //wifi_scan_obj.currentScanMode = LV_ADD_SSID; 
      wifi_scan_obj.StartScan(LV_ADD_SSID, TFT_YELLOW); 
      addSSIDGFX();
    });
  #endif
  addNodes(&wifiGeneralMenu, text_table1[28], TFT_SILVER, NULL, CLEAR_ICO, [this]() {
    changeMenu(&clearSSIDsMenu);
    wifi_scan_obj.RunClearSSIDs();
  });
  addNodes(&wifiGeneralMenu, text_table1[29], TFT_DARKGREY, NULL, CLEAR_ICO, [this]() {
    changeMenu(&clearAPsMenu);
    wifi_scan_obj.RunClearAPs();
  });
  #ifndef MARAUDER_MINI
    addNodes(&wifiGeneralMenu, text_table1[56], TFT_NAVY, NULL, KEYBOARD_ICO, [this](){
      display_obj.clearScreen(); 
      wifi_scan_obj.currentScanMode = LV_ADD_SSID; 
      wifi_scan_obj.StartScan(LV_ADD_SSID, TFT_RED);  
      addAPGFX();
    });
  #endif

  // Build shutdown wifi menu
  shutdownWiFiMenu.parentMenu = &wifiGeneralMenu;
  addNodes(&shutdownWiFiMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    changeMenu(shutdownWiFiMenu.parentMenu);
  });

  // Build generate ssids menu
  generateSSIDsMenu.parentMenu = &wifiGeneralMenu;
  addNodes(&generateSSIDsMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    changeMenu(generateSSIDsMenu.parentMenu);
  });

  // Build clear ssids menu
  clearSSIDsMenu.parentMenu = &wifiGeneralMenu;
  addNodes(&clearSSIDsMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    changeMenu(clearSSIDsMenu.parentMenu);
  });
  clearAPsMenu.parentMenu = &wifiGeneralMenu;
  addNodes(&clearAPsMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    changeMenu(clearAPsMenu.parentMenu);
  });


  // Build Bluetooth Menu
  bluetoothMenu.parentMenu = &mainMenu; // Second Menu is third menu parent
  addNodes(&bluetoothMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    changeMenu(bluetoothMenu.parentMenu);
  });
  addNodes(&bluetoothMenu, text_table1[31], TFT_YELLOW, NULL, SNIFFERS, [this]() {
    changeMenu(&bluetoothSnifferMenu);
  });
  //addNodes(&bluetoothMenu, "Scanners", TFT_ORANGE, NULL, SCANNERS, [this]() {
  //  changeMenu(&bluetoothScannerMenu);
  //});
  addNodes(&bluetoothMenu, text_table1[33], TFT_PURPLE, NULL, GENERAL_APPS, [this]() {
    changeMenu(&bluetoothGeneralMenu);
  });

  // Build bluetooth sniffer Menu
  bluetoothSnifferMenu.parentMenu = &bluetoothMenu; // Second Menu is third menu parent
  addNodes(&bluetoothSnifferMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    changeMenu(bluetoothSnifferMenu.parentMenu);
  });
  addNodes(&bluetoothSnifferMenu, text_table1[34], TFT_GREEN, NULL, BLUETOOTH_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(BT_SCAN_ALL, TFT_GREEN);
  });
  addNodes(&bluetoothSnifferMenu, text_table1[35], TFT_MAGENTA, NULL, CC_SKIMMERS, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(BT_SCAN_SKIMMERS, TFT_MAGENTA);
  });

  // Build bluetooth scanner Menu
  //bluetoothScannerMenu.parentMenu = &bluetoothMenu; // Second Menu is third menu parent
  //addNodes(&bluetoothScannerMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
  //  changeMenu(bluetoothScannerMenu.parentMenu);
  //});

  // Build bluetooth general menu
  bluetoothGeneralMenu.parentMenu = &bluetoothMenu;
  addNodes(&bluetoothGeneralMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    changeMenu(bluetoothGeneralMenu.parentMenu);
  });
  addNodes(&bluetoothGeneralMenu, text_table1[26], TFT_ORANGE, NULL, SHUTDOWN, [this]() {
    changeMenu(&shutdownBLEMenu);
    wifi_scan_obj.RunShutdownBLE();
  });

  // Build shutdown BLE menu
  shutdownBLEMenu.parentMenu = &bluetoothGeneralMenu;
  addNodes(&shutdownBLEMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    changeMenu(shutdownBLEMenu.parentMenu);
  });

  // Bad USB Menu
  badusbMenu.parentMenu = &mainMenu;
  addNodes(&badusbMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    changeMenu(badusbMenu.parentMenu);
  });
  addNodes(&badusbMenu, text_table1[36], TFT_PURPLE, NULL, TEST_BAD_USB_ICO, [this]() {
    a32u4_obj.test();
  });
  #ifndef MARAUDER_MINI
    addNodes(&badusbMenu, text_table1[37], TFT_RED, NULL, BAD_USB_ICO, [this](){
      display_obj.clearScreen(); 
      wifi_scan_obj.currentScanMode = LV_ADD_SSID; 
      wifi_scan_obj.StartScan(LV_ADD_SSID, TFT_RED); 
      writeBadUSB();
    });
  #endif

  // General apps menu
  generalMenu.parentMenu = &mainMenu;
  addNodes(&generalMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    display_obj.draw_tft = false;
    changeMenu(generalMenu.parentMenu);
  });
  addNodes(&generalMenu, text_table1[38], TFT_WHITE, NULL, DRAW, [this]() {
    display_obj.clearScreen();
    display_obj.setupDraw();
    display_obj.draw_tft = true;
  });

  // Device menu
  deviceMenu.parentMenu = &mainMenu;
  addNodes(&deviceMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    changeMenu(deviceMenu.parentMenu);
  });
  //addNodes(&deviceMenu, "Update Firmware", TFT_ORANGE, NULL, UPDATE, [this](){wifi_scan_obj.currentScanMode = OTA_UPDATE; changeMenu(&updateMenu); web_obj.setupOTAupdate();});
  addNodes(&deviceMenu, text_table1[15], TFT_ORANGE, NULL, UPDATE, [this]() {
    wifi_scan_obj.currentScanMode = OTA_UPDATE;
    changeMenu(&whichUpdateMenu);
  });

  addNodes(&deviceMenu, text_table1[16], TFT_GREEN, NULL, LANGUAGE, [this]() {

    wifi_scan_obj.currentScanMode = SHOW_INFO;
   changeMenu(&languageMenu);   
  });
  addNodes(&deviceMenu, text_table1[17], TFT_WHITE, NULL, DEVICE_INFO, [this]() {
    wifi_scan_obj.currentScanMode = SHOW_INFO;
    changeMenu(&infoMenu);
    wifi_scan_obj.RunInfo();
  });
  addNodes(&deviceMenu, text08, TFT_NAVY, NULL, KEYBOARD_ICO, [this]() {
    changeMenu(&settingsMenu);
  });
  /*addNodes(&deviceMenu, text08, TFT_NAVY, NULL, KEYBOARD_ICO, [this](){
    display_obj.clearScreen(); 
    wifi_scan_obj.currentScanMode = LV_ADD_SSID; 
    wifi_scan_obj.StartScan(LV_ADD_SSID, TFT_RED);  
    displaySettingsGFX();
  });*/

  // Settings menu
  // Device menu
  settingsMenu.parentMenu = &deviceMenu;
  addNodes(&settingsMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    changeMenu(settingsMenu.parentMenu);
  });
  for (int i = 0; i < settings_obj.getNumberSettings(); i++) {
    if (this->callSetting(settings_obj.setting_index_to_name(i)) == "bool")
      addNodes(&settingsMenu, settings_obj.setting_index_to_name(i), TFT_LIGHTGREY, NULL, 0, [this, i]() {
      settings_obj.toggleSetting(settings_obj.setting_index_to_name(i));
      changeMenu(&specSettingMenu);
      //this->callSetting(settings_obj.setting_index_to_name(i));
      this->displaySetting(settings_obj.setting_index_to_name(i), &settingsMenu, i + 1);
    }, settings_obj.loadSetting<bool>(settings_obj.setting_index_to_name(i)));
  }

  // Specific setting menu
  specSettingMenu.parentMenu = &settingsMenu;
  addNodes(&specSettingMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    changeMenu(specSettingMenu.parentMenu);
  });
 
  // Select update
  whichUpdateMenu.parentMenu = &deviceMenu;
  addNodes(&whichUpdateMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    changeMenu(whichUpdateMenu.parentMenu);
  });
  addNodes(&whichUpdateMenu, text_table1[39], TFT_GREEN, NULL, WEB_UPDATE, [this]() {
    wifi_scan_obj.currentScanMode = OTA_UPDATE;
    changeMenu(&updateMenu);
    web_obj.setupOTAupdate();
  });
  if (sd_obj.supported) addNodes(&whichUpdateMenu, text_table1[40], TFT_MAGENTA, NULL, SD_UPDATE, [this]() {
    wifi_scan_obj.currentScanMode = OTA_UPDATE;
    changeMenu(&confirmMenu);
  });
  addNodes(&whichUpdateMenu, text_table1[41], TFT_RED, NULL, ESP_UPDATE_ICO, [this]() {
    wifi_scan_obj.currentScanMode = ESP_UPDATE;
    changeMenu(&espUpdateMenu);
    esp_obj.RunUpdate();
  });

  // ESP Update Menu
  espUpdateMenu.parentMenu = &whichUpdateMenu;
  addNodes(&espUpdateMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    wifi_scan_obj.currentScanMode = WIFI_SCAN_OFF;
    esp_obj.bootRunMode();
    changeMenu(espUpdateMenu.parentMenu);
  });

  // Confirm SD update menu
  confirmMenu.parentMenu = &whichUpdateMenu;
  addNodes(&confirmMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    changeMenu(confirmMenu.parentMenu);
  });
  //addNodes(&confirmMenu, "Yes", TFT_ORANGE, NULL, UPDATE, [this](){wifi_scan_obj.currentScanMode = OTA_UPDATE; changeMenu(&updateMenu); sd_obj.runUpdate();});
  addNodes(&confirmMenu, text14, TFT_ORANGE, NULL, UPDATE, [this]() {
    wifi_scan_obj.currentScanMode = OTA_UPDATE;
    changeMenu(&failedUpdateMenu);
    sd_obj.runUpdate();
  });

  // Web Update
  updateMenu.parentMenu = &deviceMenu;
  addNodes(&updateMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    wifi_scan_obj.currentScanMode = WIFI_SCAN_OFF;
    changeMenu(updateMenu.parentMenu);
    WiFi.softAPdisconnect(true);
    web_obj.shutdownServer();
  });
  //addNodes(&updateMenu, text09, TFT_LIGHTGREY, NULL, 0, [this](){wifi_scan_obj.currentScanMode = WIFI_SCAN_OFF; changeMenu(updateMenu.parentMenu);});

  // Failed update menu
  failedUpdateMenu.parentMenu = &whichUpdateMenu;
  addNodes(&failedUpdateMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    wifi_scan_obj.currentScanMode = WIFI_SCAN_OFF;
    changeMenu(failedUpdateMenu.parentMenu);
  });

  // Device info menu
  infoMenu.parentMenu = &deviceMenu;
  addNodes(&infoMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
    wifi_scan_obj.currentScanMode = WIFI_SCAN_OFF;
    changeMenu(infoMenu.parentMenu);
  });
  //language info menu
  languageMenu.parentMenu = &deviceMenu;
    addNodes(&languageMenu, text09, TFT_LIGHTGREY, NULL, 0, [this]() {
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
  menu->list->add(MenuNode{name, command, color, place, &new_button, selected, callable});
  //menu->list->add(MenuNode{name, color, place, callable});
}

void MenuFunctions::buildButtons(Menu * menu)
{
  //Serial.println("Bulding buttons...");
  if (menu->list != NULL)
  {
    //for (int i = 0; i < sizeof(key); i++)
    //  key[i] = NULL;
    for (uint8_t i = 0; i < menu->list->size(); i++)
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
  //Serial.println(F("Displaying current menu..."));
  display_obj.clearScreen();
  display_obj.tft.setTextColor(TFT_LIGHTGREY, TFT_DARKGREY);
  this->drawStatusBar();

  if (current_menu->list != NULL)
  {
    #ifndef MARAUDER_MINI
      display_obj.tft.setFreeFont(MENU_FONT);
    #endif

    #ifdef MARAUDER_MINI
      display_obj.tft.setFreeFont(NULL);
      display_obj.tft.setTextSize(1);
    #endif
    for (uint8_t i = 0; i < current_menu->list->size(); i++)
    {
      #ifndef MARAUDER_MINI
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

      #ifdef MARAUDER_MINI
        if (current_menu->selected != i)
          display_obj.key[i].drawButton(false, current_menu->list->get(i).name);
        else
          display_obj.key[i].drawButton(true, current_menu->list->get(i).name);
      #endif
    }
    display_obj.tft.setFreeFont(NULL);
  }
}

#endif
