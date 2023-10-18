#ifndef GpsInterface_h
#define GpsInterface_h

#include <MicroNMEA.h>

#include "configs.h"

class GpsInterface {
  public:
    void begin();
    void main();

    String getNumSatsString();
    bool getFixStatus();
    String getFixStatusAsString();
    bool getGpsModuleStatus();
    String getLat();
    String getLon();
    float getAlt();
    float getAccuracy();
    String getDatetime();

  private:
    // GPS Info
    String lat = "";
    String lon = "";
    float altf = 0.0;
    float accuracy = 0.0;
    String datetime = "";
    
    bool gps_enabled = false;
    bool good_fix = false;
    uint8_t num_sats = 0;

    String dt_string_from_gps();
    void setGPSInfo();
};

#endif