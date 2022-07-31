#ifndef WiFiScan_h
#define WiFiScan_h

#include "configs.h"

//#include <BLEDevice.h>
//#include <BLEUtils.h>
//#include <BLEScan.h>
//#include <BLEAdvertisedDevice.h>
#include <ArduinoJson.h>

// Testing NimBLE
#ifdef HAS_BT
  #include <NimBLEDevice.h>
#endif

#include <WiFi.h>
#include <math.h>
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#ifdef HAS_BT
  #include "esp_bt.h"
#endif
#ifdef HAS_SCREEN
  #include "Display.h"
#endif
#include "SDInterface.h"
#include "Buffer.h"
#include "BatteryInterface.h"
#include "TemperatureInterface.h"
#include "settings.h"
#include "Assets.h"
#include "flipperLED.h"
//#include "MenuFunctions.h"

#define bad_list_length 3

#define OTA_UPDATE 100
#define SHOW_INFO 101
#define ESP_UPDATE 102
#define WIFI_SCAN_OFF 0
#define WIFI_SCAN_PROBE 1
#define WIFI_SCAN_AP 2
#define WIFI_SCAN_PWN 3
#define WIFI_SCAN_EAPOL 4
#define WIFI_SCAN_DEAUTH 5
#define WIFI_SCAN_ALL 6
#define WIFI_PACKET_MONITOR 7
#define WIFI_ATTACK_BEACON_SPAM 8
#define WIFI_ATTACK_RICK_ROLL 9
#define BT_SCAN_ALL 10
#define BT_SCAN_SKIMMERS 11
#define WIFI_SCAN_ESPRESSIF 12
#define LV_JOIN_WIFI 13
#define LV_ADD_SSID 14
#define WIFI_ATTACK_BEACON_LIST 15
#define WIFI_SCAN_TARGET_AP 16
#define LV_SELECT_AP 17
#define WIFI_ATTACK_AUTH 18
#define WIFI_ATTACK_MIMIC 19
#define WIFI_ATTACK_DEAUTH 20
#define WIFI_ATTACK_AP_SPAM 21
#define WIFI_SCAN_TARGET_AP_FULL 22
#define WIFI_SCAN_ACTIVE_EAPOL 23

#define GRAPH_REFRESH 100

#define MAX_CHANNEL 14

#ifdef HAS_SCREEN
  extern Display display_obj;
#endif
extern SDInterface sd_obj;
extern Buffer buffer_obj;
extern BatteryInterface battery_obj;
extern TemperatureInterface temp_obj;
extern Settings settings_obj;
extern flipperLED flipper_led;

esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);
//int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3);

struct ssid {
  String essid;
  int bssid[6];
  bool selected;
};

struct AccessPoint {
  String essid;
  int channel;
  int bssid[6];
  bool selected;
  LinkedList<char>* beacon;
};

class WiFiScan
{
  private:
    // Settings
    int channel_hop_delay = 1;
    bool force_pmkid = false;
    bool force_probe = false;
    bool save_pcap = false;
  
    int x_pos; //position along the graph x axis
    float y_pos_x; //current graph y axis position of X value
    float y_pos_x_old = 120; //old y axis position of X value
    float y_pos_y; //current graph y axis position of Y value
    float y_pos_y_old = 120; //old y axis position of Y value
    float y_pos_z; //current graph y axis position of Z value
    float y_pos_z_old = 120; //old y axis position of Z value
    int midway = 0;
    byte x_scale = 1; //scale of graph x axis, controlled by touchscreen buttons
    byte y_scale = 1;

    bool do_break = false;

    bool wsl_bypass_enabled = false;

    //int num_beacon = 0; // GREEN
    //int num_probe = 0; // BLUE
    //int num_deauth = 0; // RED

    uint32_t initTime = 0;
    bool run_setup = true;
    void initWiFi(uint8_t scan_mode);
    int bluetoothScanTime = 5;
    int packets_sent = 0;
    const wifi_promiscuous_filter_t filt = {.filter_mask=WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA};
    #ifdef HAS_BT
      NimBLEScan* pBLEScan;
    #endif

    //String connected_network = "";
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
      uint8_t payload[0];
      WifiMgmtHdr hdr;
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

    /*uint8_t auth_packet[128] = {0xB0, 0x00, 0x3C, 0x00, // Frame Control, Duration
                                0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // Dest
                                0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // Source
                                0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // Dest BSSID
                                0x00, 0x01, // Sequence number
                                0x00, 0x00, // Algo
                                0x01, 0x00, // Auth sequence number
                                0x00, 0x00, // Status Code
                                0x7F, 0x08,
                                0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x40,
                                0xDD, 0x0B, 0x00, 0x17, 0xF2, 0x0A, 0x00, 0x01, // Say it was Apple
                                0x04, 0x00, 0x00, 0x00, 0x00, 0xDD, 0x0A, 0x00,
                                0x10, 0x18, 0x02, 0x00, 0x00, 0x10, 0x00, 0x00,
                                0x00
                                };*/
    uint8_t auth_packet[65] = {0xb0, 0x00, 0x3c, 0x00, 
                              0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
                              0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
                              0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
                              0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 
                              0x7f, 0x08, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 
                              0x00, 0x40, 0xdd, 0x0b, 0x00, 0x17, 0xf2, 0x0a, 
                              0x00, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0xdd, 
                              0x0a, 0x00, 0x10, 0x18, 0x02, 0x00, 0x00, 0x10, 
                              0x00, 0x00, 0x00};

    uint8_t prob_req_packet[128] = {0x40, 0x00, 0x00, 0x00, 
                                  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // Destination
                                  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // Source
                                  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // Dest
                                  0x01, 0x00, // Sequence
                                  0x00, // SSID Parameter
                                  0x00, // SSID Length
                                  /* SSID */
                                  };

    uint8_t deauth_frame_default[26] = {
                              0xc0, 0x00, 0x3a, 0x01,
                              0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                              0xf0, 0xff, 0x02, 0x00
                          };

    void startWiFiAttacks(uint8_t scan_mode, uint16_t color, String title_string);

    void packetMonitorMain(uint32_t currentTime);
    void eapolMonitorMain(uint32_t currentTime);
    void updateMidway();
    void tftDrawXScalButtons();
    void tftDrawYScaleButtons();
    void tftDrawChannelScaleButtons();
    void tftDrawColorKey();
    void tftDrawGraphObjects();
    void sendProbeAttack(uint32_t currentTime);
    void sendDeauthAttack(uint32_t currentTime);
    void sendDeauthFrame(uint8_t bssid[6], int channel);
    void broadcastRandomSSID(uint32_t currentTime);
    void broadcastCustomBeacon(uint32_t current_time, ssid custom_ssid);
    void broadcastCustomBeacon(uint32_t current_time, AccessPoint custom_ssid);
    void broadcastSetSSID(uint32_t current_time, char* ESSID);
    void RunAPScan(uint8_t scan_mode, uint16_t color);
    //void RunRickRoll(uint8_t scan_mode, uint16_t color);
    //void RunBeaconSpam(uint8_t scan_mode, uint16_t color);
    //void RunProbeFlood(uint8_t scan_mode, uint16_t color);
    //void RunDeauthFlood(uint8_t scan_mode, uint16_t color);
    void RunMimicFlood(uint8_t scan_mode, uint16_t color);
    //void RunBeaconList(uint8_t scan_mode, uint16_t color);
    void RunEspressifScan(uint8_t scan_mode, uint16_t color);
    void RunPwnScan(uint8_t scan_mode, uint16_t color);
    void RunBeaconScan(uint8_t scan_mode, uint16_t color);
    void RunDeauthScan(uint8_t scan_mode, uint16_t color);
    void RunEapolScan(uint8_t scan_mode, uint16_t color);
    void RunProbeScan(uint8_t scan_mode, uint16_t color);
    void RunPacketMonitor(uint8_t scan_mode, uint16_t color);
    void RunBluetoothScan(uint8_t scan_mode, uint16_t color);
    void RunLvJoinWiFi(uint8_t scan_mode, uint16_t color);
    #ifdef HAS_BT
      static void scanCompleteCB(BLEScanResults scanResults);
    #endif

    //int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3);

  public:
    WiFiScan();

    //AccessPoint ap_list;

    //LinkedList<ssid>* ssids;

    int set_channel = 1;

    int old_channel = 0;

    bool orient_display = false;
    bool wifi_initialized = false;
    bool ble_initialized = false;

    String free_ram = "";
    String old_free_ram = "";
    String connected_network = "";

    //lv_obj_t * scr = lv_cont_create(NULL, NULL);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT(); 

    char* stringToChar(String string);
    void RunSetup();
    int clearSSIDs();
    int clearAPs();
    bool addSSID(String essid);
    int generateSSIDs(int count = 20);
    bool shutdownWiFi();
    bool shutdownBLE();
    bool scanning();
    void joinWiFi(String ssid, String password);
    String getStaMAC();
    String getApMAC();
    String freeRAM();
    void changeChannel();
    void changeChannel(int chan);
    void RunInfo();
    void RunShutdownWiFi();
    void RunShutdownBLE();
    void RunGenerateSSIDs(int count = 20);
    void RunClearSSIDs();
    void RunClearAPs();
    void channelHop();
    uint8_t currentScanMode = 0;
    void main(uint32_t currentTime);
    void StartScan(uint8_t scan_mode, uint16_t color = 0);
    void StopScan(uint8_t scan_mode);
    
    static void getMAC(char *addr, uint8_t* data, uint16_t offset);
    static void espressifSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type);
    static void pwnSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type);
    static void beaconSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type);
    static void apSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type);
    static void apSnifferCallbackFull(void* buf, wifi_promiscuous_pkt_type_t type);
    static void deauthSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type);
    static void probeSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type);
    static void beaconListSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type);
    static void activeEapolSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type);
    static void eapolSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type);
    static void wifiSnifferCallback(void* buf, wifi_promiscuous_pkt_type_t type);
};
#endif
