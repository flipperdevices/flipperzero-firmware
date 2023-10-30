#include "GameboyLiveCamera.h"

unsigned char raw_buffer[FRAME_SIZE]; // max( 16*8*14*8, 16*14*16 ) sensor pixels , tile bytes
static uint8_t GBCAM_BUFFER[GBCAM_W * GBCAM_H];

HardwareSerial Serial3(3);


GameboyLiveCamera::GameboyLiveCamera()
{
    this->runGameboyLiveCamera = false;
}
void GameboyLiveCamera::begin()
{
    this->setup();
}
void GameboyLiveCamera::start()
{
    //  Initial values
    this->trigger = 0x03;
    this->unk1 = 0xE8;
    this->exposure_time = 0x0500;
    this->unk2 = 0x24;
    this->unk3 = 0xBF;
    this->dithering = true;
    this->rotate90 = false;

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
    this->setAddress(0x0000);

    digitalWrite(GAMEBOY_CLK, LOW);
    digitalWrite(GAMEBOY_RST, LOW);
    digitalWrite(GAMEBOY_WR, HIGH);
    digitalWrite(GAMEBOY_RD, HIGH);
    digitalWrite(GAMEBOY_CS, HIGH);

    delay(400);
    digitalWrite(GAMEBOY_RST, HIGH);
    this->runGameboyLiveCamera = true;
}
void GameboyLiveCamera::stop()
{
    //  Initial values
    this->trigger = 0x03;
    this->unk1 = 0xE8;
    this->exposure_time = 0x0500;
    this->unk2 = 0x24;
    this->unk3 = 0xBF;
    this->runGameboyLiveCamera = false;
}
void GameboyLiveCamera::dataBusAsOutput()
{
    for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
    {
        pinMode(DATA_GB_GBC_PINS[i], OUTPUT);
    }
}
void GameboyLiveCamera::dataBusAsInput()
{
    for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
    {
        pinMode(DATA_GB_GBC_PINS[i], INPUT);
    }
}

// Set Gameboy mode
void GameboyLiveCamera::gb_mode(void)
{
    // Address pins as outputs
    for (uint32_t i = 0; i < sizeof(ADDRESS_GB_GBC_PINS) / sizeof(ADDRESS_GB_GBC_PINS[0]); i++)
    {
        pinMode(ADDRESS_GB_GBC_PINS[i], OUTPUT);
    }
    // Set Data Pins (D0-D7) to Input
    this->dataBusAsInput();
}
bool GameboyLiveCamera::isRunning() {
    return this->runGameboyLiveCamera;
}
/*
// Set the 16 bit address
void GameboyLiveCamera::set_address_GB(uint16_t address)
{
    // Write each of the bits into the address pins
    for (uint32_t i = 0; i < sizeof(ADDRESS_GB_GBC_PINS) / sizeof(ADDRESS_GB_GBC_PINS[0]); i++)
    {
        digitalWrite(ADDRESS_GB_GBC_PINS[i], address & (1 << i) ? HIGH : LOW);
    }
}*/
void GameboyLiveCamera::setAddress(unsigned int addr)
{
    // Write each of the bits into the address pins
    for (uint32_t i = 0; i < sizeof(ADDRESS_GB_GBC_PINS) / sizeof(ADDRESS_GB_GBC_PINS[0]); i++)
    {
        digitalWrite(ADDRESS_GB_GBC_PINS[i], addr & (1 << i) ? HIGH : LOW);
    }
}
void GameboyLiveCamera::setWriteMode(int is_rom)
{
    if (is_rom)
        digitalWrite(GAMEBOY_CS, HIGH);
    else
        digitalWrite(GAMEBOY_CS, LOW);

    // Set Data Pins (D0-D7) to Output
    for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
    {
        pinMode(DATA_GB_GBC_PINS[i], OUTPUT);
    }
}
int GameboyLiveCamera::readCartByte(unsigned int address)
{
    this->setAddress(address);

    this->setReadMode(address < 0x8000);

    unsigned int value = this->getData();
    this->setWaitMode();

    return value;
}
void GameboyLiveCamera::setData(unsigned int value)
{
    for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
    {
        digitalWrite(DATA_GB_GBC_PINS[i], value & (1 << i) ? HIGH : LOW);
    }
}
unsigned int GameboyLiveCamera::getData(void)
{
    int i;
    unsigned int value = 0;
    for (i = 0; i < 8; i++)
        if (digitalRead(DATA_GB_GBC_PINS[i]) == HIGH)
            value |= (1 << i);
    return value;
}
unsigned int GameboyLiveCamera::getDataBit0(void)
{
    return (digitalRead(DATA_GB_GBC_PINS[0]) == HIGH);
}
void GameboyLiveCamera::performWrite(void)
{
    digitalWrite(GAMEBOY_WR, LOW);
    digitalWrite(GAMEBOY_RD, HIGH);
}
void GameboyLiveCamera::setWaitMode(void)
{
    digitalWrite(GAMEBOY_CS, HIGH);
    digitalWrite(GAMEBOY_WR, HIGH);
    digitalWrite(GAMEBOY_RD, HIGH);

    // Set Data Pins (D0-D7) to Input
    this->dataBusAsInput();
}
void GameboyLiveCamera::setReadMode(int is_rom)
{
    // Set Data Pins (D0-D7) to Input
    this->dataBusAsInput();

    digitalWrite(GAMEBOY_WR, HIGH);
    digitalWrite(GAMEBOY_RD, LOW);

    if (is_rom)
        digitalWrite(GAMEBOY_CS, HIGH);
    else
        digitalWrite(GAMEBOY_CS, LOW);
}
void GameboyLiveCamera::writeCartByte(unsigned int address, unsigned int value)
{
    this->setAddress(address);

    this->setWriteMode(address < 0x8000);

    this->setData(value);

    this->performWrite();

    if (address >= 0x8000)
    {
        // PHI clock signal
        digitalWrite(GAMEBOY_CLK, LOW);
        // wait(120);
        asm volatile("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop");
        digitalWrite(GAMEBOY_CLK, HIGH);
        asm volatile("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop");
        // wait(120);
        digitalWrite(GAMEBOY_CLK, LOW);
    }

    this->setWaitMode();
}
void GameboyLiveCamera::readPicture(bool is_thumbnail)
{
    this->writeCartByte(0x0000, 0x0A); // Enable RAM
    this->writeCartByte(0x4000, 0x00); // Set RAM mode, bank 0

    unsigned int addr = 0xA100;
    unsigned int _size = 16 * 14 * 16;
    this->setReadMode(0xA100 < 0x8000);
    int i = 0;
    while (_size--)
    {
        this->setAddress(addr++);
        unsigned char value = this->getData();
        raw_buffer[i] = value;
        i++;
    }
    const int gb_pal_colors[4] = {255, 168, 80, 0};
    //  Envía imagen pero quedan columnas separadas
    int y, x;
    for (uint8_t y = 0; y < GBCAM_H; y++)
    {
        Serial1.print("Y:");
        Serial1.print((char)y);

        for (uint8_t x = 0; x < GBCAM_W; x += 8)
        {
            char c = 0;
            for (uint8_t j = 0; j < 8; ++j)
            {
                int basetileaddr = (((y >> 3) * 16 + ((x + (7 - j)) >> 3)) * 16);
                int baselineaddr = basetileaddr + ((y & 7) << 1);

                unsigned char data = raw_buffer[baselineaddr];
                unsigned char data2 = raw_buffer[baselineaddr + 1];

                int x_ = 7 - ((x + (7 - j)) & 7);

                int color = ((data >> x_) & 1) | (((data2 >> x_) << 1) & 2);

                // Check if the color is 2 or 3
                if (color == 2 || color == 3)
                {
                    c |= 1 << j;
                }
                else
                {
                    c &= ~(1 << j); // Establece el bit j en 0
                }
            }
            Serial1.print(c); // Imprimir el valor de c para colores 2 o 3
        }

        ++y;
        Serial1.flush();
    }
    this->setWaitMode();
}
void GameboyLiveCamera::ramEnable()
{
    this->writeCartByte(0x0000, 0x0A);
}
void GameboyLiveCamera::setRegisterMode(void)
{
    this->writeCartByte(0x4000, 0x10);
}
void GameboyLiveCamera::ramDisable()
{
    this->writeCartByte(0x0000, 0x00);
}

void GameboyLiveCamera::updateMatrixRegisters()
{
    unsigned char c1 = 0x40, c2 = 0x80, c3 = 0xC0;

    // const unsigned char matrix[] = // high light
    //{
    //     0x89, 0x92, 0xA2, 0x8F, 0x9E, 0xC6, 0x8A, 0x95, 0xAB, 0x91, 0xA1, 0xCF,
    //     0x8D, 0x9A, 0xBA, 0x8B, 0x96, 0xAE, 0x8F, 0x9D, 0xC3, 0x8C, 0x99, 0xB7,
    //     0x8A, 0x94, 0xA8, 0x90, 0xA0, 0xCC, 0x89, 0x93, 0xA5, 0x90, 0x9F, 0xC9,
    //     0x8E, 0x9C, 0xC0, 0x8C, 0x98, 0xB4, 0x8E, 0x9B, 0xBD, 0x8B, 0x97, 0xB1
    // };

    const unsigned char matrix[48] = // low light
        {
            0x8C, 0x98, 0xAC, 0x95, 0xA7, 0xDB, 0x8E, 0x9B, 0xB7, 0x97, 0xAA, 0xE7,
            0x92, 0xA2, 0xCB, 0x8F, 0x9D, 0xBB, 0x94, 0xA5, 0xD7, 0x91, 0xA0, 0xC7,
            0x8D, 0x9A, 0xB3, 0x96, 0xA9, 0xE3, 0x8C, 0x99, 0xAF, 0x95, 0xA8, 0xDF,
            0x93, 0xA4, 0xD3, 0x90, 0x9F, 0xC3, 0x92, 0xA3, 0xCF, 0x8F, 0x9E, 0xBF};

    int i;
    for (i = 0; i < 48; i++)
    {
        if (this->dithering)
        {
            this->writeCartByte(0xA006 + i, matrix[i]);
        }
        else
        {
            switch (i % 3)
            {
            case 0:
                this->writeCartByte(0xA006 + i, c1);
                break;
            case 1:
                this->writeCartByte(0xA006 + i, c2);
                break;
            case 2:
                this->writeCartByte(0xA006 + i, c3);
                break;
            }
        }
    }
}
unsigned int GameboyLiveCamera::waitPictureReady(void)
{
    this->setRegisterMode();
    this->writeCartByte(0x4000, 0x10); // Set register mode
    unsigned long int clocks = 0;
    this->setAddress(0xA000);
    this->setReadMode(0xA000 < 0x8000);
    while (1)
    {
        if (this->getDataBit0() == 0)
            break;
        digitalWrite(GAMEBOY_CLK, LOW); // clock
        asm volatile("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop");
        digitalWrite(GAMEBOY_CLK, HIGH);
        asm volatile("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop");
        clocks++;
    }
    digitalWrite(GAMEBOY_CLK, LOW);
    asm volatile("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop");
    this->setWaitMode();
    return clocks;
}

void GameboyLiveCamera::setup()
{
    //  Initial values
    this->trigger = 0x03;
    this->unk1 = 0xE8;
    this->exposure_time = 0x0500;
    this->unk2 = 0x24;
    this->unk3 = 0xBF;
    delay(400);
}
void GameboyLiveCamera::enableDithering() {
    this->dithering = true;
}
void GameboyLiveCamera::disableDithering() {
    this->dithering = false;
}
void GameboyLiveCamera::increaseExposure() {
    this->exposure_time += 0x80;
}
void GameboyLiveCamera::decreaseExposure() {
    this->exposure_time -= 0x80;
}


LinkedList<String> GameboyLiveCamera::parseCommand(String input, char* delim) {
  LinkedList<String> cmd_args;

  bool inQuote = false;
  bool inApostrophe = false;
  String buffer = "";

  for (int i = 0; i < input.length(); i++) {
    char c = input.charAt(i);

    if (c == '"') {
      // Check if the quote is within an apostrophe
      if (inApostrophe) {
        buffer += c;
      } else {
        inQuote = !inQuote;
      }
    } else if (c == '\'') {
      // Check if the apostrophe is within a quote
      if (inQuote) {
        buffer += c;
      } else {
        inApostrophe = !inApostrophe;
      }
    } else if (!inQuote && !inApostrophe && strchr(delim, c) != NULL) {
      cmd_args.add(buffer);
      buffer = "";
    } else {
      buffer += c;
    }
  }

  // Add the last argument
  if (!buffer.isEmpty()) {
    cmd_args.add(buffer);
  }

  return cmd_args;
}
String GameboyLiveCamera::getSerialInput() {
  String input = "";

  if (Serial.available() > 0)
    input = Serial.readStringUntil('\n');

  input.trim();
  return input;
}
void GameboyLiveCamera::main()
{
    if (this->runGameboyLiveCamera)
    {
        
        //  Capture Picture

        this->ramEnable();                 // Enable RAM
        this->setRegisterMode();           // Set register mode
        this->writeCartByte(0xA000, 0x00); // Set RAM mode, bank 0
        this->writeCartByte(0xA001, this->unk1);
        this->writeCartByte(0xA002, (this->exposure_time >> 8) & 0xFF);
        this->writeCartByte(0xA003, this->exposure_time & 0xFF);
        this->writeCartByte(0xA004, this->unk2);
        this->writeCartByte(0xA005, this->unk3);
        this->updateMatrixRegisters();
        this->writeCartByte(0xA000, this->trigger);
        unsigned int clks = 0;
        while (1)
        {
            clks += this->waitPictureReady();
            int a = this->readCartByte(0xA000);
            if ((a & 1) == 0)
                break;
        }
        this->ramDisable();

        //  Read Picture
        this->readPicture(0);

    }
}