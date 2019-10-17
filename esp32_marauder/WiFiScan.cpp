#include "WiFiScan.h"


class bluetoothScanAllCallback: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
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
      Serial.print("Device: ");
      if(advertisedDevice.getName().length() != 0)
      {
        Serial.print(advertisedDevice.getName().c_str());
        for(int i = 0; i < bad_list_length; i++)
        {
          if(strcmp(advertisedDevice.getName().c_str(), bad_list[i].c_str()) == 0)
          {
            Serial.println("Found some shit");
          }
        }
      }
      else
        Serial.print(advertisedDevice.getAddress().toString().c_str());
      Serial.print(" RSSI: ");
      Serial.println(advertisedDevice.getRSSI());
    }
};


WiFiScan::WiFiScan()
{
}

// Function to prepare to run a specific scan
void WiFiScan::StartScan(uint8_t scan_mode, uint16_t color)
{  
  if (scan_mode == WIFI_SCAN_OFF)
    StopScan(scan_mode);
  else if (scan_mode == WIFI_SCAN_PROBE)
    RunProbeScan(scan_mode, color);
  else if (scan_mode == WIFI_SCAN_AP)
    RunBeaconScan(scan_mode, color);
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
  (currentScanMode == WIFI_SCAN_ALL))
  {
    esp_wifi_set_promiscuous(false);
  }
  else if ((currentScanMode == BT_SCAN_ALL) ||
  (currentScanMode == BT_SCAN_SKIMMERS))
  {
    Serial.println("Stopping BLE scan...");
    pBLEScan->stop();
    Serial.println("BLE Scan Stopped");
  }

  display_obj.display_buffer->clear();
  Serial.print("display_buffer->size(): ");
  Serial.println(display_obj.display_buffer->size());
}

// Function for updating scan status
void WiFiScan::main(uint32_t currentTime)
{
  // WiFi operations
  if ((currentScanMode == WIFI_SCAN_PROBE) ||
  (currentScanMode == WIFI_SCAN_AP) ||
  (currentScanMode == WIFI_SCAN_ST) ||
  (currentScanMode == WIFI_SCAN_ALL))
  {
    if (currentTime - initTime >= 1000)
    {
      initTime = millis();
      channelHop();
    }
  }
}

// Function to start running a beacon scan
void WiFiScan::RunBeaconScan(uint8_t scan_mode, uint16_t color)
{
  display_obj.print_delay_1 = 15;
  display_obj.print_delay_2 = 10;
  display_obj.clearScreen();
  display_obj.initScrollValues();
  display_obj.tft.setTextWrap(false);
  display_obj.tft.setTextColor(TFT_WHITE, color);
  display_obj.tft.fillRect(0,0,240,16, color);
  display_obj.tft.drawCentreString(" Beacon Sniffer ",120,0,2);
  display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
  display_obj.setupScrollArea(TOP_FIXED_AREA, BOT_FIXED_AREA);
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_filter(&filt);
  esp_wifi_set_promiscuous_rx_cb(&beaconSnifferCallback);
  esp_wifi_set_channel(set_channel, WIFI_SECOND_CHAN_NONE);
  initTime = millis();
}


// Function for running probe request scan
void WiFiScan::RunProbeScan(uint8_t scan_mode, uint16_t color)
{
  display_obj.print_delay_1 = 15;
  display_obj.print_delay_2 = 10;
  display_obj.clearScreen();
  display_obj.initScrollValues();
  display_obj.tft.setTextWrap(false);
  display_obj.tft.setTextColor(TFT_BLACK, color);
  display_obj.tft.fillRect(0,0,240,16, color);
  display_obj.tft.drawCentreString(" Probe Request Sniffer ",120,0,2);
  display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
  display_obj.setupScrollArea(TOP_FIXED_AREA, BOT_FIXED_AREA);
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
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
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  if (scan_mode == BT_SCAN_ALL)
  {
    display_obj.clearScreen();
    display_obj.initScrollValues();
    display_obj.tft.setTextWrap(false);
    display_obj.tft.setTextColor(TFT_BLACK, color);
    display_obj.tft.fillRect(0,0,240,16, color);
    display_obj.tft.drawCentreString(" Bluetooth Sniff ",120,0,2);
    display_obj.tft.setTextColor(TFT_CYAN, TFT_BLACK);
    display_obj.setupScrollArea(TOP_FIXED_AREA, BOT_FIXED_AREA);
    pBLEScan->setAdvertisedDeviceCallbacks(new bluetoothScanAllCallback());
  }
  else if (scan_mode == BT_SCAN_SKIMMERS)
  {
    display_obj.clearScreen();
    display_obj.initScrollValues();
    display_obj.tft.setTextWrap(false);
    display_obj.tft.setTextColor(TFT_BLACK, color);
    display_obj.tft.fillRect(0,0,240,16, color);
    display_obj.tft.drawCentreString(" Detect Card Skimmers ",120,0,2);
    display_obj.tft.setTextColor(TFT_ORANGE, TFT_BLACK);
    display_obj.setupScrollArea(TOP_FIXED_AREA, BOT_FIXED_AREA);
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

//void WiFiScan::sniffer_callback(void* buf, wifi_promiscuous_pkt_type_t type) {
//  wifi_promiscuous_pkt_t *snifferPacket = (wifi_promiscuous_pkt_t*)buf;
//  showMetadata(snifferPacket, type);
//}

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
