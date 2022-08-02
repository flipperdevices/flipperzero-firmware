//#define DEBUG

#ifdef ESP8266
#include <ESP8266WiFi.h>
#else // ESP32
#include <WiFi.h>
#endif

#include <climits>
#include "DebugHelpers.h"
#include "FlipperZeroWiFiModuleDefines.h"

#if FLIPPERZERO_DEV_BOARD_V_1 && !ESP32
Please_choose_ESP32S2_Dev_Module_from_Board_Manager
#endif


const uint16_t ASSOCIATIVE_INDEX_ARRAY_MAX_ELEMENTS = UCHAR_MAX + 1;
uint8_t g_associativeIndexArray[ASSOCIATIVE_INDEX_ARRAY_MAX_ELEMENTS];

enum EContext
{
  INVALID = -1,
  ERROR = 0,
  INITIALIZATION,
  SCAN_ANIMATION,
  MONITOR_ANIMATION,
  SCAN_PAGE ,
  MONITOR_PAGE
} g_context = INVALID;

struct SScanInfoDisplay
{
  String m_ssid = "N/A";
  uint8_t m_encryptionType = NA;
  int32_t m_rssi = NA;
  String m_bssid = "N/A";
  int32_t m_channel = NA;
  bool m_isHidden = false;

  uint8_t m_currentAp;
  uint8_t m_totalAp;
};

bool pageChanged = true;
int g_totalAp = 0;
int g_currentNetworkIndex = 0;

bool g_apLocked = false;
String g_bssid;
String g_ssid;

const char* ContextToString(EContext context)
{
  switch(context)
  {
    case INVALID:
      return "invalid";
      break;
    case ERROR:
      return "error";
      break;
    case INITIALIZATION:
      return MODULE_CONTEXT_INITIALIZATION;
      break;
    case SCAN_ANIMATION:
      return MODULE_CONTEXT_SCAN_ANIMATION;
      break;
    case MONITOR_ANIMATION:
      return MODULE_CONTEXT_MONITOR_ANIMATION;
      break;
    case SCAN_PAGE:
      return MODULE_CONTEXT_SCAN;
      break;
    case MONITOR_PAGE:
      return MODULE_CONTEXT_MONITOR;
      break;
    default:
      return "invalid";
      break;
  }
}

void ChangeContext(EContext context)
{
  g_context = context;
  if(context <= EContext::MONITOR_ANIMATION)
  {
    Serial.write(ContextToString(context));
    Serial.flush();
  }
}

const char* EncryptionTypeToString(uint8_t encType)
{
#ifdef ESP8266
  switch(encType)
  {
    case ENC_TYPE_WEP:
      return "WEP";
    case ENC_TYPE_TKIP:
      return "TKIP";
    case ENC_TYPE_CCMP:
      return "CCMP";
    case ENC_TYPE_NONE:
      return "OPEN";
    case ENC_TYPE_AUTO:
      return "AUTO";
    default:
      return "FAIL";
  }
#elif ESP32
  switch(encType)
  {
    case WIFI_AUTH_OPEN:
      return "OPEN";
    case WIFI_AUTH_WEP:
      return "WEP";
    case WIFI_AUTH_WPA_PSK:
      return "WPA";
    case WIFI_AUTH_WPA2_PSK:
      return "WPA2";
    case WIFI_AUTH_WPA_WPA2_PSK:
      return "WP1/2";
    case WIFI_AUTH_WPA2_ENTERPRISE:
      return "WPA2 E";
    default:
      return "FAIL";
  }
#else
  return "FAIL";
#endif
}

void SendError(const char* str)
{
  // Serial.write(str);
}

void SerilizeAndSend(EContext context, const SScanInfoDisplay& scanInfo)
{
  FUNCTION_PERF();
  
  char serilizedData[150];
  sprintf(serilizedData, "%s+%s+%s+%d+%s+%u+%u+%u+%u",
    ContextToString(context),
    scanInfo.m_ssid.c_str(), 
    EncryptionTypeToString(scanInfo.m_encryptionType), 
    scanInfo.m_rssi, 
    scanInfo.m_bssid.c_str(), 
    scanInfo.m_channel, 
    scanInfo.m_isHidden, 
    scanInfo.m_currentAp, 
    scanInfo.m_totalAp);
  Serial.write(serilizedData);
  Serial.flush();
}

bool IsAnyAPLocked()
{
  return g_apLocked;
}

void LockAP()
{
  g_bssid = WiFi.BSSIDstr(g_associativeIndexArray[g_currentNetworkIndex]); 
  g_ssid = WiFi.SSID(g_associativeIndexArray[g_currentNetworkIndex]);
  g_apLocked = true;
}

void UnlockAP()
{
  g_apLocked = false;
}

bool FindLockedAP(int& foundAPIndex)
{
  FUNCTION_PERF();
  
  if(IsAnyAPLocked())
  {
    for(int i = 0; i < g_totalAp; ++i)
    {
      if(g_bssid.equals(WiFi.BSSIDstr(i)))
      {
        foundAPIndex = i;
        return true;
      }
    }
  }
  return false;
}

void NextAP() 
{
  DEBUG_LOG_LN(F("NextAP"));

  if(g_context != EContext::SCAN_PAGE)
  {
    return;
  }
  
  g_currentNetworkIndex = ++g_currentNetworkIndex % g_totalAp;
  pageChanged = true;

  DEBUG_LOG(F("NextAP Index: "));
  DEBUG_LOG_LN(g_currentNetworkIndex);
}

void PreviousAP() 
{
  DEBUG_LOG_LN(F("PreviousAP"));

  if(g_context != EContext::SCAN_PAGE)
  {
    return;
  }
  
  g_currentNetworkIndex = --g_currentNetworkIndex < 0 ? g_totalAp - 1 : g_currentNetworkIndex;
  pageChanged = true;

  DEBUG_LOG(F("PreviousAP Index: "));
  DEBUG_LOG_LN(g_currentNetworkIndex);
}

void ScanMode() 
{
  DEBUG_LOG_LN(F("ScanMode Click"));

  if(g_context < EContext::SCAN_PAGE)
  {
    return;
  }
  
  DEBUG_LOG_LN(F("ScanMode Processing"));
    
  UnlockAP();
  ScanNetworks();
}

void setup() {
#if ESP32 // For FlipperZero's Dev Module v.1
  pinMode(17, OUTPUT);
  digitalWrite(17, false);
#endif
  
  Serial.begin(FLIPPERZERO_SERIAL_BAUD);
  while (!Serial) {
    ; // wait for serial port to connect
  }

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  ChangeContext(EContext::INITIALIZATION);

  delay(500);

  ScanNetworks();
}

bool GetNetworkInfo(const uint8_t currentAp, String& ssid, uint8_t& encType, int32_t& rssi, String& bssid, int32_t& channel, bool& isHidden)
{
  FUNCTION_PERF();

  uint8_t index = g_associativeIndexArray[currentAp];
  uint8_t* fake_BSSID;

#ifdef ESP8266
  bssid = WiFi.BSSIDstr(index);
  return WiFi.getNetworkInfo(index, ssid, encType, rssi, fake_BSSID, channel, isHidden);
#elif ESP32
  bssid = WiFi.BSSIDstr(index);
  const bool result = WiFi.getNetworkInfo(index, ssid, encType, rssi, fake_BSSID, channel);
  isHidden = ssid.length() == 0 ? true : false;
  return result;
#else
  return false;
#endif
}

void clearSortArray()
{
  FUNCTION_PERF();

  for(uint16_t i = 0; i < ASSOCIATIVE_INDEX_ARRAY_MAX_ELEMENTS; ++i)
  {
    g_associativeIndexArray[i] = i;
  }
}

void SortScan(int16_t totalAps) // Sort scan by RSSI
{
  FUNCTION_PERF();
  
  clearSortArray();

  uint8_t temp = 0;
  for(uint16_t i = 0; i < totalAps; ++i)
  {
    for(uint16_t j = i; j < totalAps; ++j)
    {
      if(WiFi.RSSI(g_associativeIndexArray[j]) > WiFi.RSSI(g_associativeIndexArray[i]))
      {
        temp = g_associativeIndexArray[i];
        g_associativeIndexArray[i] = g_associativeIndexArray[j];
        g_associativeIndexArray[j] = temp;
      }
    }
  }
}

void OnScanComplete(int16_t totalAps)
{
  g_totalAp = totalAps;
  g_currentNetworkIndex = 0;
  pageChanged = true;
  if(g_totalAp > 0)
  {
    SortScan(totalAps);
    ChangeContext(EContext::SCAN_PAGE);
  }
  else
  {
    DEBUG_LOG_LN(F("No APs found. Scan again."));
    ScanNetworks();
    //SendError("No APs found. Scan again.");
    //ChangeContext(EContext::ERROR);
  }  
}

void OnMonScanComplete(int16_t totalAps)
{
  FUNCTION_PERF();
  DEBUG_LOG_LN(F("DisplayMonitorInfo()"));
  
  g_totalAp = totalAps;
  
  if(g_totalAp > 0)
  {
    SScanInfoDisplay scanInfo;
    scanInfo.m_bssid = g_bssid;
    scanInfo.m_ssid = g_ssid;
    
    int foundAp = 0;
    if(FindLockedAP(foundAp))
    {
      scanInfo.m_currentAp = foundAp;
      
      if(GetNetworkInfo(scanInfo.m_currentAp, scanInfo.m_ssid, scanInfo.m_encryptionType, scanInfo.m_rssi, scanInfo.m_bssid, scanInfo.m_channel, scanInfo.m_isHidden))
      {
        SerilizeAndSend(EContext::MONITOR_PAGE, scanInfo);
      }
      else
      {
        DEBUG_LOG_LN(F("GetNetworkInfo() - Failed"));
        scanInfo.m_rssi = NA;
        SerilizeAndSend(EContext::MONITOR_PAGE, scanInfo);
      }
    }
    else
    {
      DEBUG_LOG_LN(F("AP LOST"));
      scanInfo.m_rssi = NA;
      SerilizeAndSend(EContext::MONITOR_PAGE, scanInfo);
    }
  }
  else
  {
    DEBUG_LOG_LN(F("Monitor scan failed"));
    SendError("Monitor scan failed");
  }

  if(g_context == EContext::MONITOR_ANIMATION)
  {
    ChangeContext(EContext::MONITOR_PAGE);
  }
}

void ScanNetworks()
{
  DEBUG_LOG_LN(F("ScanNetworks()"));
  
  WiFi.scanDelete();
  
  const bool async = true;
  const bool showHidden = true;
  WiFi.scanNetworks(async, showHidden);

  ChangeContext(EContext::SCAN_ANIMATION);
}

void MonitorNetwork(const bool firstRun = false)
{
  const int16_t scanResult = WiFi.scanComplete();
  if(scanResult != WIFI_SCAN_RUNNING)
  {
    DEBUG_LOG_LN(F("MonitorNetwork() - Scan Completed"));
    
    if(firstRun)
    {
      clearSortArray();
    }
    else
    {
      OnMonScanComplete(scanResult);
    }
    
    const bool async = true;
    const bool showHidden = true;
    WiFi.scanNetworks(async, showHidden);
  }
}

void CheckScanComplition()
{
  int16_t scanResult = WiFi.scanComplete();
  if(scanResult != WIFI_SCAN_RUNNING)
  {
    OnScanComplete(scanResult);
  }  
}

void ChangeBetweenMonitorAndScanMode() 
{
  DEBUG_LOG_LN(F("ChangeBetweenMonitorAndScanMode"));

  if(g_context != EContext::SCAN_PAGE)
  {
    return;
  }
  
  DEBUG_LOG_LN(F("ChangeBetweenMonitorAndScanMode processing"));
  
  switch(g_context)
  {
    case SCAN_PAGE:
    {
      LockAP();
      const bool firstRun = true;
      MonitorNetwork(firstRun);
      ChangeContext(EContext::MONITOR_ANIMATION);
    }
      break;
    default:
      break;  
  }
}

void DisplayScannedAP()
{
  if(pageChanged)
  {
    DEBUG_LOG_LN(F("DisplayScannedAP() - pageChanged"));
    pageChanged = false;
    
    SScanInfoDisplay scanInfo;
    scanInfo.m_currentAp = g_currentNetworkIndex + 1;
    scanInfo.m_totalAp = g_totalAp;

    if(GetNetworkInfo(g_currentNetworkIndex, scanInfo.m_ssid, scanInfo.m_encryptionType, scanInfo.m_rssi, scanInfo.m_bssid, scanInfo.m_channel, scanInfo.m_isHidden))
    {
      SerilizeAndSend(EContext::SCAN_PAGE, scanInfo);
    }  
    else
    {
      DEBUG_LOG_LN(F("FAILED to retrieve scan result info."));
      SendError("FAILED to retrieve scan result info");
    }
  }
}

void CheckForFlipperCommands()
{
  while(Serial.available() > 0)
  {
    FUNCTION_PERF();
    
    int incommingCommand = Serial.read();
    if(incommingCommand == MODULE_CONTROL_COMMAND_RESTART)
    {
      DEBUG_LOG_LN(F("Restart module"));
      ESP.restart();
    }
    else
    if(g_context >= EContext::SCAN_PAGE)
    {
      switch(incommingCommand)
      {
        case MODULE_CONTROL_COMMAND_NEXT:
          DEBUG_LOG_LN(F("Next command"));
          NextAP();
          break;
        case MODULE_CONTROL_COMMAND_PREVIOUS:
          DEBUG_LOG_LN(F("Previous command"));
          PreviousAP();     
          break;
        case MODULE_CONTROL_COMMAND_SCAN:
          DEBUG_LOG_LN(F("Scan command"));
          ScanMode();
          break;
        case MODULE_CONTROL_COMMAND_MONITOR:
          DEBUG_LOG_LN(F("Monitor command"));
          ChangeBetweenMonitorAndScanMode();        
          break;
        case MODULE_CONTROL_COMMAND_RESTART:
          DEBUG_LOG_LN(F("Restart module"));
          ESP.restart();
          break;
        default:
          break;
      }
    }
    else
    {
#ifdef DEBUG
      char buffer[80];
      sprintf(buffer, "Skip command due to unpropriate context for command '%c'", char(incommingCommand));
      DEBUG_LOG_LN(buffer);
#endif // DEBUG
    }
  }  
}

void loop() 
{
  CheckForFlipperCommands();
  
  switch(g_context)
  {
    case EContext::SCAN_PAGE:
      DisplayScannedAP();
      break;
    case EContext::MONITOR_PAGE:
      MonitorNetwork();
      break;
    case EContext::SCAN_ANIMATION:
      CheckScanComplition();
      break;
    case EContext::MONITOR_ANIMATION:
      MonitorNetwork();
      break;
    case EContext::ERROR:
      break;
    default:
      break;
  }
}
