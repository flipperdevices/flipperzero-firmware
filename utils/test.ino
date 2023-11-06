#include <Arduino.h>
#include "setup.h"
#include <stdio.h>
#include <string>

#define F_CPU 8000000 // 8 MHz
#define PCB_VERSION 1
#define FIRMWARE_VERSION 2


uint32_t address = 0;
uint8_t eepromSize = EEPROM_4KBIT;
uint8_t cartMode = GB_MODE;
uint8_t resetCommonLines = 1;

void setup() {

    Serial.begin(115200);

    // Reset common lines
	rd_wr_csmreq_cs2_reset();
	
	// Set outputs
    pinMode(GAMEBOY_RST, OUTPUT);
    pinMode(GAMEBOY_CLK, OUTPUT);
    pinMode(GAMEBOY_CS, OUTPUT);
    pinMode(GAMEBOY_WR, OUTPUT);
    pinMode(GAMEBOY_RD, OUTPUT);

	// Set all pins as inputs
    for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
    {
        pinMode(DATA_GB_GBC_PINS[i], INPUT);
    }
    for (uint32_t i = 0; i < sizeof(ADDRESS_GB_GBC_PINS) / sizeof(ADDRESS_GB_GBC_PINS[0]); i++)
    {
        pinMode(ADDRESS_GB_GBC_PINS[i], INPUT);
    }

}

void loop() {
    if (resetCommonLines == 1) {
        rd_wr_csmreq_cs2_reset();
    }

    receivedChar = USART_Receive(); // Wait for 1 byte of data



    Serial.println(receivedChar);

    cartMode = GB_MODE;
    if (receivedChar == GB_CART_MODE) {
        gb_mode();
    } 
    // Set address
    else if (receivedChar == SET_START_ADDRESS) {
        usart_read_chars(); // Read start address
        address = strtol(receivedBuffer, NULL, 16); // Convert address string in hex to dec
    }
    // ****** Gameboy / Gameboy Colour ******
		
    // Read 64 bytes of ROM/RAM from address (and increment) until anything but 1 is received
    else if (receivedChar == READ_ROM_RAM) {
        gb_mode();
        receivedChar = '1';
        while (receivedChar == '1') {
            for (uint8_t x = 0; x < 64; x++) {
                USART_Transmit(read_8bit_data(address));
                address++;
            }
            receivedChar = USART_Receive();
        }
    }
    // Write 64 bytes to RAM on address (and increment)
    else if (receivedChar == WRITE_RAM) {
        gb_mode();
        
        // Read 64 bytes first as CH340G sends them all at once
        usart_read_bytes(64);
        
        for (uint8_t x = 0; x < 64; x++) {
            write_8bit_data(address, receivedBuffer[x], MEMORY_WRITE);
            address++;
        }
        
        USART_Transmit('1'); // Send back acknowledgement
    }
    // Set bank address and write a byte
    else if (receivedChar == SET_BANK) {
        gb_mode();
        
        usart_read_chars(); // Read start address
        uint16_t bankaddress = strtol(receivedBuffer, NULL, 16); // Convert address string in hex to dec
        
        receivedChar = USART_Receive(); // Wait for bank number
        if (receivedChar == 'B') {
            usart_read_chars(); // Read data
            uint8_t data = atoi(receivedBuffer); // Convert data string to dec
            
            write_8bit_data(bankaddress, data, BANK_WRITE);
        }
    }
    // ---------- General commands ----------
    // Set any pin as input/output
    // Reads the DDR/PORT (e.g. DDRB/PORTB is 'B') and the hex value that represents pins to set to an input (e.g. PB7 is 0x80)
    else if (receivedChar == SET_INPUT || receivedChar == SET_OUTPUT) {
        char portChar = USART_Receive();
        usart_read_chars();
        uint8_t setValue = strtol(receivedBuffer, NULL, 16);
        
        // if (receivedChar == SET_INPUT) {
        //     if (portChar == 'A') {
        //         DDRA &= ~(setValue);
        //     }
        //     else if (portChar == 'B') {
        //         DDRB &= ~(setValue);
        //     }
        //     else if (portChar == 'C') {
        //         DDRC &= ~(setValue);
        //     }
        //     else if (portChar == 'D') {
        //         DDRD &= ~(setValue);
        //     }
        // }
        // else if (receivedChar == SET_OUTPUT) {
        //     if (portChar == 'A') {
        //         DDRA |= (setValue);
        //     }
        //     else if (portChar == 'B') {
        //         DDRB |= (setValue);
        //     }
        //     else if (portChar == 'C') {
        //         DDRC |= (setValue);
        //     }
        //     else if (portChar == 'D') {
        //         DDRD |= (setValue);
        //     }
        // }
    }
}