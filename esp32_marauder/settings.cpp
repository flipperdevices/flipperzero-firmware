#include "settings.h"

String Settings::getSettingsString() {
  return this->json_settings_string;
}

bool Settings::begin() {
  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
    Serial.println("Settings SPIFFS Mount Failed");
    return false;
  }

  File settingsFile;

  //SPIFFS.remove("/settings.json"); // NEED TO REMOVE THIS LINE

  if (SPIFFS.exists("/settings.json")) {
    settingsFile = SPIFFS.open("/settings.json", FILE_READ);
    
    if (!settingsFile) {
      settingsFile.close();
      Serial.println(F("Could not find settings file"));
      if (this->createDefaultSettings(SPIFFS))
        return true;
      else
        return false;    
    }
  }
  else {
    Serial.println("Settings file does not exist");
    if (this->createDefaultSettings(SPIFFS))
      return true;
    else
      return false;
  }

  String json_string;
  DynamicJsonDocument jsonBuffer(1024);
  DeserializationError error = deserializeJson(jsonBuffer, settingsFile);
  serializeJson(jsonBuffer, json_string);
  Serial.println("Settings: " + (String)json_string + "\n");
  this->printJsonSettings(json_string);

  this->json_settings_string = json_string;
  
  return true;
}

void Settings::printJsonSettings(String json_string) {
  DynamicJsonDocument json(1024); // ArduinoJson v6

  if (deserializeJson(json, json_string)) {
    Serial.println("\nCould not parse json");
  }
  
  Serial.println("Settings\n----------------------------------------------");
  for (int i = 0; i < json["Settings"].size(); i++) {
    Serial.println("Name: " + json["Settings"][i]["name"].as<String>());
    Serial.println("Type: " + json["Settings"][i]["type"].as<String>());
    Serial.println("Value: " + json["Settings"][i]["value"].as<String>());
    Serial.println("----------------------------------------------");
  }
}

bool Settings::createDefaultSettings(fs::FS &fs) {
  Serial.println(F("Creating default settings file: settings.json"));
  
  File settingsFile = fs.open("/settings.json", FILE_WRITE);

  if (!settingsFile) {
    Serial.println(F("Failed to create settings file"));
    return false;
  }

  DynamicJsonDocument jsonBuffer(1024);
  String settings_string;

  jsonBuffer["Settings"][0]["name"] = "channel";
  jsonBuffer["Settings"][0]["type"] = "int";
  jsonBuffer["Settings"][0]["value"] = 11;

  jsonBuffer["Settings"][1]["name"] = "force pmkid";
  jsonBuffer["Settings"][1]["type"] = "bool";
  jsonBuffer["Settings"][1]["value"] = true;

  jsonBuffer["Settings"][2]["name"] = "save pcap";
  jsonBuffer["Settings"][2]["type"] = "bool";
  jsonBuffer["Settings"][2]["value"] = true;

  //jsonBuffer.printTo(settingsFile);
  if (serializeJson(jsonBuffer, settingsFile) == 0) {
    Serial.println(F("Failed to write to file"));
  }
  if (serializeJson(jsonBuffer, settings_string) == 0) {
    Serial.println(F("Failed to write to string"));
  }

  // Close the file
  settingsFile.close();

  this->printJsonSettings(settings_string);

  return true;
}

void Settings::main(uint32_t currentTime) {
  
}
