#ifndef Settings_h
#define Settings_h

#include "SPIFFS.h"
#include <FS.h>
#include <ArduinoJson.h>

#define FORMAT_SPIFFS_IF_FAILED true

#include "Display.h"

extern Display display_obj;

class Settings {

  private:
    String json_settings_string;
    
    void printJsonSettings(String json_string);
    bool createDefaultSettings(fs::FS &fs);

  public:
    bool begin();

    String getSettingsString();
    void main(uint32_t currentTime);
};

#endif
