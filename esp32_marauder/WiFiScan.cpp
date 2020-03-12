#include "WiFiScan.h"

//esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);

//int num_beacon = 0;
int num_beacon = 0;
int num_deauth = 0;
int num_probe = 0;

class bluetoothScanAllCallback: public BLEAdvertisedDeviceCallbacks {
    
    void onResult(BLEAdvertisedDevice advertisedDevice) {

      //advertisedDevice.getScan()->stop();
      
      String display_string = "";
      if (display_obj.display_buffer->size() >= 0)
      {
        display_string.concat(" RSSI: ");
        display_string.concat(advertisedDevice.getRSSI());
        Serial.print(" RSSI: ");
        Serial.print(advertisedDevice.getRSSI());

        display_string.concat(" ");
        Serial.print(" ");
        
        Serial.print("Device: ");
        if(advertisedDevice.getName().length() != 0)
        {
          display_string.concat(advertisedDevice.getName().c_str());
          Serial.print(advertisedDevice.getName().c_str());
          
        }
        else
        {
          display_string.concat(advertisedDevice.getAddress().toString().c_str());
          Serial.print(advertisedDevice.getAddress().toString().c_str());
        }

        int temp_len = display_string.length();
        for (int i = 0; i < 40 - temp_len; i++)
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
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      String bad_list[bad_list_length] = {"HC-03", "HC-05", "HC-06"};
      if (display_obj.display_buffer->size() >= 0)
      {
        Serial.print("Device: ");
        String display_string = "";
        //display_string.concat(" RSSI: ");
        //display_string.concat(advertisedDevice.getRSSI());
        //display_string.concat(" ");
        if(advertisedDevice.getName().length() != 0)
        {
          //display_string.concat(advertisedDevice.getName().c_str());
          Serial.print(advertisedDevice.getName().c_str());
          for(int i = 0; i < bad_list_length; i++)
          {
            if(strcmp(advertisedDevice.getName().c_str(), bad_list[i].c_str()) == 0)
            {
              display_string.concat("Potential Skimmer: ");
              display_string.concat(" ");
              display_string.concat(advertisedDevice.getName().c_str());
              int temp_len = display_string.length();
              for (int i = 0; i < 40 - temp_len; i++)
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
          Serial.print(advertisedDevice.getAddress().toString().c_str());
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
        Serial.println(advertisedDevice.getRSSI());

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

// Function to prepare to run a specific scan
void WiFiScan::StartScan(uint8_t scan_mode, uint16_t color)
{  
  //Serial.println("Starting Scan...");
  if (scan_mode == WIFI_SCAN_OFF)
    StopScan(scan_mode);
  else if (scan_mode == WIFI_SCAN_PROBE)
    RunProbeScan(scan_mode, color);
  else if (scan_mode == WIFI_SCAN_AP)
    RunBeaconScan(scan_mode, color);
  else if (scan_mode == WIFI_SCAN_DEAUTH)
    RunDeauthScan(scan_mode, color);
  else if (scan_mode == WIFI_PACKET_MONITOR)
    RunPacketMonitor(scan_mode, color);
  else if (scan_mode == WIFI_ATTACK_BEACON_SPAM)
    RunBeaconSpam(scan_mode, color);
  else if (scan_mode == WIFI_ATTACK_RICK_ROLL)
    RunRickRoll(scan_mode, color);
  else if (scan_mode == BT_SCAN_ALL)
    RunBluetoothScan(scan_mode, color);
  else if (scan_mode == BT_SCAN_SKIMMERS)
    RunBluetoothScan(scan_mode, color);

  WiFiScan::currentScanMode = scan_mode;
}

// Function to stop all wifi scans
void WiFiScan::StopScan(uint8_t scan_mode)
{
  if ((currentScanMode == WIFI_SCAN_PROBE) ||
  (currentScanMode == WIFI_SCAN_AP) ||
  (currentScanMode == WIFI_SCAN_ST) ||
  (currentScanMode == WIFI_SCAN_ALL) ||
  (currentScanMode == WIFI_SCAN_DEAUTH) ||
  (currentScanMode == WIFI_ATTACK_BEACON_SPAM) ||
  (currentScanMode == WIFI_ATTACK_RICK_ROLL) ||
  (currentScanMode == WIFI_PACKET_MONITOR))
  {
    Serial.println("Ahhh yes...promiscuity will end");
    esp_wifi_set_promiscuous(false);
    WiFi.mode(WIFI_OFF);
  }
  else if ((currentScanMode == BT_SCAN_ALL) ||
  (currentScanMode == BT_SCAN_SKIMMERS))
  {
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
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  esp_err_t mac_status = esp_wifi_get_mac(ESP_IF_WIFI_STA, mac);
  sprintf(macAddrChr, 
          "%02X:%02X:%02X:%02X:%02X:%02X",
          mac[0],
          mac[1],
          mac[2],
          mac[3],
          mac[4],
          mac[5]);
  return String(macAddrChr);
}

String WiFiScan::getApMAC()
{
  char *buf;
  uint8_t mac[6];
  char macAddrChr[18] = {0};
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  esp_err_t mac_status = esp_wifi_get_mac(ESP_IF_WIFI_AP, mac);
  sprintf(macAddrChr, 
          "%02X:%02X:%02X:%02X:%02X:%02X",
          mac[0],
          mac[1],
          mac[2],
          mac[3],
          mac[4],
          mac[5]);
  return String(macAddrChr);
}


String WiFiScan::freeRAM()
{
  char s[150];
  sprintf(s, "RAM Free: %u bytes", system_get_free_heap_size());
  return String(s);
}


void WiFiScan::RunInfo()
{
  //String sta_mac = this->getStaMAC();
  //String ap_mac = this->getApMAC();
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

  //display_obj.tft.println(" Station MAC: " + sta_mac);
  //display_obj.tft.println("      AP MAC: " + ap_mac);
  display_obj.tft.println("    Firmware: Marauder");
  display_obj.tft.println("     Version: " + display_obj.version_number + "\n");
  display_obj.tft.println("    " + free_ram);

  if (sd_obj.supported) {
    display_obj.tft.println("     SD Card: Connected");
    display_obj.tft.print("SD Card Size: ");
    display_obj.tft.print(sd_obj.card_sz);
    display_obj.tft.println("MB");
  }
  else {
    display_obj.tft.println("     SD Card: Not Connected");
    display_obj.tft.print("SD Card Size: 0");
  }

  
}

void WiFiScan::RunPacketMonitor(uint8_t scan_mode, uint16_t color)
{
  display_obj.tft.init();
  display_obj.tft.setRotation(1);
  display_obj.tft.fillScreen(TFT_BLACK);

  sd_obj.openCapture();
  
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
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_filter(&filt);
  esp_wifi_set_promiscuous_rx_cb(&wifiSnifferCallback);
  esp_wifi_set_channel(set_channel, WIFI_SECOND_CHAN_NONE);
  uint32_t initTime = millis();
}

void WiFiScan::RunRickRoll(uint8_t scan_mode, uint16_t color)
{
  //Serial.println("Rick Roll...");
  display_obj.TOP_FIXED_AREA_2 = 32;
  display_obj.tteBar = true;
  display_obj.print_delay_1 = 15;
  display_obj.print_delay_2 = 10;
  display_obj.clearScreen();
  display_obj.initScrollValues(true);
  display_obj.tft.setTextWrap(false);
  display_obj.tft.setTextColor(TFT_BLACK, color);
  display_obj.tft.fillRect(0,0,240,16, color);
  display_obj.tft.drawCentreString(" Rick Roll Beacon ",120,0,2);
  display_obj.touchToExit();
  display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
  packets_sent = 0;
  //esp_wifi_set_mode(WIFI_MODE_STA);
  WiFi.mode(WIFI_AP_STA);
  esp_wifi_start();
  esp_wifi_set_promiscuous_filter(NULL);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_max_tx_power(78);
  initTime = millis();
  //display_obj.clearScreen();
  //Serial.println("End of func");
}

// Function to prepare for beacon spam
void WiFiScan::RunBeaconSpam(uint8_t scan_mode, uint16_t color)
{
  //Serial.println("Beacon Spam...");
  display_obj.TOP_FIXED_AREA_2 = 32;
  display_obj.tteBar = true;
  display_obj.print_delay_1 = 15;
  display_obj.print_delay_2 = 10;
  display_obj.clearScreen();
  display_obj.initScrollValues(true);
  display_obj.tft.setTextWrap(false);
  display_obj.tft.setTextColor(TFT_BLACK, color);
  display_obj.tft.fillRect(0,0,240,16, color);
  display_obj.tft.drawCentreString(" Beacon Spam Random ",120,0,2);
  display_obj.touchToExit();
  display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
  packets_sent = 0;
  //esp_wifi_set_mode(WIFI_MODE_STA);
  WiFi.mode(WIFI_AP_STA);
  esp_wifi_start();
  esp_wifi_set_promiscuous_filter(NULL);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_max_tx_power(78);
  initTime = millis();
  //display_obj.clearScreen();
  //Serial.println("End of func");
}

// Function to start running a beacon scan
void WiFiScan::RunBeaconScan(uint8_t scan_mode, uint16_t color)
{
  display_obj.TOP_FIXED_AREA_2 = 32;
  display_obj.tteBar = true;
  display_obj.print_delay_1 = 15;
  display_obj.print_delay_2 = 10;
  display_obj.clearScreen();
  display_obj.initScrollValues(true);
  display_obj.tft.setTextWrap(false);
  display_obj.tft.setTextColor(TFT_WHITE, color);
  display_obj.tft.fillRect(0,0,240,16, color);
  display_obj.tft.drawCentreString(" Beacon Sniffer ",120,0,2);
  display_obj.touchToExit();
  display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
  display_obj.setupScrollArea(display_obj.TOP_FIXED_AREA_2, BOT_FIXED_AREA);
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_filter(&filt);
  esp_wifi_set_promiscuous_rx_cb(&beaconSnifferCallback);
  esp_wifi_set_channel(set_channel, WIFI_SECOND_CHAN_NONE);
  initTime = millis();
}

void WiFiScan::RunDeauthScan(uint8_t scan_mode, uint16_t color)
{
  display_obj.TOP_FIXED_AREA_2 = 32;
  display_obj.tteBar = true;
  display_obj.print_delay_1 = 15;
  display_obj.print_delay_2 = 10;
  display_obj.clearScreen();
  display_obj.initScrollValues(true);
  display_obj.tft.setTextWrap(false);
  display_obj.tft.setTextColor(TFT_BLACK, color);
  display_obj.tft.fillRect(0,0,240,16, color);
  display_obj.tft.drawCentreString(" Deauthentication Sniffer ",120,0,2);
  display_obj.touchToExit();
  display_obj.tft.setTextColor(TFT_RED, TFT_BLACK);
  display_obj.setupScrollArea(display_obj.TOP_FIXED_AREA_2, BOT_FIXED_AREA);
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_filter(&filt);
  esp_wifi_set_promiscuous_rx_cb(&deauthSnifferCallback);
  esp_wifi_set_channel(set_channel, WIFI_SECOND_CHAN_NONE);
  initTime = millis();
}


// Function for running probe request scan
void WiFiScan::RunProbeScan(uint8_t scan_mode, uint16_t color)
{
  display_obj.TOP_FIXED_AREA_2 = 32;
  display_obj.tteBar = true;
  display_obj.print_delay_1 = 15;
  display_obj.print_delay_2 = 10;
  display_obj.clearScreen();
  display_obj.initScrollValues(true);
  display_obj.tft.setTextWrap(false);
  display_obj.tft.setTextColor(TFT_BLACK, color);
  display_obj.tft.fillRect(0,0,240,16, color);
  display_obj.tft.drawCentreString(" Probe Request Sniffer ",120,0,2);
  display_obj.touchToExit();
  display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
  display_obj.setupScrollArea(display_obj.TOP_FIXED_AREA_2, BOT_FIXED_AREA);
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_filter(&filt);
  esp_wifi_set_promiscuous_rx_cb(&probeSnifferCallback);
  esp_wifi_set_channel(set_channel, WIFI_SECOND_CHAN_NONE);
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
    display_obj.TOP_FIXED_AREA_2 = 32;
    display_obj.tteBar = true;
    display_obj.clearScreen();
    display_obj.initScrollValues(true);
    display_obj.tft.setTextWrap(false);
    display_obj.tft.setTextColor(TFT_BLACK, color);
    display_obj.tft.fillRect(0,0,240,16, color);
    display_obj.tft.drawCentreString(" Bluetooth Sniff ",120,0,2);
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
    display_obj.clearScreen();
    display_obj.tft.fillScreen(TFT_DARKGREY);
    display_obj.initScrollValues(true);
    display_obj.tft.setTextWrap(false);
    display_obj.tft.setTextColor(TFT_BLACK, color);
    display_obj.tft.fillRect(0,0,240,16, color);
    display_obj.tft.drawCentreString(" Detect Card Skimmers ",120,0,2);
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

void WiFiScan::beaconSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type)
{
  wifi_promiscuous_pkt_t *snifferPacket = (wifi_promiscuous_pkt_t*)buf;
  WifiMgmtHdr *frameControl = (WifiMgmtHdr*)snifferPacket->payload;
  wifi_pkt_rx_ctrl_t ctrl = (wifi_pkt_rx_ctrl_t)snifferPacket->rx_ctrl;
  int len = snifferPacket->rx_ctrl.sig_len;

  String display_string = "";

  if (type == WIFI_PKT_MGMT)
  {
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
    }
  }
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
  }
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


// Function for updating scan status
void WiFiScan::main(uint32_t currentTime)
{
  // WiFi operations
  if ((currentScanMode == WIFI_SCAN_PROBE) ||
  (currentScanMode == WIFI_SCAN_AP) ||
  (currentScanMode == WIFI_SCAN_ST) ||
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
      display_obj.showCenterText(displayString2, 160);
      display_obj.showCenterText(displayString, 160);
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
      display_obj.showCenterText(displayString2, 160);
      display_obj.showCenterText(displayString, 160);
      packets_sent = 0;
    }
  }
}
