#include "APMode.h"
#include "APIndex.h"
#include "Global.h"

WiFiServer server(80);

const char *ssidAP = "FlipperZero-IFTTT";
const char *passwordAP = "FlipperZero";

APMode::APMode()
{
}

String midString(String str, String start, String finish){
  int locStart = str.indexOf(start);
  if (locStart==-1) return "";
  locStart += start.length();
  int locFinish = str.indexOf(finish, locStart);
  if (locFinish==-1) return "";
  return str.substring(locStart, locFinish);
}

void APMode::RunSetup(){
  Serial.println();
  Serial.println("Configuring access point...");
  WiFi.softAP(ssidAP, passwordAP);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();
  Serial.println("Server started");
}

void APMode::APCommand(){
  WiFiClient client=server.available();
  if(client){
    String request = client.readStringUntil('\r');
      if(request.indexOf("test1") != -1){
        Serial.println( midString( request, "=", " " ) );
      }
      if(request.indexOf("test2") != -1){
        Serial.println( midString( request, "=", " " ) );
      }
    client.print(html);
    request="";
  }
}
