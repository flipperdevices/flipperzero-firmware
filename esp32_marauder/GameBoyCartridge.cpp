#include "GameBoyCartridge.h"
#include "save.h"
#include "soc/rtc_wdt.h"


GameBoyCartridge::GameBoyCartridge()
{
    this->runGameBoyCartridge = false;
    this->writtingRAM = false;
    this->writtingROM = false;
    this->restoringRAM = false;
    this->lastByte = 0;
    this->cartridgeType = 0;
    this->romSize = 0;
    this->romBanks = 0;
    this->ramSize = 0;
    this->ramBanks = 0;
    this->sramSize = 0;
    this->romEndAddress = 0x7FFF;
    this->sramBanks = 0;
    this->romType = 0;
    this->currentBank = 0;
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
    // for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
    // {
    //     pinMode(DATA_GB_GBC_PINS[i], HIGH);
    // }
    delay(400);

    // RST to H
    digitalWrite(GAMEBOY_RST, HIGH);

    transferJSON.clear();
    this->lastByte = 0;
    // #ifdef WRITE_PACKETS_SERIAL
    // buffer_obj.open();
    // #endif
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
    // this->dataBusAsOutput();
    // Write each of the bits into the address pins
    for (uint32_t i = 0; i < sizeof(ADDRESS_GB_GBC_PINS) / sizeof(ADDRESS_GB_GBC_PINS[0]); i++)
    {
        digitalWrite(ADDRESS_GB_GBC_PINS[i], address & (1 << i) ? HIGH : LOW);
    }
}
byte GameBoyCartridge::read_byte_GB(uint16_t address)
{
    this->set_address_GB(address);

    __asm__("nop\n\t"
          "nop\n\t"
          "nop\n\t"
          "nop\n\t");
          
    // cs_mreqPin_low;
    rdPin_low;

    __asm__("nop\n\t"
          "nop\n\t"
          "nop\n\t"
          "nop\n\t");
    // asm volatile("nop");
    // asm volatile("nop");

    byte data = 0; // Read data
    for (int i = 0; i < 8; i++)
    {
        int raw_data = digitalRead(DATA_GB_GBC_PINS[i]);
        data |= raw_data << i;
        // asm volatile("nop"); // Delay a little (minimum is 2 nops, using 3 to be sure)
        // asm volatile("nop");
        // asm volatile("nop");
        // asm volatile("nop");
    }
    // Switch and RD to HIGH
    rdPin_high;
    // cs_mreqPin_high;
    __asm__("nop\n\t"
          "nop\n\t"
          "nop\n\t"
          "nop\n\t");
    // asm volatile("nop"); // Delay a little (minimum is 2 nops, using 3 to be sure)
    // asm volatile("nop");
    // asm volatile("nop");
    // asm volatile("nop");
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

    this->cartridgeType = this->startRomBuffer[0x0147];
    this->romType = this->cartridgeType;
    this->romSize = this->startRomBuffer[0x0148];
    this->ramSize = this->startRomBuffer[0x0149];
    this->sramSize = this->startRomBuffer[0x149];

    // Get Checksum as string
    sprintf(checksumStr, "%02X%02X", this->startRomBuffer[0x14E], this->startRomBuffer[0x14F]);

    // ROM banks
    switch (this->romSize)
    {
    case 0x00:
        this->romBanks = 2;
        break;
    case 0x01:
        this->romBanks = 4;
        break;
    case 0x02:
        this->romBanks = 8;
        break;
    case 0x03:
        this->romBanks = 16;
        break;
    case 0x04:
        this->romBanks = 32;
        break;
    case 0x05:
        this->romBanks = 64;
        break;
    case 0x06:
        this->romBanks = 128;
        break;
    case 0x07:
        this->romBanks = 256;
        break;
    case 0x08:
        this->romBanks = 512;
        break;
    default:
        this->romBanks = 2;
    }

    // SRAM banks
    this->sramBanks = 0;
    if (this->romType == 6)
    {
        this->sramBanks = 1;
    }

    // SRAM size
    switch (this->sramSize)
    {
    case 2:
        this->sramBanks = 1;
        break;
    case 3:
        this->sramBanks = 4;
        break;
    case 4:
        this->sramBanks = 16;
        break;
    case 5:
        this->sramBanks = 8;
        break;
    }

    // RAM end address
    if (this->cartridgeType == 6)
    {
        this->ramEndAddress = 0xA1FF;
    } // MBC2 512 bytes (nibbles)
    if (this->ramSize == 1)
    {
        this->ramEndAddress = 0xA7FF;
    } // 2K RAM
    if (this->ramSize > 1)
    {
        this->ramEndAddress = 0xBFFF;
    } // 8K RAM

    // M161 banks are double size and start with 0
    if (this->romType == 0x104)
    {
        this->romStartBank = 0;
        this->romBanks >>= 1;
        this->romEndAddress = 0x7FFF;
    }
    // MBC6 banks are half size
    else if (this->romType == 32)
    {
        this->romBanks <<= 1;
        this->romEndAddress = 0x3FFF;
    }
    if (strcmp(checksumStr, "00") != 0)
    {
        transferJSON["success"] = 1;
        transferJSON["message"] = "";
        transferJSON["checksum"] = checksumStr;
        transferJSON["title"] = this->gameTitle;
        transferJSON["ramBanks"] = this->sramBanks;
        transferJSON["ramEndAddress"] = this->ramEndAddress;
        transferJSON["romBanks"] = this->romBanks;
        transferJSON["romEndAddress"] = this->romEndAddress;
        if (cartID[0] != 0)
        {
            transferJSON["serial"] = cartID;
        }
        transferJSON["rev"] = romVersion;
        if ((this->romType == 0) || (this->romType == 8) || (this->romType == 9))
        {
            transferJSON["mapper"] = "None";
        }
        else if ((this->romType == 1) || (this->romType == 2) || (this->romType == 3))
        {
            transferJSON["mapper"] = "MBC1";
        }
        else if ((this->romType == 5) || (this->romType == 6))
        {
            transferJSON["mapper"] = "MBC2";
        }
        else if ((this->romType == 11) || (this->romType == 12) || (this->romType == 13))
        {
            transferJSON["mapper"] = "MMM01";
        }
        else if ((this->romType == 15) || (this->romType == 16) || (this->romType == 17) || (this->romType == 18) || (this->romType == 19))
        {
            transferJSON["mapper"] = "MBC3";
        }
        else if ((this->romType == 21) || (this->romType == 22) || (this->romType == 23))
        {
            transferJSON["mapper"] = "MBC4";
        }
        else if ((this->romType == 25) || (this->romType == 26) || (this->romType == 27) || (this->romType == 28) || (this->romType == 29) || (this->romType == 309))
        {
            transferJSON["mapper"] = "MBC5";
        }
        else if (this->romType == 32)
        {
            transferJSON["mapper"] = "MBC6";
        }
        else if (this->romType == 34)
        {
            transferJSON["mapper"] = "MBC7";
        }
        else if (this->romType == 252)
        {
            transferJSON["mapper"] = "Camera";
        }
        else if (this->romType == 253)
        {
            transferJSON["mapper"] = "TAMA5";
        }
        else if (this->romType == 254)
        {
            transferJSON["mapper"] = "HuC-3";
        }
        else if (this->romType == 255)
        {
            transferJSON["mapper"] = "HuC-1";
        }
        else if ((this->romType == 0x101) || (this->romType == 0x103))
        {
            transferJSON["mapper"] = "MBC1M";
        }
        else if (this->romType == 0x104)
        {
            transferJSON["mapper"] = "M161";
        }
        switch (this->romSize)
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

        switch (this->sramSize)
        {
        case 0:
            if (this->romType == 6)
            {
                transferJSON["RAMSize"] = "512 Byte";
            }
            else if (this->romType == 0x22)
            {
                transferJSON["RAMSize"] = String(this->lastByte) + " Byte";
            }
            else if (this->romType == 0xFD)
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
            if (this->romType == 0x20)
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
    wait(120);
    // __asm__("nop\n\t"
    //         "nop\n\t"
    //         "nop\n\t"
    //         "nop\n\t");
    // Pull WR low
    wrPin_low;
    // Leave WR low for at least 60ns
    wait(120);
    // __asm__("nop\n\t"
    //         "nop\n\t"
    //         "nop\n\t"
    //         "nop\n\t");

    // Pull WR HIGH
    wrPin_high;

    // Leave WR high for at least 50ns
    wait(120);
    // __asm__("nop\n\t"
    //         "nop\n\t"
    //         "nop\n\t"
    //         "nop\n\t");

    // Switch data pins to input
    for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
    {
        pinMode(DATA_GB_GBC_PINS[i], INPUT);
    }
    // Enable pullups
    // for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
    // {
    //     pinMode(DATA_GB_GBC_PINS[i], HIGH);
    // }
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
    this->romEndAddress = 0x7FFF;
    this->romAddress = 0;
    this->romStartBank = 0;
    this->processedProgressBar = 0;
    
    // M161 banks are double size and start with 0
    // if (this->romType == 0x104) {
    //     this->romStartBank = 0;
    //     this->romBanks >>= 1;
    //     this->romEndAddress = 0x7FFF;
    // }
    // // MBC6 banks are half size
    // else if (this->romType == 32) {
    //     this->romBanks <<= 1;
    //     this->romEndAddress = 0x3FFF;
    // }


    this->totalProgressBar = (uint32_t)(this->romBanks) * 16384;

    // transferJSON["type"] = "rom";
    // transferJSON["total"] = this->totalProgressBar ;
    // transferJSON["progress"] = this->processedProgressBar * 100 / this->totalProgressBar ;
    // transferJSON["romBanks"] = this->romBanks;
    // delay(200);
    // Serial.print("JSON:");
    // serializeJson(transferJSON, Serial);
    // Serial.println();


    this->currentBank = this->romStartBank;

    // this->writtingROM = true;
    delay(400);
    /*
    for (byte bank = 0; bank < romBanks; bank++)
    {
        // DumpROMBank(i);
        word offset = 0;
  
        if (bank > 0)
        {
            offset = 0x4000;  
            // SwitchROMBank(bank);
            this->dataBusAsOutput();
  
            switch (this->romType)
            {
                case 0xFF:
                {
                    this->write_byte_GB(0x2100, bank);
                    break;
                }
                case 0x06:
                case 0xFC:
                {
                    this->write_byte_GB(0x2100, bank);
                    break;
                }
                case 0x1E:
                {
                    this->write_byte_GB(0x2100, bank);
                    break;
                }
            }
            this->dataBusAsInput();
        }
        
        for (word address = 0; address < 0x4000; address++)
        {
            byte data = this->read_byte_GB(address + offset);
            Serial.write(data);
        }
        
    }
    */
    
   
    word romAddress = 0;
    word romEndAddress;
    uint32_t processedProgressBar = 0;
  uint32_t totalProgressBar = (uint32_t)(this->romBanks)*16384;
    for (word currBank = romStartBank; currBank < romBanks; currBank++) {
    // Second bank starts at 0x4000
    if (currBank > 1) {
      romAddress = 0x4000;

      // MBC6 banks are half size
      if (this->romType == 32) {
        romEndAddress = 0x5FFF;
      }
    }

    // Set ROM bank for M161
    if (this->romType == 0x104) {
      romAddress = 0;
      // Set CS2(PH0) to LOW
      // PORTH &= ~(1 << 0);
      cs2Pin_low;
      delay(50);
      // Set CS2(PH0) to HIGH
      // PORTH |= (1 << 0);
      cs2Pin_high;
      this->write_byte_GB(0x4000, currBank & 0x7);
    }

    // Set ROM bank for MBC1M
    else if (this->romType == 0x101 || this->romType == 0x103) {
      if (currBank < 10) {
        this->write_byte_GB(0x4000, currBank >> 4);
        this->write_byte_GB(0x2000, (currBank & 0x1f));
      } else {
        this->write_byte_GB(0x4000, currBank >> 4);
        this->write_byte_GB(0x2000, 0x10 | (currBank & 0x1f));
      }
    }

    // Set ROM bank for MBC6
    else if (this->romType == 32) {
      this->write_byte_GB(0x2800, 0);
      this->write_byte_GB(0x3800, 0);
      this->write_byte_GB(0x2000, currBank);
      this->write_byte_GB(0x3000, currBank);
    }

    // Set ROM bank for TAMA5
    else if (this->romType == 0xFD) {
      // writeByteSRAM_GB(0xA001, 0);
      // writeByteSRAM_GB(0xA000, currBank & 0x0f);
      // writeByteSRAM_GB(0xA001, 1);
      // writeByteSRAM_GB(0xA000, (currBank >> 4) & 0x0f);
    }

    // Set ROM bank for MBC2/3/4/5
    else if (this->romType >= 5) {
      if (this->romType >= 11 && this->romType <= 13) {
        if ((currBank & 0x1f) == 0) {
          // reset MMM01
          // PORTH &= ~(1 << 0);
          cs2Pin_low;
          delay(50);
          // PORTH |= (1 << 0);
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
        } else {
          this->write_byte_GB(0x6000, 0);
          this->write_byte_GB(0x2000, (currBank & 0x1f));
        }
      } else {
        if ((this->romType >= 0x19 && this->romType <= 0x1E) && (currBank == 0 || currBank == 256)) {
          this->write_byte_GB(0x3000, (currBank >> 8) & 0xFF);
        }
        this->write_byte_GB(0x2100, currBank & 0xFF);
      }
    }
    // Set ROM bank for MBC1
    else {
      this->write_byte_GB(0x6000, 0);
      this->write_byte_GB(0x4000, currBank >> 5);
      this->write_byte_GB(0x2000, currBank & 0x1F);
    }
    
    // Read banks and save to SD
    while (romAddress <= romEndAddress) {
      for (int i = 0; i < 512; i++) {
        char hexbuffer[3]; // Se reserva espacio para almacenar el valor hexadecimal y el carácter nulo
        sdBuffer[i] = this->read_byte_GB(romAddress + i);
        
      }
      Serial.write(sdBuffer, 512);
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
    if (this->romType == 252)
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
    this->processedProgressBar = 0;
    this->totalProgressBar  = (uint32_t)(this->sramBanks) * 8192;

    transferJSON["type"] = "ram";
    transferJSON["total"] = this->totalProgressBar;
    transferJSON["progress"] = this->processedProgressBar * 100 / this->totalProgressBar;
    transferJSON["ramBanks"] = this->sramBanks;
    transferJSON["lastByte"] = this->ramEndAddress;
    
    delay(200);
    Serial.print("JSON:");
    serializeJson(transferJSON, Serial);
    Serial.println();
    
    this->rd_wr_mreq_reset();

    // MBC2 Fix (unknown why this fixes reading the ram, maybe has to read ROM before RAM?)
    this->read_byte_GB(0x0134);
    if(this->ramEndAddress > 0) {
        if (this->cartridgeType <= 4)
        {                                   // MBC1
            this->write_byte_GB(0x6000, 1); // Set RAM Mode
        }
        this->dataBusAsOutput();
        // Initialise MBC
        this->write_byte_GB(0x0000, 0x0A);
        delayMicroseconds(50);
        this->dataBusAsInput();

        // this->write_byte_GB(0x4000, 0);
        this->writtingRAM = true;
    }
}
void GameBoyCartridge::ramEnable() {
    this->dataBusAsOutput();
    this->write_byte_GB(0x0000, 0x0A);
    delayMicroseconds(10);
    this->dataBusAsInput();
}
void GameBoyCartridge::ramDisable() {
    this->dataBusAsOutput();
    // Disable SRAM
    this->write_byte_GB(0x0000, 0x00);
    delay(50);
    this->dataBusAsInput();
}
// Receive Serial data
uint8_t GameBoyCartridge::serial_receive() {
    while (Serial.available() <= 0);
	return Serial.read(); // Get and return received data from buffer
}
// Transmit Serial data
void GameBoyCartridge::serial_transmit(uint8_t data) {
  while (!(Serial.availableForWrite() > 0)); // Wait for the serial buffer to have space
  Serial.write(data);
}
// Read 1-128 bytes from the Serial 
void GameBoyCartridge::serial_read_bytes(uint8_t count) {
	for (uint8_t x = 0; x < count; x++) {
		this->receivedBuffer[x] = this->serial_receive();
	}
}
// Read from Serial until a 0 (string terminator byte) is received
void GameBoyCartridge::serial_read_chars() {
  uint8_t x = 0;
  while (1) {
    char receivedChar = Serial1.read();
    if (receivedChar == 0 || x >= sizeof(receivedBuffer) - 1) {
      break;
    }
    this->receivedBuffer[x] = receivedChar;
    x++;
  }
  this->receivedBuffer[x] = 0; // Null-terminate the string
}

// Triggers CS and CLK pin
void GameBoyCartridge::writeByteSRAM_GB(uint16_t address, uint8_t data) {
  // Set address

    this->set_address_GB(address);
                
    for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
    {
        digitalWrite(DATA_GB_GBC_PINS[i], data & (1 << i) ? HIGH : LOW);
    }

    //  CS RD WR (010)

    delayMicroseconds(10);
    wrPin_low;
    rdPin_high;
    cs_mreqPin_low;

    delayMicroseconds(10);

    //  CS RD WR (111)
    wrPin_high;
    rdPin_high;
    cs_mreqPin_high;
    delayMicroseconds(10);
}

void GameBoyCartridge::test(uint16_t maxBufferSize) {

    uint8_t sav_file[maxBufferSize];
    memset(sav_file, 0, maxBufferSize);  // Inicializar el arreglo con ceros
    size_t currentIndex = 0;
    uint16_t counter = 0;
    int bytesRead = 0;

    // while(Serial.available() <= 0);
    Serial.flush();
    String input = "";
    while(true) {
        if (Serial.available() > 0) {
            sav_file[bytesRead] = Serial.read();
            bytesRead++;

            // Si has leído la cantidad de datos esperada
            if (bytesRead == maxBufferSize) {
                // Has guardado todos los datos en sav_file
                break;
            }
        }
            
        // if(Serial.available() > 0) {
            // for (uint8_t x = 0; x < 64; x++) {
                // this->receivedBuffer[counter + x] = Serial.read();
                // sav_file[counter] = this->serial_receive();
                // sav_file[counter] = (uint8_t)Serial.read();
                // currentIndex++; 
            // }

            
            counter++;

        if (counter >= maxBufferSize-1) {
            break;
        }
    }
    transferJSON.clear();
    transferJSON["type"] = "success";
    Serial.print("JSON:");
    serializeJson(transferJSON, Serial);
    Serial.println();

    WiFi.mode(WIFI_AP);
    gbStartAP("Tester", "12345678");
    _server.on("/image", HTTP_GET,  [maxBufferSize, &sav_file](AsyncWebServerRequest *request){
        AsyncResponseStream *response = request->beginResponseStream("text/html");
        for (size_t i = 0; i < maxBufferSize; i++) {
            if (i % 16 == 0) {
                response->print("\n"); // Add a vertical bar to separate each line
            }
            response->print(" ");
            response->printf("%02X", sav_file[i]);
        }
        request->send(response);
    });
    _server.begin();
}
void GameBoyCartridge::restoreRAM(size_t maxBufferSize) {
    
    pinMode(GAMEBOY_RST, OUTPUT);
    pinMode(GAMEBOY_CLK, OUTPUT);
    pinMode(GAMEBOY_WR, OUTPUT);
    pinMode(GAMEBOY_RD, OUTPUT);
    pinMode(GAMEBOY_CS, OUTPUT);

    // Set Control Pins to Output RST(PH0) CLK(PH1) CS(PH3) WR(PH5) RD(PH6)
    for (uint32_t i = 0; i < sizeof(ADDRESS_GB_GBC_PINS)/sizeof(ADDRESS_GB_GBC_PINS[0]); i++)
    {
        pinMode(ADDRESS_GB_GBC_PINS[i], OUTPUT);
    }
    // Set Data Pins (D0-D7) to Input
    for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS)/sizeof(DATA_GB_GBC_PINS[0]); i++)
    {
        pinMode(DATA_GB_GBC_PINS[i], INPUT);
    }

    digitalWrite(GAMEBOY_CLK, LOW);
    digitalWrite(GAMEBOY_RST, LOW);
    digitalWrite(GAMEBOY_WR, HIGH);
    digitalWrite(GAMEBOY_RD, HIGH);
    digitalWrite(GAMEBOY_CS, HIGH);

    delay(400);
    digitalWrite(GAMEBOY_RST, HIGH);

    if(this->ramEndAddress > 0) {

        // this->gb_mode();
        // this->rd_wr_mreq_reset();
        this->totalRamBytes = (this->sramBanks) * 8192;
        this->currentAddress = 0xA000;

        // MBC2 Fix
        this->read_byte_GB(0x0134);
        
        // Enable SRAM for MBC1
        if (this->romType <= 4 || (this->romType >= 11 && this->romType <= 13)) {
            this->write_byte_GB(0x6000, 1); // Set RAM Mode
        }
        // Initialise MBC
        //  EnableRAM
        this->ramEnable();
        this->currentBank = 0;


        // Switch RAM banks
        int x = 0;
        for (uint8_t currBank = 0; currBank < this->sramBanks; currBank++) {
            this->dataBusAsOutput();
            this->write_byte_GB(0x4000, currBank);
            this->dataBusAsInput();
            // Write RAM
            this->dataBusAsOutput();
            
            for (word sramAddress = 0xA000; sramAddress <= this->ramEndAddress; sramAddress++)
            {   
                /*
                while(Serial.available() <= 0);
                // if (Serial.available() > 0) {
                for (uint8_t x = 0; x < 64; x++) {
                    // this->receivedBuffer[x] = Serial.read();
                    this->writeByteSRAM_GB(sramAddress + x,  this->receivedBuffer[x]);
                    // sav_file[counter + x] = Serial.read();
                    // currentIndex++; 
                }
                // }
                */
               this->writeByteSRAM_GB(sramAddress,  save_gb[x]);
               x++;
            }

            this->dataBusAsInput();
        }

        this->ramDisable();
        // free(sav_file);
        transferJSON.clear();
        transferJSON["type"] = "success";
        Serial.print("JSON:");
        serializeJson(transferJSON, Serial);
        Serial.println();

    }
    /*
    uint8_t sav_file[maxBufferSize];
    // memset(sav_file, 0, maxBufferSize);  // Inicializar el arreglo con ceros
    size_t currentIndex = 0;
    uint16_t counter = 0;
    // transferJSON.clear();
    // transferJSON["type"] = "ack";

    // transferJSON["str"] = String(maxBufferSize);

    // Serial.print("JSON:");
    // serializeJson(transferJSON, Serial);
    // Serial.println();
    while (1) {
        if (Serial.available()) {
            for (uint8_t x = 0; x < 64; x++) {
                // this->receivedBuffer[counter + x] = Serial.read();
                sav_file[counter + x] = Serial.read();
                currentIndex++; 
            }
            counter += 64;
            
            // transferJSON.clear();
            // transferJSON["type"] = "ack";
            // String hexString = "";
            // char hexCar[3];
            // snprintf(hexCar, sizeof(hexCar), "%02X", this->receivedBuffer[0]); // 00
            // hexString += hexCar;
            // snprintf(hexCar, sizeof(hexCar), "%02X", this->receivedBuffer[1]); // 00
            // hexString += hexCar;    
            // snprintf(hexCar, sizeof(hexCar), "%02X", this->receivedBuffer[58]); // E1
            // hexString += hexCar;
            // snprintf(hexCar, sizeof(hexCar), "%02X", this->receivedBuffer[59]); // 00
            // hexString += hexCar;
            // snprintf(hexCar, sizeof(hexCar), "%02X", this->receivedBuffer[60]); // 38
            // hexString += hexCar;
            // snprintf(hexCar, sizeof(hexCar), "%02X", this->receivedBuffer[61]); // 05
            // hexString += hexCar;
            // snprintf(hexCar, sizeof(hexCar), "%02X", this->receivedBuffer[62]); // 08
            // hexString += hexCar;
            // snprintf(hexCar, sizeof(hexCar), "%02X", this->receivedBuffer[63]); // 00
            // hexString += hexCar;

            // transferJSON["str"] = hexString;

            // Serial.print("JSON:");
            // serializeJson(transferJSON, Serial);
            // Serial.println();
            
            delayMicroseconds(10);
            transferJSON.clear();
            transferJSON["type"] = "ack";
            String hexString = "";
            char hexCar[3];
            snprintf(hexCar, sizeof(hexCar), "%02X", this->receivedBuffer[0]); // 00
            hexString += hexCar;
            snprintf(hexCar, sizeof(hexCar), "%02X", this->receivedBuffer[1]); // 00
            hexString += hexCar;    
            snprintf(hexCar, sizeof(hexCar), "%02X", this->receivedBuffer[58]); // E1
            hexString += hexCar;
            snprintf(hexCar, sizeof(hexCar), "%02X", this->receivedBuffer[59]); // 00
            hexString += hexCar;
            snprintf(hexCar, sizeof(hexCar), "%02X", this->receivedBuffer[60]); // 38
            hexString += hexCar;
            snprintf(hexCar, sizeof(hexCar), "%02X", this->receivedBuffer[61]); // 05
            hexString += hexCar;
            snprintf(hexCar, sizeof(hexCar), "%02X", this->receivedBuffer[62]); // 08
            hexString += hexCar;
            snprintf(hexCar, sizeof(hexCar), "%02X", this->receivedBuffer[63]); // 00
            hexString += hexCar;
            transferJSON["str"] = hexString;

            Serial.print("JSON:");
            serializeJson(transferJSON, Serial);
            Serial.println();
        }
        if (counter >= maxBufferSize-1) {
            break;
        }
    }

    transferJSON.clear();
    transferJSON["type"] = "success";
    
    // String hexString = "";
    // char hexCar[3];
    // snprintf(hexCar, sizeof(hexCar), "%02X", sav_file[58]); // E1
    // hexString += hexCar;
    // snprintf(hexCar, sizeof(hexCar), "%02X", sav_file[59]); // 00
    // hexString += hexCar;
    // snprintf(hexCar, sizeof(hexCar), "%02X", sav_file[60]); // 38
    // hexString += hexCar;
    // snprintf(hexCar, sizeof(hexCar), "%02X", sav_file[61]); // 05
    // hexString += hexCar;
    // snprintf(hexCar, sizeof(hexCar), "%02X", sav_file[62]); // 08
    // hexString += hexCar;
    // snprintf(hexCar, sizeof(hexCar), "%02X", sav_file[63]); // 00
    // hexString += hexCar;
    // snprintf(hexCar, sizeof(hexCar), "%02X", sav_file[64]); // 00
    // hexString += hexCar;

    // snprintf(hexCar, sizeof(hexCar), "%02X", sav_file[17257]); // 83
    // hexString += hexCar;


    // snprintf(hexCar, sizeof(hexCar), "%02X", sav_file[26048]); // C8
    // hexString += hexCar;


    // snprintf(hexCar, sizeof(hexCar), "%02X", sav_file[30670]); // FF
    // hexString += hexCar;

    // transferJSON["str"] = hexString;
   transferJSON["str"] = "end";
    Serial.print("JSON:");
    serializeJson(transferJSON, Serial);
    Serial.println();
    /*
    // Serial1.flush();
    pinMode(GAMEBOY_RST, OUTPUT);
    pinMode(GAMEBOY_CLK, OUTPUT);
    pinMode(GAMEBOY_WR, OUTPUT);
    pinMode(GAMEBOY_RD, OUTPUT);
    pinMode(GAMEBOY_CS, OUTPUT);

    // Set Control Pins to Output RST(PH0) CLK(PH1) CS(PH3) WR(PH5) RD(PH6)
    for (uint32_t i = 0; i < sizeof(ADDRESS_GB_GBC_PINS)/sizeof(ADDRESS_GB_GBC_PINS[0]); i++)
    {
        pinMode(ADDRESS_GB_GBC_PINS[i], OUTPUT);
    }
    // Set Data Pins (D0-D7) to Input
    for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS)/sizeof(DATA_GB_GBC_PINS[0]); i++)
    {
        pinMode(DATA_GB_GBC_PINS[i], INPUT);
    }

    digitalWrite(GAMEBOY_CLK, LOW);
    digitalWrite(GAMEBOY_RST, LOW);
    digitalWrite(GAMEBOY_WR, HIGH);
    digitalWrite(GAMEBOY_RD, HIGH);
    digitalWrite(GAMEBOY_CS, HIGH);

    delay(100);
    digitalWrite(GAMEBOY_RST, HIGH);


    // this->ramEndAddress = 0xC000;
    if(this->ramEndAddress > 0) {

        // this->gb_mode();
        // this->rd_wr_mreq_reset();
        this->totalRamBytes = (this->sramBanks) * 8192;
        this->currentAddress = 0xA000;

        // MBC2 Fix
        this->read_byte_GB(0x0134);
        
        // Enable SRAM for MBC1
        if (this->romType <= 4 || (this->romType >= 11 && this->romType <= 13)) {
            this->write_byte_GB(0x6000, 1); // Set RAM Mode
        }
        // Initialise MBC
        //  EnableRAM
        this->ramEnable();
        this->currentBank = 0;


        // Switch RAM banks
        uint8_t x = 0;
        for (uint8_t currBank = 0; currBank < this->sramBanks; currBank++) {
            this->dataBusAsOutput();
            this->write_byte_GB(0x4000, currBank);
            this->dataBusAsInput();
            // Write RAM
            this->dataBusAsOutput();
            
            for (word sramAddress = 0xA000; sramAddress <= this->ramEndAddress; sramAddress++)
            {
                this->writeByteSRAM_GB(sramAddress,  this->receivedBuffer[x]);
                x++;
            }

            this->dataBusAsInput();
        }

        this->ramDisable();
        free(sav_file);
        transferJSON.clear();
        transferJSON["type"] = "success";
        Serial.print("JSON:");
        serializeJson(transferJSON, Serial);
        Serial.println();

    }
    */
    
}
// void GameBoyCartridge::startReadRAM_GB() {

// }
// void GameBoyCartridge::endReadRAM_GB() {

// }
// void GameBoyCartridge::startWriteRAM_GB()
// {
//     //  Load ROM header
//     this->headerROM_GB(false);
//     this->writtingRAM = true;

//     this->rd_wr_mreq_reset();

//     // MBC2 Fix (unknown why this fixes it, maybe has to read ROM before RAM?)
//     this->read_byte_GB(0x0134);

//     // Does cartridge have RAM
//     if (this->ramEndAddress > 0)
//     {
//         if (this->cartridgeType <= 4)
//         {                                   // MBC1
//             this->write_byte_GB(0x6000, 1); // Set RAM Mode
//         }

//         // Initialise MBC
//         this->write_byte_GB(0x0000, 0x0A);

//         // Switch RAM banks
//         for (uint8_t bank = 0; bank < ramBanks; bank++)
//         {
//             this->write_byte_GB(0x4000, bank);

//             // Write RAM
//             for (uint16_t ramAddress = 0xA000; ramAddress <= this->ramEndAddress; ramAddress++)
//             {
//                 // Wait for serial input
//                 while (Serial.available()){
//                 }

//                 // Read input
//                 uint8_t readValue = (uint8_t)Serial.read();

//                 // Write to RAM
//                 cs_mreqPin_low;
//                 this->write_byte_GB(ramAddress, readValue);
//                 asm volatile("nop");
//                 asm volatile("nop");
//                 asm volatile("nop");
//                 cs_mreqPin_high;
//             }
//         }

//         // Disable RAM
//         this->write_byte_GB(0x0000, 0x00);
//         Serial.flush(); // Flush any serial data that wasn't processed
//     }
// }
// void GameBoyCartridge::endWriteRAM_GB()
// {
//     this->writtingRAM = false;
// }
bool GameBoyCartridge::isWrittingROM()
{
    return this->writtingROM;
}
bool GameBoyCartridge::isWrittingRAM()
{
    return this->writtingRAM;
}
bool GameBoyCartridge::isRestoringRAM()
{
    return this->restoringRAM;
}
void GameBoyCartridge::setup()
{
    disableCore0WDT();  
    // buffer_obj = Buffer();
    this->writtingRAM = false;
    this->writtingROM = false;
    this->lastByte = 0;
    this->cartridgeType = 0;
    this->romSize = 0;
    this->romBanks = 0;
    this->ramSize = 0;
    this->ramBanks = 0;
    this->sramSize = 0;
    this->romEndAddress = 0x7FFF;
    this->sramBanks = 0;
    this->romType = 0;
    this->currentBank = 0;
}
void GameBoyCartridge::main()
{
    if (this->isWrittingRAM())
    {
        if (this->currentBank < this->sramBanks) {
            this->write_byte_GB(0x4000, this->currentBank);
            // Read RAM
            for (word ramAddress = 0xA000; ramAddress <= this->ramEndAddress; ramAddress += 64)
            {
                uint8_t readData[64];
                for (uint8_t i = 0; i < 64; i++)
                {
                    readData[i] = this->readByteSRAM_GB(ramAddress + i);
                }
                Serial1.write(readData, 64); // Send the 64 byte chunk
                Serial1.flush();
                this->processedProgressBar += 64;
            }
            this->currentBank++;
        } else {
                // Disable RAM
            this->write_byte_GB(0x0000, 0x00);
            delay(50);
            this->totalProgressBar = (uint32_t)(this->sramBanks) * 8192;
            transferJSON["type"] = "success";
            transferJSON["total"] = this->totalProgressBar;
            transferJSON["progress"] = this->processedProgressBar * 100 / this->totalProgressBar;
            transferJSON["ramBanks"] = this->sramBanks;

            delay(200);
            Serial.print("JSON:");
            serializeJson(transferJSON, Serial);
            Serial.println();
            this->writtingRAM = false;

            this->rd_wr_mreq_off();
        }
    } 
    else if(this->isRestoringRAM()) 
    {   
        // if (Serial1.available() > 0) {
            if (this->ramEndAddress > 0) {
                // this->serial_read_bytes(64);
                // for (uint8_t i = 0; i < 64; i++) {
                //     this->receivedBuffer[i] = (uint8_t)Serial1.read();
                // }

                // if (this->currentBank < this->sramBanks) {
                    
                //     this->write_byte_GB(0x4000, this->currentBank);
                    
                //     // Write RAM
                //     for (uint8_t x = 0; x < 64; x++) {
                //         this->write_byte_GB(this->currentAddress, this->receivedBuffer[x], MEMORY_WRITE );
                //         this->currentAddress++;
                //     }
                //     // this->serial_transmit('1');

                //     this->currentBank++;

                //     transferJSON.clear();
                //     transferJSON["type"] = "ack";
                //     Serial.print("JSON:");
                //     serializeJson(transferJSON, Serial);
                //     Serial.println();

                //     this->serial_read_bytes(64);
                // } else {
                //     // Disable SRAM
                //     this->write_byte_GB(0x0000, 0x00);

                //     transferJSON.clear();
                //     transferJSON["type"] = "success";
                //     Serial.print("JSON:");
                //     serializeJson(transferJSON, Serial);
                //     Serial.println();
                
                //     this->restoringRAM = false;
                // }
            
            }
        // } else {
        //     if (this->currentBank == this->sramBanks) {
        //         // Disable SRAM
        //         this->write_byte_GB(0x0000, 0x00);

        //         transferJSON.clear();
        //         transferJSON["type"] = "success";
        //         Serial.print("JSON:");
        //         serializeJson(transferJSON, Serial);
        //         Serial.println();
            
        //         this->restoringRAM = false;
        //     }
        // }
        
        
    } else if (this->isWrittingROM())
    {
        if (this->currentBank < this->romBanks) {
            /*
            // Second bank starts at 0x4000
            if (this->currentBank > 0)
            {
                this->romAddress = 0x4000;
                this->dataBusAsOutput();
                if ((this->romType == 1) || (this->romType == 2) || (this->romType == 3)) {
                    //  MBC_1
                    this->write_byte_GB(0x2100, this->currentBank);
                } else if (((this->romType == 5) || (this->romType == 6)) || ((this->romType == 15) || (this->romType == 16) || (this->romType == 17) || (this->romType == 18) || (this->romType == 19))) {
                    //  MBC_2 MBC_3
                    this->write_byte_GB(0x2100, this->currentBank);
                } else if ((this->romType == 25) || (this->romType == 26) || (this->romType == 27) || (this->romType == 28) || (this->romType == 29) || (this->romType == 309)) {
                    //  MBC_5
                    this->write_byte_GB(0x2100, this->currentBank);
                }
                this->dataBusAsInput();
                // // MBC6 banks are half size
                // if (this->romType == 32)
                // {
                //     this->romEndAddress = 0x5FFF;
                // }
            }
            transferJSON.clear();
            transferJSON["type"] = "transfer";
            transferJSON["offset"] =this->romAddress;
            JsonArray chunkArray = transferJSON.createNestedArray("chunk");
            for (word address = 0; address < 0x4000; address++) {
                byte data = this->read_byte_GB(address + this->romAddress);
                chunkArray.add(data);
                this->processedProgressBar++;
                // Serial1.write(data);
            }
            transferJSON["transfered"] = this->processedProgressBar;
            Serial.print("JSON:");
            serializeJson(transferJSON, Serial);
            Serial.println();
            // Serial1.flush();

            // // Set ROM bank for M161
            // if (this->romType == 0x104)
            // {
            //     this->romAddress = 0;
            //     // Set CS2 to LOW
            //     cs2Pin_low;
            //     delay(50);
            //     // Set CS2 to HIGH
            //     cs2Pin_high;
            //     this->write_byte_GB(0x4000, this->currentBank & 0x7);
            // }

            // // Set ROM bank for MBC1M
            // else if (this->romType == 0x101 || this->romType == 0x103)
            // {
            //     if (this->currentBank < 10)
            //     {
            //         this->write_byte_GB(0x4000, this->currentBank >> 4);
            //         this->write_byte_GB(0x2000, (this->currentBank & 0x1f));
            //     }
            //     else
            //     {
            //         this->write_byte_GB(0x4000, this->currentBank >> 4);
            //         this->write_byte_GB(0x2000, 0x10 | (this->currentBank & 0x1f));
            //     }
            // }

            // // Set ROM bank for MBC6
            // else if (this->romType == 32)
            // {
            //     this->write_byte_GB(0x2800, 0);
            //     this->write_byte_GB(0x3800, 0);
            //     this->write_byte_GB(0x2000, this->currentBank);
            //     this->write_byte_GB(0x3000, this->currentBank);
            // }

            // // Set ROM bank for TAMA5
            // else if (this->romType == 0xFD)
            // {
            //     //  TODO:
            //     // this->write_byte_SRAM_GB(0xA001, 0);
            //     // this->write_byte_SRAM_GB(0xA000, this->currentBank & 0x0f);
            //     // this->write_byte_SRAM_GB(0xA001, 1);
            //     // this->write_byte_SRAM_GB(0xA000, (this->currentBank >> 4) & 0x0f);
            // }

            // // Set ROM bank for MBC2/3/4/5
            // else if (this->romType >= 5)
            // {
            //     if (this->romType >= 11 && this->romType <= 13)
            //     {
            //         if ((this->currentBank & 0x1f) == 0)
            //         {
            //             // reset MMM01
            //             cs2Pin_low;
            //             delay(50);
            //             cs2Pin_high;

            //             // remap to higher 4Mbits ROM
            //             this->write_byte_GB(0x3fff, 0x20);
            //             this->write_byte_GB(0x5fff, 0x40);
            //             this->write_byte_GB(0x7fff, 0x01);
            //             this->write_byte_GB(0x1fff, 0x3a);
            //             this->write_byte_GB(0x1fff, 0x7a);

            //             // for every 4Mbits ROM, restart from 0x0000
            //             this->romAddress = 0x0000;
            //             this->currentBank++;
            //         }
            //         else
            //         {
            //             this->write_byte_GB(0x6000, 0);
            //             this->write_byte_GB(0x2000, (this->currentBank & 0x1f));
            //         }
            //     }
            //     else
            //     {
            //         if ((this->romType >= 0x19 && this->romType <= 0x1E) && (this->currentBank == 0 || this->currentBank == 256))
            //         {
            //             this->write_byte_GB(0x3000, (this->currentBank >> 8) & 0xFF);
            //         }
            //         this->write_byte_GB(0x2100, this->currentBank & 0xFF);
            //     }
            // }
            // // Set ROM bank for MBC1
            // else
            // {
            //     this->write_byte_GB(0x6000, 0);
            //     this->write_byte_GB(0x4000, this->currentBank >> 5);
            //     this->write_byte_GB(0x2000, this->currentBank & 0x1F);
            // }
            
            // Read banks and save to SD
            // while (this->romAddress <= this->romEndAddress) {
            //     for (int i = 0; i < 512; i++)
            //     {
            //         sdBuffer[i] = this->read_byte_GB(this->romAddress + i);
            //     }
            //     Serial1.write(sdBuffer, 512);
            //     Serial1.flush();
            //     this->romAddress += 512;
            //     this->processedProgressBar += 512;
            // }

            // transferJSON["type"] = "progress";
            // transferJSON["total"] = this->totalProgressBar;
            // transferJSON["transfered"] = this->processedProgressBar;
            
            // transferJSON["progress"] = this->processedProgressBar * 100 / this->totalProgressBar;
            // transferJSON["romBanks"] = this->romBanks;
            // Serial.print("JSON:");
            // serializeJson(transferJSON, Serial);
            // Serial.println();
            // transferJSON.clear();
            
            // delay(1000);
            // __asm__("nop\n\t"
            //         "nop\n\t"
            //         "nop\n\t"
            //         "nop\n\t");
            */
           // Second bank starts at 0x4000
            if (this->currentBank > 1)
            {
                this->romAddress = 0x4000;
                // MBC6 banks are half size
                if (this->romType == 32)
                {
                    this->romEndAddress = 0x5FFF;
                }
            }

            // Set ROM bank for M161
            if (this->romType == 0x104)
            {
                this->romAddress = 0;
                // Set CS2 to LOW
                cs2Pin_low;
                delay(50);
                // Set CS2 to HIGH
                cs2Pin_high;
                this->write_byte_GB(0x4000, this->currentBank & 0x7);
            }

            // Set ROM bank for MBC1M
            else if (this->romType == 0x101 || this->romType == 0x103)
            {
                if (this->currentBank < 10)
                {
                    this->write_byte_GB(0x4000, this->currentBank >> 4);
                    this->write_byte_GB(0x2000, (this->currentBank & 0x1f));
                }
                else
                {
                    this->write_byte_GB(0x4000, this->currentBank >> 4);
                    this->write_byte_GB(0x2000, 0x10 | (this->currentBank & 0x1f));
                }
            }

            // Set ROM bank for MBC6
            else if (this->romType == 32)
            {
                this->write_byte_GB(0x2800, 0);
                this->write_byte_GB(0x3800, 0);
                this->write_byte_GB(0x2000, this->currentBank);
                this->write_byte_GB(0x3000, this->currentBank);
            }

            // Set ROM bank for TAMA5
            else if (this->romType == 0xFD)
            {
                //  TODO:
                // this->write_byte_SRAM_GB(0xA001, 0);
                // this->write_byte_SRAM_GB(0xA000, this->currentBank & 0x0f);
                // this->write_byte_SRAM_GB(0xA001, 1);
                // this->write_byte_SRAM_GB(0xA000, (this->currentBank >> 4) & 0x0f);
            }

            // Set ROM bank for MBC2/3/4/5
            else if (this->romType >= 5)
            {
                if (this->romType >= 11 && this->romType <= 13)
                {
                    if ((this->currentBank & 0x1f) == 0)
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
                        this->romAddress = 0x0000;
                        this->currentBank++;
                    }
                    else
                    {
                        this->write_byte_GB(0x6000, 0);
                        this->write_byte_GB(0x2000, (this->currentBank & 0x1f));
                    }
                }
                else
                {
                    if ((this->romType >= 0x19 && this->romType <= 0x1E) && (this->currentBank == 0 || this->currentBank == 256))
                    {
                        this->write_byte_GB(0x3000, (this->currentBank >> 8) & 0xFF);
                    }
                    this->write_byte_GB(0x2100, this->currentBank & 0xFF);
                }
            }
            // Set ROM bank for MBC1
            else
            {
                this->write_byte_GB(0x6000, 0);
                this->write_byte_GB(0x4000, this->currentBank >> 5);
                this->write_byte_GB(0x2000, this->currentBank & 0x1F);
            }
            
            // Read banks and save to SD
            while (this->romAddress <= this->romEndAddress) {
                // transferJSON.clear();
                // transferJSON["type"] = "transfer";
                // transferJSON["offset"] = this->romAddress;
                // JsonArray chunkArray = transferJSON.createNestedArray("chunk");
                for (int i = 0; i < 512; i++)
                {
                    sdBuffer[i] = this->read_byte_GB(this->romAddress + i);
                    // chunkArray.add(this->read_byte_GB(this->romAddress + i));
                }
                // uint8_t read_byte = this->read_byte_GB(this->romAddress);
                // Serial1.write(sdBuffer, 512);
                Serial.write(sdBuffer, 512);
                Serial.flush();
                this->romAddress += 1;
                this->processedProgressBar += 1;

                
                // transferJSON["transfered"] = this->processedProgressBar;
                // Serial.print("JSON:");
                // serializeJson(transferJSON, Serial);
                // Serial.println();
                
                
                    
               
            }
            this->currentBank++;
        } else {
            transferJSON.clear();
            transferJSON["type"] = "success";
            transferJSON["total"] = this->totalProgressBar;
            transferJSON["transfered"] = this->processedProgressBar;
            transferJSON["progress"] = this->processedProgressBar * 100 / this->totalProgressBar;
            transferJSON["romBanks"] = this->romBanks;
            delay(200);
            Serial.print("JSON:");
            serializeJson(transferJSON, Serial);
            Serial.println();
            this->writtingROM = false;
        }
    }
}