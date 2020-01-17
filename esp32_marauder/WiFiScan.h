#ifndef WiFiScan_h
#define WiFiScan_h

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#include <WiFi.h>
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "Display.h"
//#include "MenuFunctions.h"

#define bad_list_length 3

#define WIFI_SCAN_OFF 0
#define WIFI_SCAN_PROBE 1
#define WIFI_SCAN_AP 2
#define WIFI_SCAN_ST 3
#define WIFI_SCAN_DEAUTH 4
#define WIFI_SCAN_ALL 5
#define WIFI_ATTACK_BEACON_SPAM 6
#define WIFI_ATTACK_RICK_ROLL 7
#define BT_SCAN_ALL 8
#define BT_SCAN_SKIMMERS 9

extern Display display_obj;

esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);

class WiFiScan
{
  private:
    uint32_t initTime = 0;
    bool run_setup = true;
    int set_channel = 1;
    int bluetoothScanTime = 5;
    int packets_sent = 0;
    const wifi_promiscuous_filter_t filt = {.filter_mask=WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA};
    BLEScan* pBLEScan;

    String alfa = "1234567890qwertyuiopasdfghjkklzxcvbnm QWERTYUIOPASDFGHJKLZXCVBNM_";

    char* rick_roll[8] = {
      "01 Never gonna give you up",
      "02 Never gonna let you down",
      "03 Never gonna run around",
      "04 and desert you",
      "05 Never gonna make you cry",
      "06 Never gonna say goodbye",
      "07 Never gonna tell a lie",
      "08 and hurt you"
    };

    char* prefix = "G";

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

    // barebones packet
    uint8_t packet[128] = { 0x80, 0x00, 0x00, 0x00, //Frame Control, Duration
                    /*4*/   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, //Destination address 
                    /*10*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, //Source address - overwritten later
                    /*16*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, //BSSID - overwritten to the same as the source address
                    /*22*/  0xc0, 0x6c, //Seq-ctl
                    /*24*/  0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, //timestamp - the number of microseconds the AP has been active
                    /*32*/  0x64, 0x00, //Beacon interval
                    /*34*/  0x01, 0x04, //Capability info
                    /* SSID */
                    /*36*/  0x00
                    };

    void broadcastRandomSSID(uint32_t currentTime);
    void broadcastSetSSID(uint32_t current_time, char* ESSID);
    void RunRickRoll(uint8_t scan_mode, uint16_t color);
    void RunBeaconSpam(uint8_t scan_mode, uint16_t color);
    void RunBeaconScan(uint8_t scan_mode, uint16_t color);
    void RunDeauthScan(uint8_t scan_mode, uint16_t color);
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
    static void deauthSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type);
    static void probeSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type);
};
#endif
