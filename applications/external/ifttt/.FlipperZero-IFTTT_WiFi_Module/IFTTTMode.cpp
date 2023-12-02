#include "IFTTTMode.h"
#include "Global.h"

IFTTTMode::IFTTTMode()
{
}

void IFTTTMode::RunSetup(){
  Serial.print("\nConnecting to: "); 
  Serial.print(ssidIFTTT);
  WiFi.begin(ssidIFTTT, passwordIFTTT);  

  while(WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.print("\nIP address: "); 
  Serial.println(WiFi.localIP());
}

void IFTTTMode::IFTTTCommand(){
  WiFiClient client;
  HTTPClient http;
  http.begin(client, "http://maker.ifttt.com/trigger/" + eventIFTTT + "/json/with/key/" + keyIFTTT);
  int httpCode = http.GET();
  http.end();
}
