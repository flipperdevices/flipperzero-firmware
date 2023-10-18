#ifndef gameboy_h
#define gameboy_h

// #include "SerialTransfer.h"
// #include "MemoryFile.h"
// #include <iostream>
// #include <string>
// #include <vector>
// #include <iomanip>  // Para std::setw y std::setfill

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



// Common
// 21 chars for SNES ROM name, one char for termination
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

// const int phi_pin = 13; // (SCK)    (GIPIO12)                        // PORTB.5        
// const int nwr_pin = 12; // (MISO)   (GIPIO13)
// const int nrd_pin = 11; // (MOSI)   (GIPIO11)
// const int ncs_pin = 10; // (SS)     (GIPIO10)


// #define GAMEBOY_CLK 1   //  
// #define GAMEBOY_WR 3    //  PORTL0
// #define GAMEBOY_RD 4    //  PORTL1
// #define GAMEBOY_CS 5    //  PORTL2

#define GAMEBOY_CLK 12   //             (FSPICLK)(SCLK)SCK
#define GAMEBOY_WR 13    //  PORTL0     (FSPIQ)(DC)MISO
#define GAMEBOY_RD 11    //  PORTL1     (FSPID)(SDA)MOSI
#define GAMEBOY_CS 10    //  PORTL2     (FSPICS0)(CS)SS

// #define GAMEBOY_A00 40  //  DDRF0
// #define GAMEBOY_A01 39  //  DDRF1
// #define GAMEBOY_A02 38  //  DDRF2
// #define GAMEBOY_A03 37  //  DDRF3       (FSPIQ)(DC)MISO
// #define GAMEBOY_A04 36  //  DDRF4       (FSPICLK)(SCLK)SCK
// #define GAMEBOY_A05 35  //  DDRF5       (FSPID)(SDA)MOSI
// #define GAMEBOY_A06 34  //  DDRF6       (FSPICS0)(CS)SS
// #define GAMEBOY_A07 33  //  DDRF7

#define GAMEBOY_A00 42  //  DDRF0
#define GAMEBOY_A01 41  //  DDRF1
#define GAMEBOY_A02 40  //  DDRF2
#define GAMEBOY_A03 39  //  DDRF3       
#define GAMEBOY_A04 38  //  DDRF4       
#define GAMEBOY_A05 37  //  DDRF5       (FSPIQ)(DC)MISO
#define GAMEBOY_A06 36  //  DDRF6       (FSPICLK)(SCLK)SCK
#define GAMEBOY_A07 35  //  DDRF7       (FSPID)(SDA)MOSI

// #define GAMEBOY_A08 26  //  PORTK0
// #define GAMEBOY_A09 21  //  PORTK1
// #define GAMEBOY_A10 20  //  PORTK2
// #define GAMEBOY_A11 19  //  PORTK3
// #define GAMEBOY_A12 16  //  PORTK4
// #define GAMEBOY_A13 15  //  PORTK5
// #define GAMEBOY_A14 14  //  PORTK6      (SCL)
// #define GAMEBOY_A15 9  //  PORTK7 -     (SDA)

#define GAMEBOY_A08 34  //  PORTK0      (FSPICS0)(CS)SS
#define GAMEBOY_A09 33  //  PORTK1
#define GAMEBOY_A10 26  //  PORTK2
#define GAMEBOY_A11 20  //  PORTK3
#define GAMEBOY_A12 19  //  PORTK4
#define GAMEBOY_A13 16  //  PORTK5
#define GAMEBOY_A14 15  //  PORTK6      
#define GAMEBOY_A15 14  //  PORTK7 -     


// #define GAMEBOY_D00 8   //  PORTA0
// #define GAMEBOY_D01 7   //  PORTA1
// #define GAMEBOY_D02 6   //  PORTA2
// #define GAMEBOY_D03 5  //  PORTA3
// #define GAMEBOY_D04 4  //  PORTA4
// #define GAMEBOY_D05 3  //  PORTA5
// #define GAMEBOY_D06 2  //  PORTA6
// #define GAMEBOY_D07 1  //  PORTA7

#define GAMEBOY_D00 8   //  PORTA0     (SCL)
#define GAMEBOY_D01 7   //  PORTA1      (SDA)
#define GAMEBOY_D02 6   //  PORTA2
#define GAMEBOY_D03 5  //  PORTA3
#define GAMEBOY_D04 4  //  PORTA4
#define GAMEBOY_D05 3  //  PORTA5
#define GAMEBOY_D06 2  //  PORTA6
#define GAMEBOY_D07 1  //  PORTA7

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


// #define GAMEBOY_LED_1 2  //  ahora es por el pin A4 deL Flipper Zero

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


#define F_CPU 16000000
#define CHARLEN ((F_CPU/115200)-13)


#define GB_MODE 1
#define GBA_MODE 2


#define BANK_WRITE 0
#define MEMORY_WRITE 1

// GBA
#define EEPROM_WRITE 1
#define EEPROM_READ 0

#define EEPROM_NONE 0
#define EEPROM_4KBIT 1
#define EEPROM_64KBIT 2

#define AD0 GPIO34
#define ad0Pin_high		digitalWrite(AD0, HIGH);
#define ad0Pin_low		digitalWrite(AD0, LOW);

#define A23 GPIO14
#define a23Pin_high		digitalWrite(A23, HIGH);
#define a23Pin_low		digitalWrite(A23, LOW);

void set_16bit_address(uint16_t address);
byte readByteSRAM_GB(word myAddress);
byte read_8bit_data(uint16_t address);
void writeByte_GB(int address, byte data);
// // GB/GBC commands
// #define SET_START_ADDRESS 'A'
// #define READ_ROM_RAM 'R'
// #define READ_ROM_4000H 'Q'
// #define WRITE_RAM 'W'
// #define SET_BANK 'B'
// #define GB_CART_MODE 'G'

// // GBA commands
// #define GBA_READ_ROM 'r'
// #define GBA_READ_ROM_256BYTE 'j'
// #define GBA_READ_ROM_8000H 'Z'
// #define GBA_READ_SRAM 'm'
// #define GBA_WRITE_SRAM 'w'
// #define GBA_WRITE_ONE_BYTE_SRAM 'o'
// #define GBA_CART_MODE 'g'

// #define GBA_FLASH_READ_ID 'i'
// #define GBA_FLASH_SET_BANK 'k'
// #define GBA_FLASH_4K_SECTOR_ERASE 's'
// #define GBA_FLASH_WRITE_BYTE 'b'
// #define GBA_FLASH_WRITE_ATMEL 'a'

// #define GBA_SET_EEPROM_SIZE 'S'
// #define GBA_READ_EEPROM 'e'
// #define GBA_WRITE_EEPROM 'p'

// // Flash Cart commands
// #define GB_FLASH_WE_PIN 'P'
// #define WE_AS_AUDIO_PIN 'A'
// #define WE_AS_WR_PIN 'W'

// #define GB_FLASH_PROGRAM_METHOD 'E'
// #define GB_FLASH_PROGRAM_555 0
// #define GB_FLASH_PROGRAM_AAA 1
// #define GB_FLASH_PROGRAM_555_BIT01_SWAPPED 2
// #define GB_FLASH_PROGRAM_AAA_BIT01_SWAPPED 3
// #define GB_FLASH_PROGRAM_5555 4

// #define GB_FLASH_WRITE_BYTE 'F'
// #define GB_FLASH_WRITE_BUFFERED_32BYTE 'Y'
// #define GB_FLASH_WRITE_BUFFERED_256BYTE 'U'
// #define GB_FLASH_WRITE_64BYTE 'T'
// #define GB_FLASH_WRITE_64BYTE_PULSE_RESET 'J'
// #define GB_FLASH_WRITE_256BYTE 'X'
// #define GB_FLASH_WRITE_NP_128BYTE 'z'
// #define GB_FLASH_WRITE_INTEL_BUFFERED_32BYTE 'y'

// #define GB_FLASH_BANK_1_COMMAND_WRITES 'N'

// #define GBA_FLASH_CART_WRITE_BYTE 'n'
// #define GBA_FLASH_WRITE_64BYTE_SWAPPED_D0D1 'q'
// #define GBA_FLASH_WRITE_256BYTE_SWAPPED_D0D1 't'
// #define GBA_FLASH_WRITE_256BYTE 'f'
// #define GBA_FLASH_WRITE_BUFFERED_256BYTE 'c'
// #define GBA_FLASH_WRITE_INTEL_64BYTE 'l'
// #define GBA_FLASH_WRITE_INTEL_64BYTE_WORD 'u'
// #define GBA_FLASH_WRITE_INTEL_INTERLEAVED_256BYTE 'v'
// #define GBA_FLASH_WRITE_SHARP_64BYTE 'x'

// #define D0D1_NOT_SWAPPED 0
// #define D0D1_SWAPPED 1

// General commands
// #define SEND_ACK '1'
// #define CART_MODE 'C'
// #define SET_INPUT 'I'
// #define SET_OUTPUT 'O'
// #define SET_OUTPUT_LOW 'L'
// #define SET_OUTPUT_HIGH 'H'
// #define READ_INPUT 'D'
// #define RESET_COMMON_LINES 'M'
// #define READ_FIRMWARE_VERSION 'V'
// #define READ_PCB_VERSION 'h'
// #define FAST_READ_CHECK '+'
// #define VOLTAGE_3_3V '3'
// #define VOLTAGE_5V '5'

// #define RESET_AVR '*'
// #define RESET_VALUE 0x7E5E1


// #define GB_FRAME_SIZE 	3584		// 128 * 112 * 2bpp / 8 bits
// #define ROW_BUFFER_LENGTH (GB_FRAME_SIZE / 8) // 128/8 = 16

#define GBCAM_W (128)
#define GBCAM_H (112)

#define FRAME_SIZE 	3584		// 128 * 112 * 2bpp / 8 bits

// #define GBC_TIMEOUT		50000		// CPU speed dependent !



// // Extract ASCII printable characters from input, collapsing underscores and spaces.
// // Use when extracting titles from cartridges, to build a rom title.
// byte buildRomName(char* output, const byte* input, byte length) {
// byte input_char;
// byte output_len = 0;
// for (unsigned int i = 0; i < length; i++) {
//     input_char = input[i];
//     if (isprint(input_char) && input_char != '<' && input_char != '>' && input_char != ':' && input_char != '"' && input_char != '/' && input_char != '\\' && input_char != '|' && input_char != '?' && input_char != '*') {
//     output[output_len++] = input_char;
//     } else {
//     if (output_len == 0 || output[output_len - 1] != '_') {
//         output[output_len++] = '_';
//     }
//     }
// }
// while (
//     output_len && (output[output_len - 1] == '_' || output[output_len - 1] == ' ')) {
//     output_len--;
// }
// output[output_len] = 0;
// return output_len;
// }


//******************************************
// Functions for CRC32 database
//******************************************
//Skip line
// void skip_line(MemoryFile* readfile) {
//     int i = 0;
//     char str_buf;

//     while (readfile->available()) {
//         //Read 1 byte from file
//         str_buf = static_cast<char>(readfile->read());

//         //if end of file or newline found, execute command
//         if (str_buf == '\r') {
//             readfile->read();  //dispose \n because \r\n
//             break;
//         }
//         i++;
//     }  //End while
// }

// //Get line from file
// void get_line(char* str_buf, MemoryFile* readfile, uint8_t maxi) {
//     int read_len = readfile->read(str_buf, maxi - 1);

//     for (int i = 0; i < read_len; i++) {
//         //if end of file or newline found, execute command
//         if (str_buf[i] == '\r') {
//             str_buf[i] = 0;
//             readfile->seekCur(i - read_len + 2);  // +2 to skip over \n because \r\n
//             return;
//         }
//     }
//     str_buf[maxi - 1] = 0;
//     // EOL was not found, keep looking (slower)
//     while (readfile->available()) {
//         if (readfile->read() == '\r') {
//             readfile->read();  // read \n because \r\n
//             break;
//         }
//     }
// }
// void rewind_line(MemoryFile& readfile, byte count = 1) {
//     uint32_t position = readfile.curPosition();
//     // To seek one line back, this code must step over the first newline it finds
//     // in order to exit the current line and enter the end of the previous one.
//     // Convert <count> from how-many-lines-back into how-many-newlines-to-look-for
//     // by incrementing it by 1.
//     count++;
//     for (byte count_newline = 0; count_newline < count; count_newline++) {
//         // Go to the strictly previous '\n', or file start.
//         while (position) {
//         // Seek back first (keeping position updated)...
//         position--;
//         readfile.seekCur(-1);
//         // ...and check current byte second.
//         // Note: this code assumed all files use ASCII with DOS-style newlines
//         // so \n is encountered first when seeking backwards.
//         if (readfile.peek() == '\n')
//             break;
//         }
//     }
//     // If not at file start, the current character is the '\n' just before the
//     // desired line, so advance by one.
//     if (position)
//         readfile.seekCur(1);
// }
    

#endif
