#ifndef WiFiScan_h
#define WiFiScan_h

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "Display.h"
//#include "MenuFunctions.h"

#define bad_list_length 3

#define WIFI_SCAN_OFF 0
#define WIFI_SCAN_PROBE 1
#define WIFI_SCAN_AP 2
#define WIFI_SCAN_ST 3
#define WIFI_SCAN_ALL 4
#define BT_SCAN_ALL 5
#define BT_SCAN_SKIMMERS 6

extern Display display_obj;


class WiFiScan
{
  private:
    uint32_t initTime = 0;
    bool run_setup = true;
    int set_channel = 1;
    int bluetoothScanTime = 5;
    const wifi_promiscuous_filter_t filt = {.filter_mask=WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA};
    BLEScan* pBLEScan;

    typedef struct
    {
      int16_t fctl;
      int16_t duration;
      uint8_t da;
      uint8_t sa;
      uint8_t bssid;
      int16_t seqctl;
      unsigned char payload[];
    } __attribute__((packed)) WifiMgmtHdr;
    
    typedef struct {
      WifiMgmtHdr hdr;
      uint8_t payload[0];
    } wifi_ieee80211_packet_t;

    void RunBeaconScan(uint8_t scan_mode, uint16_t color);
    void RunProbeScan(uint8_t scan_mode, uint16_t color);
    void RunBluetoothScan(uint8_t scan_mode, uint16_t color);
    static void scanCompleteCB(BLEScanResults scanResults);

  public:
    WiFiScan();
    
    void channelHop();
    uint8_t currentScanMode = 0;
    void main(uint32_t currentTime);
    void StartScan(uint8_t scan_mode, uint16_t color = 0);
    void StopScan(uint8_t scan_mode);
    
    static void getMAC(char *addr, uint8_t* data, uint16_t offset);
    static void beaconSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type);
    static void probeSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type);
};
#endif
