#include "SDInterface.h"
#include "lang_var.h"

bool SDInterface::initSD() {
  String display_string = "";

  /*#ifdef KIT
    pinMode(SD_DET, INPUT);
    if (digitalRead(SD_DET) == LOW) {
      Serial.println(F("SD Card Detect Pin Detected"));
    }
    else {
      Serial.println(F("SD Card Detect Pin Not Detected"));
      this->supported = false;
      return false;
    }
  #endif

  pinMode(SD_CS, OUTPUT);

  delay(10);*/

  if (!SD_MMC.begin("/sdcard", true, false, SDMMC_FREQ_DEFAULT)) {
    Serial.println(F("Failed to mount SD Card"));
    this->supported = false;
    return false;
  }
  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No MicroSD Card found");
    this->supported = false;
    return false;
  } else {
    this->supported = true;
    this->cardType = cardType;
    //if (cardType == CARD_MMC)
    //  Serial.println(F("SD: MMC Mounted"));
    //else if(cardType == CARD_SD)
    //    Serial.println(F("SD: SDSC Mounted"));
    //else if(cardType == CARD_SDHC)
    //    Serial.println(F("SD: SDHC Mounted"));
    //else
    //    Serial.println(F("SD: UNKNOWN Card Mounted"));

    this->cardSizeMB = SD_MMC.cardSize() / (1024 * 1024);
    
    //Serial.printf("SD Card Size: %lluMB\n", this->cardSizeMB);

    if (this->supported) {
      const int NUM_DIGITS = log10(this->cardSizeMB) + 1;
    
      char sz[NUM_DIGITS + 1];
     
      sz[NUM_DIGITS] =  0;
      for ( size_t i = NUM_DIGITS; i--; this->cardSizeMB /= 10)
      {
          sz[i] = '0' + (this->cardSizeMB % 10);
          display_string.concat((String)sz[i]);
      }
  
      this->card_sz = sz;
    }

    buffer_obj = Buffer();
    
    if (!SD_MMC.exists("/SCRIPTS")) {
      Serial.println("/SCRIPTS does not exist. Creating...");

      SD_MMC.mkdir("/SCRIPTS");
      Serial.println("/SCRIPTS created");
    }
    
    return true;
  }
}

void SDInterface::addPacket(uint8_t* buf, uint32_t len) {
  if ((this->supported) && (this->do_save)) {
    buffer_obj.addPacket(buf, len);
  }
}

void SDInterface::openCapture(String file_name) {
  if (this->supported)
    buffer_obj.createPcapFile(&SD_MMC, file_name);
    buffer_obj.open();
}

void SDInterface::runUpdate() {
  #ifdef HAS_SCREEN
    display_obj.tft.setTextWrap(false);
    display_obj.tft.setFreeFont(NULL);
    display_obj.tft.setCursor(0, 100);
    display_obj.tft.setTextSize(1);
    display_obj.tft.setTextColor(TFT_WHITE);
  
    display_obj.tft.println(F(text15));
  #endif
  File updateBin = SD_MMC.open("/update.bin");
  if (updateBin) {
    if(updateBin.isDirectory()){
      #ifdef HAS_SCREEN
        display_obj.tft.setTextColor(TFT_RED);
        display_obj.tft.println(F(text_table2[0]));
      #endif
      Serial.println(F("Error, could not find \"update.bin\""));
      #ifdef HAS_SCREEN
        display_obj.tft.setTextColor(TFT_WHITE);
      #endif
      updateBin.close();
      return;
    }

    size_t updateSize = updateBin.size();

    if (updateSize > 0) {
      #ifdef HAS_SCREEN
        display_obj.tft.println(F(text_table2[1]));
      #endif
      Serial.println(F("Starting update over SD. Please wait..."));
      this->performUpdate(updateBin, updateSize);
    }
    else {
      #ifdef HAS_SCREEN
        display_obj.tft.setTextColor(TFT_RED);
        display_obj.tft.println(F(text_table2[2]));
      #endif
      Serial.println(F("Error, file is empty"));
      #ifdef HAS_SCREEN
        display_obj.tft.setTextColor(TFT_WHITE);
      #endif
      return;
    }

    updateBin.close();
    
      // whe finished remove the binary from sd card to indicate end of the process
    #ifdef HAS_SCREEN
      display_obj.tft.println(F(text_table2[3]));
    #endif
    Serial.println(F("rebooting..."));
    //SD.remove("/update.bin");      
    delay(1000);
    ESP.restart();
  }
  else {
    #ifdef HAS_SCREEN
      display_obj.tft.setTextColor(TFT_RED);
      display_obj.tft.println(F(text_table2[4]));
    #endif
    Serial.println(F("Could not load update.bin from sd root"));
    #ifdef HAS_SCREEN
      display_obj.tft.setTextColor(TFT_WHITE);
    #endif
  }
}

void SDInterface::performUpdate(Stream &updateSource, size_t updateSize) {
  if (Update.begin(updateSize)) {   
    #ifdef HAS_SCREEN
      display_obj.tft.println(text_table2[5] + String(updateSize));
      display_obj.tft.println(F(text_table2[6]));
    #endif
    size_t written = Update.writeStream(updateSource);
    if (written == updateSize) {
      #ifdef HAS_SCREEN
        display_obj.tft.println(text_table2[7] + String(written) + text_table2[10]);
      #endif
      Serial.println("Written : " + String(written) + " successfully");
    }
    else {
      #ifdef HAS_SCREEN
        display_obj.tft.println(text_table2[8] + String(written) + "/" + String(updateSize) + text_table2[9]);
      #endif
      Serial.println("Written only : " + String(written) + "/" + String(updateSize) + ". Retry?");
    }
    if (Update.end()) {
      Serial.println("OTA done!");
      if (Update.isFinished()) {
        #ifdef HAS_SCREEN
          display_obj.tft.println(F(text_table2[11]));
        #endif
        Serial.println(F("Update successfully completed. Rebooting."));
      }
      else {
        #ifdef HAS_SCREEN
          display_obj.tft.setTextColor(TFT_RED);
          display_obj.tft.println(text_table2[12]);
        #endif
        Serial.println("Update not finished? Something went wrong!");
        #ifdef HAS_SCREEN
          display_obj.tft.setTextColor(TFT_WHITE);
        #endif
      }
    }
    else {
      #ifdef HAS_SCREEN
        display_obj.tft.println(text_table2[13] + String(Update.getError()));
      #endif
      Serial.println("Error Occurred. Error #: " + String(Update.getError()));
    }

  }
  else
  {
    #ifdef HAS_SCREEN
      display_obj.tft.println(text_table2[14]);
    #endif
    Serial.println("Not enough space to begin OTA");
  }
}

bool SDInterface::checkDetectPin() {
  /* #ifdef KIT
    if (digitalRead(SD_DET) == LOW)
      return true;
    else
      return false;
  #endif*/

  return false;
}

void SDInterface::main() {
  if ((this->supported) && (this->do_save)) {
    //Serial.println("Saving packet...");
    buffer_obj.forceSave(&SD_MMC);
  }
  else if (!this->supported) {
    if (checkDetectPin()) {
      delay(100);
      this->initSD();
    }
  }
}
