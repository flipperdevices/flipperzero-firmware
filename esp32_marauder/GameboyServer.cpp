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
  _server.on("/favicon.ico.gz", HTTP_GET, [](AsyncWebServerRequest *request){
      AsyncWebServerResponse *response = request->beginResponse_P(200, "image/x-icon", favicon_ico_gz, favicon_ico_gz_len);
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
  });
  _server.on("/malveke_pattern.png.gz", HTTP_GET, [](AsyncWebServerRequest *request){
      AsyncWebServerResponse *response = request->beginResponse_P(200, "image/png", malveke_pattern_png_gz, malveke_pattern_png_gz_len);
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
  });
  
//   _server.on("/assets/_plugin-vue_export-helper.js", HTTP_GET, [](AsyncWebServerRequest *request){
//       AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", assets__plugin_vue_export_helper_js_gz, assets__plugin_vue_export_helper_js_gz_len);
//       response->addHeader("Content-Encoding", "gzip");
//       request->send(response);
//   });
//   _server.on("/assets/_plugin-vue_export-helper.css", HTTP_GET, [](AsyncWebServerRequest *request){
//       AsyncWebServerResponse *response = request->beginResponse_P(200, "text/css", assets__plugin_vue_export_helper_css_gz, assets__plugin_vue_export_helper_css_gz_len);
//       response->addHeader("Content-Encoding", "gzip");
//       request->send(response);
//   });
}