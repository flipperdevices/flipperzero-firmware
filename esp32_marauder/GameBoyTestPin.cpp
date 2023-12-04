#include "GameBoyTestPin.h"

unsigned char raw_buffer[FRAME_SIZE]; // max( 16*8*14*8, 16*14*16 ) sensor pixels , tile bytes
static unsigned char GBCAM_BUFFER[14336];

HardwareSerial Serial3(3);


GameBoyTestPin::GameBoyTestPin()
{
    this->setup();
}
void GameBoyTestPin::setup() {
    // Set Data Pins (D0-D7) to Input
    for (uint32_t i = 0; i < sizeof(ADDRESS_GB_GBC_PINS) / sizeof(ADDRESS_GB_GBC_PINS[0]); i++)
    {
        pinMode(ADDRESS_GB_GBC_PINS[i], OUTPUT);
        digitalWrite(ADDRESS_GB_GBC_PINS[i], LOW);
    }
    // Set Data Pins (D0-D7) to Input
    for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
    {
        pinMode(DATA_GB_GBC_PINS[i], OUTPUT);
        digitalWrite(DATA_GB_GBC_PINS[i], LOW);
    }
    // Set Control Pins to Output RST(PH0) CLK(PH1) CS(PH3) WR(PH5) RD(PH6)

    
    pinMode(GAMEBOY_CLK, OUTPUT);
    pinMode(GAMEBOY_CS, OUTPUT);
    pinMode(GAMEBOY_WR, OUTPUT);
    pinMode(GAMEBOY_RD, OUTPUT);
    pinMode(GAMEBOY_RST, OUTPUT);
    pinMode(GAMEBOY_AUDIO, OUTPUT);


    digitalWrite(GAMEBOY_CLK, LOW);
    digitalWrite(GAMEBOY_CS, LOW);
    digitalWrite(GAMEBOY_WR, LOW);
    digitalWrite(GAMEBOY_RD, LOW);
    digitalWrite(GAMEBOY_RST, LOW);
    digitalWrite(GAMEBOY_AUDIO, LOW);
}
void GameBoyTestPin::begin()
{
    this->setup();
}
void GameBoyTestPin::highPin(int pin)
{
    //  LOW ALL
    digitalWrite(GAMEBOY_RST, LOW);
    digitalWrite(GAMEBOY_AUDIO, LOW);
    digitalWrite(GAMEBOY_CLK, LOW);
    digitalWrite(GAMEBOY_CS, LOW);
    digitalWrite(GAMEBOY_WR, LOW);
    digitalWrite(GAMEBOY_RD, LOW);
    for (uint32_t i = 0; i < sizeof(ADDRESS_GB_GBC_PINS) / sizeof(ADDRESS_GB_GBC_PINS[0]); i++)
    {
        digitalWrite(ADDRESS_GB_GBC_PINS[i], LOW);
    }
    for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
    {
        digitalWrite(DATA_GB_GBC_PINS[i], LOW);
    }
    Serial.print("HIGH: ");
    Serial.println(pin);
    if(pin == 2) {
        digitalWrite(GAMEBOY_CLK, HIGH);
    } else if(pin == 3) {
        digitalWrite(GAMEBOY_WR, HIGH);
    } else if(pin == 4) {
        digitalWrite(GAMEBOY_RD, HIGH);
    } else if(pin == 5) {
        digitalWrite(GAMEBOY_CS, HIGH);
    } else if(pin == 6) {   //  A0
        digitalWrite(ADDRESS_GB_GBC_PINS[0], HIGH);
    } else if(pin == 7) {   //  A0
        digitalWrite(ADDRESS_GB_GBC_PINS[1], HIGH);
    } else if(pin == 8) {   //  A0
        digitalWrite(ADDRESS_GB_GBC_PINS[2], HIGH);
    } else if(pin == 9) {   //  A0
        digitalWrite(ADDRESS_GB_GBC_PINS[3], HIGH);
    } else if(pin == 10) {   //  A0
        digitalWrite(ADDRESS_GB_GBC_PINS[4], HIGH);
    } else if(pin == 11) {   //  A0
        digitalWrite(ADDRESS_GB_GBC_PINS[5], HIGH);
    } else if(pin == 12) {   //  A0
        digitalWrite(ADDRESS_GB_GBC_PINS[6], HIGH);
    } else if(pin == 13) {   //  A0
        digitalWrite(ADDRESS_GB_GBC_PINS[7], HIGH);
    } else if(pin == 14) {   //  A0
        digitalWrite(ADDRESS_GB_GBC_PINS[8], HIGH);
    } else if(pin == 15) {   //  A0
        digitalWrite(ADDRESS_GB_GBC_PINS[9], HIGH);
    } else if(pin == 16) {   //  A0
        digitalWrite(ADDRESS_GB_GBC_PINS[10], HIGH);
    } else if(pin == 17) {   //  A0
        digitalWrite(ADDRESS_GB_GBC_PINS[11], HIGH);
    } else if(pin == 18) {   //  A0
        digitalWrite(ADDRESS_GB_GBC_PINS[12], HIGH);
    } else if(pin == 19) {   //  A0
        digitalWrite(ADDRESS_GB_GBC_PINS[13], HIGH);
    } else if(pin == 20) {   //  A0
        digitalWrite(ADDRESS_GB_GBC_PINS[14], HIGH);
    } else if(pin == 21) {   //  A0
        digitalWrite(ADDRESS_GB_GBC_PINS[15], HIGH);
    } else if(pin == 22) {   //  A0
        digitalWrite(DATA_GB_GBC_PINS[0], HIGH);
    } else if(pin == 23) {   //  A0
        digitalWrite(DATA_GB_GBC_PINS[1], HIGH);
    } else if(pin == 24) {   //  A0
        digitalWrite(DATA_GB_GBC_PINS[2], HIGH);
    } else if(pin == 25) {   //  A0
        digitalWrite(DATA_GB_GBC_PINS[3], HIGH);
    } else if(pin == 26) {   //  A0
        digitalWrite(DATA_GB_GBC_PINS[4], HIGH);
    } else if(pin == 27) {   //  A0
        digitalWrite(DATA_GB_GBC_PINS[5], HIGH);
    } else if(pin == 28) {   //  A0
        digitalWrite(DATA_GB_GBC_PINS[6], HIGH);
    } else if(pin == 29) {   //  A0
        digitalWrite(DATA_GB_GBC_PINS[7], HIGH);
    } else if(pin == 30) {   //  A0
        digitalWrite(GAMEBOY_RST, HIGH);
    } else if(pin == 31) {   //  A0
        digitalWrite(GAMEBOY_AUDIO, HIGH);
    }
}
void GameBoyTestPin::main()
{

}