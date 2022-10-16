#include "IFTTTMode.h"
#include "Global.h"

//insert trigger name in "triggername" field and your Webhoook Key ID in "keyID" field
const char* resource = "https://maker.ifttt.com/trigger/triggername/json/with/key/keyID";
const char* serverIFTTT = "maker.ifttt.com";
//insert SSID and Password in field
const char *ssidIFTTT = "SSID";
const char *passwordIFTTT = "password";

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
  Serial.print("Connecting to "); 
  Serial.print(serverIFTTT);
  
  WiFiClient client;
  if (!client.connect(serverIFTTT, 80)) {
    Serial.println("connection failed");
  }
  
  Serial.print("Request resource: "); 
  Serial.println(resource);
  client.print(String("GET ") + resource + " HTTP/1.1\r\n" +
                  "Host: " + serverIFTTT + "\r\n" + 
                  "Connection: close\r\n\r\n");
                  

  unsigned long timeout = millis();
  // Read all the lines of the reply from server and print them to Serial
  while (client.available() == 0) {
    if (millis() - timeout > 5000){
      Serial.println(">>> Client Timeout !");
      client.stop(); return;
    }
  } 

  while(client.available()){
    Serial.write(client.read());
  }
  Serial.println("\nclosing connection");
  client.stop();
}
