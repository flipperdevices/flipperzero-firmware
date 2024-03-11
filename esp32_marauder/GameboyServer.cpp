#include "GameboyServer.h"

void gbStartAP(char* ssid, char* password) {
  WiFi.softAP(ssid, password);
  _IP = WiFi.softAPIP();
  _json["type"] = "connected";
  _json["ip"] = _IP;
  _json["ssid"] = ssid;
  _json["password"] = password;
  
  Serial.print("JSON:");
  serializeJson(_json, Serial);
  Serial.println();

  _dnsServer.start(53, "*", _IP);
}