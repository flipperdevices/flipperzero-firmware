#include "SDInterface.h"

bool SDInterface::initSD() {
  String display_string = "";
  
  if (!SD.begin(SD_CS)) {
    Serial.println("Failed to mount SD Card");
    this->supported = false;
    return false;
  }
  else {
    this->supported = true;
    this->cardType = SD.cardType();
    if (cardType == CARD_MMC)
      Serial.println("SD: MMC Mounted");
    else if(cardType == CARD_SD)
        Serial.println("SD: SDSC Mounted");
    else if(cardType == CARD_SDHC)
        Serial.println("SD: SDHC Mounted");
    else
        Serial.println("SD: UNKNOWN Card Mounted");

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
    
    return true;
  }
}

void SDInterface::addPacket(uint8_t* buf, uint32_t len) {
  if ((this->supported) && (this->do_save)) {
    //Serial.println("Adding packet to buffer...");
    buffer_obj.addPacket(buf, len);
  }
}

void SDInterface::openCapture() {
  if (this->supported)
    buffer_obj.open(&SD);
}

void SDInterface::main() {
  if ((this->supported) && (this->do_save)) {
    //Serial.println("Saving packet...");
    buffer_obj.save(&SD);
  }
}

//void SDInterface::savePacket(uint8_t* buf, uint32_t len) {
//  if (this->supported)
//    buffer_obj.save(
//}
