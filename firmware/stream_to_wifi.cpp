#include "stream_to_wifi.h"

// char password[30] = "test123";
char ssid[30] = "ESP";

bool is_wifi_streaming = false;
char index_html[MAX_HTML_SIZE] = "TEST";

DNSServer dnsServer;
AsyncWebServer server(80);

class RequestHandler : public AsyncWebHandler {
public:
  RequestHandler() {}
  virtual ~RequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request) { return true; }

  void handleRequest(AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  }
};

void stream_to_wifi() {
  if (!is_wifi_streaming) {
    Serial.println("Starting WiFi stream...");

    // Connect to WiFi AP
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid);
    WiFi.setSleep(false);

    // Start the web server
    start_server();

    Serial.print("Camera Ready! Use 'http://");
    Serial.print(WiFi.softAPIP());
    Serial.println("' to connect");

    Serial.flush();

    is_wifi_streaming = true;
  } else {
    dnsServer.processNextRequest();
  }
}

void start_server() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
    Serial.println("Client connected.");
  });

  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(
        200, "text/html",
        "<html><head><script>setTimeout(() => { window.location.href ='/' }, 100);</script></head><body></body></html>");
  });

  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new RequestHandler()).setFilter(ON_AP_FILTER);
  server.begin();
}

void start_wifi_stream() {
  // Physical test indicator that we're streaming.
  turn_flash_on(); 

  camera_model.isStreamToSerialEnabled = false;

  set_camera_config_defaults(CAMERA_FUNCTION_WIFI);
  set_camera_model_defaults(CAMERA_FUNCTION_WIFI);
  set_camera_defaults(CAMERA_FUNCTION_WIFI);

  // @todo - Dynamically set ssid and password via prompts.

  camera_model.isStreamToWiFiEnabled = true;

  turn_flash_off();
}

void stop_wifi_stream() {
  if (is_wifi_streaming) {
    WiFi.setSleep(true);
    server.end();
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);
    is_wifi_streaming = false;
    camera_model.isStreamToWiFiEnabled = false;
  }
}
