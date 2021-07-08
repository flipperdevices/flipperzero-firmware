#include "SDInterface.h"

bool SDInterface::initSD() {
  String display_string = "";

  #ifdef KIT
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
  
  if (!SD.begin(SD_CS)) {
    Serial.println(F("Failed to mount SD Card"));
    this->supported = false;
    return false;
  }
  else {
    this->supported = true;
    this->cardType = SD.cardType();
    if (cardType == CARD_MMC)
      Serial.println(F("SD: MMC Mounted"));
    else if(cardType == CARD_SD)
        Serial.println(F("SD: SDSC Mounted"));
    else if(cardType == CARD_SDHC)
        Serial.println(F("SD: SDHC Mounted"));
    else
        Serial.println(F("SD: UNKNOWN Card Mounted"));

    //this->cardSizeBT = SD.cardSize();
    //this->cardSizeKB = SD.cardSize() / 1024;
    this->cardSizeMB = SD.cardSize() / (1024 * 1024);
    //this->cardSizeGB = SD.cardSize() / (1024 * 1024 * 1024);
    
    //Serial.printf("SD Card Size: %llu Bytes\n", this->cardSizeBT);
    //Serial.printf("SD Card Size: %lluKB\n", this->cardSizeKB);
    Serial.printf("SD Card Size: %lluMB\n", this->cardSizeMB);
    //Serial.printf("SD Card Size: %lluGB\n", this->cardSizeGB);

    if (this->supported) {
      //display_obj.tft.println((byte)(sd_obj.cardSizeMB % 10));
      const int NUM_DIGITS = log10(this->cardSizeMB) + 1;
    
      char sz[NUM_DIGITS + 1];
     
      sz[NUM_DIGITS] =  0;
      for ( size_t i = NUM_DIGITS; i--; this->cardSizeMB /= 10)
      {
          sz[i] = '0' + (this->cardSizeMB % 10);
          display_string.concat((String)sz[i]);
      }
  
      //this->card_sz = display_string;  
      this->card_sz = sz;
    }

    buffer_obj = Buffer();

    //if (this->supported)
    //  buffer_obj.open(&SD);

    // Check for SCRIPTS folder
    if (!SD.exists("/SCRIPTS")) {
      Serial.println("/SCRIPTS does not exist. Creating...");

      SD.mkdir("/SCRIPTS");
      Serial.println("/SCRIPTS created");
    }
    
    return true;
  }
}

void SDInterface::addPacket(uint8_t* buf, uint32_t len) {
  if ((this->supported) && (this->do_save)) {
    //Serial.println("Adding packet to buffer...");
    buffer_obj.addPacket(buf, len);
  }
}

void SDInterface::openCapture(String file_name) {
  if (this->supported)
    buffer_obj.open(&SD, file_name);
}

void SDInterface::runUpdate() {
  //display_obj.clearScreen();
  display_obj.tft.setTextWrap(false);
  display_obj.tft.setFreeFont(NULL);
  display_obj.tft.setCursor(0, 100);
  display_obj.tft.setTextSize(1);
  display_obj.tft.setTextColor(TFT_WHITE);

  display_obj.tft.println(F("Opening /update.bin..."));
  File updateBin = SD.open("/update.bin");
  if (updateBin) {
    if(updateBin.isDirectory()){
      display_obj.tft.setTextColor(TFT_RED);
      display_obj.tft.println(F("Error, could not find update.bin"));
      Serial.println(F("Error, update.bin is not a file"));
      display_obj.tft.setTextColor(TFT_WHITE);
      updateBin.close();
      return;
    }

    size_t updateSize = updateBin.size();

    if (updateSize > 0) {
      display_obj.tft.println(F("Starting SD Update..."));
      Serial.println(F("Try to start update"));
      this->performUpdate(updateBin, updateSize);
    }
    else {
      display_obj.tft.setTextColor(TFT_RED);
      display_obj.tft.println(F("Error, update.bin is empty"));
      Serial.println(F("Error, file is empty"));
      display_obj.tft.setTextColor(TFT_WHITE);
      return;
    }

    updateBin.close();
    
      // whe finished remove the binary from sd card to indicate end of the process
    display_obj.tft.println(F("rebooting..."));
    Serial.println(F("rebooting..."));
    //SD.remove("/update.bin");      
    delay(1000);
    ESP.restart();
  }
  else {
    display_obj.tft.setTextColor(TFT_RED);
    display_obj.tft.println(F("Could not load update.bin from /"));
    Serial.println(F("Could not load update.bin from sd root"));
    display_obj.tft.setTextColor(TFT_WHITE);
  }
}

void SDInterface::performUpdate(Stream &updateSource, size_t updateSize) {
  if (Update.begin(updateSize)) {   
    display_obj.tft.println("File size: " + String(updateSize));
    display_obj.tft.println(F("Writing file to partition..."));
    size_t written = Update.writeStream(updateSource);
    if (written == updateSize) {
      display_obj.tft.println("Written: " + String(written) + " successfully");
      Serial.println("Written : " + String(written) + " successfully");
    }
    else {
      display_obj.tft.println("Written only : " + String(written) + "/" + String(updateSize) + ". Retry?");
      Serial.println("Written only : " + String(written) + "/" + String(updateSize) + ". Retry?");
    }
    if (Update.end()) {
      Serial.println("OTA done!");
      if (Update.isFinished()) {
        display_obj.tft.println(F("Update complete"));
        Serial.println(F("Update successfully completed. Rebooting."));
      }
      else {
        display_obj.tft.setTextColor(TFT_RED);
        display_obj.tft.println("Update could not complete");
        Serial.println("Update not finished? Something went wrong!");
        display_obj.tft.setTextColor(TFT_WHITE);
      }
    }
    else {
      display_obj.tft.println("Error Occurred. Error #: " + String(Update.getError()));
      Serial.println("Error Occurred. Error #: " + String(Update.getError()));
    }

  }
  else
  {
    display_obj.tft.println("Not enough space to begin OTA");
    Serial.println("Not enough space to begin OTA");
  }
}

bool SDInterface::checkDetectPin() {
  #ifdef KIT
    if (digitalRead(SD_DET) == LOW)
      return true;
    else
      return false;
  #endif

  return false;
}

void SDInterface::main() {
  if ((this->supported) && (this->do_save)) {
    //Serial.println("Saving packet...");
    buffer_obj.forceSave(&SD);
  }
  else if (!this->supported) {
    if (checkDetectPin()) {
      delay(100);
      this->initSD();
    }
  }
}

//void SDInterface::savePacket(uint8_t* buf, uint32_t len) {
//  if (this->supported)
//    buffer_obj.save(
//}
