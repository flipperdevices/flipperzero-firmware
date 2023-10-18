//https://gist.github.com/igrr/1515d369310479bbaf9239afeed4aac5
#include "Gameboy.h"


uint8_t trigger = 0x03;
uint8_t unk1 = 0xE8;
uint16_t exposure_time = 0x0500;
uint8_t unk2 = 0x24;
uint8_t unk3 = 0xBF;
// uint8_t dithering = 1;
bool dithering = true;
bool rotate90 = false;


// uint8_t picturedata[16*14*16]; // max( 16*8*14*8, 16*14*16 ) sensor pixels , tile bytes

void wait(int i)
{
  for(int j = 0; j < i; j++)
  {
    __asm__("nop\n"); 
  }
}


static inline unsigned int asciihextoint(char c)
{
  if((c >= '0') && (c <= '9')) return c - '0';
  if((c >= 'A') && (c <= 'F')) return c - 'A' + 10;
  return 0;
}

static inline unsigned int asciidectoint(char c)
{
  if((c >= '0') && (c <= '9')) return c - '0';
  return 0;
}

static inline char inttoasciihex(int n)
{
  if(n < 10) return '0' + n;
  if(n < 16) return 'A' + n - 10;
  return 0;
}
static inline void setAddress(unsigned int addr)
{
  // Write each of the bits into the address pins
  for (uint32_t i = 0; i < sizeof(ADDRESS_GB_GBC_PINS) / sizeof(ADDRESS_GB_GBC_PINS[0]); i++)
  {
    digitalWrite(ADDRESS_GB_GBC_PINS[i], addr & (1 << i) ? HIGH : LOW);
  }
}
static inline void setWriteMode(int is_rom)
{
  if(is_rom)
    digitalWrite(GAMEBOY_CS, HIGH);
  else
    digitalWrite(GAMEBOY_CS, LOW);
  
  // Set Data Pins (D0-D7) to Output
  for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS)/sizeof(DATA_GB_GBC_PINS[0]); i++)
  {
      pinMode(DATA_GB_GBC_PINS[i], OUTPUT);
  }
}
unsigned int readCartByte(unsigned int address)
{
  setAddress(address);
  
  setReadMode(address < 0x8000);

  unsigned int value = getData();
  setWaitMode();
  
  return value;
}
static inline void setData(unsigned int value)
{
  for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS) / sizeof(DATA_GB_GBC_PINS[0]); i++)
  {
    digitalWrite(DATA_GB_GBC_PINS[i], value & (1 << i) ? HIGH : LOW);
  }
}
static inline unsigned int getData(void)
{
  int i;
  unsigned int value = 0;
  for(i = 0; i < 8; i++)
    if(digitalRead(DATA_GB_GBC_PINS[i]) == HIGH) value |= (1<<i);
  return value;
}
static inline unsigned int getDataBit0(void)
{
  return (digitalRead(DATA_GB_GBC_PINS[0]) == HIGH);
  // return gpio_get_level(GPIO_NUM_14) == 1;
}
static inline void performWrite(void)
{
  digitalWrite(GAMEBOY_WR, LOW);
  digitalWrite(GAMEBOY_RD, HIGH);
}
static inline void setWaitMode(void)
{
  digitalWrite(GAMEBOY_CS, HIGH);
  digitalWrite(GAMEBOY_WR, HIGH);
  digitalWrite(GAMEBOY_RD, HIGH);
  
  // Set Data Pins (D0-D7) to Input
  for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS)/sizeof(DATA_GB_GBC_PINS[0]); i++)
  {
      pinMode(DATA_GB_GBC_PINS[i], INPUT);
  }
}
static inline void setReadMode(int is_rom)
{
  // Set Data Pins (D0-D7) to Input
  for (uint32_t i = 0; i < sizeof(DATA_GB_GBC_PINS)/sizeof(DATA_GB_GBC_PINS[0]); i++)
  {
      pinMode(DATA_GB_GBC_PINS[i], INPUT);
  }
  
  digitalWrite(GAMEBOY_WR, HIGH);
  digitalWrite(GAMEBOY_RD, LOW);
  
  if(is_rom)
    digitalWrite(GAMEBOY_CS, HIGH);
  else
    digitalWrite(GAMEBOY_CS, LOW);
}
void writeCartByte(unsigned int address, unsigned int value)
{
  setAddress(address);
  
  setWriteMode(address < 0x8000);
 
  setData(value);
  
  performWrite();

  if(address >= 0x8000)
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
  
  setWaitMode();
  
}

// uint8_t raw_buffer[FRAME_SIZE];
unsigned char raw_buffer[FRAME_SIZE]; // max( 16*8*14*8, 16*14*16 ) sensor pixels , tile bytes
static uint8_t GBCAM_BUFFER[GBCAM_W * GBCAM_H];

void readPicture(bool is_thumbnail)
{
  writeCartByte(0x0000,0x0A); // Enable RAM
  writeCartByte(0x4000,0x00); // Set RAM mode, bank 0
  
  unsigned int addr = 0xA100;
  unsigned int _size = 16 * 14 * 16;
  setReadMode(0xA100 < 0x8000);
  int i = 0;
  while(_size--)
  {
    setAddress(addr++);
    unsigned char value = getData();
    // Serial.write(value)
    raw_buffer[i] = value;
    i++;
  }
  const int gb_pal_colors[4] = {255, 168, 80, 0};
  //  Envía imagen pero quedan columnas separadas
  int y, x;
  for (uint8_t y = 0; y < GBCAM_H; y++) {
    Serial.print("Y:");
    Serial.print((char)y);

    for (uint8_t x = 0; x < GBCAM_W; x += 8) {
        char c = 0;
        for (uint8_t j = 0; j < 8; ++j) {
            int basetileaddr = (((y >> 3) * 16 + ((x + (7 - j)) >> 3)) * 16);
            int baselineaddr = basetileaddr + ((y & 7) << 1);

            unsigned char data = raw_buffer[baselineaddr];
            unsigned char data2 = raw_buffer[baselineaddr + 1];

            int x_ = 7 - ((x + (7 - j)) & 7);

            int color = ((data >> x_) & 1) | (((data2 >> x_) << 1) & 2);

            // Check if the color is 2 or 3
            if (color == 2 || color == 3) {
                c |= 1 << j;
            }else {
                c &= ~(1 << j); // Establece el bit j en 0
            }
        }
        Serial.print(c); // Imprimir el valor de c para colores 2 o 3
    }

    ++y;
    // Serial.println(); // Agrega un salto de línea después de cada fila
    Serial.flush();
  }
  

  setWaitMode();
}


void ramEnable() {
  writeCartByte(0x0000,0x0A);
}
void setRegisterMode(void)
{
    writeCartByte(0x4000,0x10);
}
void ramDisable() {
  writeCartByte(0x0000,0x00);
}

void updateMatrixRegisters(bool dithering)
{
    unsigned char c1 = 0x40, c2 = 0x80, c3 = 0xC0;

    //const unsigned char matrix[] = // high light
    //{
    //    0x89, 0x92, 0xA2, 0x8F, 0x9E, 0xC6, 0x8A, 0x95, 0xAB, 0x91, 0xA1, 0xCF,
    //    0x8D, 0x9A, 0xBA, 0x8B, 0x96, 0xAE, 0x8F, 0x9D, 0xC3, 0x8C, 0x99, 0xB7,
    //    0x8A, 0x94, 0xA8, 0x90, 0xA0, 0xCC, 0x89, 0x93, 0xA5, 0x90, 0x9F, 0xC9,
    //    0x8E, 0x9C, 0xC0, 0x8C, 0x98, 0xB4, 0x8E, 0x9B, 0xBD, 0x8B, 0x97, 0xB1
    //};

    const unsigned char matrix[48] = // low light
    {
        0x8C, 0x98, 0xAC, 0x95, 0xA7, 0xDB, 0x8E, 0x9B, 0xB7, 0x97, 0xAA, 0xE7,
        0x92, 0xA2, 0xCB, 0x8F, 0x9D, 0xBB, 0x94, 0xA5, 0xD7, 0x91, 0xA0, 0xC7,
        0x8D, 0x9A, 0xB3, 0x96, 0xA9, 0xE3, 0x8C, 0x99, 0xAF, 0x95, 0xA8, 0xDF,
        0x93, 0xA4, 0xD3, 0x90, 0x9F, 0xC3, 0x92, 0xA3, 0xCF, 0x8F, 0x9E, 0xBF
    };

    int i;
    for(i = 0; i < 48; i++)
    {
        if(dithering)
        {
            writeCartByte(0xA006+i,matrix[i]);
        }
        else
        {
            switch(i%3)
            {
                case 0: writeCartByte(0xA006+i,c1); break;
                case 1: writeCartByte(0xA006+i,c2); break;
                case 2: writeCartByte(0xA006+i,c3); break;
            }
        }
    }
}
unsigned int waitPictureReady(void)
{
    setRegisterMode();
    writeCartByte(0x4000,0x10); // Set register mode
    unsigned long int clocks = 0;
    setAddress(0xA000);
    setReadMode(0xA000 < 0x8000);
    while(1)
    {
      if( getDataBit0() == 0 ) break;
      digitalWrite(GAMEBOY_CLK, LOW); //clock
      asm volatile("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop");
      digitalWrite(GAMEBOY_CLK, HIGH);
      asm volatile("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop");
      clocks++;
    }
    digitalWrite(GAMEBOY_CLK, LOW);
    asm volatile("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop");
    setWaitMode();
    return clocks;
}

void gb_live_camera_setup() {
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

  setAddress(0x0000);

  digitalWrite(GAMEBOY_CLK, LOW);
  digitalWrite(GAMEBOY_RST, LOW);
  digitalWrite(GAMEBOY_WR, HIGH);
  digitalWrite(GAMEBOY_RD, HIGH);
  digitalWrite(GAMEBOY_CS, HIGH);


  delay(400);
  digitalWrite(GAMEBOY_RST, HIGH);
}

void gb_live_camera_loop() {
  if (Serial.available() > 0) {
    char r = Serial.read();
    switch (r) {
      case 'S':
        //  TODO: 
        // Serial.println("Stop Stream");
      break;
      case 's':
        //  TODO:
      break;
      case 'D':
        dithering = true;
        break;
      case 'd':
        dithering = false;
        break;
      case 'E':  // Toggle Mirror
        exposure_time += 0x80; 
        break;
      case 'e':
        exposure_time -= 0x80; 
        break;
      default:
        break;
    }

  }

  //  Capture Picture

  ramEnable();                // Enable RAM
  setRegisterMode();          // Set register mode
  writeCartByte(0xA000,0x00); // Set RAM mode, bank 0
  writeCartByte(0xA001,unk1);
  writeCartByte(0xA002,(exposure_time>>8)&0xFF);
  writeCartByte(0xA003,exposure_time&0xFF);
  writeCartByte(0xA004,unk2);
  writeCartByte(0xA005,unk3);
  updateMatrixRegisters(dithering);
  writeCartByte(0xA000,trigger);
  unsigned int clks = 0;
  while(1)
  {
      clks += waitPictureReady();
      int a = readCartByte(0xA000);
      if((a & 1) == 0) break;
  }
  ramDisable();

  //  Read Picture
  readPicture(0);
}