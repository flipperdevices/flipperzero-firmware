#include "WiFiScan.h"

//esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);

//int num_beacon = 0;
int num_beacon = 0;
int num_deauth = 0;
int num_probe = 0;
int num_eapol = 0;

LinkedList<ssid>* ssids;
LinkedList<AccessPoint>* access_points;

class bluetoothScanAllCallback: public BLEAdvertisedDeviceCallbacks {

    /* Original BLE */
    //void onResult(BLEAdvertisedDevice advertisedDevice) {

    /* NimBLE */
    void onResult(BLEAdvertisedDevice *advertisedDevice) {

      //advertisedDevice.getScan()->stop();
      
      String display_string = "";
      if (display_obj.display_buffer->size() >= 0)
      {
        display_string.concat(" RSSI: ");
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
      }
    }
};

class bluetoothScanSkimmersCallback: public BLEAdvertisedDeviceCallbacks {
    /* Original BLE */
    //void onResult(BLEAdvertisedDevice advertisedDevice) {

    /* NimBLE */
    void onResult(BLEAdvertisedDevice *advertisedDevice) {
      String bad_list[bad_list_length] = {"HC-03", "HC-05", "HC-06"};
      if (display_obj.display_buffer->size() >= 0)
      {
        Serial.print("Device: ");
        String display_string = "";
        //display_string.concat(" RSSI: ");
        //display_string.concat(advertisedDevice.getRSSI());
        //display_string.concat(" ");
        if(advertisedDevice->getName().length() != 0)
        {
          //display_string.concat(advertisedDevice.getName().c_str());
          Serial.print(advertisedDevice->getName().c_str());
          for(uint8_t i = 0; i < bad_list_length; i++)
          {
            if(strcmp(advertisedDevice->getName().c_str(), bad_list[i].c_str()) == 0)
            {
              display_string.concat("Potential Skimmer: ");
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
          }
        }
        else
        {
          Serial.print(advertisedDevice->getAddress().toString().c_str());
          //display_string.concat(advertisedDevice.getAddress().toString().c_str());
        }
        /*
        int temp_len = display_string.length();
        for (int i = 0; i < 40 - temp_len; i++)
        {
          display_string.concat(" ");
        }
        */
        Serial.print(" RSSI: ");
        Serial.println(advertisedDevice->getRSSI());

        /*
        while (display_obj.printing)
          delay(1);
        display_obj.loading = true;
        display_obj.display_buffer->add(display_string);
        display_obj.loading = false;
        */
      }
    }
};


WiFiScan::WiFiScan()
{
}

void WiFiScan::RunSetup() {
  ssids = new LinkedList<ssid>();
  access_points = new LinkedList<AccessPoint>();
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  this->ble_initialized = true;
  
  this->shutdownBLE();
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
  ssid s = {essid, {random(256), random(256), random(256), random(256), random(256), random(256)}};
  ssids->add(s);
  Serial.println(ssids->get(ssids->size() - 1).essid);

  return true;
}

int WiFiScan::generateSSIDs() {
  uint8_t num_gen = 20;
  for (uint8_t x = 0; x < num_gen; x++) {
    String essid = "";

    for (uint8_t i = 0; i < 6; i++)
      essid.concat(alfa[random(65)]);

    ssid s = {essid, {random(256), random(256), random(256), random(256), random(256), random(256)}};
    ssids->add(s);
    Serial.println(ssids->get(ssids->size() - 1).essid);
  }

  return num_gen;
}

void WiFiScan::joinWiFi(String ssid, String password)
{
  static const char * btns[] ={"Close", ""};
  int count = 0;
  
  if ((WiFi.status() == WL_CONNECTED) && (ssid == connected_network) && (ssid != "")) {
    lv_obj_t * mbox1 = lv_msgbox_create(lv_scr_act(), NULL);
    lv_msgbox_set_text(mbox1, "Already Connected");
    lv_msgbox_add_btns(mbox1, btns);
    lv_obj_set_width(mbox1, 200);
    //lv_obj_set_event_cb(mbox1, event_handler);
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
      lv_msgbox_set_text(mbox1, "Failed to connect");
      lv_msgbox_add_btns(mbox1, btns);
      lv_obj_set_width(mbox1, 200);
      //lv_obj_set_event_cb(mbox1, event_handler);
      lv_obj_align(mbox1, NULL, LV_ALIGN_CENTER, 0, 0); /*Align to the corner*/
      WiFi.mode(WIFI_OFF);
      return;
    }
  }

  lv_obj_t * mbox1 = lv_msgbox_create(lv_scr_act(), NULL);
  lv_msgbox_set_text(mbox1, "Connected");
  lv_msgbox_add_btns(mbox1, btns);
  lv_obj_set_width(mbox1, 200);
  //lv_obj_set_event_cb(mbox1, event_handler);
  lv_obj_align(mbox1, NULL, LV_ALIGN_CENTER, 0, 0); /*Align to the corner*/

  connected_network = ssid;
  
  Serial.println("\nConnected to the WiFi network");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  this->wifi_initialized = true;
}

// Function to prepare to run a specific scan
void WiFiScan::StartScan(uint8_t scan_mode, uint16_t color)
{  
  //Serial.println("Starting Scan...");
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
  else if (scan_mode == WIFI_SCAN_PWN)
    RunPwnScan(scan_mode, color);
  else if (scan_mode == WIFI_SCAN_DEAUTH)
    RunDeauthScan(scan_mode, color);
  else if (scan_mode == WIFI_PACKET_MONITOR)
    RunPacketMonitor(scan_mode, color);
  else if (scan_mode == WIFI_ATTACK_BEACON_LIST)
    RunBeaconList(scan_mode, color);
  else if (scan_mode == WIFI_ATTACK_BEACON_SPAM)
    RunBeaconSpam(scan_mode, color);
  else if (scan_mode == WIFI_ATTACK_RICK_ROLL)
    RunRickRoll(scan_mode, color);
  else if (scan_mode == WIFI_ATTACK_AUTH)
    RunProbeFlood(scan_mode, color);
  else if (scan_mode == BT_SCAN_ALL)
    RunBluetoothScan(scan_mode, color);
  else if (scan_mode == BT_SCAN_SKIMMERS)
    RunBluetoothScan(scan_mode, color);
  else if (scan_mode == WIFI_SCAN_ESPRESSIF)
    RunEspressifScan(scan_mode, color);
  else if (scan_mode == LV_JOIN_WIFI)
    RunLvJoinWiFi(scan_mode, color);
  else if (scan_mode == LV_ADD_SSID)
    RunLvJoinWiFi(scan_mode, color);

  WiFiScan::currentScanMode = scan_mode;
}

bool WiFiScan::shutdownWiFi() {
  if (this->wifi_initialized) {
    Serial.println("Ahhh yes...promiscuity will end");
    esp_wifi_set_promiscuous(false);
    //WiFi.persistent(false);
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
  
    esp_wifi_set_mode(WIFI_MODE_NULL);
    esp_wifi_stop();
    esp_wifi_deinit();
  
    this->wifi_initialized = false;
    return true;
  }
  else {
    Serial.println(F("WiFi is not currently running"));
    return false;
  }
}

bool WiFiScan::shutdownBLE() {
  if (this->ble_initialized) {
    Serial.println("Stopping BLE scan...");
    pBLEScan->stop();
    Serial.println("BLE Scan Stopped");
    
    
    Serial.println("Clearing BLE Results...");
    pBLEScan->clearResults();
    Serial.println("Deinitializing BT Controller...");
    BLEDevice::deinit();
    //Serial.println("Disable and Deinit BLE...");
    //esp_bt_controller_disable();
    //esp_bt_controller_deinit();
    //Serial.println("Releasing BLE Memory...");
    //esp_bt_controller_mem_release(ESP_BT_MODE_BLE);
    //Serial.println("BT Controller Status: " + (String)esp_bt_controller_get_status());
  
    this->ble_initialized = false;
    return true;
  }
  else {
    Serial.println(F("BLE is not currently running"));
    return false;
  }
}

// Function to stop all wifi scans
void WiFiScan::StopScan(uint8_t scan_mode)
{
  if ((currentScanMode == WIFI_SCAN_PROBE) ||
  (currentScanMode == WIFI_SCAN_AP) ||
  (currentScanMode == WIFI_SCAN_TARGET_AP) ||
  (currentScanMode == WIFI_SCAN_PWN) ||
  (currentScanMode == WIFI_SCAN_ESPRESSIF) ||
  (currentScanMode == WIFI_SCAN_EAPOL) ||
  (currentScanMode == WIFI_SCAN_ALL) ||
  (currentScanMode == WIFI_SCAN_DEAUTH) ||
  (currentScanMode == WIFI_ATTACK_BEACON_LIST) ||
  (currentScanMode == WIFI_ATTACK_BEACON_SPAM) ||
  (currentScanMode == WIFI_ATTACK_AUTH) ||
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
    //Serial.println("Stopping BLE scan...");
    //pBLEScan->stop();
    //Serial.println("BLE Scan Stopped");
    
    
    //Serial.println("Clearing BLE Results...");
    //pBLEScan->clearResults();
    /*Serial.println("Deinitializing BT Controller...");
    BLEDevice::deinit();
    //Serial.println("Disable and Deinit BLE...");
    //esp_bt_controller_disable();
    //esp_bt_controller_deinit();
    //Serial.println("Releasing BLE Memory...");
    //esp_bt_controller_mem_release(ESP_BT_MODE_BLE);
    //Serial.println("BT Controller Status: " + (String)esp_bt_controller_get_status());
    */
    this->shutdownBLE();
    
  }

  display_obj.display_buffer->clear();
  Serial.print("display_buffer->size(): ");
  Serial.println(display_obj.display_buffer->size());

  display_obj.tteBar = false;
}

String WiFiScan::getStaMAC()
{
  char *buf;
  uint8_t mac[6];
  char macAddrChr[18] = {0};
  //wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  esp_err_t mac_status = esp_wifi_get_mac(ESP_IF_WIFI_STA, mac);
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
  //wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  esp_err_t mac_status = esp_wifi_get_mac(ESP_IF_WIFI_AP, mac);
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
  sprintf(s, "RAM Free: %u bytes", system_get_free_heap_size());
  this->free_ram = String(system_get_free_heap_size());
  return String(s);
}

// Function to start running a beacon scan
void WiFiScan::RunAPScan(uint8_t scan_mode, uint16_t color)
{
  sd_obj.openCapture("ap");

  Serial.println("Clearing APs: " + (String)access_points->size());
  //access_points->clear();
  display_obj.TOP_FIXED_AREA_2 = 48;
  display_obj.tteBar = true;
  display_obj.print_delay_1 = 15;
  display_obj.print_delay_2 = 10;
  //display_obj.clearScreen();
  display_obj.initScrollValues(true);
  display_obj.tft.setTextWrap(false);
  display_obj.tft.setTextColor(TFT_WHITE, color);
  display_obj.tft.fillRect(0,16,240,16, color);
  display_obj.tft.drawCentreString(" AP Scan ",120,16,2);
  display_obj.touchToExit();
  display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
  display_obj.setupScrollArea(display_obj.TOP_FIXED_AREA_2, BOT_FIXED_AREA);
  //wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_filter(&filt);
  esp_wifi_set_promiscuous_rx_cb(&apSnifferCallback);
  esp_wifi_set_channel(set_channel, WIFI_SECOND_CHAN_NONE);
  this->wifi_initialized = true;
  initTime = millis();
}

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
  
  //display_obj.initLVGL();

  lv_obj_t * scr = lv_cont_create(NULL, NULL);
  lv_disp_load_scr(scr);

  //display_obj.joinWiFiGFX();
}

void WiFiScan::RunClearAPs() {
  display_obj.tft.setTextWrap(false);
  display_obj.tft.setFreeFont(NULL);
  display_obj.tft.setCursor(0, 100);
  display_obj.tft.setTextSize(1);
  display_obj.tft.setTextColor(TFT_CYAN);

  display_obj.tft.println(F("Clearing APs..."));
  display_obj.tft.println("APs Cleared: " + (String)this->clearAPs());
}

void WiFiScan::RunClearSSIDs() {
  display_obj.tft.setTextWrap(false);
  display_obj.tft.setFreeFont(NULL);
  display_obj.tft.setCursor(0, 100);
  display_obj.tft.setTextSize(1);
  display_obj.tft.setTextColor(TFT_CYAN);

  display_obj.tft.println(F("Clearing SSIDs..."));
  display_obj.tft.println("SSIDs Cleared: " + (String)this->clearSSIDs());
}

void WiFiScan::RunGenerateSSIDs() {
  display_obj.tft.setTextWrap(false);
  display_obj.tft.setFreeFont(NULL);
  display_obj.tft.setCursor(0, 100);
  display_obj.tft.setTextSize(1);
  display_obj.tft.setTextColor(TFT_CYAN);

  display_obj.tft.println(F("Generating SSIDs..."));

  display_obj.tft.println("SSIDs Generated: " + (String)this->generateSSIDs());
  display_obj.tft.println("    Total SSIDs: " + (String)ssids->size());
}

void WiFiScan::RunShutdownWiFi() {
  display_obj.tft.setTextWrap(false);
  display_obj.tft.setFreeFont(NULL);
  display_obj.tft.setCursor(0, 100);
  display_obj.tft.setTextSize(1);
  display_obj.tft.setTextColor(TFT_CYAN);

  display_obj.tft.print(F("Shutting down WiFi..."));

  if (this->wifi_initialized) {
    this->shutdownWiFi();
    display_obj.tft.setTextColor(TFT_GREEN);
    display_obj.tft.println(F("OK"));
  }
  else {
    display_obj.tft.setTextColor(TFT_RED);
    display_obj.tft.println(F("FAIL"));
    display_obj.tft.println(F("WiFi not currently initialized"));
  }
}

void WiFiScan::RunShutdownBLE() {
  display_obj.tft.setTextWrap(false);
  display_obj.tft.setFreeFont(NULL);
  display_obj.tft.setCursor(0, 100);
  display_obj.tft.setTextSize(1);
  display_obj.tft.setTextColor(TFT_CYAN);

  display_obj.tft.print(F("Shutting down BLE..."));

  if (this->ble_initialized) {
    this->shutdownBLE();
    display_obj.tft.setTextColor(TFT_GREEN);
    display_obj.tft.println(F("OK"));
  }
  else {
    display_obj.tft.setTextColor(TFT_RED);
    display_obj.tft.println(F("FAIL"));
    display_obj.tft.println(F("BLE not currently initialized"));
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

  display_obj.tft.setTextWrap(false);
  display_obj.tft.setFreeFont(NULL);
  display_obj.tft.setCursor(0, 100);
  display_obj.tft.setTextSize(1);
  display_obj.tft.setTextColor(TFT_CYAN);
  display_obj.tft.println("     Firmware: Marauder");
  display_obj.tft.println("      Version: " + display_obj.version_number + "\n");
  display_obj.tft.println("  Station MAC: " + sta_mac);
  display_obj.tft.println("       AP MAC: " + ap_mac);
  display_obj.tft.println("     " + free_ram);

  if (sd_obj.supported) {
    display_obj.tft.println("      SD Card: Connected");
    display_obj.tft.print(" SD Card Size: ");
    display_obj.tft.print(sd_obj.card_sz);
    display_obj.tft.println("MB");
  }
  else {
    display_obj.tft.println("      SD Card: Not Connected");
    display_obj.tft.println(" SD Card Size: 0");
  }  

  battery_obj.battery_level = battery_obj.getBatteryLevel();
  if (battery_obj.i2c_supported) {
    display_obj.tft.println("   IP5306 I2C: supported");
    display_obj.tft.println("  Battery Lvl: " + (String)battery_obj.battery_level + "%");
  }
  else
    display_obj.tft.println("   IP5306 I2C: not supported");

  display_obj.tft.println("Internal temp: " + (String)temp_obj.current_temp + " C");
}

void WiFiScan::RunEspressifScan(uint8_t scan_mode, uint16_t color) {
  sd_obj.openCapture("espressif");
  
  display_obj.TOP_FIXED_AREA_2 = 48;
  display_obj.tteBar = true;
  display_obj.print_delay_1 = 15;
  display_obj.print_delay_2 = 10;
  //display_obj.clearScreen();
  display_obj.initScrollValues(true);
  display_obj.tft.setTextWrap(false);
  display_obj.tft.setTextColor(TFT_WHITE, color);
  display_obj.tft.fillRect(0,16,240,16, color);
  display_obj.tft.drawCentreString(" Detect Espressif ",120,16,2);
  display_obj.touchToExit();
  display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
  display_obj.setupScrollArea(display_obj.TOP_FIXED_AREA_2, BOT_FIXED_AREA);
  //wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
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
  display_obj.tft.init();
  display_obj.tft.setRotation(1);
  display_obj.tft.fillScreen(TFT_BLACK);

  sd_obj.openCapture("packet_monitor");
  
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

  Serial.println("Running packet scan...");
  //wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
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
  display_obj.tft.init();
  display_obj.tft.setRotation(1);
  display_obj.tft.fillScreen(TFT_BLACK);

  sd_obj.openCapture("eapol");

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
  display_obj.tftDrawEapolColorKey();
  //display_obj.tftDrawXScaleButtons(x_scale);
  //display_obj.tftDrawYScaleButtons(y_scale);
  display_obj.tftDrawChannelScaleButtons(set_channel);
  display_obj.tftDrawExitScaleButtons();


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

void WiFiScan::RunRickRoll(uint8_t scan_mode, uint16_t color)
{
  //Serial.println("Rick Roll...");
  display_obj.TOP_FIXED_AREA_2 = 48;
  display_obj.tteBar = true;
  display_obj.print_delay_1 = 15;
  display_obj.print_delay_2 = 10;
  //display_obj.clearScreen();
  display_obj.initScrollValues(true);
  display_obj.tft.setTextWrap(false);
  display_obj.tft.setTextColor(TFT_BLACK, color);
  display_obj.tft.fillRect(0,16,240,16, color);
  display_obj.tft.drawCentreString(" Rick Roll Beacon ",120,16,2);
  display_obj.touchToExit();
  display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
  packets_sent = 0;
  //esp_wifi_set_mode(WIFI_MODE_STA);
  WiFi.mode(WIFI_AP_STA);
  esp_wifi_start();
  esp_wifi_set_promiscuous_filter(NULL);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_max_tx_power(78);
  this->wifi_initialized = true;
  initTime = millis();
  //display_obj.clearScreen();
  //Serial.println("End of func");
}

// Function to prepare for beacon list
void WiFiScan::RunBeaconList(uint8_t scan_mode, uint16_t color) {
  sd_obj.openCapture("beacon_list");
  
  display_obj.TOP_FIXED_AREA_2 = 48;
  display_obj.tteBar = true;
  display_obj.print_delay_1 = 15;
  display_obj.print_delay_2 = 10;
  //display_obj.clearScreen();
  display_obj.initScrollValues(true);
  display_obj.tft.setTextWrap(false);
  display_obj.tft.setTextColor(TFT_BLACK, color);
  display_obj.tft.fillRect(0,16,240,16, color);
  display_obj.tft.drawCentreString(" Beacon Spam List ",120,16,2);
  display_obj.touchToExit();
  display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
  display_obj.setupScrollArea(display_obj.TOP_FIXED_AREA_2, BOT_FIXED_AREA);
  //wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_AP_STA);
  esp_wifi_start();
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_max_tx_power(78);
  esp_wifi_set_promiscuous_filter(&filt);
  esp_wifi_set_promiscuous_rx_cb(&beaconListSnifferCallback);
  esp_wifi_set_channel(set_channel, WIFI_SECOND_CHAN_NONE);
  this->wifi_initialized = true;
  initTime = millis();
}
/*
void WiFiScan::RunBeaconList(uint8_t scan_mode, uint16_t color)
{
  //Serial.println("Beacon list...");
  display_obj.TOP_FIXED_AREA_2 = 48;
  display_obj.tteBar = true;
  display_obj.print_delay_1 = 15;
  display_obj.print_delay_2 = 10;
  //display_obj.clearScreen();
  display_obj.initScrollValues(true);
  display_obj.tft.setTextWrap(false);
  display_obj.tft.setTextColor(TFT_BLACK, color);
  display_obj.tft.fillRect(0,16,240,16, color);
  display_obj.tft.drawCentreString(" Beacon Spam List ",120,16,2);
  display_obj.touchToExit();
  display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
  packets_sent = 0;
  //esp_wifi_set_mode(WIFI_MODE_STA);
  //WiFi.mode(WIFI_AP_STA);
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  //WiFi.mode(WIFI_AP_STA);
  esp_wifi_set_mode(WIFI_AP_STA);
  esp_wifi_start();
  esp_wifi_set_promiscuous_filter(NULL);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_max_tx_power(78);
  this->wifi_initialized = true;
  initTime = millis();
  //display_obj.clearScreen();
  //Serial.println("End of func");
}
*/

// Function to prepare for beacon mimic
void WiFiScan::RunMimicFlood(uint8_t scan_mode, uint16_t color) {
  display_obj.TOP_FIXED_AREA_2 = 48;
  display_obj.tteBar = true;
  display_obj.print_delay_1 = 15;
  display_obj.print_delay_2 = 10;
  //display_obj.clearScreen();
  display_obj.initScrollValues(true);
  display_obj.tft.setTextWrap(false);
  display_obj.tft.setTextColor(TFT_BLACK, color);
  display_obj.tft.fillRect(0,16,240,16, color);
  display_obj.tft.drawCentreString(" Mimic Flood ",120,16,2);
  display_obj.touchToExit();
  display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
  packets_sent = 0;
  //esp_wifi_set_mode(WIFI_MODE_STA);
  //WiFi.mode(WIFI_AP_STA);
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  //WiFi.mode(WIFI_AP_STA);
  esp_wifi_set_mode(WIFI_AP_STA);
  esp_wifi_start();
  esp_wifi_set_promiscuous_filter(NULL);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_max_tx_power(78);
  this->wifi_initialized = true;
  initTime = millis();
  //display_obj.clearScreen();
  //Serial.println("End of func");
}

// Function to prepare for beacon spam
void WiFiScan::RunProbeFlood(uint8_t scan_mode, uint16_t color) {
  display_obj.TOP_FIXED_AREA_2 = 48;
  display_obj.tteBar = true;
  display_obj.print_delay_1 = 15;
  display_obj.print_delay_2 = 10;
  //display_obj.clearScreen();
  display_obj.initScrollValues(true);
  display_obj.tft.setTextWrap(false);
  display_obj.tft.setTextColor(TFT_BLACK, color);
  display_obj.tft.fillRect(0,16,240,16, color);
  display_obj.tft.drawCentreString(" Auth Flood ",120,16,2);
  display_obj.touchToExit();
  display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
  packets_sent = 0;
  //esp_wifi_set_mode(WIFI_MODE_STA);
  //WiFi.mode(WIFI_AP_STA);
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  //WiFi.mode(WIFI_AP_STA);
  esp_wifi_set_mode(WIFI_AP_STA);
  esp_wifi_start();
  esp_wifi_set_promiscuous_filter(NULL);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_max_tx_power(78);
  this->wifi_initialized = true;
  initTime = millis();
  //display_obj.clearScreen();
  //Serial.println("End of func");
}

// Function to prepare for beacon spam
void WiFiScan::RunBeaconSpam(uint8_t scan_mode, uint16_t color)
{
  //Serial.println("Beacon Spam...");
  display_obj.TOP_FIXED_AREA_2 = 48;
  display_obj.tteBar = true;
  display_obj.print_delay_1 = 15;
  display_obj.print_delay_2 = 10;
  //display_obj.clearScreen();
  display_obj.initScrollValues(true);
  display_obj.tft.setTextWrap(false);
  display_obj.tft.setTextColor(TFT_BLACK, color);
  display_obj.tft.fillRect(0,16,240,16, color);
  display_obj.tft.drawCentreString(" Beacon Spam Random ",120,16,2);
  display_obj.touchToExit();
  display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
  packets_sent = 0;
  //esp_wifi_set_mode(WIFI_MODE_STA);
  //WiFi.mode(WIFI_AP_STA);
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  //WiFi.mode(WIFI_AP_STA);
  esp_wifi_set_mode(WIFI_AP_STA);
  esp_wifi_start();
  esp_wifi_set_promiscuous_filter(NULL);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_max_tx_power(78);
  this->wifi_initialized = true;
  initTime = millis();
  //display_obj.clearScreen();
  //Serial.println("End of func");
}

void WiFiScan::RunPwnScan(uint8_t scan_mode, uint16_t color)
{
  sd_obj.openCapture("pwnagotchi");
  
  display_obj.TOP_FIXED_AREA_2 = 48;
  display_obj.tteBar = true;
  display_obj.print_delay_1 = 15;
  display_obj.print_delay_2 = 10;
  //display_obj.clearScreen();
  display_obj.initScrollValues(true);
  display_obj.tft.setTextWrap(false);
  display_obj.tft.setTextColor(TFT_WHITE, color);
  display_obj.tft.fillRect(0,16,240,16, color);
  display_obj.tft.drawCentreString(" Detect Pwnagotchi ",120,16,2);
  display_obj.touchToExit();
  display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
  display_obj.setupScrollArea(display_obj.TOP_FIXED_AREA_2, BOT_FIXED_AREA);
  //wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
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
  
  display_obj.TOP_FIXED_AREA_2 = 48;
  display_obj.tteBar = true;
  display_obj.print_delay_1 = 15;
  display_obj.print_delay_2 = 10;
  //display_obj.clearScreen();
  display_obj.initScrollValues(true);
  display_obj.tft.setTextWrap(false);
  display_obj.tft.setTextColor(TFT_WHITE, color);
  display_obj.tft.fillRect(0,16,240,16, color);
  display_obj.tft.drawCentreString(" Beacon Sniffer ",120,16,2);
  display_obj.touchToExit();
  display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
  display_obj.setupScrollArea(display_obj.TOP_FIXED_AREA_2, BOT_FIXED_AREA);
  //wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
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
  
  display_obj.TOP_FIXED_AREA_2 = 48;
  display_obj.tteBar = true;
  display_obj.print_delay_1 = 15;
  display_obj.print_delay_2 = 10;
  //display_obj.clearScreen();
  display_obj.initScrollValues(true);
  display_obj.tft.setTextWrap(false);
  display_obj.tft.setTextColor(TFT_BLACK, color);
  display_obj.tft.fillRect(0,16,240,16, color);
  display_obj.tft.drawCentreString(" Deauthentication Sniffer ",120,16,2);
  display_obj.touchToExit();
  display_obj.tft.setTextColor(TFT_RED, TFT_BLACK);
  display_obj.setupScrollArea(display_obj.TOP_FIXED_AREA_2, BOT_FIXED_AREA);
  //wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
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
  
  display_obj.TOP_FIXED_AREA_2 = 48;
  display_obj.tteBar = true;
  display_obj.print_delay_1 = 15;
  display_obj.print_delay_2 = 10;
  //display_obj.clearScreen();
  display_obj.initScrollValues(true);
  display_obj.tft.setTextWrap(false);
  display_obj.tft.setTextColor(TFT_BLACK, color);
  display_obj.tft.fillRect(0,16,240,16, color);
  display_obj.tft.drawCentreString(" Probe Request Sniffer ",120,16,2);
  display_obj.touchToExit();
  display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
  display_obj.setupScrollArea(display_obj.TOP_FIXED_AREA_2, BOT_FIXED_AREA);
  //wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
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
  display_obj.print_delay_1 = 50;
  display_obj.print_delay_2 = 20;

  /*
  esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  esp_err_t init_ret = esp_bt_controller_init(&bt_cfg);
  if (init_ret != ESP_OK)
    Serial.println("Could not initialize BT Controller: " + (String)init_ret);
  
  //esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT);
  //esp_bt_controller_disable();
  
  
  esp_err_t ret = esp_bt_controller_enable(ESP_BT_MODE_BTDM);
  if (ret != ESP_OK)
    Serial.println("Could not enable BT Controller: " + (String)ret);

  Serial.println("BT Controller Status: " + (String)esp_bt_controller_get_status());
  */
  
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  if (scan_mode == BT_SCAN_ALL)
  {
    display_obj.TOP_FIXED_AREA_2 = 48;
    display_obj.tteBar = true;
    //display_obj.clearScreen();
    display_obj.initScrollValues(true);
    display_obj.tft.setTextWrap(false);
    display_obj.tft.setTextColor(TFT_BLACK, color);
    display_obj.tft.fillRect(0,16,240,16, color);
    display_obj.tft.drawCentreString(" Bluetooth Sniff ",120,16,2);
    display_obj.touchToExit();
    display_obj.tft.setTextColor(TFT_CYAN, TFT_BLACK);
    display_obj.setupScrollArea(display_obj.TOP_FIXED_AREA_2, BOT_FIXED_AREA);
    pBLEScan->setAdvertisedDeviceCallbacks(new bluetoothScanAllCallback());
    //bluetoothScanAllCallback myCallbacks;
    //pBLEScan->setAdvertisedDeviceCallbacks(&myCallbacks);
  }
  else if (scan_mode == BT_SCAN_SKIMMERS)
  {
    display_obj.TOP_FIXED_AREA_2 = 160;
    display_obj.tteBar = true;
    //display_obj.clearScreen();
    display_obj.tft.fillScreen(TFT_DARKGREY);
    display_obj.initScrollValues(true);
    display_obj.tft.setTextWrap(false);
    display_obj.tft.setTextColor(TFT_BLACK, color);
    display_obj.tft.fillRect(0,16,240,16, color);
    display_obj.tft.drawCentreString(" Detect Card Skimmers ",120,16,2);
    display_obj.twoPartDisplay("Scanning for\nBluetooth-enabled skimmers\nHC-03, HC-05, and HC-06...");
    display_obj.tft.setTextColor(TFT_BLACK, TFT_DARKGREY);
    display_obj.setupScrollArea(display_obj.TOP_FIXED_AREA_2, BOT_FIXED_AREA);
    pBLEScan->setAdvertisedDeviceCallbacks(new bluetoothScanSkimmersCallback());
  }
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
  pBLEScan->start(0, scanCompleteCB);
  Serial.println("Started BLE Scan");
  this->ble_initialized = true;
  initTime = millis();
}

// Function that is called when BLE scan is completed
void WiFiScan::scanCompleteCB(BLEScanResults scanResults) {
  printf("Scan complete!\n");
  printf("Found %d devices\n", scanResults.getCount());
  scanResults.dump();
} // scanCompleteCB


// Function to extract MAC addr from a packet at given offset
void WiFiScan::getMAC(char *addr, uint8_t* data, uint16_t offset) {
  sprintf(addr, "%02x:%02x:%02x:%02x:%02x:%02x", data[offset+0], data[offset+1], data[offset+2], data[offset+3], data[offset+4], data[offset+5]);
}

void WiFiScan::espressifSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type)
{
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

  //if (display_obj.display_buffer->size() == 0)
  //{
  display_obj.loading = true;
  display_obj.display_buffer->add(display_string);
  display_obj.loading = false;
  //}
  

  
  Serial.println();

  sd_obj.addPacket(snifferPacket->payload, len);
  //}
}

void WiFiScan::pwnSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type)
{
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
    if ((snifferPacket->payload[0] == 0x80) && (display_obj.display_buffer->size() == 0))
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
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(essid);
        if (!json.success()) {
          Serial.println("\nCould not parse Pwnagotchi json");
          display_string.concat(essid);
        }
        else {
          Serial.println("\nSuccessfully parsed json");
          String json_output;
          json.printTo(json_output);
          Serial.println(json_output);
          display_string.concat(json["name"].as<String>() + " pwnd: " + json["pwnd_tot"].as<String>());
        }
  
        int temp_len = display_string.length();
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
        
  
        
        Serial.println();
  
        sd_obj.addPacket(snifferPacket->payload, len);
      }
    }
  }
}

void WiFiScan::apSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type)
{
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
    if ((snifferPacket->payload[0] == 0x80) && (display_obj.display_buffer->size() == 0))
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
  
        if (display_obj.display_buffer->size() == 0)
        {
          display_obj.loading = true;
          display_obj.display_buffer->add(display_string);
          display_obj.loading = false;
        }
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
                          false};

        access_points->add(ap);

        Serial.print(access_points->size());

        Serial.println();
  
        sd_obj.addPacket(snifferPacket->payload, len);
      }
    }
  }
}

void WiFiScan::beaconSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type)
{
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
    if ((snifferPacket->payload[0] == 0x80) && (display_obj.display_buffer->size() == 0))
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
      

      
      Serial.println();

      sd_obj.addPacket(snifferPacket->payload, len);
    }
  }
}

void WiFiScan::deauthSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type)
{
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
    if ((snifferPacket->payload[0] == 0xA0 || snifferPacket->payload[0] == 0xC0 ) && (display_obj.display_buffer->size() == 0))
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
      display_string.concat(" RSSI: ");
      display_string.concat(snifferPacket->rx_ctrl.rssi);

      display_string.concat(" ");
      display_string.concat(addr);

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
      

      
      Serial.println();

      sd_obj.addPacket(snifferPacket->payload, len);
    }
  }
}

void WiFiScan::probeSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
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
    if ((snifferPacket->payload[0] == 0x40) && (display_obj.display_buffer->size() == 0))
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
      
      Serial.println();    

      sd_obj.addPacket(snifferPacket->payload, len);
    }
  }
}

void WiFiScan::beaconListSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
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
    if ((snifferPacket->payload[0] == 0x40) && (display_obj.display_buffer->size() == 0))
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
      
      Serial.println();    

      sd_obj.addPacket(snifferPacket->payload, len);
    }
  }
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

  
  /////////////////////////////
  //int essid_len = random(6, 10);

  // random prefix to beacon essid
  //uint8_t rand_reg[essid_len] = {};
  //for (int i = 0; i < essid_len; i++)
  //  rand_reg[i] = alfa[random(65)];

  char ESSID[custom_ssid.essid.length() + 1] = {};
  custom_ssid.essid.toCharArray(ESSID, custom_ssid.essid.length() + 1);

  int ssidLen = strlen(ESSID);
  //int rand_len = sizeof(rand_reg);
  int fullLen = ssidLen;
  packet[37] = fullLen;

  // Insert random prefix
  //for (int i = 0; i < rand_len; i++)
  //  packet[38+i] = rand_reg[i];

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
  //esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);
  //esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);
  //esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);

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

  
  /////////////////////////////
  //int essid_len = random(6, 10);

  // random prefix to beacon essid
  //uint8_t rand_reg[essid_len] = {};
  //for (int i = 0; i < essid_len; i++)
  //  rand_reg[i] = alfa[random(65)];

  int ssidLen = strlen(ESSID);
  //int rand_len = sizeof(rand_reg);
  int fullLen = ssidLen;
  packet[37] = fullLen;

  // Insert random prefix
  //for (int i = 0; i < rand_len; i++)
  //  packet[38+i] = rand_reg[i];

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
  //esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);
  //esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);
  //esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);

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

  //Serial.println("About to send packets...");

  esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);
  esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);
  esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);
  //esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);
  //esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);
  //esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);

  packets_sent = packets_sent + 3;

  //Serial.print("Packets sent: ");
  //Serial.println(packets_sent);
  
  //Serial.println("Sent packets");
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
      
      /*
      prob_req_packet[10] = 0xde;
      prob_req_packet[11] = 0xad;
      prob_req_packet[12] = 0xbe;
      prob_req_packet[13] = 0xef;
      prob_req_packet[14] = 0xde;
      prob_req_packet[15] = 0xad;
      */

      // Set dest
      //assoc_packet[4] = access_points->get(i).bssid[0];
      //assoc_packet[5] = access_points->get(i).bssid[1];
      //assoc_packet[6] = access_points->get(i).bssid[2];
      //assoc_packet[7] = access_points->get(i).bssid[3];
      //assoc_packet[8] = access_points->get(i).bssid[4];
      //assoc_packet[9] = access_points->get(i).bssid[5];

      // Set det No. 2
      //assoc_packet[16] = access_points->get(i).bssid[0];
      //assoc_packet[17] = access_points->get(i).bssid[1];
      //assoc_packet[18] = access_points->get(i).bssid[2];
      //assoc_packet[19] = access_points->get(i).bssid[3];
      //assoc_packet[20] = access_points->get(i).bssid[4];
      //assoc_packet[21] = access_points->get(i).bssid[5];

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


void WiFiScan::wifiSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type)
{
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

    sd_obj.addPacket(snifferPacket->payload, len);

    if (( (snifferPacket->payload[30] == 0x88 && snifferPacket->payload[31] == 0x8e)|| ( snifferPacket->payload[32] == 0x88 && snifferPacket->payload[33] == 0x8e) ))
      Serial.println("Oh god mgmt EAPOL");
  }

  if (( (snifferPacket->payload[30] == 0x88 && snifferPacket->payload[31] == 0x8e)|| ( snifferPacket->payload[32] == 0x88 && snifferPacket->payload[33] == 0x8e) )){
    Serial.println("EAPOL!!");
    //sd_obj.addPacket(snifferPacket->payload, len);
  }
}

void WiFiScan::eapolSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type)
{
  /*
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
  }
  if (( (snifferPacket->payload[30] == 0x88 && snifferPacket->payload[31] == 0x8e)|| ( snifferPacket->payload[32] == 0x88 && snifferPacket->payload[33] == 0x8e) )){
    delay(random(0, 10));
    Serial.print((String)random(0, 5) + "EAPOL: ");
    char srcaddr[] = "00:00:00:00:00:00";
    getMAC(srcaddr, snifferPacket->payload, 10);
    Serial.print(srcaddr);
    display_string.concat(srcaddr);
    Serial.print(" -> ");
    display_string.concat(" -> ");
    char desaddr[] = "00:00:00:00:00:00";
    getMAC(desaddr, snifferPacket->payload, 4);
    Serial.print(desaddr);
    display_string.concat(desaddr); 
    // Print spaces because of the rotating lines of the hardware scroll.
    // The same characters print from previous lines so I just overwrite them
    // with spaces.
    for (int i = 0; i < 19 - snifferPacket->payload[37]; i++)
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
    
    Serial.println();    
    sd_obj.addPacket(snifferPacket->payload, len);
  }
  */
  wifi_promiscuous_pkt_t *snifferPacket = (wifi_promiscuous_pkt_t*)buf;
  WifiMgmtHdr *frameControl = (WifiMgmtHdr*)snifferPacket->payload;
  wifi_pkt_rx_ctrl_t ctrl = (wifi_pkt_rx_ctrl_t)snifferPacket->rx_ctrl;
  int len = snifferPacket->rx_ctrl.sig_len;

  if (len == 173) {
    Serial.println("Maybe the PMKID");
    //sd_obj.addPacket(snifferPacket->payload, len);
  }

  if (type == WIFI_PKT_MGMT)
  {
    len -= 4;
    int fctl = ntohs(frameControl->fctl);
    const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)snifferPacket->payload;
    const WifiMgmtHdr *hdr = &ipkt->hdr;

    // If we dont the buffer size is not 0, don't write or else we get CORRUPT_HEAP
    /*
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
    */

    //sd_obj.addPacket(snifferPacket->payload, len);

    if (( (snifferPacket->payload[30] == 0x88 && snifferPacket->payload[31] == 0x8e)|| ( snifferPacket->payload[32] == 0x88 && snifferPacket->payload[33] == 0x8e) ))
      Serial.println("Oh god mgmt EAPOL");
  }

  if (( (snifferPacket->payload[30] == 0x88 && snifferPacket->payload[31] == 0x8e)|| ( snifferPacket->payload[32] == 0x88 && snifferPacket->payload[33] == 0x8e) )){
    Serial.println("EAPOL!!");
    //sd_obj.addPacket(snifferPacket->payload, len);
    num_eapol++;
  }

  sd_obj.addPacket(snifferPacket->payload, len);
}

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

        /*
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
        }*/

        // Channel - button pressed
        //else if (b == 4) {
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
  //tft.fillRect(56, 0, 66, 32, TFT_ORANGE); //erase time and color key and any stray lines behind them
  display_obj.tft.fillRect(12, 0, 90, 32, TFT_BLACK); // key

  //display_obj.tftDrawXScaleButtons(x_scale); //redraw stuff
  //display_obj.tftDrawYScaleButtons(y_scale);
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


//void WiFiScan::sniffer_callback(void* buf, wifi_promiscuous_pkt_type_t type) {
//  wifi_promiscuous_pkt_t *snifferPacket = (wifi_promiscuous_pkt_t*)buf;
//  showMetadata(snifferPacket, type);
//}

void WiFiScan::changeChannel()
{
  esp_wifi_set_channel(set_channel, WIFI_SECOND_CHAN_NONE);
  delay(1);
}

// Function to cycle to the next channel
void WiFiScan::channelHop()
{
  set_channel = set_channel + 1;
  if (set_channel > 13) {
    set_channel = 1;
  }
  esp_wifi_set_channel(set_channel, WIFI_SECOND_CHAN_NONE);
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
    if (currentTime - initTime >= 1000)
    {
      initTime = millis();
      channelHop();
    }
  }
  else if (currentScanMode == WIFI_PACKET_MONITOR)
  {
    packetMonitorMain(currentTime);
  }
  else if (currentScanMode == WIFI_SCAN_EAPOL)
  {
    eapolMonitorMain(currentTime);
  }
  else if (currentScanMode == WIFI_ATTACK_AUTH) {
    for (int i = 0; i < 55; i++)
      this->sendProbeAttack(currentTime);

    if (currentTime - initTime >= 1000) {
      initTime = millis();
      String displayString = "";
      String displayString2 = "";
      displayString.concat("packets/sec: ");
      displayString.concat(packets_sent);
      for (int x = 0; x < STANDARD_FONT_CHAR_LIMIT; x++)
        displayString2.concat(" ");
      display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
      display_obj.showCenterText(displayString2, 160);
      display_obj.showCenterText(displayString, 160);
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
      displayString.concat("packets/sec: ");
      displayString.concat(packets_sent);
      for (int x = 0; x < STANDARD_FONT_CHAR_LIMIT; x++)
        displayString2.concat(" ");
      display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
      display_obj.showCenterText(displayString2, 160);
      display_obj.showCenterText(displayString, 160);
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
      displayString.concat("packets/sec: ");
      displayString.concat(packets_sent);
      for (int x = 0; x < STANDARD_FONT_CHAR_LIMIT; x++)
        displayString2.concat(" ");
      display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
      display_obj.showCenterText(displayString2, 160);
      display_obj.showCenterText(displayString, 160);
      packets_sent = 0;
    }
  }
  else if ((currentScanMode == WIFI_ATTACK_BEACON_LIST)) {
    for (int i = 0; i < ssids->size(); i++)
      this->broadcastCustomBeacon(currentTime, ssids->get(i));

    if (currentTime - initTime >= 1000)
    {
      initTime = millis();
      //Serial.print("packets/sec: ");
      //Serial.println(packets_sent);
      //String displayString = "";
      //String displayString2 = "";
      //displayString.concat("packets/sec: ");
      //displayString.concat(packets_sent);
      //for (int x = 0; x < STANDARD_FONT_CHAR_LIMIT; x++)
      //  displayString2.concat(" ");
      //display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
      //display_obj.showCenterText(displayString2, 160);
      //display_obj.showCenterText(displayString, 160);
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
      displayString.concat("packets/sec: ");
      displayString.concat(packets_sent);
      for (int x = 0; x < STANDARD_FONT_CHAR_LIMIT; x++)
        displayString2.concat(" ");
      display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
      display_obj.showCenterText(displayString2, 160);
      display_obj.showCenterText(displayString, 160);
      packets_sent = 0;
    }
  }
  //else if (currentScanMode == LV_JOIN_WIFI)
  //  lv_task_handler();
}
