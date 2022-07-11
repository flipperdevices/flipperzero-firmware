//#define DEBUG

#include "ESP8266WiFi.h"
#include "DebugHelpers.h"
#include "FlipperZeroWiFiModuleDefines.h"

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

const char* EncryptionTypeToString(short encType)
{
  switch(encType)
  {
    case 5:
      return "WEP";
    case 2:
      return "TKIP";
    case 4:
      return "CCMP";
    case 7:
      return "NONE";
    case 8:
      return "AUTO";
    default:
      return "FAIL";
  }
}

void SendError(const char* str)
{
  // Serial.write(str);
}

void SerilizeAndSend(EContext context, const SScanInfoDisplay& scanInfo)
{
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
  g_bssid = WiFi.BSSIDstr(g_currentNetworkIndex); 
  g_ssid = WiFi.SSID(g_currentNetworkIndex);
  g_apLocked = true;
}

void UnlockAP()
{
  g_apLocked = false;
}

bool FindLockedAP(int& foundAPIndex)
{
  if(IsAnyAPLocked())
  {
    for(int i = 0; i < g_totalAp; ++i)
    {
      if(strcmp(g_bssid.c_str(), WiFi.BSSIDstr(i).c_str()) == 0)
      {
        foundAPIndex = i;
        return true;
      }
    }
  }
  return false;
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
      LockAP();
      MonitorNetwork();
      ChangeContext(EContext::MONITOR_ANIMATION);
      break;
    default:
      break;  
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect
  }

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  ChangeContext(EContext::INITIALIZATION);

  delay(500);

  ScanNetworks();
}

void OnScanComplete(int totalAps)
{
  g_totalAp = totalAps;
  g_currentNetworkIndex = 0;
  pageChanged = true;
  if(g_totalAp > 0)
  {
    ChangeContext(EContext::SCAN_PAGE);
  }
  else
  {
    DEBUG_LOG_LN(F("No APs found. Scan again."));
    SendError("No APs found. Scan again.");
    //scannerDisplay.DisplayError("No APs found. Scan again.");
    ChangeContext(EContext::ERROR);
  }  
}

void OnMonScanComplete(int totalAps)
{
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
      
      uint8_t* bssid_fake;
      if(WiFi.getNetworkInfo(scanInfo.m_currentAp, scanInfo.m_ssid, scanInfo.m_encryptionType, scanInfo.m_rssi, bssid_fake, scanInfo.m_channel, scanInfo.m_isHidden))
      {
        SerilizeAndSend(EContext::MONITOR_PAGE, scanInfo);
      }
      else
      {
        DEBUG_LOG_LN(F("WiFi.getNetworkInfo() - Failed"));
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
  const bool showHidden = true;
  WiFi.scanNetworksAsync(OnScanComplete, showHidden);

  ChangeContext(EContext::SCAN_ANIMATION);
}

bool MonitorNetwork()
{
  if(WiFi.scanComplete() >= 0)
  {
    DEBUG_LOG_LN(F("MonitorNetwork()"));
    
    WiFi.scanDelete();
    const bool showHidden = true;
    WiFi.scanNetworksAsync(OnMonScanComplete, showHidden);
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
    scanInfo.m_bssid = WiFi.BSSIDstr(scanInfo.m_currentAp);
    
    uint8_t* bssid_fake;
    if(WiFi.getNetworkInfo(g_currentNetworkIndex, scanInfo.m_ssid, scanInfo.m_encryptionType, scanInfo.m_rssi, bssid_fake, scanInfo.m_channel, scanInfo.m_isHidden))
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
    int incommingCommand = Serial.read();
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
        default:
          break;
      }
    }
    else
    {
      DEBUG_LOG_LN(printf("Skip command due to unpropriate context %c", char(incommingCommand)));
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
      break;
    case EContext::MONITOR_ANIMATION:
      break;
    case EContext::ERROR:
      break;
    default:
      break;
  }
}
