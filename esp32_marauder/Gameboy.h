#ifndef gameboy_h
#define gameboy_h

#include <Arduino.h>
// #include "SerialTransfer.h"
// #include "MemoryFile.h"
// #include <iostream>
// #include <string>
// #include <vector>
// #include <iomanip>  // Para std::setw y std::setfill



/*
(CS)SS    34 (FSPICS0) 10
(SDA)MOSI  35 (FSPID) 11
(DC)MISO  37 (FSPIQ) 13
(SCLK)SCK   36 (FSPICLK) 12
*/
/*
Shift Register:
MOSI V_SPI_D    11
SCL  V_SPI_WP   14
SDA  V_SPI_HD   9
*/



#ifndef WIFI_CONNECT_TIMEOUT
#define WIFI_CONNECT_TIMEOUT 10000
#endif
#ifndef GB_MISO
#define GB_MISO 5
#endif
#ifndef GB_MOSI
#define GB_MOSI 2
#endif
#ifndef GB_SCLK
#define GB_SCLK 1
#endif


uint8_t nintendoLogo[] = {0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
                          0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
                          0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E};

// Array that holds the data
byte sdBuffer[512];

uint16_t cartridgeType = 0;
uint16_t romSize = 0;
uint16_t romBanks = 0;
uint16_t ramSize = 0;
uint16_t ramBanks = 0;
uint16_t ramEndAddress = 0;
word romEndAddress = 0x7FFF;
word romStartBank = 1;

word sramBanks;



char romName[22];
unsigned long sramSize = 0;
int romType = 0;
byte saveType;
// word romSize = 0;
word numBanks = 128;
char checksumStr[9];
bool errorLvl = 0;
byte romVersion = 0;
char cartID[5];
unsigned long cartSize;
unsigned int flashid;
char flashid_str[5];
char vendorID[5];
unsigned long fileSize;
unsigned long sramBase;
unsigned long flashBanks;
bool flashX16Mode;
bool flashSwitchLastBits;


#define GAMEBOY_CLK 12   //   (FSPICLK)(SCLK)SCK
#define GAMEBOY_WR 13    //   (FSPIQ)(DC)MISO
#define GAMEBOY_RD 11    //   (FSPID)(SDA)MOSI
#define GAMEBOY_CS 10    //   (FSPICS0)(CS)SS

#define GAMEBOY_A00 42  //
#define GAMEBOY_A01 41  //
#define GAMEBOY_A02 40  //
#define GAMEBOY_A03 39  //       
#define GAMEBOY_A04 38  //       
#define GAMEBOY_A05 37  //    (FSPIQ)(DC)MISO
#define GAMEBOY_A06 36  //    (FSPICLK)(SCLK)SCK
#define GAMEBOY_A07 35  //    (FSPID)(SDA)MOSI

#define GAMEBOY_A08 34  //    (FSPICS0)(CS)SS
#define GAMEBOY_A09 33  //  
#define GAMEBOY_A10 26  //  
#define GAMEBOY_A11 20  //  
#define GAMEBOY_A12 19  //  
#define GAMEBOY_A13 16  //  
#define GAMEBOY_A14 15  //        
#define GAMEBOY_A15 14  //     

#define GAMEBOY_D00 8   //    (SCL)
#define GAMEBOY_D01 7   //    (SDA)
#define GAMEBOY_D02 6   //  
#define GAMEBOY_D03 5  //  
#define GAMEBOY_D04 4  //  
#define GAMEBOY_D05 3  //  
#define GAMEBOY_D06 2  //  
#define GAMEBOY_D07 1  //  

#define GAMEBOY_RST 21   //  RESET / CS2
#define GAMEBOY_AUDIO 9   //  RESET / CS2

#define GAMEBOY_ADVANCE_CS2 21 //
#define GAMEBOY_ADVANCE_IRQ 9 //

// Address pins
const static uint32_t ADDRESS_GB_GBC_PINS[16] = 
{
    GAMEBOY_A00,
    GAMEBOY_A01,
    GAMEBOY_A02,
    GAMEBOY_A03,
    GAMEBOY_A04,
    GAMEBOY_A05,
    GAMEBOY_A06,
    GAMEBOY_A07,
    GAMEBOY_A08,
    GAMEBOY_A09,
    GAMEBOY_A10,
    GAMEBOY_A11,
    GAMEBOY_A12,
    GAMEBOY_A13,
    GAMEBOY_A14,
    GAMEBOY_A15
};
// Data pins
const static uint32_t DATA_GB_GBC_PINS[8] = 
{
    GAMEBOY_D00,
    GAMEBOY_D01,
    GAMEBOY_D02,
    GAMEBOY_D03,
    GAMEBOY_D04,
    GAMEBOY_D05,
    GAMEBOY_D06,
    GAMEBOY_D07
};

#define WR_PIN GAMEBOY_WR
#define RD_PIN GAMEBOY_RD
#define CS_MREQ_PIN GAMEBOY_CS
#define CS2_PIN GAMEBOY_RST
#define AUDIO_IRQ_PIN GAMEBOY_AUDIO_IRQ

#define wrPin_high	digitalWrite(WR_PIN, HIGH);
#define wrPin_low		digitalWrite(WR_PIN, LOW);
#define rdPin_high	digitalWrite(RD_PIN, HIGH);
#define rdPin_low		digitalWrite(RD_PIN, LOW);
#define cs_mreqPin_high		digitalWrite(CS_MREQ_PIN, HIGH);
#define cs_mreqPin_low		digitalWrite(CS_MREQ_PIN, LOW);
#define cs2Pin_high		digitalWrite(CS2_PIN, HIGH);
#define cs2Pin_low		digitalWrite(CS2_PIN, LOW);
#define phiPin_high	digitalWrite(GAMEBOY_CLK, HIGH);
#define phiPin_low		digitalWrite(GAMEBOY_CLK, LOW);
#define audioPin_high	digitalWrite(AUDIO_IRQ_PIN, HIGH);
#define audioPin_low		digitalWrite(AUDIO_IRQ_PIN, LOW);


//  GAME BOY Camera 
#define GBCAM_W (128)
#define GBCAM_H (112)
#define FRAME_SIZE 	3584		// 128 * 112 * 2bpp / 8 bits

    
void wait(int i)
{
  for(int j = 0; j < i; j++)
  {
    __asm__("nop\n"); 
  }
}

#endif
