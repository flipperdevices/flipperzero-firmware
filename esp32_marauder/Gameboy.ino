
#include <iostream>
#include <string>
#include <esp32-hal-timer.h>
#include <esp32-hal-ledc.h>
#include <ArduinoJson.h>
#include "Gameboy.h"
#include "MemoryFile.h"
// #include "gb.h"


//  https://github.com/atsidaev/GBxCartFlasher-RW/blob/master/setup.c
//  https://github.com/sanni/cartreader/blob/34a1c6b3362eaa0537d0abbc51b34f8426d85e19/Cart_Reader/GB.ino#L56
//  https://gbdev.gg8.se/wiki/articles/Gameboy_ROM_Header_Info
//  https://gbhwdb.gekkio.fi/cartridges/
//  https://github.com/abrugsch/GBCartSlurp/blob/master/gbcartslurp.c#L79
//  https://gbdev.io/pandocs/The_Cartridge_Header.html <---
/*
| ATmega8515L | ESP32-S2-WROOM-I  |
|------------------|--------|
| PB0              | GPIO40 |
| PB1              | GPIO39 |
| PB2              | GPIO38 |
| PB3              | GPIO36 |
| PB4              | GPIO35 |
| PB5              | GPIO34 |
| PB6              | GPIO33 |
| PB7              | GPIO32 |
|------------------|--------|
| PA0              | GPIO26 |
| PA1              | GPIO21 |
| PA2              | GPIO20 |
| PA3              | GPIO19 |
| PA4              | GPIO18 |
| PA5              | GPIO17 |
| PA6              | GPIO16 |
| PA7              | GPIO15 |
|------------------|--------|
| PC0              | GPIO7  |
| PC1              | GPIO8  |
| PC2              | GPIO9  |
| PC3              | GPIO10 |
| PC4              | GPIO11 |
| PC5              | GPIO12 |
| PC6              | GPIO13 |
| PC7              | GPIO14 |
|------------------|--------|
| PD6              | GPIO3 |
| PD5              | GPIO4 |
| PD4              | GPIO5 |
*/

// Dithering layout taken from real GB Cam (see routine at ROMA:4427)
// Table of matrix offsets * 3, order is top-bottom, left-right
//     0   1   2   3
//   ---------------
// 0 | A   M   D   P
// 1 | I   E   L   H
// 2 | C   O   B   N
// 3 | K   G   J   F
static const uint8_t matrix_layout[16] = {0, 30, 24, 6,
                                          15, 45, 39, 21,
                                          12, 42, 36, 18,
                                          3, 33, 27, 9};

// Contrast values for computing dithering matrix
uint8_t qlevels[4];

char receivedBuffer[256];
char receivedChar;
uint8_t eepromBuffer[8];
uint8_t flashChipIdBuffer[2];

char flashWriteWePin;
uint16_t flashWriteCycle[3][2];
uint8_t flashBank1CommandWrites = 0;
uint8_t lastBankAccessed = 0;
volatile uint8_t writingTimedout = 0;

uint8_t cartMode = GBA_MODE;

// Receive USART data
uint8_t USART_Receive(void)
{
  while (!(Serial.available()))
    ;                   // Wait for data to be received
  return Serial.read(); // Get and return received data from buffer
}

// Transmit USART data
void USART_Transmit(unsigned char data)
{
  Serial.write(data);
}

// Read 1-256 bytes from the USART
void usart_read_bytes(int count)
{
  for (int x = 0; x < count; x++)
  {
    receivedBuffer[x] = USART_Receive();
  }
}

// Read the USART until a 0 (string terminator byte) is received
void usart_read_chars(void)
{
  int x = 0;
  while (1)
  {
    receivedBuffer[x] = USART_Receive();
    if (receivedBuffer[x] == 0)
    {
      break;
    }
    x++;
  }
}

// Turn RD, WR, CS/MREQ and CS2 to high so they are deselected (reset state)
void rd_wr_csmreq_cs2_reset(void)
{
  cs2Pin_high;     // CS2 off
  cs_mreqPin_high; // CS/MREQ off
  rdPin_high;      // RD off
  wrPin_high;      // WR off
}
// Turn RD, WR and MREQ to high so they are deselected (reset state)
void rd_wr_mreq_reset(void) {
  rdPin_high; // RD off
  wrPin_high; // WR off
  cs_mreqPin_high; // MREQ off
}
// Turn RD, WR and MREQ off as no power should be applied to GB Cart
void rd_wr_mreq_off(void) {
  rdPin_low;
  wrPin_low;
  cs_mreqPin_low; // CS
}

// Set the 16 bit address on A15-0
void set_16bit_address(uint16_t address)
{
  // Write each of the bits into the address pins
  for (uint32_t i = 0; i < sizeof(ADDRESS_GB_GBC_PINS) / sizeof(ADDRESS_GB_GBC_PINS[0]); i++)
  {
    digitalWrite(ADDRESS_GB_GBC_PINS[i], address & (1 << i) ? HIGH : LOW);
  }
  // digitalWrite(40, address >> 15 & 0x01);  // GPIO40 (PB0) - Set A15
  // digitalWrite(39, address >> 14 & 0x01);  // GPIO39 (PB1) - Set A14
  // digitalWrite(38, address >> 13 & 0x01);  // GPIO38 (PB2) - Set A13
  // digitalWrite(36, address >> 12 & 0x01);  // GPIO36 (PB3) - Set A12
  // digitalWrite(35, address >> 11 & 0x01);  // GPIO35 (PB4) - Set A11
  // digitalWrite(34, address >> 10 & 0x01);  // GPIO34 (PB5) - Set A10
  // digitalWrite(33, address >> 9 & 0x01);   // GPIO33 (PB6) - Set A9
  // digitalWrite(32, address >> 8 & 0x01);   // GPIO32 (PB7) - Set A8
  // digitalWrite(26, address >> 0 & 0x01);   // GPIO26 (PA0) - Set A0
  // digitalWrite(21, address >> 1 & 0x01);   // GPIO21 (PA1) - Set A1
  // digitalWrite(20, address >> 2 & 0x01);   // GPIO20 (PA2) - Set A2
  // digitalWrite(19, address >> 3 & 0x01);   // GPIO19 (PA3) - Set A3
  // digitalWrite(18, address >> 4 & 0x01);   // GPIO18 (PA4) - Set A4
  // digitalWrite(17, address >> 5 & 0x01);   // GPIO17 (PA5) - Set A5
  // digitalWrite(16, address >> 6 & 0x01);   // GPIO16 (PA6) - Set A6
  // digitalWrite(15, address >> 7 & 0x01);   // GPIO15 (PA7) - Set A7
}
// ---------- GB FLASH CARTS ----------

// Read a byte from the flash (No CS pin pulse)
uint8_t gb_flash_read_byte(uint16_t address)
{

  for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
  {
    pinMode(DATA_GB_GBC_PINS[i], INPUT);
  }

  set_16bit_address(address);

  rdPin_low;
  delayMicroseconds(1);
  uint8_t data = 0;
  // Read each of the data pins and construct the byte data
  for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
  {
    data |= digitalRead(DATA_GB_GBC_PINS[i]) << i;
  }
  rdPin_high;

  return data;
}

// No setting address or outputs, assuming already set
uint8_t gb_flash_read_byte_fast(void)
{
  rdPin_low;
  delayMicroseconds(1);
  uint8_t data = 0;
  // Read each of the data pins and construct the byte data
  for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
  {
    data |= digitalRead(DATA_GB_GBC_PINS[i]) << i;
  }
  rdPin_high;

  return data;
}

// Set the address and data for the write byte cycle to the flash
void gb_flash_write_bus_cycle(uint16_t address, uint8_t data)
{
  for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
  {
    pinMode(DATA_GB_GBC_PINS[i], OUTPUT);
  }
  set_16bit_address(address);
  // Write the bank to switch to
  for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
  {
    digitalWrite(DATA_GB_GBC_PINS[i], data & (1 << i) ? HIGH : LOW);
  }
  // Read each of the data pins and construct the byte data
  for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
  {
    digitalWrite(DATA_GB_GBC_PINS[i], data & (1 << i) ? HIGH : LOW);
  }

  // if (flashWriteWePin == WE_AS_AUDIO_PIN) { // Audio pin
  // audioPin_low; // WE low
  // delayMicroseconds(1);
  // audioPin_high; // WE high
  // }
  // else { // WR pin
  wrPin_low; // WE low
  delayMicroseconds(1);
  wrPin_high; // WE high
  // }

  // Clear data outputs and set data pins as inputs
  for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
  {
    pinMode(DATA_GB_GBC_PINS[i], INPUT);
  }
}

// Set Gameboy mode
void gb_mode(void)
{
  // Set inputs
  // pinMode(26, INPUT);  // GPIO26 (PA0)
  // pinMode(21, INPUT);  // GPIO21 (PA1)
  // pinMode(20, INPUT);  // GPIO20 (PA2)
  // pinMode(19, INPUT);  // GPIO19 (PA3)
  // pinMode(18, INPUT);  // GPIO18 (PA4)
  // pinMode(17, INPUT);  // GPIO17 (PA5)
  // pinMode(16, INPUT);  // GPIO16 (PA6)
  // pinMode(15, INPUT);  // GPIO15 (PA7)
  // pinMode(7, INPUT);   // GPIO7 (PC0)
  // pinMode(8, INPUT);   // GPIO8 (PC1)
  // pinMode(9, INPUT);   // GPIO9 (PC2)
  // pinMode(10, INPUT);  // GPIO10 (PC3)
  // pinMode(11, INPUT);  // GPIO11 (PC4)
  // pinMode(12, INPUT);  // GPIO12 (PC5)
  // pinMode(13, INPUT);  // GPIO13 (PC6)
  // pinMode(14, INPUT);  // GPIO14 (PC7)
  // pinMode(3, INPUT);   // GPIO3 (PD6)
  // pinMode(4, INPUT);   // GPIO4 (PD5)
  // pinMode(5, INPUT);   // GPIO5 (PD4)

  // // Set outputs
  // pinMode(40, OUTPUT);  // GPIO40 (PB0)
  // pinMode(39, OUTPUT);  // GPIO39 (PB1)
  // pinMode(38, OUTPUT);  // GPIO38 (PB2)
  // pinMode(36, OUTPUT);  // GPIO36 (PB3)
  // pinMode(35, OUTPUT);  // GPIO35 (PB4)
  // pinMode(34, OUTPUT);  // GPIO34 (PB5)
  // pinMode(33, OUTPUT);  // GPIO33 (PB6)
  // pinMode(32, OUTPUT);  // GPIO32 (PB7)

  // Address pins as outputs
  for (uint32_t i = 0; i < sizeof(ADDRESS_GB_GBC_PINS) / sizeof(ADDRESS_GB_GBC_PINS[0]); i++)
  {
    pinMode(ADDRESS_GB_GBC_PINS[i], OUTPUT);
  }

  // Set Data Pins (D0-D7) to Input
  for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
  {
    pinMode(DATA_GB_GBC_PINS[i], INPUT);
  }
}
uint8_t shift = 0;
// Set the address and read a byte from the 8 bit data line
byte read_8bit_data(uint16_t address)
{
  set_16bit_address(address);

  cs_mreqPin_low; // GPIO32 (PB7) - cs_mreqPin_low
  rdPin_low;      // GPIO3 (PD6) - rdPin_low

  asm volatile("nop"); // Delay a little (minimum is 2 nops, using 3 to be sure)
  asm volatile("nop");
  asm volatile("nop");
  asm volatile("nop");

  // delay(2);
  // asm volatile("nop"); // Delay a little (minimum needed is 1 nops, 2 nops for GB camera)
  // asm volatile("nop");
  byte data = 0; // GPIO7 (PC0) - PIN_DATA7_0 - Read data
  for (int i = 0; i < 8; i++)
  {
    int raw_data = digitalRead(DATA_GB_GBC_PINS[i]);
    // Serial.print(raw_data);
    data |= raw_data << i;
  }
  // Serial.println();
  // Switch and RD(PH6) to HIGH
  rdPin_high;      // GPIO3 (PD6) - rdPin_high
  cs_mreqPin_high; // GPIO32 (PB7) - cs_mreqPin_high

  return data;
}

void writeByte_GB(int address, byte data)
{
  // Set address
  set_16bit_address(address);
  // Set data
  // Switch data pins to output
  for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
  {
    pinMode(DATA_GB_GBC_PINS[i], OUTPUT);
  }

  for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
  {
    digitalWrite(DATA_GB_GBC_PINS[i], data & (1 << i) ? HIGH : LOW);
  }

  // Wait till output is stable
  // delay(3);
  __asm__("nop\n\t"
          "nop\n\t"
          "nop\n\t"
          "nop\n\t");
  // Pull WR(PH5) low
  wrPin_low;

  // Leave WR low for at least 60ns
  // delay(3);
  __asm__("nop\n\t"
          "nop\n\t"
          "nop\n\t"
          "nop\n\t");

  // Pull WR(PH5) HIGH
  wrPin_high;
  // delay(3);
  // Leave WR high for at least 50ns
  __asm__("nop\n\t"
          "nop\n\t"
          "nop\n\t"
          "nop\n\t");

  // Switch data pins to input
  for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
  {
    pinMode(DATA_GB_GBC_PINS[i], INPUT);
  }
  // Enable pullups
  for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
  {
    pinMode(DATA_GB_GBC_PINS[i], HIGH);
  }
}

// uint8_t convertirLEB128AEntero(uint8_t leb128) {
//   uint8_t valor = 0;
//   uint8_t byte;

//   for(uint8_t x = 0; x < 2; x++) {
//     byte = leb128 /*& 0x7F*/;
//     valor |= (byte << x);
//   }

//   return valor;
// }

uint8_t convertirLEB128AEntero(uint8_t leb128)
{
  uint8_t valor = 0;
  uint8_t shift = 0;
  uint8_t byte;

  do
  {
    byte = leb128 & 0xF0;
    valor |= (byte << shift);
    shift += 1;
  } while (leb128 >>= 7);

  return valor;
}

void imprimirHexadecimal(byte valor)
{
  // char buffer[3]; // Se reserva espacio para almacenar el valor hexadecimal y el carácter nulo

  // // Se utiliza la función sprintf para formatear el valor en hexadecimal en el buffer
  // sprintf(buffer, "%02X", valor);

  // Se imprime el valor hexadecimal en la consola serie
  Serial.print(valor, HEX);
  Serial.print(' ');
}

uint8_t startRomBuffer[385];
char gameTitle[17];
uint8_t nintendoLogo[] = {0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
                          0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
                          0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E};

// uint16_t cartridgeType = 0;
// uint16_t romSize = 0;
// uint16_t romBanks = 0;
// uint16_t ramSize = 0;
// uint16_t ramBanks = 0;
// uint16_t ramEndAddress = 0;

const int phiPulseWidth = 1; // Duración del pulso en microsegundos
DynamicJsonDocument transferJSON(2048);
bool phiState = false;
// The hardware timer pointer
hw_timer_t *timer = NULL;

void IRAM_ATTR phiCallback()
{
  phiState = !phiState;
  ledcWrite(GAMEBOY_CLK, phiState ? phiPulseWidth : 0); // Establece el ciclo de trabajo de la señal Phi
}

// used by MD and NES modules
byte eepbit[8];

// Game Boy
// word sramBanks;
// word romBanks;
word lastByte = 0;
// char checksumStr[9];
// char cartID[5];
// byte romVersion = 0;
// int romType = 0;
// unsigned long sramSize = 0;

void showCartInfo_GB(bool printInfo = true)
{
  transferJSON["type"] = "info";
  uint32_t address = 0;
  uint8_t eepromSize = EEPROM_4KBIT;
  uint8_t resetCommonLines = 1;

  // rd_wr_csmreq_cs2_reset();

  // Serial.println(">>>>rd_wr_csmreq_cs2_reset");

  gb_mode();

  /*
  //  TODO: Implementar check connection
  //cartridge connection check
	if (readByte(0x0104)==0xCE){
		if (readByte(0x0105)==0xED){
			if (readByte(0x0106)==0x66){
				if (readByte(0x0107)==0x66){uart_puts("Cartridge Connected \n");}
				if (readByte(0x0104)==readByte(0x4104)){uart_puts("But it is a faulty  connection ,please try to insert again \n");}
			}
		}
	}else{
    uart_puts("Please check the cartridge connection\n");
    return;
  }
  
  */
  // Serial.println(">>>>gb_mode");

  // delay(400);

  // // RST(PH0) to H
  // digitalWrite(CS2_PIN, HIGH);

  for (uint16_t address = 0; address < 0x0180; address++)
  {
    char hexbuffer[3]; // Se reserva espacio para almacenar el valor hexadecimal y el carácter nulo
    byte value = read_8bit_data(address);

    startRomBuffer[address] = value;

    /*
      sprintf(hexbuffer, "%02X", value);

      Serial.print(hexbuffer);
      // Serial.print(valor, HEX);
      // Si se han impreso 16 valores, se realiza un salto de línea
      if ((address + 1) % 16 == 0) {
        Serial.println();
      } else {
        Serial.print(' ');
      }
      */
  }
  byte myLength = 0;
  // Blank out game title
  for (uint8_t b = 0; b < 16; b++)
  {
    gameTitle[b] = 0;
  }
  uint8_t cart_data = startRomBuffer[0x0143];
	if (cart_data == 0x80){
    //  Cartridge with color function
    transferJSON["gb_type"] = "gbc";
	} else {
    //  Cartridge with no color function
    transferJSON["gb_type"] = "gb";
	}
	cart_data = startRomBuffer[0x0146];
	if (cart_data == 0x03){
    //  Cartridge with SGB functions
    transferJSON["gb_sgb"] = true;
	}else{
		//  Cartridge without SGB functions
    transferJSON["gb_sgb"] = false;
	}

  // Read cartridge title and check for non-printable text
  for (uint16_t titleAddress = 0x0134; titleAddress <= 0x143; titleAddress++)
  {
    char headerChar = startRomBuffer[titleAddress];
    if ((headerChar >= 0x30 && headerChar <= 0x39) || // 0-9
        (headerChar >= 0x41 && headerChar <= 0x5A) || // A-Z
        (headerChar >= 0x61 && headerChar <= 0x7A) || // a-z
        (headerChar >= 0x24 && headerChar <= 0x29) || // #$%&'()
        (headerChar == 0x2D) ||                       // -
        (headerChar == 0x2E) ||                       // .
        (headerChar == 0x5F) ||                       // _
        (headerChar == 0x20))
    { // Space
      gameTitle[(titleAddress - 0x0134)] = headerChar;
      myLength++;
    }
    // Replace with an underscore
    else if (headerChar == 0x3A)
    { //  :
      gameTitle[(titleAddress - 0x0134)] = '_';
      myLength++;
    }
    else
    {
      gameTitle[(titleAddress - 0x0134)] = '\0';
      break;
    }
  }
  // Find Game Serial
  cartID[0] = 0;
  if (startRomBuffer[0x143] == 0x80 || startRomBuffer[0x143] == 0xC0) {
    if ((gameTitle[myLength - 4] == 'A' || gameTitle[myLength - 4] == 'B' || gameTitle[myLength - 4] == 'H' || gameTitle[myLength - 4] == 'K' || gameTitle[myLength - 4] == 'V') && (gameTitle[myLength - 1] == 'A' || gameTitle[myLength - 1] == 'B' || gameTitle[myLength - 1] == 'D' || gameTitle[myLength - 1] == 'E' || gameTitle[myLength - 1] == 'F' || gameTitle[myLength - 1] == 'I' || gameTitle[myLength - 1] == 'J' || gameTitle[myLength - 1] == 'K' || gameTitle[myLength - 1] == 'P' || gameTitle[myLength - 1] == 'S' || gameTitle[myLength - 1] == 'U' || gameTitle[myLength - 1] == 'X' || gameTitle[myLength - 1] == 'Y')) {
      cartID[0] = gameTitle[myLength - 4];
      cartID[1] = gameTitle[myLength - 3];
      cartID[2] = gameTitle[myLength - 2];
      cartID[3] = gameTitle[myLength - 1];
      myLength -= 4;
      gameTitle[myLength] = 0;
    }
  }
  // Nintendo Logo Check
  uint8_t logoCheck = 1;
  uint8_t x = 0;
  uint8_t romLogo[48];

  JsonArray logoArray = transferJSON.createNestedArray("logo");
  for (uint16_t romAddress = 0x0104; romAddress <= 0x133; romAddress++)
  {
    if (nintendoLogo[x] != startRomBuffer[romAddress] && logoCheck == 1)
    {
      logoCheck = 0;
    }
    romLogo[x] = startRomBuffer[romAddress];
    logoArray.add(romLogo[x]);
    x++;
  }
  transferJSON["logo_correct"] = logoCheck;
  // Calculate header checksum
  byte headerChecksum = 0;
  for (int currByte = 0x134; currByte < 0x14D; currByte++)
  {
    headerChecksum = headerChecksum - startRomBuffer[currByte] - 1;
  }

  if (headerChecksum != startRomBuffer[0x14D])
  {
    // Read Header into array a second time
    for (int currByte = 0x100; currByte < 0x150; currByte++)
    {
      startRomBuffer[currByte] = read_8bit_data(currByte);
    }
    // Calculate header checksum a second time
    headerChecksum = 0;
    for (int currByte = 0x134; currByte < 0x14D; currByte++)
    {
      headerChecksum = headerChecksum - startRomBuffer[currByte] - 1;
    }
  }

  if (headerChecksum != startRomBuffer[0x14D])
  {
    // Serial.println(F("HEADER CHECKSUM ERROR"));
    transferJSON["message"] = "HEADER CHECKSUM ERROR";
  }

  cartridgeType = startRomBuffer[0x0147];
  romType = cartridgeType;
  romSize = startRomBuffer[0x0148];
  ramSize = startRomBuffer[0x0149];
  sramSize = startRomBuffer[0x149];

  // Get Checksum as string
  eepbit[6] = startRomBuffer[0x14E];
  eepbit[7] = startRomBuffer[0x14F];
  sprintf(checksumStr, "%02X%02X", startRomBuffer[0x14E], startRomBuffer[0x14F]);

  // ROM banks
  switch (romSize)
  {
  case 0x00:
    romBanks = 2;
    break;
  case 0x01:
    romBanks = 4;
    break;
  case 0x02:
    romBanks = 8;
    break;
  case 0x03:
    romBanks = 16;
    break;
  case 0x04:
    romBanks = 32;
    break;
  case 0x05:
    romBanks = 64;
    break;
  case 0x06:
    romBanks = 128;
    break;
  case 0x07:
    romBanks = 256;
    break;
  case 0x08:
    romBanks = 512;
    break;
  default:
    romBanks = 2;
  }

  // SRAM banks
  sramBanks = 0;
  if (romType == 6)
  {
    sramBanks = 1;
  }

  // SRAM size
  switch (sramSize)
  {
  case 2:
    sramBanks = 1;
    break;
  case 3:
    sramBanks = 4;
    break;
  case 4:
    sramBanks = 16;
    break;
  case 5:
    sramBanks = 8;
    break;
  }

  // Last byte of SRAM
  // if (romType == 6)
  // {
  //   lastByte = 0xA1FF; // MBC2 512 bytes (nibbles)
  // }
  // if (sramSize == 1)
  // {
  //   lastByte = 0xA7FF; // 2K RAM
  // }
  // else if (sramSize > 1)
  // {
  //   lastByte = 0xBFFF; // 8K RAM
  // }

  // // MBC6
  // if (romType == 32)
  // {
  //   sramBanks = 8;
  //   lastByte = 0xAFFF;
  // }
  // else if (romType == 34)
  // {                                                                 // MBC7
  //   lastByte = (*((uint16_t *)(eepbit + 6)) == 0xa5be ? 512 : 256); // Only "Command Master" use LC66 EEPROM
  // }

  // // RAM banks
  // ramBanks = 0; // Default 0K RAM
  // if (cartridgeType == 6) { ramBanks = 1; }
  // if (ramSize == 2) { ramBanks = 1; }
  // if (ramSize == 3) { ramBanks = 4; }
  // if (ramSize == 4) { ramBanks = 16; }
  // if (ramSize == 5) { ramBanks = 8; }

  // RAM end address
  if (cartridgeType == 6)
  {
    ramEndAddress = 0xA1FF;
  } // MBC2 512 bytes (nibbles)
  if (ramSize == 1)
  {
    ramEndAddress = 0xA7FF;
  } // 2K RAM
  if (ramSize > 1)
  {
    ramEndAddress = 0xBFFF;
  } // 8K RAM

  
  // M161 banks are double size and start with 0
  if (romType == 0x104) {
    romStartBank = 0;
    romBanks >>= 1;
    romEndAddress = 0x7FFF;
  }
  // MBC6 banks are half size
  else if (romType == 32) {
    romBanks <<= 1;
    romEndAddress = 0x3FFF;
  }


  // Serial.println("showCartInfo_GB:");
  // Serial.println(checksumStr);
  if (strcmp(checksumStr, "00") != 0)
  {
    transferJSON["success"] = 1;
    transferJSON["message"] = "";
    transferJSON["checksum"] = checksumStr;
    transferJSON["title"] = gameTitle;
    transferJSON["ramBanks"] = sramBanks;
    transferJSON["ramEndAddress"] = ramEndAddress;
    transferJSON["romBanks"] = romBanks;
    transferJSON["romEndAddress"] = romEndAddress;
    // Serial.print(F("Title: "));
    // Serial.println(gameTitle);
    // buffer_obj.addPacket(reinterpret_cast<uint8_t*>(gameTitle), sizeof(gameTitle));
    // buffer_obj.forceSaveSerial();
    if (cartID[0] != 0)
    {
      // Serial.print(F("Serial: "));
      // Serial.println(cartID);
      transferJSON["serial"] = cartID;
    }
    transferJSON["rev"] = romVersion;
    // Serial.print(F("Revision: "));
    // Serial.println(romVersion);

    // Serial.print(F("Mapper: "));
    if ((romType == 0) || (romType == 8) || (romType == 9)) {
      // Serial.print(F("none"));
      transferJSON["mapper"] = "None";
    }
    else if ((romType == 1) || (romType == 2) || (romType == 3)) {
      // Serial.print(F("MBC1"));
      transferJSON["mapper"] = "MBC1";
    } else if ((romType == 5) || (romType == 6)) {
      // Serial.print(F("MBC2"));
      transferJSON["mapper"] = "MBC2";
    } else if ((romType == 11) || (romType == 12) || (romType == 13)){
      // Serial.print(F("MMM01"));
      transferJSON["mapper"] = "MMM01";
    } else if ((romType == 15) || (romType == 16) || (romType == 17) || (romType == 18) || (romType == 19)) {
      // Serial.print(F("MBC3"));
      transferJSON["mapper"] = "MBC3";
    } else if ((romType == 21) || (romType == 22) || (romType == 23)) {
      // Serial.print(F("MBC4"));
      transferJSON["mapper"] = "MBC4";
    } else if ((romType == 25) || (romType == 26) || (romType == 27) || (romType == 28) || (romType == 29) || (romType == 309)) {
      // Serial.print(F("MBC5"));
      transferJSON["mapper"] = "MBC5";
    } else if (romType == 32) {
      // Serial.print(F("MBC6"));
      transferJSON["mapper"] = "MBC6";
    } else if (romType == 34) {
      // Serial.print(F("MBC7"));
      transferJSON["mapper"] = "MBC7";
    } else if (romType == 252) {
      // Serial.print(F("Camera"));
      transferJSON["mapper"] = "Camera";
    } else if (romType == 253) {
      // Serial.print(F("TAMA5"));
      transferJSON["mapper"] = "TAMA5";
    } else if (romType == 254) {
      // Serial.print(F("HuC-3"));
      transferJSON["mapper"] = "HuC-3";
    } else if (romType == 255) {
      // Serial.print(F("HuC-1"));
      transferJSON["mapper"] = "HuC-1";
    } else if ((romType == 0x101) || (romType == 0x103)) {
      // Serial.print(F("MBC1M"));
      transferJSON["mapper"] = "MBC1M";
    } else if (romType == 0x104) {
      // Serial.print(F("M161"));
      transferJSON["mapper"] = "M161";
    }

    // Serial.println(F(""));
    // Serial.print(F("ROM Size: "));
    switch (romSize)
    {
    case 0:
      // Serial.print(F("32 KB"));
      transferJSON["ROMSize"] = "32 KB";
      break;

    case 1:
      // Serial.print(F("64 KB"));
      transferJSON["ROMSize"] = "64 KB";
      break;

    case 2:
      // Serial.print(F("128 KB"));
      transferJSON["ROMSize"] = "128 KB";
      break;

    case 3:
      // Serial.print(F("256 KB"));
      transferJSON["ROMSize"] = "256 KB";
      break;

    case 4:
      // Serial.print(F("512 KB"));
      transferJSON["ROMSize"] = "512 KB";
      break;

    case 5:
      // Serial.print(F("1 MB"));
      transferJSON["ROMSize"] = "1 MB";
      break;

    case 6:
      // Serial.print(F("2 MB"));
      transferJSON["ROMSize"] = "2 MB";
      break;

    case 7:
      // Serial.print(F("4 MB"));
      transferJSON["ROMSize"] = "4 MB";
      break;

    case 8:
      // Serial.print(F("8 MB"));
      transferJSON["ROMSize"] = "8 MB";
      break;
    }

    // Serial.println(F(""));
    // Serial.print(F("Banks: "));
    // Serial.println(romBanks);

    // Serial.print(F("Save Size: "));
    switch (sramSize)
    {
    case 0:
      if (romType == 6)
      {
        // Serial.print(F("512 Byte"));
        transferJSON["RAMSize"] = "512 Byte";
      }
      else if (romType == 0x22)
      {
        transferJSON["RAMSize"] = String(lastByte) + " Byte";
        // Serial.print(lastByte);
        // Serial.print(F(" Byte"));
      }
      else if (romType == 0xFD)
      {
        transferJSON["RAMSize"] = "32 Byte";
        // Serial.print(F("32 Byte"));
      }
      else
      {
        // Serial.print(F("None"));
        transferJSON["RAMSize"] = "None";
      }
      break;
    case 1:
      // Serial.print(F("2 KB"));
      transferJSON["RAMSize"] = "2 KB";
      break;

    case 2:
      // Serial.print(F("8 KB"));
      transferJSON["RAMSize"] = "8 KB";
      break;

    case 3:
      if (romType == 0x20)
      {
        // Serial.print(F("1.03 MB"));
        transferJSON["RAMSize"] = "1.03 MB";
      }
      else
      {
        // Serial.print(F("32 KB"));
        transferJSON["RAMSize"] = "32 KB";
      }
      break;

    case 4:
      // Serial.print(F("128 KB"));
      transferJSON["RAMSize"] = "128 KB";
      break;

    case 5:
      // Serial.print(F("64 KB"));
      transferJSON["RAMSize"] = "64 KB";
      break;

    default:
      // Serial.print(F("None"));
      transferJSON["RAMSize"] = "None";
    }
    // Serial.println(F(""));
    delay(2);

      uint32_t crc32sum = 1;
      boolean renamerom = 0;
      int offset = 0;
      char crcStr[9];

      // Convert precalculated crc to string
      sprintf(crcStr, "%08lX", ~crc32sum);
      /*
      //  TODO: Implepement Rom name from memory file
      MemoryFile myFile(gb, sizeof(gb));

      if (myFile.open())
      {
        char gamename[96];
        char crc_search[9];
        // Loop through file
        while (myFile.available())
        {
          //Read 2 lines (game name and CRC)
          get_line(gamename, &myFile, sizeof(gamename));
          get_line(crc_search, &myFile, sizeof(crc_search));
          skip_line(&myFile);  //Skip every 3rd line
          //if checksum search successful, rename the file and end search
          if (strcmp(crc_search, crcStr) == 0) {
            transferJSON["ROMName"] = gamename;
            // Serial.print("gamename: ");
            // Serial.println(gamename);
          }
        }
      }
      */
  }
  else
  {
    transferJSON["success"] = 0;
    transferJSON["message"] = "GAMEPAK ERROR";
    // Serial.println(F("GAMEPAK ERROR"));
  }
  if(printInfo) {
    Serial.print("JSON:");
    serializeJson(transferJSON, Serial);
    Serial.println();
  }
}


// uint32_t luma_hist[4];
// uint32_t luma_acc = 0;
// uint32_t luma_idx = 0;

void gameboy_setup()
{

  Serial.begin(115200);
  Serial1.begin(115200);
  buffer_obj = Buffer();
  // Set Data Pins (D0-D7) to Input
  for (uint32_t i = 0; i < sizeof(ADDRESS_GB_GBC_PINS)/sizeof(ADDRESS_GB_GBC_PINS[0]); i++)
  {
      pinMode(ADDRESS_GB_GBC_PINS[i], OUTPUT);
  }

  // Set Control Pins to Output RST(PH0) CLK(PH1) CS(PH3) WR(PH5) RD(PH6)

  pinMode(GAMEBOY_RST, OUTPUT);
  pinMode(GAMEBOY_CLK, OUTPUT);
  pinMode(GAMEBOY_CS, OUTPUT);
  pinMode(GAMEBOY_WR, OUTPUT);
  pinMode(GAMEBOY_RD, OUTPUT);
  // pinMode(GB_AUDIO, OUTPUT);
  
  // Output a high signal on all pins, pins are active low therefore everything is disabled now
  digitalWrite(GAMEBOY_CS, HIGH);
  digitalWrite(GAMEBOY_WR, HIGH);
  digitalWrite(GAMEBOY_RD, HIGH);

  // Output a low signal on CLK(PH1) to disable writing GB Camera RAM
  // Output a low signal on RST(PH0) to initialize MMC correctly
  digitalWrite(GAMEBOY_CLK, LOW);
  digitalWrite(GAMEBOY_RST, LOW);


  // Set Data Pins (D0-D7) to Input
  for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS)/sizeof(DATA_GB_GBC_PINS[0]); i++)
  {
      pinMode(DATA_GB_GBC_PINS[i], INPUT);
  }
  for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS)/sizeof(DATA_GB_GBC_PINS[0]); i++)
  {
      pinMode(DATA_GB_GBC_PINS[i], HIGH);
  }
  delay(400);

  // RST(PH0) to H
  digitalWrite(GAMEBOY_RST, HIGH);
}
// Read ROM
void readROM_GB() {
  // word endAddress = 0x7FFF;
  word romAddress = 0;
  // word startBank = 1;

  //Initialize progress bar
  uint32_t processedProgressBar = 0;
  uint32_t totalProgressBar = (uint32_t)(romBanks)*16384;


  // M161 banks are double size and start with 0
  // if (romType == 0x104) {
  //   romStartBank = 0;
  //   romBanks >>= 1;
  //   romEndAddress = 0x7FFF;
  // }
  // // MBC6 banks are half size
  // else if (romType == 32) {
  //   romBanks <<= 1;
  //   romEndAddress = 0x3FFF;
  // }
  transferJSON["type"] = "rom";
  transferJSON["total"] = totalProgressBar;
  transferJSON["progress"] = processedProgressBar * 100 / totalProgressBar;
  transferJSON["romBanks"] = romBanks;

  Serial.print("JSON:");
  serializeJson(transferJSON, Serial);
  Serial.println();

  //delay(1000);
  // Serial.print("total: ");
  // Serial.println(totalProgressBar);
  // Serial.print("romBanks: ");
  // Serial.println(romBanks);
  for (word currBank = romStartBank; currBank < romBanks; currBank++) {
    // Second bank starts at 0x4000
    if (currBank > 1) {
      romAddress = 0x4000;

      // MBC6 banks are half size
      if (romType == 32) {
        romEndAddress = 0x5FFF;
      }
    }

    // Set ROM bank for M161
    if (romType == 0x104) {
      romAddress = 0;
      // Set CS2(PH0) to LOW
      // PORTH &= ~(1 << 0);
      cs2Pin_low;
      delay(50);
      // Set CS2(PH0) to HIGH
      // PORTH |= (1 << 0);
      cs2Pin_high;
      writeByte_GB(0x4000, currBank & 0x7);
    }

    // Set ROM bank for MBC1M
    else if (romType == 0x101 || romType == 0x103) {
      if (currBank < 10) {
        writeByte_GB(0x4000, currBank >> 4);
        writeByte_GB(0x2000, (currBank & 0x1f));
      } else {
        writeByte_GB(0x4000, currBank >> 4);
        writeByte_GB(0x2000, 0x10 | (currBank & 0x1f));
      }
    }

    // Set ROM bank for MBC6
    else if (romType == 32) {
      writeByte_GB(0x2800, 0);
      writeByte_GB(0x3800, 0);
      writeByte_GB(0x2000, currBank);
      writeByte_GB(0x3000, currBank);
    }

    // Set ROM bank for TAMA5
    else if (romType == 0xFD) {
      // writeByteSRAM_GB(0xA001, 0);
      // writeByteSRAM_GB(0xA000, currBank & 0x0f);
      // writeByteSRAM_GB(0xA001, 1);
      // writeByteSRAM_GB(0xA000, (currBank >> 4) & 0x0f);
    }

    // Set ROM bank for MBC2/3/4/5
    else if (romType >= 5) {
      if (romType >= 11 && romType <= 13) {
        if ((currBank & 0x1f) == 0) {
          // reset MMM01
          // PORTH &= ~(1 << 0);
          cs2Pin_low;
          delay(50);
          // PORTH |= (1 << 0);
          cs2Pin_high;

          // remap to higher 4Mbits ROM
          writeByte_GB(0x3fff, 0x20);
          writeByte_GB(0x5fff, 0x40);
          writeByte_GB(0x7fff, 0x01);
          writeByte_GB(0x1fff, 0x3a);
          writeByte_GB(0x1fff, 0x7a);

          // for every 4Mbits ROM, restart from 0x0000
          romAddress = 0x0000;
          currBank++;
        } else {
          writeByte_GB(0x6000, 0);
          writeByte_GB(0x2000, (currBank & 0x1f));
        }
      } else {
        if ((romType >= 0x19 && romType <= 0x1E) && (currBank == 0 || currBank == 256)) {
          writeByte_GB(0x3000, (currBank >> 8) & 0xFF);
        }
        writeByte_GB(0x2100, currBank & 0xFF);
      }
    }
    // Set ROM bank for MBC1
    else {
      writeByte_GB(0x6000, 0);
      writeByte_GB(0x4000, currBank >> 5);
      writeByte_GB(0x2000, currBank & 0x1F);
    }
    
    // Read banks and save to SD
    while (romAddress <= romEndAddress) {
      for (int i = 0; i < 512; i++) {
        char hexbuffer[3]; // Se reserva espacio para almacenar el valor hexadecimal y el carácter nulo
        sdBuffer[i] = read_8bit_data(romAddress + i);
        // sprintf(hexbuffer, "%02X", sdBuffer[i]);
        // Serial.print(hexbuffer);
        // if ((i + 1) % 16 == 0) {
        //   Serial.println();
        // } else {
        //   Serial.print(' ');
        // }
      }
      Serial1.write(sdBuffer, 512);
      romAddress += 512;
      processedProgressBar += 512;
      //transferJSON["progress"] = processedProgressBar * 100 / totalProgressBar;

      //Serial.print("JSON:");
      //serializeJson(transferJSON, Serial);
      //Serial.println();
      // Serial.print(processedProgressBar * 100 / totalProgressBar );
      // Serial.println("%");
    }
    
  }
  delay(200);
  // Serial.println("Success");
  transferJSON["type"] = "success";
  transferJSON["total"] = totalProgressBar;
  transferJSON["progress"] = processedProgressBar * 100 / totalProgressBar;
  transferJSON["romBanks"] = romBanks;

  Serial.print("JSON:");
  serializeJson(transferJSON, Serial);
  Serial.println();

}


// Triggers CS and CLK pin
byte readByteSRAM_GB(word myAddress) {
  // PORTF = myAddress & 0xFF;
  // PORTK = (myAddress >> 8) & 0xFF;
  set_16bit_address(myAddress);

  // Switch data pins to input
  // DDRC = 0x00;
  for (int i = 0; i < 8; i++)
  {
    pinMode(DATA_GB_GBC_PINS[i], INPUT);
  }
  // Enable pullups
  // for (int i = 0; i < 8; i++)
  // {
  //   pinMode(DATA_GB_GBC_PINS[i], HIGH);
  // }
  // PORTC = 0xFF;
  // delay(3);
  __asm__("nop\n\t"
          "nop\n\t"
          "nop\n\t"
          "nop\n\t");

  // Pull CS(PH3) CLK(PH1)(for FRAM MOD) LOW
  // PORTH &= ~((1 << 3) | (1 << 1));
  cs_mreqPin_low; // GPIO32 (PB7) - cs_mreqPin_low
  phiPin_low;      // GphiPin_low
  // Pull RD(PH6) LOW
  // PORTH &= ~(1 << 6);
  rdPin_low;
  // delay(3);
  __asm__("nop\n\t"
          "nop\n\t"
          "nop\n\t"
          "nop\n\t");
  // __asm__("nop\n\t"
  //         "nop\n\t"
  //         "nop\n\t"
  //         "nop\n\t");

  // Read
  // byte tempByte = PINC;
  byte tempByte = 0; // GPIO7 (PC0) - PIN_DATA7_0 - Read data
  for (int i = 0; i < 8; i++)
  {
    int raw_data = digitalRead(DATA_GB_GBC_PINS[i]);
    // Serial.print(raw_data);
    tempByte |= raw_data << i;
  }

  // Pull RD(PH6) HIGH
  // PORTH |= (1 << 6);
  rdPin_high;
  if (romType == 252) {
    // Pull CS(PH3) HIGH
    // PORTH |= (1 << 3);
    cs_mreqPin_high; // GPIO32 (PB7) - cs_mreqPin_low
  } else {
    // Pull CS(PH3) CLK(PH1)(for FRAM MOD) HIGH
    cs_mreqPin_high; // GPIO32 (PB7) - cs_mreqPin_low
    phiPin_high;
    // PORTH |= (1 << 3) | (1 << 1);
  }
  // delay(3);
  __asm__("nop\n\t"
          "nop\n\t"
          "nop\n\t"
          "nop\n\t");
  // __asm__("nop\n\t"
  //         "nop\n\t"
  //         "nop\n\t"
  //         "nop\n\t");

  return tempByte;
}
// Read RAM
void readSRAM_GB() {
  // Does cartridge have RAM
  // if (lastByte > 0) {
    //Initialize progress bar
    uint32_t processedProgressBar = 0;
    uint32_t totalProgressBar = (uint32_t)(sramBanks)*8192;
    // // Get name, add extension and convert to char array for sd lib
    // strcpy(fileName, romName);
    // strcat(fileName, ".sav");

    // create a new folder for the save file
    // EEPROM_readAnything(0, foldern);
    // sprintf(folder, "GB/SAVE/%s/%d", romName, foldern);
    // sd.mkdir(folder, true);
    // sd.chdir(folder);

    // write new folder number back to eeprom
    // foldern = foldern + 1;
    // EEPROM_writeAnything(0, foldern);

    // //open file on sd card
    // if (!myFile.open(fileName, O_RDWR | O_CREAT)) {
    //   print_FatalError(sd_error_STR);
    // }

    transferJSON["type"] = "ram";
    transferJSON["total"] = totalProgressBar;
    transferJSON["progress"] = processedProgressBar * 100 / totalProgressBar;
    transferJSON["ramBanks"] = sramBanks;
    transferJSON["lastByte"] = ramEndAddress;
    

    Serial.print("JSON:");
    serializeJson(transferJSON, Serial);
    Serial.println();

    rd_wr_mreq_reset();
    
    
    // MBC2 Fix (unknown why this fixes reading the ram, maybe has to read ROM before RAM?)
    read_8bit_data(0x0134);

    if (cartridgeType <= 4) { // MBC1
      writeByte_GB(0x6000, 1); // Set RAM Mode
    }
    // if cartridge have RAM test
    if (ramEndAddress > 0) {
      
      // Initialise MBC
      writeByte_GB(0x0000, 0x0A);
      
      // Switch RAM banks
      for (uint8_t bank = 0; bank < sramBanks; bank++) {
        writeByte_GB(0x4000, bank);
        
        // Read RAM
        for (uint16_t ramAddress = 0xA000; ramAddress <= ramEndAddress; ramAddress += 128) {
          uint8_t readData[128];
          for (uint8_t i = 0; i < 128; i++) {
            readData[i] = readByteSRAM_GB(ramAddress+i);
          }
          // Serial.write(readData, 64); // Send the 64 byte chunk
          Serial1.write(readData, 128);
          processedProgressBar+=128;
        }
      }
      
      // Disable RAM
      writeByte_GB(0x0000, 0x00);

      transferJSON["type"] = "success";
      transferJSON["total"] = totalProgressBar;
      transferJSON["progress"] = processedProgressBar * 100 / totalProgressBar;
      transferJSON["ramBanks"] = sramBanks;

      Serial.print("JSON:");
      serializeJson(transferJSON, Serial);
      Serial.println();
    }

    rd_wr_mreq_off();
    /*
    // MBC2 Fix
    read_8bit_data(0x0134);

    if (romType <= 4 || (romType >= 11 && romType <= 13)) {
      writeByte_GB(0x6000, 1);
    }

    // Initialise MBC
    writeByte_GB(0x0000, 0x0A);

    // Switch SRAM banks
    for (byte currBank = 0; currBank < sramBanks; currBank++) {
      writeByte_GB(0x4000, currBank);

      // Read SRAM
      for (word sramAddress = 0xA000; sramAddress <= ramEndAddress; sramAddress += 64) {
        for (byte i = 0; i < 64; i++) {
          sdBuffer[i] = readByteSRAM_GB(sramAddress + i);
        }
        // myFile.write(sdBuffer, 64);
        Serial1.write(sdBuffer, 64);
        processedProgressBar+=64;
      }
    }

    // Disable SRAM
    writeByte_GB(0x0000, 0x00);

    transferJSON["type"] = "success";
    transferJSON["total"] = totalProgressBar;
    transferJSON["progress"] = processedProgressBar * 100 / totalProgressBar;
    transferJSON["ramBanks"] = sramBanks;

    Serial.print("JSON:");
    serializeJson(transferJSON, Serial);
    Serial.println();
    // Close the file:
    // myFile.close();

    // Signal end of process
    // print_Msg(F("Saved to "));
    // print_Msg(folder);
    // println_Msg(F("/"));
    // display_Update();
    */
}

void gameboy_loop()
{
  if (Serial.available() > 0) {

    uint8_t cmd = Serial.read();

    transferJSON.clear();
    //  Cartridge Info
    if(cmd == 'i') {
      showCartInfo_GB(true);
    }
    //  Backup Cartridge ROM
    if(cmd == 'o') {
      // Serial.println("Backup ROM");
      // showCartInfo_GB(false);
      readROM_GB();
    }
    //  Backup Cartridge RAM
    if(cmd == 'a') {
      // showCartInfo_GB(false);
      // Serial.println("Backup RAM");
      readSRAM_GB();
    }
  }
}