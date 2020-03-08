#ifndef Web_h
#define Web_h

/*
Code taken from espressif ESP32 OTA Update example
*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>

#include "Assets.h"
#include "Display.h"
#include "WiFiScan.h"

extern Display display_obj;
extern WiFiScan wifi_scan_obj;

class Web
{
  private:
    
    PROGMEM const char* host = "esp32marauder";
    PROGMEM const char* ssid = "MarauderOTA";
    PROGMEM const char* password = "justcallmekoko";

    bool serving = false;
    
    int num_sta = 0;

    PROGMEM const char* loginIndex = 
     "<form name='loginForm'>"
        "<table width='20%' bgcolor='A09F9F' align='center'>"
            "<tr>"
                "<td colspan=2>"
                    "<center><font size=4><b>ESP32 Login Page</b></font></center>"
                    "<br>"
                "</td>"
                "<br>"
                "<br>"
            "</tr>"
            "<td>Username:</td>"
            "<td><input type='text' size=25 name='userid'><br></td>"
            "</tr>"
            "<br>"
            "<br>"
            "<tr>"
                "<td>Password:</td>"
                "<td><input type='Password' size=25 name='pwd'><br></td>"
                "<br>"
                "<br>"
            "</tr>"
            "<tr>"
                "<td><input type='submit' onclick='check(this.form)' value='Login'></td>"
            "</tr>"
        "</table>"
    "</form>"
    "<script>"
        "function check(form)"
        "{"
        "if(form.userid.value=='admin' && form.pwd.value=='admin')"
        "{"
        "window.open('/serverIndex')"
        "}"
        "else"
        "{"
        " alert('Error Password or Username')/*displays error message*/"
        "}"
        "}"
    "</script>";
     
    /*
     * Server Index Page
     */
     
    PROGMEM const char* serverIndex = 
    "<script src='/jquery.min.js'></script>"
    "Because the lack of an asynchronous webserver in this Arduino sketch like 'ESPAsyncWebServer', <br/>"
    "both file 'serverIndex' and 'jquery.min.js' can't be read from the webserver at the same time. <br/><br/>"
    "Your web browser probably requests those two files simultaneously and therefore <br/>"
    "the javascript file failed to load. By a refresh of this page, the browser cash has already <br/>"
    "load 'serverIndex' file, the web browser will do a second attempt to only read the javascript file. <br/>"
    "This second attempt, with an idle webserver, will be processed.<br/><br/>"
    "Long story short, press F5 (refresh web browser) before uploading your firmware. <br/><br/>"
    "<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
       "<input type='file' name='update'>"
            "<input type='submit' value='Update'>"
        "</form>"
     "<div id='prg'>progress: 0%</div>"
     "<script>"
      "$('form').submit(function(e){"
      "e.preventDefault();"
      "var form = $('#upload_form')[0];"
      "var data = new FormData(form);"
      " $.ajax({"
      "url: '/update',"
      "type: 'POST',"
      "data: data,"
      "contentType: false,"
      "processData:false,"
      "xhr: function() {"
      "var xhr = new window.XMLHttpRequest();"
      "xhr.upload.addEventListener('progress', function(evt) {"
      "if (evt.lengthComputable) {"
      "var per = evt.loaded / evt.total;"
      "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
      "}"
      "}, false);"
      "return xhr;"
      "},"
      "success:function(d, s) {"
      "console.log('success!')" 
     "},"
     "error: function (a, b, c) {"
     "}"
     "});"
     "});"
     "</script>";

  public:
  
    Web();

    void main();
    PROGMEM static void onJavaScript();
    void setupOTAupdate();
    void shutdownServer();
};

#endif
