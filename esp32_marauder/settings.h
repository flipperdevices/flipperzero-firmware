#ifndef Settings_h
#define Settings_h

#include "configs.h"

#include "SPIFFS.h"
#include <FS.h>
#include <ArduinoJson.h>

#define FORMAT_SPIFFS_IF_FAILED true

#ifdef HAS_SCREEN
  #include "Display.h"

  extern Display display_obj;
#endif

class Settings {

  private:
    String json_settings_string;
    
    bool createDefaultSettings(fs::FS &fs);

  public:
    bool begin();

    template <typename T>
    T loadSetting(String name);

    template <typename T>
    T saveSetting(String key, bool value);

    bool toggleSetting(String key);
    String getSettingType(String key);
    String setting_index_to_name(int i);
    int getNumberSettings();

    //template<>
    //int loadSetting<int>(String key);
    
    //template<>
    //String loadSetting<String>(String key);
    
    //template<>
    //bool loadSetting<bool>(String key);
    
    //template<>
    //uint8_t loadSetting<uint8_t>(String key);

    String getSettingsString();
    void printJsonSettings(String json_string);
    void main(uint32_t currentTime);
};

#endif
