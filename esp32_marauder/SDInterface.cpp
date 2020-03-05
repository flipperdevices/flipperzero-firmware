#include "SDInterface.h"

bool SDInterface::initSD() {
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

    this->cardSizeBT = SD.cardSize();
    this->cardSizeKB = SD.cardSize() / 1024;
    this->cardSizeMB = SD.cardSize() / (1024 * 1024);
    this->cardSizeGB = SD.cardSize() / (1024 * 1024 * 1024);
    
    Serial.printf("SD Card Size: %llu Bytes\n", this->cardSizeBT);
    Serial.printf("SD Card Size: %lluKB\n", this->cardSizeKB);
    Serial.printf("SD Card Size: %lluMB\n", this->cardSizeMB);
    Serial.printf("SD Card Size: %lluGB\n", this->cardSizeGB);
    return true;
  }
}
