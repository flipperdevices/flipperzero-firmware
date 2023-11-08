#include "GameBoyAdvanceCartridge.h"
#include "save.h"
#include "soc/rtc_wdt.h"


GameBoyAdvanceCartridge::GameBoyAdvanceCartridge()
{
    this->runGameBoyAdvanceCartridge = false;
    this->writtingRAM = false;
    this->writtingROM = false;
    this->restoringRAM = false;
    this->romAddress = 0;
    this->romSize = 0;
    this->sramSize = 0;
    
}
void GameBoyAdvanceCartridge::begin()
{
    this->setup();
}
void GameBoyAdvanceCartridge::start()
{
    pinMode(RD_PIN, OUTPUT);
    pinMode(CS_MREQ_PIN, OUTPUT);

    digitalWrite(RD_PIN, HIGH);
    digitalWrite(WR_PIN, HIGH);
    digitalWrite(CS_MREQ_PIN, LOW);
    digitalWrite(CS2_PIN, HIGH);

    transferJSON.clear();
    this->runGameBoyAdvanceCartridge = true;
}
void GameBoyAdvanceCartridge::stop()
{
    this->runGameBoyAdvanceCartridge = false;
}

void GameBoyAdvanceCartridge::dataBusAsOutput()
{
    for (uint32_t i = 0; i < sizeof(DATA_GBA_PINS) / sizeof(DATA_GBA_PINS[0]); i++)
    {
        pinMode(DATA_GBA_PINS[i], OUTPUT);
    }
}
void GameBoyAdvanceCartridge::dataBusAsInput()
{
    for (uint32_t i = 0; i < sizeof(DATA_GBA_PINS) / sizeof(DATA_GBA_PINS[0]); i++)
    {
        pinMode(DATA_GBA_PINS[i], INPUT);
    }
}

// Set the 16 bit address
void GameBoyAdvanceCartridge::set_address_GBA(word address)
{   
    // Set address ports to output
    for (uint32_t i = 0; i < sizeof(ADDRESS_GBA_PINS) / sizeof(ADDRESS_GBA_PINS[0]); i++)
    {
        pinMode(ADDRESS_GBA_PINS[i], OUTPUT);
    }
    // Divide address by two to get word addressing
    address = address >> 1;
    // Output address to address pins,
    for (uint32_t i = 0; i < sizeof(ADDRESS_GBA_PINS) / sizeof(ADDRESS_GBA_PINS[0]); i++)
    {
        digitalWrite(ADDRESS_GBA_PINS[i], (address >> i) & 1);
    }
}
word GameBoyAdvanceCartridge::read_word_GBA(word address)
{
    this->set_address_GBA(address);

    // Pull CS(PH3) to LOW
    cs_mreqPin_low;

    // Set address ports to input
    for (uint32_t i = 0; i < sizeof(ADDRESS_GBA_PINS) / sizeof(ADDRESS_GBA_PINS[0]); i++)
    {
        pinMode(ADDRESS_GBA_PINS[i], INPUT);
    }

    // Pull RD(PH6) to LOW
    rdPin_low;

    __asm__("nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t");

            
    // Read data
    word data = 0;
    for (int i = 0; i < 16; i++) {
        data |= (digitalRead(DATA_GBA_PINS[i]) << i);
    }


    // Switch RD(PH6) to HIGH

    rdPin_high;

    // Switch CS_ROM(PH3) to HIGH
    cs_mreqPin_high;


    return data;
}
// void GameBoyAdvanceCartridge::write_word_GBA(int address, word data)
// {
//     // Set address
//     this->set_address_GB(address);
//     // Set data
//     // Switch data pins to output
//     for (uint32_t i = 0; i < sizeof(DATA_GBA_PINS) / sizeof(DATA_GBA_PINS[0]); i++)
//     {
//         pinMode(DATA_GBA_PINS[i], OUTPUT);
//     }

//     for (uint32_t i = 0; i < sizeof(DATA_GBA_PINS) / sizeof(DATA_GBA_PINS[0]); i++)
//     {
//         digitalWrite(DATA_GBA_PINS[i], data & (1 << i) ? HIGH : LOW);
//     }

//     // Wait till output is stable
//     wait(120);
//     // __asm__("nop\n\t"
//     //         "nop\n\t"
//     //         "nop\n\t"
//     //         "nop\n\t");
//     // Pull WR low
//     wrPin_low;
//     // Leave WR low for at least 60ns
//     wait(120);
//     // __asm__("nop\n\t"
//     //         "nop\n\t"
//     //         "nop\n\t"
//     //         "nop\n\t");

//     // Pull WR HIGH
//     wrPin_high;

//     // Leave WR high for at least 50ns
//     wait(120);
//     // __asm__("nop\n\t"
//     //         "nop\n\t"
//     //         "nop\n\t"
//     //         "nop\n\t");

//     // Switch data pins to input
//     for (uint32_t i = 0; i < sizeof(DATA_GBA_PINS) / sizeof(DATA_GBA_PINS[0]); i++)
//     {
//         pinMode(DATA_GBA_PINS[i], INPUT);
//     }
//     // Enable pullups
//     // for (uint32_t i = 0; i < sizeof(DATA_GBA_PINS) / sizeof(DATA_GBA_PINS[0]); i++)
//     // {
//     //     pinMode(DATA_GBA_PINS[i], HIGH);
//     // }
// }

// Compute the checksum of rom header
// "header" must contain at least the rom's first 188 bytes
uint8_t GameBoyAdvanceCartridge::checksum_header_GBA(const uint8_t *header)
{
    uint8_t result = 0x00;
    for (uint8_t n = 0xA0; n < 0xBD; n++)
    {
        result -= header[n];
    }
    return result - 0x19;
}
void GameBoyAdvanceCartridge::headerROM_GBA(bool printInfo = true)
{
    transferJSON.clear();

    transferJSON["type"] = "info";
    transferJSON["gb_type"] = "gba";
    char saveTypeStr[14];

    // Read Header into array
    for (int currWord = 0; currWord < 192; currWord += 2)
    {
        word tempWord = this->read_word_GBA(currWord);

        sdBuffer[currWord] = tempWord & 0xFF;
        sdBuffer[currWord + 1] = (tempWord >> 8) & 0xFF;
    }
    // Compare Nintendo logo against known checksum, 156 bytes starting at 0x04
    word logoChecksum = 0;
    // JsonArray logoArray = transferJSON.createNestedArray("logo");
    for (int currByte = 0x4; currByte < 0xA0; currByte++)
    {
        logoChecksum += sdBuffer[currByte];
        // logoArray.add(sdBuffer[currByte]);
    }

    transferJSON["logo_correct"] = logoChecksum == 0x4B1B; 
    if (logoChecksum == 0x4B1B)
    {
        char tempStr2[2];
        char tempStr[5];

        // cart not in list
        cartSize = 0;
        saveType = 0;

        // Get cart ID
        cartID[0] = char(sdBuffer[0xAC]);
        cartID[1] = char(sdBuffer[0xAD]);
        cartID[2] = char(sdBuffer[0xAE]);
        cartID[3] = char(sdBuffer[0xAF]);

        //  TODO: Implement MemoryFile
        MemoryFile myFile(gba, sizeof(gba));

        if (myFile.open())
        {
            // Loop through file
            while (myFile.available())
            {
                // Skip first line with name
                skip_line(&myFile);

                // Skip over the CRC checksum
                myFile.seekCur(9);

                // Read 4 bytes into String, do it one at a time so byte order doesn't get mixed up
                sprintf(tempStr, "%c", myFile.read());
                for (byte i = 0; i < 3; i++)
                {
                    sprintf(tempStr2, "%c", myFile.read());
                    strcat(tempStr, tempStr2);
                }

                // Check if string is a match
                if (strcmp(tempStr, cartID) == 0)
                {
                    // Rewind to start of entry
                    rewind_line(myFile);

                    // Display database
                    // Read game name
                    get_line(this->gameTitle, &myFile, 96);

                    // Skip over the CRC checksum
                    myFile.seekCur(9);

                    // Read 4 bytes into String, do it one at a time so byte order doesn't get mixed up
                    sprintf(tempStr, "%c", myFile.read());
                    for (byte i = 0; i < 3; i++)
                    {
                        sprintf(tempStr2, "%c", myFile.read());
                        strcat(tempStr, tempStr2);
                    }

                    // Skip the , in the file
                    myFile.seekCur(1);

                    // Read the next ascii character and subtract 48 to convert to decimal
                    cartSize = myFile.read() - 48;
                    // Remove leading 0 for single digit cart sizes
                    if (cartSize != 0)
                    {
                        cartSize = cartSize * 10 + myFile.read() - 48;
                    }
                    else
                    {
                        cartSize = myFile.read() - 48;
                    }

                    // Skip the , in the file
                    myFile.seekCur(1);

                    // Read save type into string
                    get_line(saveTypeStr, &myFile, 14);

                    // skip third empty line
                    skip_line(&myFile);

                    this->romSize = cartSize;
                    
                    transferJSON["success"] = 1;
                    transferJSON["message"] = "";
                    transferJSON["title"] = this->gameTitle;
                    transferJSON["serial"] = tempStr;
                    transferJSON["RAMSize"] = "";
                    transferJSON["ROMSize"] = this->romSize;
                }
                // If no match advance and try again
                else
                {
                    // skip rest of line
                    skip_line(&myFile);
                    // skip third empty line
                    skip_line(&myFile);
                }
            }
            // Close the file:
            myFile.close();
        }

        buildRomName(this->gameTitle, &sdBuffer[0xA0], 12);
        transferJSON["rom_title"] = this->gameTitle;
        // Get ROM version
        romVersion = sdBuffer[0xBC];

        // Calculate Checksum
        byte calcChecksum = this->checksum_header_GBA(sdBuffer);

        // Convert checksum from header into string
        // (used in compare_checksum_GBA... it should just exchange an integer
        // instead)
        sprintf(checksumStr, "%02X", sdBuffer[0xBD]);

        // Compare checksum
        transferJSON["checksum"] = checksumStr;
        if (sdBuffer[0xBD] != calcChecksum)
        {
            char calcChecksumStr[3];
            // Turn into string
            sprintf(calcChecksumStr, "%02X", calcChecksum);
        }

        /* Convert saveTypeStr to saveType
        Save types in ROM
        EEPROM_Vnnn    EEPROM 512 bytes or 8 Kbytes (4Kbit or 64Kbit)
        SRAM_Vnnn      SRAM 32 Kbytes (256Kbit)
        SRAM_F_Vnnn    FRAM 32 Kbytes (256Kbit)
        FLASH_Vnnn     FLASH 64 Kbytes (512Kbit) (ID used in older files)
        FLASH512_Vnnn  FLASH 64 Kbytes (512Kbit) (ID used in newer files)
        FLASH1M_Vnnn   FLASH 128 Kbytes (1Mbit)

        Save types in Cart Reader Code
        0 = Unknown or no save
        1 = 4K EEPROM
        2 = 64K EEPROM
        3 = 256K SRAM
        4 = 512K FLASH
        5 = 1M FLASH
        6 = 512K SRAM
        */

        if (saveTypeStr[0] == 'N')
        {
            saveType = 0;
            transferJSON["RAMSize"] = "";
        }
        else if (saveTypeStr[0] == 'E')
        {
            // Test if 4kbit or 64kbit EEPROM

            // Disable interrupts for more uniform clock pulses
            // noInterrupts();
            // // Fill sd Buffer
            // // readBlock_EEP(0, 64);
            // interrupts();
            // delay(1000);
            // Enable ROM again
            // setROM_GBA();

            saveType = 1;
            this->sramSize = 4;
            transferJSON["sramSize"] = 4; // 4K
            transferJSON["RAMSize"] = "4K EEPROM";

            // // Reading 4kbit EEPROM as 64kbit just gives the same 8 bytes repeated
            // for (int currByte = 0; currByte < 512 - 8; currByte++) {
            //     if (sdBuffer[currByte] != sdBuffer[currByte + 8]) {
            //         saveType = 2;
            //         break;
            //     }
            // }
        }
        else if (saveTypeStr[0] == 'S')
        {
            saveType = 3;
            this->sramSize = 32768;
            transferJSON["sramSize"] = 32768; // 256K SRAM/FRAM
            transferJSON["RAMSize"] = "256K SRAM";
        }
        else if ((saveTypeStr[0] == 'F') && (saveTypeStr[5] == '1'))
        {
            saveType = 5;
            this->sramSize = 65536*2;
            transferJSON["sramSize"] = 65536*2; // 1M FLASH (divided into two banks)
            transferJSON["RAMSize"] = "1M FLASH";
        }
        else if (saveTypeStr[0] == 'F')
        {
            saveType = 4;
            this->sramSize = 65536;
            transferJSON["sramSize"] = 65536; // 512K FLASH
            transferJSON["RAMSize"] = "512K FLASH";
        }

        JsonArray logoArray = transferJSON.createNestedArray("logo");
        for (int currByte = 0x4; currByte < 0xA0; currByte++)
        {
            logoArray.add(sdBuffer[currByte]);
        }
    } else {
        transferJSON["message"] = "HEADER CHECKSUM ERROR";
        transferJSON["success"] = 0;
    }

    if (printInfo)
    {
        Serial.print("JSON:");
        serializeJson(transferJSON, Serial);
        Serial.println();
    }
}

void GameBoyAdvanceCartridge::setup()
{
    // disableCore0WDT();
    this->runGameBoyAdvanceCartridge = false;
    this->writtingRAM = false;
    this->writtingROM = false;
    this->restoringRAM = false;
    this->romAddress = 0;
    this->romSize = 0;
    this->sramSize = 0;
}
void GameBoyAdvanceCartridge::main()
{
    //  TODO: 
}