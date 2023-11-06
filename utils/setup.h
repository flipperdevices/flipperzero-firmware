#include <Arduino.h>

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


#define GB_MODE 1
#define GBA_MODE 2

// GB/GBC
#define PORT_ADDR7_0 PORTB
#define PORT_ADDR15_8 PORTA
#define PORT_DATA7_0 PORTC

#define DDR_ADDR7_0 DDRB
#define DDR_ADDR15_8 DDRA
#define DDR_DATA7_0 DDRC

#define PIN_ADDR7_0 PINB
#define PIN_ADDR15_8 PINA
#define PIN_DATA7_0 PINC

#define BANK_WRITE 0
#define MEMORY_WRITE 1

// GB/GBC commands
#define SET_START_ADDRESS 'A'
#define READ_ROM_RAM 'R'
#define WRITE_RAM 'W'
#define SET_BANK 'B'
#define GB_CART_MODE 'G'


// General commands
#define CART_MODE 'C'
#define SET_INPUT 'I'
#define SET_OUTPUT 'O'
#define SET_OUTPUT_LOW 'L'
#define SET_OUTPUT_HIGH 'H'
#define READ_INPUT 'D'
#define RESET_COMMON_LINES 'M'
#define READ_FIRMWARE_VERSION 'V'
#define READ_PCB_VERSION 'h'

char receivedBuffer[129];
char receivedChar;
uint8_t eepromBuffer[8];
uint8_t flashChipIdBuffer[2];

// Receive USART data
uint8_t USART_Receive(void) {
    uint8_t read_byte;
	while ( Serial.available()) { // Wait for data to be received
        read_byte = Serial.read();
    }
	return read_byte; // Get and return received data from buffer
}

// Transmit USART data
void USART_Transmit(unsigned char data) {
    Serial.write(data, 1);
}

// Read 1-128 bytes from the USART 
void usart_read_bytes(uint8_t count) {
	for (uint8_t x = 0; x < count; x++) {
		receivedBuffer[x] = USART_Receive();
	}
}

// Read the USART until a 0 (string terminator byte) is received
void usart_read_chars(void) {
	uint8_t x = 0;
	while (1) {
		receivedBuffer[x] = USART_Receive();
		if (receivedBuffer[x] == 0) {
			break;
		}
		x++;
	}
}

// Turn RD, WR, CS/MREQ and CS2 to high so they are deselected (reset state)
void rd_wr_csmreq_cs2_reset(void) {
	cs2Pin_high; // CS2 off
	cs_mreqPin_high; // CS/MREQ off
	rdPin_high; // RD off
	wrPin_high; // WR off
}



// ****** Gameboy / Gameboy Colour functions ******

// Set Gameboy mode
void gb_mode(void) {
	// Set inputs
	for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
    {
        pinMode(DATA_GB_GBC_PINS[i], INPUT);
    }
	
	// Set outputs
	for (uint32_t i = 0; i < sizeof(ADDRESS_GB_GBC_PINS) / sizeof(ADDRESS_GB_GBC_PINS[0]); i++)
    {
        pinMode(ADDRESS_GB_GBC_PINS[i], OUTPUT);
    }
}

// Set the 16 bit address on A15-0
void set_16bit_address(uint16_t address) {
	for (uint32_t i = 0; i < sizeof(ADDRESS_GB_GBC_PINS) / sizeof(ADDRESS_GB_GBC_PINS[0]); i++)
    {
        digitalWrite(ADDRESS_GB_GBC_PINS[i], address & (1 << i) ? HIGH : LOW);
    }
}

// Set the address and read a byte from the 8 bit data line
uint8_t read_8bit_data(uint16_t address) {
	set_16bit_address(address);
	
	cs_mreqPin_low;
	rdPin_low;
	
	asm volatile("nop"); // Delay a little (minimum needed is 1 nops, 2 nops for GB camera)
	asm volatile("nop");
    uint8_t data; // Read data
    for (int i = 0; i < 8; i++)
    {
        int raw_data = digitalRead(DATA_GB_GBC_PINS[i]);
        data |= raw_data << i;
    }
	
	rdPin_high;
	cs_mreqPin_high;
	
	return data;
}

// Set the address and write a byte to the 8 bit data line and pulse cs/mREQ if writing to RAM
void write_8bit_data(uint16_t address, uint8_t data, uint8_t type) {
	set_16bit_address(address);
	
	for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
    {
        pinMode(DATA_GB_GBC_PINS[i], OUTPUT);
    }

    for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
    {
        digitalWrite(DATA_GB_GBC_PINS[i], data & (1 << i) ? HIGH : LOW);
    }
	
	// Pulse WR and mREQ if the type matches
	wrPin_low;
	if (type == MEMORY_WRITE) {
		cs_mreqPin_low;
	}
	
	asm volatile("nop");
	
	if (type == MEMORY_WRITE) {
		cs_mreqPin_high;
	}
	wrPin_high;
	
	// Clear data outputs and set data pins as inputs
	for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
    {
        pinMode(DATA_GB_GBC_PINS[i], INPUT);
    }
}
