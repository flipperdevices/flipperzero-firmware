#include <hidboot.h>
#include <usbhub.h>
#include "Keyboard.h"

#define Serial Serial1

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
  #include <spi4teensy3.h>
  #include <SPI.h>
#endif

#include <SPI.h>
#include <SD.h>

#define    CAPS_LOCK 0x39
#define     NUM_LOCK 0x53
#define  SCROLL_LOCK 0x47
#define PRINT_SCREEN 0x46
#define NUM_1        0x59
#define NUM_2        0x5A
#define NUM_3        0x5B
#define NUM_4        0x5C
#define NUM_5        0x5D
#define NUM_6        0x5E
#define NUM_7        0x5F
#define NUM_8        0x60
#define NUM_9        0x61
#define NUM_0        0x62

#define KEY_CAPS_LOCK 0x01
#define KEY_NUM_LOCK 0x02
#define KEY_SCROLL_LOCK 0x04
#define KEY_PRINT_SCREEN 0x05
#define KEY_NUM_1        0x31
#define KEY_NUM_2        0x32
#define KEY_NUM_3        0x33
#define KEY_NUM_4        0x34
#define KEY_NUM_5        0x35
#define KEY_NUM_6        0x36
#define KEY_NUM_7        0x37
#define KEY_NUM_8        0x38
#define KEY_NUM_9        0x39
#define KEY_NUM_0        0x30

String bufferStr = "";
String last = "";

int defaultDelay = 0;

bool shift = false;
bool num_lock = false; // false is working half way

USB     Usb;
HIDBoot<USB_HID_PROTOCOL_KEYBOARD>    HidKeyboard(&Usb);
uint32_t next_time;

class KbdRptParser : public KeyboardReportParser{
  public:
    uint8_t _parse(uint8_t key);
    String _getChar(uint8_t key);
  protected:
    void OnControlKeysChanged(uint8_t before, uint8_t after);

    void OnKeyDown  (uint8_t mod, uint8_t key);
    void OnKeyUp  (uint8_t mod, uint8_t key);
    void OnKeyPressed(uint8_t key);
    
    void _press(uint8_t key);
    void _release(uint8_t key);
};

void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key){
  digitalWrite(LED_BUILTIN, HIGH);
  int parsedKey = _parse(key);
  if(parsedKey == key){
    uint8_t c = OemToAscii(mod, key);
    OnKeyPressed(c);
    if(c != 0x20 && c != 0x00) _press(c);
    else _press(key);
  }else _press(parsedKey);
  digitalWrite(LED_BUILTIN, LOW);
}

void KbdRptParser::OnKeyUp(uint8_t mod, uint8_t key){
  int parsedKey = _parse(key);
  if(parsedKey == key){
    uint8_t c = OemToAscii(mod, key);
    OnKeyPressed(c);
    if(c != 0x20 && c != 0x00){
      _release(c);
      Serial.print((char)c);
    }
    else{
      _release(key);
      Serial.print("0x");
      Serial.print(key, HEX);
    }
  }else{
    _release(parsedKey);
    Serial.print(_getChar(key));
  }
}

void KbdRptParser::OnControlKeysChanged(uint8_t before, uint8_t after) {

  MODIFIERKEYS beforeMod;
  *((uint8_t*)&beforeMod) = before;

  MODIFIERKEYS afterMod;
  *((uint8_t*)&afterMod) = after;

  //left
  if(beforeMod.bmLeftCtrl != afterMod.bmLeftCtrl){
    if(afterMod.bmLeftCtrl) Keyboard.press(KEY_LEFT_CTRL);
    else Keyboard.release(KEY_LEFT_CTRL);
    Serial.print("<ctrl "+(String)afterMod.bmLeftCtrl+">");
  }
  
  if(beforeMod.bmLeftShift != afterMod.bmLeftShift){
    if(afterMod.bmLeftShift) Keyboard.press(KEY_LEFT_SHIFT);
    else Keyboard.release(KEY_LEFT_SHIFT);
    shift = afterMod.bmLeftShift;
    //Serial.print("<shift "+(String)afterMod.bmLeftShift+">");
  }
  
  if(beforeMod.bmLeftAlt != afterMod.bmLeftAlt){
    if(afterMod.bmLeftAlt) Keyboard.press(KEY_LEFT_ALT);
    else Keyboard.release(KEY_LEFT_ALT);
    Serial.print("<alt "+(String)afterMod.bmLeftAlt+">");
  }
  
  if(beforeMod.bmLeftGUI != afterMod.bmLeftGUI){
    if(afterMod.bmLeftGUI) Keyboard.press(KEY_LEFT_GUI);
    else Keyboard.release(KEY_LEFT_GUI);
    Serial.print("<gui "+(String)afterMod.bmLeftGUI+">");
  }

  //right
  if(beforeMod.bmRightCtrl != afterMod.bmRightCtrl){
    if(afterMod.bmRightCtrl) Keyboard.press(KEY_RIGHT_CTRL);
    else Keyboard.release(KEY_RIGHT_CTRL);
    Serial.print("<ctrl "+(String)afterMod.bmRightCtrl+">");
  }
  
  if(beforeMod.bmRightShift != afterMod.bmRightShift){
    if(afterMod.bmRightShift) Keyboard.press(KEY_RIGHT_SHIFT);
    else Keyboard.release(KEY_RIGHT_SHIFT);
    shift = afterMod.bmLeftShift;
    //Serial.print("<shift "+(String)afterMod.bmRightShift+">");
  }
  
  if(beforeMod.bmRightAlt != afterMod.bmRightAlt){
    if(afterMod.bmRightAlt) Keyboard.press(KEY_RIGHT_ALT);
    else Keyboard.release(KEY_RIGHT_ALT);
    Serial.print("<alt "+(String)afterMod.bmRightAlt+">");
  }
  
  if(beforeMod.bmRightGUI != afterMod.bmRightGUI){
    if(afterMod.bmRightGUI) Keyboard.press(KEY_RIGHT_GUI);
    else Keyboard.release(KEY_RIGHT_GUI);
    Serial.print("<gui "+(String)afterMod.bmRightGUI+">");
  }

}

void KbdRptParser::OnKeyPressed(uint8_t key){
  /*Serial.print("ASCII: \"");
  Serial.print((char)key);
  Serial.print("\" = 0x");
  Serial.print(key, HEX);
  Serial.print("; ");*/
};

uint8_t KbdRptParser::_parse(uint8_t key){
  /*
  Serial.print("0x");
  Serial.print(key, HEX);
  Serial.print(" = ");*/
  switch(key){
    case CAPS_LOCK:    return KEY_CAPS_LOCK; break; // CAPS
    case NUM_LOCK:     return KEY_NUM_LOCK; break; // NUM LOCK
    case SCROLL_LOCK:  return KEY_SCROLL_LOCK; break; // SCROLL LOCK
    case PRINT_SCREEN: return KEY_PRINT_SCREEN; break; // PRINT SCREEN
    case NUM_1       : if (!num_lock) return KEY_NUM_1; break;
    case NUM_2       : if (!num_lock) return KEY_NUM_2; break;
    case NUM_3       : if (!num_lock) return KEY_NUM_3; break;
    case NUM_4       : if (!num_lock) return KEY_NUM_4; break;
    case NUM_5       : if (!num_lock) return KEY_NUM_5; break;
    case NUM_6       : if (!num_lock) return KEY_NUM_6; break;
    case NUM_7       : if (!num_lock) return KEY_NUM_7; break;
    case NUM_8       : if (!num_lock) return KEY_NUM_8; break;
    case NUM_9       : if (!num_lock) return KEY_NUM_9; break;
    case NUM_0       : if (!num_lock) return KEY_NUM_0; break;
    case 0x2C: return 0x20; break; // SPACE
    case 40: return KEY_RETURN; break;
    case 41: return KEY_ESC; break;
    case 42: return KEY_BACKSPACE; break;
    case 43: return KEY_TAB; break;
    case 58: return KEY_F1; break;
    case 59: return KEY_F2; break;
    case 60: return KEY_F3; break;
    case 61: return KEY_F4; break;
    case 62: return KEY_F5; break;
    case 63: return KEY_F6; break;    
    case 64: return KEY_F7; break;
    case 65: return KEY_F8; break;
    case 66: return KEY_F9; break;
    case 67: return KEY_F10; break;
    case 68: return KEY_F11; break;
    case 69: return KEY_F12; break;
    case 73: return KEY_INSERT; break;
    case 74: return KEY_HOME; break;
    case 75: return KEY_PAGE_UP; break;
    case 76: return KEY_DELETE; break;
    case 77: return KEY_END; break;
    case 78: return KEY_PAGE_DOWN; break;
    case 79: return KEY_RIGHT_ARROW; break;
    case 80: return KEY_LEFT_ARROW; break;
    case 81: return KEY_DOWN_ARROW; break;
    case 82: return KEY_UP_ARROW; break;
    case 88: return KEY_RETURN; break;
    //=====[DE-Keyboard]=====//
    case 0x64: return 236; break; // <
    case 0x32: return 92; break; // #
    //======================//
    default: {
      //Serial.print(" N/A ");
      return key;
    }
  }
}

String KbdRptParser::_getChar(uint8_t key){
  switch(key){
    case CAPS_LOCK:    return "<CAPS_LOCK>"; break; // CAPS
    case NUM_LOCK:     return "<NUM_LOCK>"; break; // NUM LOCK
    case SCROLL_LOCK:  return "<SCROLL_LOCK>"; break; // SCROLL LOCK
    case PRINT_SCREEN: return "<PRINT_SCREEN>"; break; // PRINT SCREEN
    //case NUM_1       : if (num_lock) return "<NUM_1>"; break;
    //case NUM_2       : if (num_lock) return "<NUM_2>"; break;
    //case NUM_3       : if (num_lock) return "<NUM_3>"; break;
    //case NUM_4       : if (num_lock) return "<NUM_4>"; break;
    //case NUM_5       : if (num_lock) return "<NUM_5>"; break;
    //case NUM_6       : if (num_lock) return "<NUM_6>"; break;
    //case NUM_7       : if (num_lock) return "<NUM_7>"; break;
    //case NUM_8       : if (num_lock) return "<NUM_8>"; break;
    //case NUM_9       : if (num_lock) return "<NUM_9>"; break;
    //case NUM_0       : if (num_lock) return "<NUM_0>"; break;
    case 0x2C: return " "; break;
    case 40: return "<RETURN>\n"; break;
    case 41: return "<ESC>\n"; break;
    case 42: return "<BACKSPCAE>"; break;
    case 43: return "<TAB>\n"; break;
    case 58: return "<F1>\n"; break;
    case 59: return "<F2>\n"; break;
    case 60: return "<F3>\n"; break;
    case 61: return "<F4>\n"; break;
    case 62: return "<F5>\n"; break;
    case 63: return "<F6>\n"; break;    
    case 64: return "<F7>\n"; break;
    case 65: return "<F8>\n"; break;
    case 66: return "<F9>\n"; break;
    case 67: return "<F10>\n"; break;
    case 68: return "<F11>\n"; break;
    case 69: return "<F12>\n"; break;
    case 73: return "<INSERT>"; break;
    case 74: return "<HOME>\n"; break;
    case 75: return "<PAGE_UP>\n"; break;
    case 76: return "<DELETE>"; break;
    case 77: return "<END>\n"; break;
    case 78: return "<PAGE_DOWN>\n"; break;
    case 79: return "<RIGHT_ARROW>\n"; break;
    case 80: return "<LEFT_ARROW>\n"; break;
    case 81: return "<DOWN_ARROW>\n"; break;
    case 82: return "<UP_ARROW>\n"; break;
    case 88: return "<RETURN>\n"; break;
    //=====[DE-Keyboard]=====//
    case 0x64: {
      if(shift) return "<";
      else return ">";
      break;
    }
    case 0x32:{
      if(shift) return "'";
      else return "#";
      break;
    }
    //======================//
    default: {
      return "";
    }
  }
}

void KbdRptParser::_press(uint8_t key){
  /*Serial.print("0x");
  Serial.print(key, HEX);
  Serial.println(" DOWN");*/
  Keyboard.press(key);
}

void KbdRptParser::_release(uint8_t key){
  /*Serial.print("0x");
  Serial.print(key, HEX);
  Serial.println(" UP");
  Serial.println();*/
  Keyboard.release(key);
}

KbdRptParser parser;

void Line(String _line)
{
  int firstSpace = _line.indexOf(" ");
  if(firstSpace == -1) Press(_line);
  else if(_line.substring(0,firstSpace) == "STRING"){
    for(int i=firstSpace+1;i<_line.length();i++) Keyboard.write(_line[i]);
  }
  else if(_line.substring(0,firstSpace) == "DELAY"){
    int delaytime = _line.substring(firstSpace + 1).toInt();
    delay(delaytime);
  }
  else if(_line.substring(0,firstSpace) == "DEFAULTDELAY") defaultDelay = _line.substring(firstSpace + 1).toInt();
  else if(_line.substring(0,firstSpace) == "REM"){} //nothing :/
  else if(_line.substring(0,firstSpace) == "REPLAY") {
    int replaynum = _line.substring(firstSpace + 1).toInt();
    while(replaynum)
    {
      Line(last);
      --replaynum;
    }
  } else{
      String remain = _line;

      while(remain.length() > 0){
        int latest_space = remain.indexOf(" ");
        if (latest_space == -1){
          Press(remain);
          remain = "";
        }
        else{
          Press(remain.substring(0, latest_space));
          remain = remain.substring(latest_space + 1);
        }
        delay(5);
      }
  }

  Keyboard.releaseAll();
  delay(defaultDelay);
}


void Press(String b){
  if(b.length() == 1) Keyboard.press(char(b[0]));
  else if (b.equals("ENTER")) Keyboard.press(KEY_RETURN);
  else if (b.equals("CTRL")) Keyboard.press(KEY_LEFT_CTRL);
  else if (b.equals("SHIFT")) Keyboard.press(KEY_LEFT_SHIFT);
  else if (b.equals("ALT")) Keyboard.press(KEY_LEFT_ALT);
  else if (b.equals("GUI")) Keyboard.press(KEY_LEFT_GUI);
  else if (b.equals("UP") || b.equals("UPARROW")) Keyboard.press(KEY_UP_ARROW);
  else if (b.equals("DOWN") || b.equals("DOWNARROW")) Keyboard.press(KEY_DOWN_ARROW);
  else if (b.equals("LEFT") || b.equals("LEFTARROW")) Keyboard.press(KEY_LEFT_ARROW);
  else if (b.equals("RIGHT") || b.equals("RIGHTARROW")) Keyboard.press(KEY_RIGHT_ARROW);
  else if (b.equals("DELETE")) Keyboard.press(KEY_DELETE);
  else if (b.equals("PAGEUP")) Keyboard.press(KEY_PAGE_UP);
  else if (b.equals("PAGEDOWN")) Keyboard.press(KEY_PAGE_DOWN);
  else if (b.equals("HOME")) Keyboard.press(KEY_HOME);
  else if (b.equals("ESC")) Keyboard.press(KEY_ESC);
  else if (b.equals("BACKSPACE")) Keyboard.press(KEY_BACKSPACE);
  else if (b.equals("INSERT")) Keyboard.press(KEY_INSERT);
  else if (b.equals("TAB")) Keyboard.press(KEY_TAB);
  else if (b.equals("END")) Keyboard.press(KEY_END);
  else if (b.equals("CAPSLOCK")) Keyboard.press(KEY_CAPS_LOCK);
  else if (b.equals("F1")) Keyboard.press(KEY_F1);
  else if (b.equals("F2")) Keyboard.press(KEY_F2);
  else if (b.equals("F3")) Keyboard.press(KEY_F3);
  else if (b.equals("F4")) Keyboard.press(KEY_F4);
  else if (b.equals("F5")) Keyboard.press(KEY_F5);
  else if (b.equals("F6")) Keyboard.press(KEY_F6);
  else if (b.equals("F7")) Keyboard.press(KEY_F7);
  else if (b.equals("F8")) Keyboard.press(KEY_F8);
  else if (b.equals("F9")) Keyboard.press(KEY_F9);
  else if (b.equals("F10")) Keyboard.press(KEY_F10);
  else if (b.equals("F11")) Keyboard.press(KEY_F11);
  else if (b.equals("F12")) Keyboard.press(KEY_F12);
  else if (b.equals("SPACE")) Keyboard.press(' ');
  //else Serial.println("not found :'"+b+"'("+String(b.length())+")");
}


void setup()
{
  Serial.begin(115200);
  //Serial1.begin(115200);
  Keyboard.begin();
  delay(2000);

  pinMode(LED_BUILTIN, OUTPUT);

  // Turn signal LED off
  digitalWrite(LED_BUILTIN, LOW);

  //Serial1.println("\n\nLogging keys...\n");
/*
  for(int i=0;i<256;i++){
    
    int key = parser._parse(i);
    if(key == i){
      Keyboard.print((String)i+" ");
      Keyboard.write(i);
      delay(200);
      Keyboard.write(KEY_RETURN);
    }
  }*/
  
  if(Usb.Init() == -1) Serial.println("OSC did not start.");
  
  delay(200);

  next_time = millis() + 5000;

  HidKeyboard.SetReportParser(0, &parser);

}

void loop(){
  Usb.Task();

  if(Serial.available()) {
    bufferStr = Serial.readStringUntil("END");
    //Serial.println(bufferStr);
  }
  
  if(bufferStr.length() > 0){
    
    bufferStr.replace("\r","\n");
    bufferStr.replace("\n\n","\n");
    
    while(bufferStr.length() > 0){
      int latest_return = bufferStr.indexOf("\n");
      if(latest_return == -1){
        //Serial.println("run: "+bufferStr);
        Line(bufferStr);
        bufferStr = "";
      } else{
        //Serial.println("run: '"+bufferStr.substring(0, latest_return)+"'");
        Line(bufferStr.substring(0, latest_return));
        last=bufferStr.substring(0, latest_return);
        bufferStr = bufferStr.substring(latest_return + 1);
      }
    }
    
    bufferStr = "";
    Serial.write(0x99);
    //Serial.println("done");
  }
}
