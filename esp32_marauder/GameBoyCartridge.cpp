#include "GameBoyCartridge.h"

GameBoyCartridge::GameBoyCartridge()
{
    this->runGameBoyCartridge = false;
    this->lastByte = 0;
}
void GameBoyCartridge::begin()
{
    this->setup();
}
void GameBoyCartridge::start()
{
    // Set Data Pins (D0-D7) to Input
    for (uint32_t i = 0; i < sizeof(ADDRESS_GB_GBC_PINS) / sizeof(ADDRESS_GB_GBC_PINS[0]); i++)
    {
        pinMode(ADDRESS_GB_GBC_PINS[i], OUTPUT);
    }

    // Set Control Pins to Output RST(PH0) CLK(PH1) CS(PH3) WR(PH5) RD(PH6)

    pinMode(GAMEBOY_RST, OUTPUT);
    pinMode(GAMEBOY_CLK, OUTPUT);
    pinMode(GAMEBOY_CS, OUTPUT);
    pinMode(GAMEBOY_WR, OUTPUT);
    pinMode(GAMEBOY_RD, OUTPUT);

    // Output a high signal on all pins, pins are active low therefore everything is disabled now
    digitalWrite(GAMEBOY_CS, HIGH);
    digitalWrite(GAMEBOY_WR, HIGH);
    digitalWrite(GAMEBOY_RD, HIGH);

    // Output a low signal on CLK(PH1) to disable writing GB Camera RAM
    // Output a low signal on RST(PH0) to initialize MMC correctly
    digitalWrite(GAMEBOY_CLK, LOW);
    digitalWrite(GAMEBOY_RST, LOW);

    // Set Data Pins (D0-D7) to Input
    for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
    {
        pinMode(DATA_GB_GBC_PINS[i], INPUT);
    }
    for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
    {
        pinMode(DATA_GB_GBC_PINS[i], HIGH);
    }
    delay(400);

    // RST to H
    digitalWrite(GAMEBOY_RST, HIGH);
    
    transferJSON.clear();
    this->lastByte = 0;
    this->runGameBoyCartridge = true;
}
void GameBoyCartridge::stop()
{
    this->runGameBoyCartridge = false;
}

void GameBoyCartridge::dataBusAsOutput()
{
    for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
    {
        pinMode(DATA_GB_GBC_PINS[i], OUTPUT);
    }
}
void GameBoyCartridge::dataBusAsInput()
{
    for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
    {
        pinMode(DATA_GB_GBC_PINS[i], INPUT);
    }
}
// Set Gameboy mode
void GameBoyCartridge::gb_mode(void)
{
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
// Set the 16 bit address
void GameBoyCartridge::set_address_GB(uint16_t address)
{
    // Write each of the bits into the address pins
    for (uint32_t i = 0; i < sizeof(ADDRESS_GB_GBC_PINS) / sizeof(ADDRESS_GB_GBC_PINS[0]); i++)
    {
        digitalWrite(ADDRESS_GB_GBC_PINS[i], address & (1 << i) ? HIGH : LOW);
    }
}
byte GameBoyCartridge::read_byte_GB(uint16_t address)
{
    this->set_address_GB(address);

    cs_mreqPin_low;
    rdPin_low;

    asm volatile("nop"); // Delay a little (minimum is 2 nops, using 3 to be sure)
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");

    byte data = 0; // Read data
    for (int i = 0; i < 8; i++)
    {
        int raw_data = digitalRead(DATA_GB_GBC_PINS[i]);
        data |= raw_data << i;
    }
    // Switch and RD to HIGH
    rdPin_high;
    cs_mreqPin_high;

    return data;
}
void GameBoyCartridge::headerROM_GB(bool printInfo = true)
{
    transferJSON.clear();
    this->lastByte = 0;

    transferJSON["type"] = "info";

    this->gb_mode();

    for (uint16_t address = 0; address < 0x0180; address++)
    {
        byte value = this->read_byte_GB(address);
        this->startRomBuffer[address] = value;
    }
    byte myLength = 0;
    // Blank out game title
    for (uint8_t b = 0; b < 16; b++)
    {
        this->gameTitle[b] = 0;
    }
    uint8_t cart_data = this->startRomBuffer[0x0143];
    if (cart_data == 0x80)
    {
        //  Cartridge with color function
        transferJSON["gb_type"] = "gbc";
    }
    else
    {
        //  Cartridge with no color function
        transferJSON["gb_type"] = "gb";
    }
    cart_data = this->startRomBuffer[0x0146];
    if (cart_data == 0x03)
    {
        //  Cartridge with SGB functions
        transferJSON["gb_sgb"] = true;
    }
    else
    {
        //  Cartridge without SGB functions
        transferJSON["gb_sgb"] = false;
    }

    // Read cartridge title and check for non-printable text
    for (uint16_t titleAddress = 0x0134; titleAddress <= 0x143; titleAddress++)
    {
        char headerChar = this->startRomBuffer[titleAddress];
        if ((headerChar >= 0x30 && headerChar <= 0x39) || // 0-9
            (headerChar >= 0x41 && headerChar <= 0x5A) || // A-Z
            (headerChar >= 0x61 && headerChar <= 0x7A) || // a-z
            (headerChar >= 0x24 && headerChar <= 0x29) || // #$%&'()
            (headerChar == 0x2D) ||                       // -
            (headerChar == 0x2E) ||                       // .
            (headerChar == 0x5F) ||                       // _
            (headerChar == 0x20))
        { // Space
            this->gameTitle[(titleAddress - 0x0134)] = headerChar;
            myLength++;
        }
        // Replace with an underscore
        else if (headerChar == 0x3A)
        {
            this->gameTitle[(titleAddress - 0x0134)] = '_';
            myLength++;
        }
        else
        {
            this->gameTitle[(titleAddress - 0x0134)] = '\0';
            break;
        }
    }
    // Find Game Serial
    cartID[0] = 0;
    if (this->startRomBuffer[0x143] == 0x80 || this->startRomBuffer[0x143] == 0xC0)
    {
        if ((this->gameTitle[myLength - 4] == 'A' || this->gameTitle[myLength - 4] == 'B' || this->gameTitle[myLength - 4] == 'H' || this->gameTitle[myLength - 4] == 'K' || this->gameTitle[myLength - 4] == 'V') && (this->gameTitle[myLength - 1] == 'A' || this->gameTitle[myLength - 1] == 'B' || this->gameTitle[myLength - 1] == 'D' || this->gameTitle[myLength - 1] == 'E' || this->gameTitle[myLength - 1] == 'F' || this->gameTitle[myLength - 1] == 'I' || this->gameTitle[myLength - 1] == 'J' || this->gameTitle[myLength - 1] == 'K' || this->gameTitle[myLength - 1] == 'P' || this->gameTitle[myLength - 1] == 'S' || this->gameTitle[myLength - 1] == 'U' || this->gameTitle[myLength - 1] == 'X' || this->gameTitle[myLength - 1] == 'Y'))
        {
            cartID[0] = this->gameTitle[myLength - 4];
            cartID[1] = this->gameTitle[myLength - 3];
            cartID[2] = this->gameTitle[myLength - 2];
            cartID[3] = this->gameTitle[myLength - 1];
            myLength -= 4;
            this->gameTitle[myLength] = 0;
        }
    }
    // Nintendo Logo Check
    uint8_t logoCheck = 1;
    uint8_t x = 0;
    uint8_t romLogo[48];

    JsonArray logoArray = transferJSON.createNestedArray("logo");
    for (uint16_t romAddress = 0x0104; romAddress <= 0x133; romAddress++)
    {
        if (nintendoLogo[x] != this->startRomBuffer[romAddress] && logoCheck == 1)
        {
            logoCheck = 0;
        }
        romLogo[x] = this->startRomBuffer[romAddress];
        logoArray.add(romLogo[x]);
        x++;
    }
    transferJSON["logo_correct"] = logoCheck;
    // Calculate header checksum
    byte headerChecksum = 0;
    for (int currByte = 0x134; currByte < 0x14D; currByte++)
    {
        headerChecksum = headerChecksum - this->startRomBuffer[currByte] - 1;
    }

    if (headerChecksum != this->startRomBuffer[0x14D])
    {
        // Read Header into array a second time
        for (int currByte = 0x100; currByte < 0x150; currByte++)
        {
            this->startRomBuffer[currByte] = this->read_byte_GB(currByte);
        }
        // Calculate header checksum a second time
        headerChecksum = 0;
        for (int currByte = 0x134; currByte < 0x14D; currByte++)
        {
            headerChecksum = headerChecksum - this->startRomBuffer[currByte] - 1;
        }
    }

    if (headerChecksum != this->startRomBuffer[0x14D])
    {
        transferJSON["message"] = "HEADER CHECKSUM ERROR";
    }

    cartridgeType = this->startRomBuffer[0x0147];
    romType = cartridgeType;
    romSize = this->startRomBuffer[0x0148];
    ramSize = this->startRomBuffer[0x0149];
    sramSize = this->startRomBuffer[0x149];

    // Get Checksum as string
    sprintf(checksumStr, "%02X%02X", this->startRomBuffer[0x14E], this->startRomBuffer[0x14F]);

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
    if (romType == 0x104)
    {
        romStartBank = 0;
        romBanks >>= 1;
        romEndAddress = 0x7FFF;
    }
    // MBC6 banks are half size
    else if (romType == 32)
    {
        romBanks <<= 1;
        romEndAddress = 0x3FFF;
    }
    if (strcmp(checksumStr, "00") != 0)
    {
        transferJSON["success"] = 1;
        transferJSON["message"] = "";
        transferJSON["checksum"] = checksumStr;
        transferJSON["title"] = this->gameTitle;
        transferJSON["ramBanks"] = sramBanks;
        transferJSON["ramEndAddress"] = ramEndAddress;
        transferJSON["romBanks"] = romBanks;
        transferJSON["romEndAddress"] = romEndAddress;
        if (cartID[0] != 0)
        {
            transferJSON["serial"] = cartID;
        }
        transferJSON["rev"] = romVersion;
        if ((romType == 0) || (romType == 8) || (romType == 9))
        {
            transferJSON["mapper"] = "None";
        }
        else if ((romType == 1) || (romType == 2) || (romType == 3))
        {
            transferJSON["mapper"] = "MBC1";
        }
        else if ((romType == 5) || (romType == 6))
        {
            transferJSON["mapper"] = "MBC2";
        }
        else if ((romType == 11) || (romType == 12) || (romType == 13))
        {
            transferJSON["mapper"] = "MMM01";
        }
        else if ((romType == 15) || (romType == 16) || (romType == 17) || (romType == 18) || (romType == 19))
        {
            transferJSON["mapper"] = "MBC3";
        }
        else if ((romType == 21) || (romType == 22) || (romType == 23))
        {
            transferJSON["mapper"] = "MBC4";
        }
        else if ((romType == 25) || (romType == 26) || (romType == 27) || (romType == 28) || (romType == 29) || (romType == 309))
        {
            transferJSON["mapper"] = "MBC5";
        }
        else if (romType == 32)
        {
            transferJSON["mapper"] = "MBC6";
        }
        else if (romType == 34)
        {
            transferJSON["mapper"] = "MBC7";
        }
        else if (romType == 252)
        {
            transferJSON["mapper"] = "Camera";
        }
        else if (romType == 253)
        {
            transferJSON["mapper"] = "TAMA5";
        }
        else if (romType == 254)
        {
            transferJSON["mapper"] = "HuC-3";
        }
        else if (romType == 255)
        {
            transferJSON["mapper"] = "HuC-1";
        }
        else if ((romType == 0x101) || (romType == 0x103))
        {
            transferJSON["mapper"] = "MBC1M";
        }
        else if (romType == 0x104)
        {
            transferJSON["mapper"] = "M161";
        }
        switch (romSize)
        {
        case 0:
            transferJSON["ROMSize"] = "32 KB";
            break;

        case 1:
            transferJSON["ROMSize"] = "64 KB";
            break;

        case 2:
            transferJSON["ROMSize"] = "128 KB";
            break;

        case 3:
            transferJSON["ROMSize"] = "256 KB";
            break;

        case 4:
            transferJSON["ROMSize"] = "512 KB";
            break;

        case 5:
            transferJSON["ROMSize"] = "1 MB";
            break;

        case 6:
            transferJSON["ROMSize"] = "2 MB";
            break;

        case 7:
            transferJSON["ROMSize"] = "4 MB";
            break;

        case 8:
            transferJSON["ROMSize"] = "8 MB";
            break;
        }

        switch (sramSize)
        {
        case 0:
            if (romType == 6)
            {
                transferJSON["RAMSize"] = "512 Byte";
            }
            else if (romType == 0x22)
            {
                transferJSON["RAMSize"] = String(this->lastByte) + " Byte";
            }
            else if (romType == 0xFD)
            {
                transferJSON["RAMSize"] = "32 Byte";
            }
            else
            {
                transferJSON["RAMSize"] = "None";
            }
            break;
        case 1:
            transferJSON["RAMSize"] = "2 KB";
            break;

        case 2:
            transferJSON["RAMSize"] = "8 KB";
            break;

        case 3:
            if (romType == 0x20)
            {
                transferJSON["RAMSize"] = "1.03 MB";
            }
            else
            {
                transferJSON["RAMSize"] = "32 KB";
            }
            break;

        case 4:
            transferJSON["RAMSize"] = "128 KB";
            break;

        case 5:
            transferJSON["RAMSize"] = "64 KB";
            break;

        default:
            transferJSON["RAMSize"] = "None";
        }
        delay(2);

        uint32_t crc32sum = 1;
        boolean renamerom = 0;
        int offset = 0;
        /*
        char crcStr[9];

        // Convert precalculated crc to string
        sprintf(crcStr, "%08lX", ~crc32sum);

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
    }
    if (printInfo)
    {
        Serial.print("JSON:");
        serializeJson(transferJSON, Serial);
        Serial.println();
    }
}
void GameBoyCartridge::write_byte_GB(int address, byte data)
{
    // Set address
    this->set_address_GB(address);
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
    __asm__("nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t");
    // Pull WR low
    wrPin_low;
    // Leave WR low for at least 60ns
    __asm__("nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t");

    // Pull WR HIGH
    wrPin_high;

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

// Turn RD, WR, CS/MREQ and CS2 to high so they are deselected (reset state)
void GameBoyCartridge::rd_wr_csmreq_cs2_reset(void)
{
    cs2Pin_high;     // CS2 off
    cs_mreqPin_high; // CS/MREQ off
    rdPin_high;      // RD off
    wrPin_high;      // WR off
}
// Turn RD, WR and MREQ to high so they are deselected (reset state)
void GameBoyCartridge::rd_wr_mreq_reset(void)
{
    rdPin_high;      // RD off
    wrPin_high;      // WR off
    cs_mreqPin_high; // MREQ off
}
// Turn RD, WR and MREQ off as no power should be applied to GB Cart
void GameBoyCartridge::rd_wr_mreq_off(void)
{
    rdPin_low;
    wrPin_low;
    cs_mreqPin_low; // CS
}

void GameBoyCartridge::readROM_GB()
{
    transferJSON.clear();
    word romAddress = 0;
    uint32_t processedProgressBar = 0;
    uint32_t totalProgressBar = (uint32_t)(romBanks) * 16384;

    transferJSON["type"] = "rom";
    transferJSON["total"] = totalProgressBar;
    transferJSON["progress"] = processedProgressBar * 100 / totalProgressBar;
    transferJSON["romBanks"] = romBanks;
    delay(200);
    Serial.print("JSON:");
    serializeJson(transferJSON, Serial);
    Serial.println();

    for (word currBank = romStartBank; currBank < romBanks; currBank++)
    {
        // Second bank starts at 0x4000
        if (currBank > 1)
        {
            romAddress = 0x4000;
            // MBC6 banks are half size
            if (romType == 32)
            {
                romEndAddress = 0x5FFF;
            }
        }

        // Set ROM bank for M161
        if (romType == 0x104)
        {
            romAddress = 0;
            // Set CS2 to LOW
            cs2Pin_low;
            delay(50);
            // Set CS2 to HIGH
            cs2Pin_high;
            this->write_byte_GB(0x4000, currBank & 0x7);
        }

        // Set ROM bank for MBC1M
        else if (romType == 0x101 || romType == 0x103)
        {
            if (currBank < 10)
            {
                this->write_byte_GB(0x4000, currBank >> 4);
                this->write_byte_GB(0x2000, (currBank & 0x1f));
            }
            else
            {
                this->write_byte_GB(0x4000, currBank >> 4);
                this->write_byte_GB(0x2000, 0x10 | (currBank & 0x1f));
            }
        }

        // Set ROM bank for MBC6
        else if (romType == 32)
        {
            this->write_byte_GB(0x2800, 0);
            this->write_byte_GB(0x3800, 0);
            this->write_byte_GB(0x2000, currBank);
            this->write_byte_GB(0x3000, currBank);
        }

        // Set ROM bank for TAMA5
        else if (romType == 0xFD)
        {
            // writeByteSRAM_GB(0xA001, 0);
            // writeByteSRAM_GB(0xA000, currBank & 0x0f);
            // writeByteSRAM_GB(0xA001, 1);
            // writeByteSRAM_GB(0xA000, (currBank >> 4) & 0x0f);
        }

        // Set ROM bank for MBC2/3/4/5
        else if (romType >= 5)
        {
            if (romType >= 11 && romType <= 13)
            {
                if ((currBank & 0x1f) == 0)
                {
                    // reset MMM01
                    cs2Pin_low;
                    delay(50);
                    cs2Pin_high;

                    // remap to higher 4Mbits ROM
                    this->write_byte_GB(0x3fff, 0x20);
                    this->write_byte_GB(0x5fff, 0x40);
                    this->write_byte_GB(0x7fff, 0x01);
                    this->write_byte_GB(0x1fff, 0x3a);
                    this->write_byte_GB(0x1fff, 0x7a);

                    // for every 4Mbits ROM, restart from 0x0000
                    romAddress = 0x0000;
                    currBank++;
                }
                else
                {
                    this->write_byte_GB(0x6000, 0);
                    this->write_byte_GB(0x2000, (currBank & 0x1f));
                }
            }
            else
            {
                if ((romType >= 0x19 && romType <= 0x1E) && (currBank == 0 || currBank == 256))
                {
                    this->write_byte_GB(0x3000, (currBank >> 8) & 0xFF);
                }
                this->write_byte_GB(0x2100, currBank & 0xFF);
            }
        }
        // Set ROM bank for MBC1
        else
        {
            this->write_byte_GB(0x6000, 0);
            this->write_byte_GB(0x4000, currBank >> 5);
            this->write_byte_GB(0x2000, currBank & 0x1F);
        }

        // Read banks and save to SD
        while (romAddress <= romEndAddress)
        {
            for (int i = 0; i < 512; i++)
            {
                sdBuffer[i] = this->read_byte_GB(romAddress + i);
            }
            Serial1.write(sdBuffer, 512);
            romAddress += 512;
            processedProgressBar += 512;
        }
    }
    transferJSON["type"] = "success";
    transferJSON["total"] = totalProgressBar;
    transferJSON["progress"] = processedProgressBar * 100 / totalProgressBar;
    transferJSON["romBanks"] = romBanks;
    delay(200);
    Serial.print("JSON:");
    serializeJson(transferJSON, Serial);
    Serial.println();
}

byte GameBoyCartridge::readByteSRAM_GB(uint16_t myAddress)
{
    this->set_address_GB(myAddress);

    // Switch data pins to input
    __asm__("nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t");
    // Pull CS, CLK (for FRAM MOD) LOW
    cs_mreqPin_low;

    //      CS  RD  WR
    //    0   0   1
    // Pull RD LOW
    rdPin_low;
    wrPin_high;
    phiPin_low;

    __asm__("nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t");
    // Read
    byte tempByte = 0; // Read data
    for (int i = 0; i < 8; i++)
    {
        int raw_data = digitalRead(DATA_GB_GBC_PINS[i]);
        // Serial.print(raw_data);
        tempByte |= raw_data << i;
    }

    //      CS  RD  WR
    //    1   1   1
    // Pull RD HIGH
    rdPin_high;
    if (romType == 252)
    {
        // Pull CS HIGH
        cs_mreqPin_high;
    }
    else
    {
        // Pull CS, CLK (for FRAM MOD) HIGH
        cs_mreqPin_high;
        phiPin_high;
    }
    wrPin_high;
    __asm__("nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t");
    return tempByte;
}
void GameBoyCartridge::readSRAM_GB()
{
    transferJSON.clear();
    // Initialize progress bar
    uint32_t processedProgressBar = 0;
    uint32_t totalProgressBar = (uint32_t)(sramBanks) * 8192;

    transferJSON["type"] = "ram";
    transferJSON["total"] = totalProgressBar;
    transferJSON["progress"] = processedProgressBar * 100 / totalProgressBar;
    transferJSON["ramBanks"] = sramBanks;
    transferJSON["lastByte"] = ramEndAddress;

    delay(200);
    Serial.print("JSON:");
    serializeJson(transferJSON, Serial);
    Serial.println();

    this->rd_wr_mreq_reset();

    // MBC2 Fix (unknown why this fixes reading the ram, maybe has to read ROM before RAM?)
    this->read_byte_GB(0x0134);

    // if cartridge have RAM test
    if (ramEndAddress > 0)
    {
        if (cartridgeType <= 4)
        {                                  // MBC1
            this->write_byte_GB(0x6000, 1); // Set RAM Mode
        }

        this->dataBusAsOutput();
        // Initialise MBC
        this->write_byte_GB(0x0000, 0x0A);
        delayMicroseconds(50);
        this->dataBusAsInput();

        // Switch RAM banks
        for (uint8_t bank = 0; bank < sramBanks; bank++)
        {
            this->write_byte_GB(0x4000, bank);

            // Read RAM
            for (word ramAddress = 0xA000; ramAddress <= ramEndAddress; ramAddress += 64)
            {
                uint8_t readData[64];
                for (uint8_t i = 0; i < 64; i++)
                {
                    readData[i] = this->read_byte_GB(ramAddress + i);
                }
                Serial1.write(readData, 64); // Send the 64 byte chunk
                processedProgressBar += 64;
            }
        }

        // Disable RAM
        this->write_byte_GB(0x0000, 0x00);
        delay(50);

        transferJSON["type"] = "success";
        transferJSON["total"] = totalProgressBar;
        transferJSON["progress"] = processedProgressBar * 100 / totalProgressBar;
        transferJSON["ramBanks"] = sramBanks;

        delay(200);
        Serial.print("JSON:");
        serializeJson(transferJSON, Serial);
        Serial.println();
    }

    this->rd_wr_mreq_off();
}

void GameBoyCartridge::setup()
{
    
}
void GameBoyCartridge::main()
{
}