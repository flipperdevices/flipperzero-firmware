/*
 * Copyright (c) 2009 Andrew Brown
 * Copyright (c) 2018 rcombs
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "crc_table.h"

#ifdef TEENSYDUINO
#include <BufferedPrint.h>
#include <FreeStack.h>
#include <RingBuf.h>
#include <SdFat.h>
#include <sdios.h>
#include <SdFatConfig.h>
#include <MinimumSerial.h>
#include <DMAChannel.h>
#include <EEPROM.h>

#ifndef PIT_LTMR64H
#define PIT_LTMR64H             (*(volatile uint32_t *)0x400370E0) // PIT Upper Lifetime Timer Register
#define PIT_LTMR64L             (*(volatile uint32_t *)0x400370E4) // PIT Lower Lifetime Timer Register
#endif

#define SD_CONFIG SdSpiConfig(SDCARD_SS_PIN, SHARED_SPI, SD_SCK_MHZ(50))

#define STATUS_PIN 13
#define POWER_PIN 32
#define VI_PIN 35
#define FIELD_PIN 36
#define HAVE_EEPROM
#define HAVE_GPIO_COMMANDS

#define SERIAL_BAUD_RATE 115200

#define N64_PIN 38
#define SNES_LATCH_PIN 3
#define SNES_CLOCK_PIN 4
#define SNES_DATA_PIN 5

#define N64_HIGH (CORE_PIN38_DDRREG &= ~CORE_PIN38_BITMASK) //digitalWriteFast(N64_PIN, HIGH)
#define N64_LOW (CORE_PIN38_DDRREG |= CORE_PIN38_BITMASK) //digitalWriteFast(N64_PIN, LOW)
#define N64_QUERY ((CORE_PIN38_PINREG & CORE_PIN38_BITMASK) ? 1 : 0) //digitalReadFast(N64_PIN)

#define LED_HIGH (CORE_PIN13_PORTSET = CORE_PIN13_BITMASK) //digitalWriteFast(STATUS_PIN, HIGH)
#define LED_LOW (CORE_PIN13_PORTCLEAR = CORE_PIN13_BITMASK) //digitalWriteFast(STATUS_PIN, LOW)

#define SERIAL_TIMEOUT_US 5000000

#define DISABLE_N64_INTERRUPT() \
  volatile uint32_t *config = portConfigRegister(N64_PIN); \
  uint32_t oldConfig = *config; \
  *config &= ~0x000F0000;
#define ENABLE_N64_INTERRUPT() \
  *config = oldConfig;
#endif

#define INPUT_BUFFER_SIZE 2048 // Multiples of 512 are ideal since we can read 256*4/2 = 512 bytes at once.
// 512 bytes is an optimization for reading the sd card and skips using another buffer.

#define INPUT_BUFFER_UPDATE_TIMEOUT 10 // 10 ms

#define MICRO_CYCLES (F_CPU / 1000000)
#ifdef F_BUS
#define MICRO_BUS_CYCLES (F_BUS / 1000000ULL)
#endif

#define MAX_CMD_LEN 4096
#define MAX_LOOP_LEN 10240

typedef enum Console {
  N64 = 0,
  NES = 1,
  SNES = 2,
} Console;

static Console console = N64;

static bool skippingInput = false;
static String inputString;

static char n64_raw_dump[36]; // maximum recv is 1+2+32 bytes + 1 bit
// n64_raw_dump does not include the command byte. That gets pushed into
// n64_command:
static int n64_command;
// bytes to send to the 64
// maximum we'll need to send is 33, 32 for a read request and 1 CRC byte
static unsigned char output_buffer[33];

// Simple switch buffer. (If buffer A fails to load while buffer B is in use,
// we still okay, and will try again next loop)
static unsigned char inputBuffer[INPUT_BUFFER_SIZE];
static size_t bufferEndPos;
static volatile size_t bufferPos;
static volatile bool bufferHasData;
static void updateInputBuffer();

static bool hold = false;
static bool finished = false;

static SdFile tasFile;

//static unsigned int progressPos = 0;
static String filename;
static unsigned long numFrames = 0, curFrame = 0;
static int edgesRead = 0;
static int incompleteCommand = 0;
static int firstEdgeTime = 0;
static int secondEdgeTime = 0;
static volatile unsigned long viCount = 0;

static SdFs sd;

#ifdef SNES_DATA_PIN
static uint16_t currentSnesFrame;
static uint32_t frameDuration = 0;
#endif

static uint64_t finalTime = 0;
static uint64_t lastFrameTime = 0;
static int doLoop = 0;

#define N_SER_BUFS 16
#define SER_BUF_SIZE 256
static char serBuffer[N_SER_BUFS][SER_BUF_SIZE] = {0};
static volatile bool bufHasData[N_SER_BUFS] = {0};

static void n64Interrupt();

template<class T> void lockedPrintln(const T& input)
{
  noInterrupts();
  Serial.println(input);
  interrupts();
}

template<class A, class B> void lockedPrintln(const A& a, const B& b)
{
  noInterrupts();
  Serial.print(a);
  Serial.println(b);
  interrupts();
}

#ifdef ARM_DWT_CYCCNT

static void startTimer()
{
  ARM_DWT_CYCCNT = 0;
}

static uint32_t readTimer()
{
  return ARM_DWT_CYCCNT;
}

static uint32_t readAndResetTimer()
{
  return __atomic_exchange_n(&ARM_DWT_CYCCNT, 0, __ATOMIC_SEQ_CST);
}

#endif

#ifdef PIT_LTMR64H
static bool timer64Started = false;

static void start64Timer()
{
  // turn on PIT
  SIM_SCGC6 |= SIM_SCGC6_PIT;
  __asm__ volatile("nop"); // solves timing problem on Teensy 3.5
  PIT_MCR = 0x00;

  // Timer 1
  PIT_TCTRL1 = 0x0; // disable timer 1 and its interrupts
  PIT_LDVAL1 = 0xFFFFFFFF; // setup timer 1 for maximum counting period
  PIT_TCTRL1 |= PIT_TCTRL_CHN; // chain timer 1 to timer 0
  PIT_TCTRL1 |= PIT_TCTRL_TEN; // start timer 1

  // Timer 0
  PIT_TCTRL0 = 0; // disable timer 0 and its interrupts
  PIT_LDVAL0 = 0xFFFFFFFF; // setup timer 0 for maximum counting period
  PIT_TCTRL0 = PIT_TCTRL_TEN; // start timer 0
  timer64Started = true;
}

static uint64_t read64Timer()
{
  if (!timer64Started)
    return 0;

  uint64_t current_uptime = (uint64_t)PIT_LTMR64H << 32;
  current_uptime = current_uptime + PIT_LTMR64L;
  return 0xffffffffffffffffull - current_uptime;
}
#endif


static size_t getNextRegion(size_t& size) {
  size_t currentPos = bufferPos;
  if ((currentPos == bufferEndPos || (currentPos == 0 && bufferEndPos == INPUT_BUFFER_SIZE)) && bufferHasData) {
    size = 0;
    return 0;
  }
  size_t ret = bufferEndPos;
  if (ret == INPUT_BUFFER_SIZE)
    ret = 0;
  size = INPUT_BUFFER_SIZE - ret;
  if (ret < currentPos)
    size = currentPos - ret;
  return ret;
}

void initBuffers()
{
  bufferEndPos = 0;
  bufferPos = 0;
  bufferHasData = false;
}

static bool overflowed = 0;

void logFromISR(const char *fmt, ...)
{
  int i;
  for (i = 0; i < N_SER_BUFS && bufHasData[i]; i++);
  if (i >= N_SER_BUFS) {
    overflowed = 1;
    return;
  }

  char* buf = &serBuffer[i][0];
  size_t size = SER_BUF_SIZE - 1;

  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, size, fmt, ap);
  va_end(ap);

  bufHasData[i] = 1;
  overflowed = 0;

#ifdef ISR_LOG_CB
  ISR_LOG_CB();
#endif
}

static void advanceBuffer(long bytes)
{
  curFrame++;

#ifdef PIT_LTMR64H
  lastFrameTime = read64Timer();
#endif

  if (curFrame == numFrames) {
    logFromISR("C:%f",
#ifdef MICRO_BUS_CYCLES
        lastFrameTime / (double)(MICRO_BUS_CYCLES * 1000 * 1000)
#else
        double(0.)
#endif
    );
    finished = true;
    finalTime = lastFrameTime;
  }

  size_t pos = bufferPos + bytes;
  if (pos == bufferEndPos) {
    if (hold)
      return;
    bufferHasData = false;
    bufferPos = pos;
    return;
  }
  if (pos >= INPUT_BUFFER_SIZE)
      pos = 0;
  bufferPos = pos;
}

static size_t appendInputs(const String& data)
{
  size_t totalWritten = 0;
  size_t writeLen = 0;
  do {
    size_t dataLeft = data.length() - totalWritten;
    size_t writePos = getNextRegion(writeLen);
    if (writeLen > dataLeft)
      writeLen = dataLeft;
    if (writeLen) {
      memcpy(inputBuffer + writePos, data.c_str() + totalWritten, writeLen);
      totalWritten += writeLen;
      noInterrupts();
      finished = false;
      bufferEndPos = writePos + writeLen;
      bufferHasData = true;
      interrupts();
    }
  } while (writeLen > 0);
  return totalWritten;
}

static void emitList(const String& path)
{
  SdFile dir;
  if (!dir.open(&sd, path.c_str(), O_READ)) {
    Serial.println(F("E:Failed to open requested directory"));
    return;
  }
  dir.rewind();
  SdFile listFile;
  while (listFile.openNext(&dir)) {
    char name[256];
    listFile.getName(name, sizeof(name));
    lockedPrintln("A:", name);
    listFile.close();
  }
  dir.close();
}

void logFrame(const unsigned char *dat, size_t count, size_t num)
{
#define LOG_FRAME_START "F:%u %f %lu"
#define LOG_FRAME_BYTE  " %hhx"
#define LOG_FRAME_END   " %lu %i %s"

#ifdef PIT_LTMR64H
#define LOG_TIME (read64Timer() / (double)(MICRO_BUS_CYCLES * 1000 * 1000))
#else
#define LOG_TIME (double)0.
#endif

#define LOG_FRAME_START_ARGS num, LOG_TIME, viCount
#define LOG_FRAME_END_ARGS numFrames, overflowed, filename.c_str()

  if (count == 4)
    logFromISR(LOG_FRAME_START LOG_FRAME_BYTE LOG_FRAME_BYTE LOG_FRAME_BYTE LOG_FRAME_BYTE LOG_FRAME_END,
               LOG_FRAME_START_ARGS, dat[0], dat[1], dat[2], dat[3], LOG_FRAME_END_ARGS);
  else if (count == 2)
    logFromISR(LOG_FRAME_START LOG_FRAME_BYTE LOG_FRAME_BYTE LOG_FRAME_END,
               LOG_FRAME_START_ARGS, dat[0], dat[1], LOG_FRAME_END_ARGS);
  else if (count == 1)
    logFromISR(LOG_FRAME_START LOG_FRAME_BYTE LOG_FRAME_END,
               LOG_FRAME_START_ARGS, dat[0], LOG_FRAME_END_ARGS);

#ifdef FRAME_CB
  FRAME_CB(dat, count);
#endif
}

#ifdef HAVE_GPIO_COMMANDS
static bool setPinMode(const String& cmd)
{
  if (cmd.length() < 3)
    return false;
  if (cmd[1] != ':')
    return false;

  int pinNumber = cmd.substring(2).toInt();

  if (cmd[0] == 'I')
    pinMode(pinNumber, INPUT);
  else if (cmd[0] == 'O')
    pinMode(pinNumber, OUTPUT);
  else if (cmd[0] == 'U')
    pinMode(pinNumber, INPUT_PULLUP);
  else if (cmd[0] == 'D')
    pinMode(pinNumber, INPUT_PULLDOWN);
  else
    return false;

  return true;
}

static bool writePin(const String& cmd)
{
  if (cmd.length() < 3)
    return false;
  if (cmd[1] != ':')
    return false;

  int pinNumber = cmd.substring(2).toInt();

  if (cmd[0] == '0')
    digitalWrite(pinNumber, LOW);
  else if (cmd[0] == '1')
    digitalWrite(pinNumber, HIGH);
  else
    return false;

  return true;
}
#endif

#ifdef POWER_PIN
static bool setPower(const String& cmd)
{
  if (cmd.length() < 1)
    return false;

  if (cmd[0] == '0') {
    digitalWrite(POWER_PIN, LOW);
  } else if (cmd[0] == '1') {
    viCount = 0;
    timer64Started = 0;
    digitalWrite(POWER_PIN, HIGH);
  } else {
    return false;
  }

  return true;
}
#endif

static bool setLoop(const String& cmd)
{
  if (cmd.length() < 1)
    return false;

  if (cmd[0] == '0') {
    doLoop = 0;
  } else if (cmd[0] == '1') {
    doLoop = 1;
  } else {
    return false;
  }

  return true;
}

void setupConsole()
{
  if (console == N64) {
#ifdef N64_PIN
    attachInterrupt(N64_PIN, n64Interrupt, FALLING);
#endif
  } else if (console == NES || console == SNES) {
#ifdef SNES_LATCH_PIN
    attachInterrupt(SNES_LATCH_PIN, snesLatchInterrupt, RISING);
    attachInterrupt(SNES_CLOCK_PIN, snesClockInterrupt, RISING);
#endif
  }
}

static bool setConsole(const String& cmd)
{
  if (cmd == "N64")
    console = N64;
  else if (cmd == "NES")
    console = NES;
  else if (cmd == "SNES")
    console = SNES;
  else
    return false;

  setupConsole();

  return true;
}

String hextobin(const String& hex)
{
  String ret;
  ret.reserve((hex.length() + 1) / 2);

  // mapping of ASCII characters to hex values
  static const uint8_t map[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, // 01234567
    0x08, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 89:;<=>?
    0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x00, // @ABCDEFG
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // HIJKLMNO
  };

  for (size_t pos = 0; pos < hex.length(); pos += 2) {
    size_t idx0 = ((uint8_t)hex[pos]     & 0x1F) ^ 0x10,
           idx1 = ((uint8_t)hex[pos + 1] & 0x1F) ^ 0x10;
    ret += (char)((map[idx0] << 4) | map[idx1]);
  }

  return ret;
}

static SdFile writeFile;

static void appendData(const String& data) {
  if (writeFile.write(data.c_str(), data.length()) == data.length())
    Serial.println("AP:OK");
  else
    Serial.println("AP:NAK");
}

static void waitForIdle(unsigned us)
{
  LED_HIGH;
  N64_HIGH;
  startTimer();
  int lastReset = 0;
  while (readTimer() - lastReset < us * MICRO_CYCLES && readTimer() < us * MICRO_CYCLES * 4) {
    if (!N64_QUERY)
      lastReset = readTimer();
  }
  LED_LOW;
}

static String basename(const String& path) {
  int pos = path.lastIndexOf('/');
  if (pos < 0)
    return path;

  return path.substring(pos + 1);
}

static bool openTAS(const String& path) {
    char signature[4];
    int version;

    Serial.write("M:");
    Serial.println(path);

    // Open the file for reading:
    Serial.print(F("L:Opening file '"));
    Serial.print(path);
    Serial.println(F("'..."));

    Serial.flush();

    if (tasFile.isOpen())
      tasFile.close();

    // Error check
    if (!tasFile.open(&sd, path.c_str(), O_READ)) {
        Serial.println(F("E:Error in opening file"));
        return false;
    }

    // Open header
    if (tasFile.read(signature, 4) != 4 || tasFile.read(&version, 4) != 4) {
        tasFile.close();
        Serial.println(F("E:Failed to read signature"));
        return false;
    }

    // Validate file signature
    if (memcmp(signature, "M64\x1A", 4) != 0) {
        Serial.println(F("E:m64 signature invalid"));
        tasFile.close();
        return false;
    }

    // Print version
    Serial.print(F("L:M64 Version: "));
    Serial.println(version);

    Serial.flush();

    tasFile.seekSet(0x018);

    // Open header
    uint32_t newNumFrames;
    if (tasFile.read(&newNumFrames, 4) != 4) {
        tasFile.close();
        Serial.println(F("E:Failed to read frame count"));
        return false;
    }

    // Get header size
    switch(version) {
        case 1:
        case 2:
            tasFile.seekSet(0x200);
            break;
        case 3:
            tasFile.seekSet(0x400);
            break;
        default:
          // Unknown version
            Serial.println(F("E:unknown M64 version"));
            tasFile.close();
            return false;
    }

    // Final check
    if (!tasFile.available()) {
        Serial.println(F("E:No input data found in file"));
        tasFile.close();
        return false;
    }

    newNumFrames = min(newNumFrames, (tasFile.fileSize() - tasFile.curPosition()) / 4);

    Serial.write("N:");
    Serial.println(newNumFrames);
    Serial.flush();

    // Wait for the line to go idle, then begin listening
    /*
    for (int idle_wait=32; idle_wait>0; --idle_wait) {
        if (!N64_QUERY) {
            idle_wait = 32;
        }
    }*/

    noInterrupts();
    finished = false;
    initBuffers();
    curFrame = 0;
    numFrames = newNumFrames;
    console = N64;
    filename = basename(path);
    interrupts();

    setupConsole();

#ifdef POWER_PIN
    if (doLoop)
      setPower("1");
#endif

    return true;
}

static void updateInputBuffer() {
    if (finished || !tasFile.isOpen())
      return;

    // Check for file end
    if (!tasFile.available()) {
        tasFile.close();
        return;
    }

    size_t availableSize;
    size_t writePos = getNextRegion(availableSize);

    // Optimized chunk reads
    if ((availableSize < 512 && (writePos % 512) == 0) || !availableSize)
        return;

    int readBytes = tasFile.read(inputBuffer + writePos, availableSize);
    if (readBytes <= 0) {
      lockedPrintln(F("W:Failed to read next inputs from file. (This is recoverable)"));
      return;
    }

    noInterrupts();
    bufferEndPos = writePos + readBytes;
    bufferHasData = true;
    interrupts();
}

/**
 * Complete copy and paste of gc_send, but with the N64
 * pin being manipulated instead.
 */
static void n64_send(unsigned char *buffer, char length, bool wide_stop)
{
  unsigned long target;
  LED_HIGH;
  N64_LOW;
  startTimer();

  for (int i = 0; i < length * 8; i++) {
    char bit = (buffer[i >> 3] >> (7 - (i & 7))) & 1;
    target = MICRO_CYCLES * (3 - bit * 2);
    while (readTimer() < target);
      N64_HIGH;
    while (readTimer() < MICRO_CYCLES * 4);
      N64_LOW;
    startTimer();
  }

  target = MICRO_CYCLES * (1 + wide_stop);

  while (readTimer() < target);

  N64_HIGH;
  startTimer();
  while (!N64_QUERY && readTimer() < MICRO_CYCLES * 4);
  LED_LOW;
}

/**
  * Waits for an incomming signal on the N64 pin and reads the command,
  * and if necessary, any trailing bytes.
  * 0x00 is an identify request
  * 0x01 is a status request
  * 0x02 is a controller pack read
  * 0x03 is a controller pack write
  *
  * for 0x02 and 0x03, additional data is passed in after the command byte,
  * which is also read by this function.
  *
  * All data is raw dumped to the n64_raw_dump array, 1 bit per byte, except
  * for the command byte, which is placed all packed into n64_command
  */
static void get_n64_command()
{
    int bitcount = 8;
    n64_command = 0;
    char newByte = 0;

    LED_HIGH;

#define FAIL_TIMEOUT {\
  if (readTimer() >= MICRO_CYCLES * 10) {\
    n64_command = -1; \
    goto fail; \
  } \
}\

    edgesRead = 0;

    for (int i = 1; i <= bitcount; i++) {
        while (!N64_QUERY)
          FAIL_TIMEOUT;
        long lowTime = readAndResetTimer();
        if (!edgesRead)
            firstEdgeTime = lowTime;
        edgesRead++;
        while (N64_QUERY)
          FAIL_TIMEOUT;
        long highTime = readAndResetTimer();
        if (edgesRead == 1)
            secondEdgeTime = highTime;
        edgesRead++;
        char bit =
#ifdef USE_COMPARE_ABSOLUTE
            (highTime >= MICRO_CYCLES * 2)
#else
            (lowTime < highTime)
#endif
        ;
        newByte <<= 1;
        newByte |= bit;

        if (i == 8) {
          n64_command = newByte;
          switch (newByte) {
              case (0x03):
                  // write command
                  // we expect a 2 byte address and 32 bytes of data
                  bitcount += 272; // 34 bytes * 8 bits per byte
                  //Serial.println("command is 0x03, write");
                  break;
              case (0x02):
                  // read command 0x02
                  // we expect a 2 byte address
                  bitcount += 16;
                  //Serial.println("command is 0x02, read");
                  break;
              case (0x00):
              case (0x01):
              default:
                  // get the last (stop) bit
                  break;
          }
        } else if (!(i & 7)) {
          n64_raw_dump[(i >> 3) - 1] = newByte;
        }
    }

    // Wait for the stop bit
    while (!N64_QUERY)
      FAIL_TIMEOUT;
    startTimer();
    while (readTimer() < MICRO_CYCLES * 2);

fail:
    incompleteCommand = newByte;

    LED_LOW;
}

static void n64Interrupt()
{
    noInterrupts();

    unsigned char data, addr;
    int ticksSinceLast = readAndResetTimer();
    bool haveFrame;

#ifdef DISABLE_N64_INTERRUPT
    DISABLE_N64_INTERRUPT();
#endif

    // Wait for incoming 64 command
    // this will block until the N64 sends us a command
    get_n64_command();

    // 0x00 is identify command
    // 0x01 is status
    // 0x02 is read
    // 0x03 is write
    switch (n64_command)
    {
        case 0x00:
        case 0xFF:
            // identify
            // mutilate the output_buffer array with our status
            // we return 0x050001 to indicate we have a rumble pack
            // or 0x050002 to indicate the expansion slot is empty
            //
            // 0xFF I've seen sent from Mario 64 and Shadows of the Empire.
            // I don't know why it's different, but the controllers seem to
            // send a set of status bytes afterwards the same as 0x00, and
            // it won't work without it.
            output_buffer[0] = 0x05;
            output_buffer[1] = 0x00;
            output_buffer[2] = 0x01;

            n64_send(output_buffer, 3, 1);
            logFromISR("P:%ld", viCount);
            viCount = 0;
#ifdef PIT_LTMR64H
            start64Timer();
#endif
            break;
        case 0x01:
            haveFrame = (!finished && bufferHasData);
            // If the TAS is finished, there's nothing left to do.
            if (haveFrame)
              memcpy(output_buffer, inputBuffer + bufferPos, 4);
            else
              memset(output_buffer, 0, 4);

            // blast out the pre-assembled array in output_buffer
            n64_send(output_buffer, 4, 1);

            logFrame(output_buffer, 4, curFrame + (haveFrame ? 1 : 0));

            if (!haveFrame)
              break;

            // update input buffer and make sure it doesn't take too long

           /*Serial.print(F("Pos: "));
            Serial.print(bufferPos);
            Serial.print(F(" Data: "));
            Serial.println(inputBuffer[bufferPos]);*/

            advanceBuffer(4);

//            while (Serial.availableForWrite() && readTimer() < 10 * 1000 * MICRO_CYCLES && N64_QUERY);

/*            if (!finished)
              curPos = (curFrame * screen.width()) / numFrames;
            screen.fill(255,0,0);
            while (progressPos < curPos) {
              screen.point(progressPos++, screen.height() - 1);
            }
            screen.fill(255,255,255);*/

            break;
        case 0x02:
            // A read. If the address is 0x8000, return 32 bytes of 0x80 bytes,
            // and a CRC byte.  this tells the system our attached controller
            // pack is a rumble pack

            // Assume it's a read for 0x8000, which is the only thing it should
            // be requesting anyways
            memset(output_buffer, 0x80, 32);
            output_buffer[32] = 0xB8; // CRC

            n64_send(output_buffer, 33, 1);
            logFromISR("L:Got a read, what?");

            //Serial.println("It was 0x02: the read command");
            break;
        case 0x03:
            // A write. we at least need to respond with a single CRC byte.  If
            // the write was to address 0xC000 and the data was 0x01, turn on
            // rumble! All other write addresses are ignored. (but we still
            // need to return a CRC)

            // decode the first data byte (fourth overall byte), bits indexed
            // at 24 through 31
            data = 0;
            data |= (n64_raw_dump[16] != 0) << 7;
            data |= (n64_raw_dump[17] != 0) << 6;
            data |= (n64_raw_dump[18] != 0) << 5;
            data |= (n64_raw_dump[19] != 0) << 4;
            data |= (n64_raw_dump[20] != 0) << 3;
            data |= (n64_raw_dump[21] != 0) << 2;
            data |= (n64_raw_dump[22] != 0) << 1;
            data |= (n64_raw_dump[23] != 0);

            // get crc byte, invert it, as per the protocol for
            // having a memory card attached
            output_buffer[0] = crc_repeating_table[data] ^ 0xFF;

            // send it
            n64_send(output_buffer, 1, 1);
            logFromISR("L:Got a write, what?");

            // end of time critical code
            // was the address the rumble latch at 0xC000?
            // decode the first half of the address, bits
            // 8 through 15
            addr = 0;
            addr |= (n64_raw_dump[0] != 0) << 7;
            addr |= (n64_raw_dump[1] != 0) << 6;
            addr |= (n64_raw_dump[2] != 0) << 5;
            addr |= (n64_raw_dump[3] != 0) << 4;
            addr |= (n64_raw_dump[4] != 0) << 3;
            addr |= (n64_raw_dump[5] != 0) << 2;
            addr |= (n64_raw_dump[6] != 0) << 1;
            addr |= (n64_raw_dump[7] != 0);

            //Serial.println("It was 0x03: the write command");
            //Serial.print("Addr was 0x");
            //Serial.print(addr, HEX);
            //Serial.print(" and data was 0x");
            //Serial.println(data, HEX);
            break;

        case -1:
            if (finished)
              break;
            logFromISR("W:RTO; %d read; b:%02hhx; %d since; %d/%d to 1/2", edgesRead, incompleteCommand, ticksSinceLast, firstEdgeTime, secondEdgeTime);
            if (curFrame > 100)
              logFromISR("D:timeout");
            waitForIdle(1000);
            break;

        default:
            if (finished)
              break;
            logFromISR("W:Unknown; %d read; b:%02hhx; %d since; %d/%d to 1/2", edgesRead, n64_command, ticksSinceLast, firstEdgeTime, secondEdgeTime);
            if (curFrame > 100)
              logFromISR("D:inval");
            waitForIdle(1000);
            break;
    }

#ifdef ENABLE_N64_INTERRUPT
    ENABLE_N64_INTERRUPT();
#endif

    interrupts();
}

#ifdef SNES_DATA_PIN
static void snesWriteBit()
{
  digitalWrite(SNES_DATA_PIN, (currentSnesFrame & 0x8000) ? LOW : HIGH);
  if (currentSnesFrame & 0x8000)
    LED_HIGH;
  else
    LED_LOW;
  currentSnesFrame <<= 1;
  currentSnesFrame |= 1;
}

static void snesLatchInterrupt()
{
  noInterrupts();
  int haveFrame = (!finished && bufferHasData);
  int len = (console == SNES) ? 2 : 1;
  if (!haveFrame) {
    unsigned char logBuf[2] = {0, 0};
    currentSnesFrame = 0;
    logFrame(logBuf, len, curFrame + 1);
  } else {
    if (console == NES) {
      currentSnesFrame = (inputBuffer[bufferPos] << 8) | 0xFF;
    } else {
      currentSnesFrame = (inputBuffer[bufferPos] << 8) | inputBuffer[bufferPos + 1];
    }
    logFrame(inputBuffer + bufferPos, len, curFrame + 1);
  }
  snesWriteBit();
  if (readTimer() >= frameDuration) {
    startTimer();
    if (haveFrame)
      advanceBuffer(len);
  }
  interrupts();
}

static void snesClockInterrupt()
{
  noInterrupts();
  snesWriteBit();
  interrupts();
}
#endif

#ifdef HAVE_EEPROM
static void setEEPROM(const String& cmd)
{
  // Write terminator first, so we won't overread (by much) if we die early
  EEPROM.write(cmd.length(), 0);
  unsigned j = 0;
  for (unsigned i = 0; i < cmd.length(); i++, j++) {
    if (cmd[i] == '\\' && cmd[i + 1] == 'n') {
      EEPROM.write(j, '\n');
      i++;
    } else if (cmd[i] == '\\' && cmd[i + 1] == '\\') {
      EEPROM.write(j, '\\');
      i++;
    } else {
      EEPROM.write(j, cmd[i]);
    }
  }
  EEPROM.write(j, 0);

  lockedPrintln("L:Finished writing to EEPROM:", cmd.c_str());
}
#endif

static void handleCommand(const String& cmd)
{
  if (cmd.startsWith("M:")) {
    openTAS(cmd.substring(2));
  } else if (cmd.startsWith("O:")) {
    //dummy
  } else if (cmd.startsWith("L:")) {
    emitList(cmd.substring(2));
  } else if (cmd.startsWith("MK:")) {
    lockedPrintln("MK:", sd.mkdir(cmd.substring(3).c_str()));
  } else if (cmd.startsWith("CR:")) {
    if (writeFile.isOpen()) {
      writeFile.close();
    }
    if (writeFile.open(&sd, cmd.substring(3).c_str(), O_WRITE | O_CREAT | O_TRUNC)) {
      lockedPrintln("CR:OK");
    } else {
      lockedPrintln("CR:NAK");
    }
  } else if (cmd.startsWith("AP:")) {
    appendData(hextobin(cmd.substring(3)));
  } else if (cmd.startsWith("IN:")) {
    size_t len = appendInputs(hextobin(cmd.substring(3)));
    lockedPrintln("IN:", len);
  } else if (cmd.startsWith("HL:")) {
    hold = cmd[3] == '1';
  } else if (cmd.startsWith("CL:")) {
    writeFile.close();
    lockedPrintln("CL:OK");
#ifdef HAVE_GPIO_COMMANDS
  } else if (cmd.startsWith("PM:")) {
    setPinMode(cmd.substring(3));
  } else if (cmd.startsWith("DW:")) {
    writePin(cmd.substring(3));
#endif
#ifdef POWER_PIN
  } else if (cmd.startsWith("PW:")) {
    setPower(cmd.substring(3));
#endif
  } else if (cmd.startsWith("SC:")) {
    setConsole(cmd.substring(3));
#ifdef HAVE_EEPROM
  } else if (cmd.startsWith("WN:")) {
    setEEPROM(cmd.substring(3));
#endif
  } else if (cmd.startsWith("LO:")) {
    setLoop(cmd.substring(3));
  } else {
    lockedPrintln("E:Unknown CMD:", cmd.c_str());
  }
}

static void handleChar(int newChar)
{
  if (newChar == '\n') {
    if (skippingInput)
      Serial.println("E:Skipped too-long line");
    else
      handleCommand(inputString);
    inputString = "";
    skippingInput = false;
  } else if (inputString.length() > MAX_CMD_LEN) {
    skippingInput = true;
  } else {
    inputString += (char)newChar;
  }
}

static void inputLoop()
{
  int newChar;
  int charsRead = 0;
  while ((newChar = Serial.read()) != -1 && (charsRead++ <= MAX_LOOP_LEN))
    handleChar(newChar);
}

static void mainLoop()
{
  updateInputBuffer();

  // Record if it took longer than expected
  /*updateTime = readTimer();
  if (updateTime > 1000 * MICRO_CYCLES) {
      Serial.print(F("Input buffer update took too long ("));
      Serial.print(updateTime / MICRO_CYCLES);
      Serial.println(F(" us)"));
  }*/
}

#ifdef HAVE_EEPROM
static void runEEPROM()
{
  int i = 0;
  while (i < 1000) {
    int c = EEPROM.read(i);
    if (!c)
      break;
    handleChar(c);
    i++;
  }
  if (i)
    handleChar('\n');
}
#endif

void loop()
{
  inputLoop();
  mainLoop();
#ifdef HAVE_EEPROM
  if (doLoop) {
    static bool isClearing = false;
    uint64_t curTime = read64Timer();

    if ((finished && timer64Started && (curTime - finalTime) > (MICRO_BUS_CYCLES * 1000 * 1000 * (isClearing ? 1 : 30)))
//        || (timer64Started && (curTime - lastFrameTime) > (MICRO_BUS_CYCLES * 1000 * 1000 * 30))
       ) {
      setPower("0");
      while ((read64Timer() - curTime) < (MICRO_BUS_CYCLES * 1000 * 1000 * 2));
      isClearing = !isClearing;
      if (isClearing)
        openTAS("clear.m64");
      else
        runEEPROM();
    }
  }
#endif

  bool first = true;
  bool gotData = false;
  do {
    int i = 0;
    gotData = false;
    if (first) {
      for (; i < N_SER_BUFS; i++) {
        if (!bufHasData[i])
          break;
        else
          gotData = true;
      }
    }
    first = false;
    for (; i < N_SER_BUFS; i++) {
      if (bufHasData[i]) {
#ifdef LOG_CB
        LOG_CB(&serBuffer[i][0]);
#endif
        Serial.println(&serBuffer[i][0]);
        bufHasData[i] = 0;
        Serial.flush();
        gotData = true;
      }
    }
  } while (gotData);
}

#ifdef VI_PIN
static void viInterrupt()
{
  viCount++;
}
#endif

void printSDError(const char* msg) {
  (void)msg;
  if (sd.sdErrorCode()) {
    if (sd.sdErrorCode() == SD_CARD_ERROR_ACMD41) {
      Serial.println("Try power cycling the SD card.");
    }
    printSdErrorSymbol(&Serial, sd.sdErrorCode());
    Serial.print(", ErrorData: 0X");
    Serial.println(sd.sdErrorData(), HEX);
  }
}

void setup()
{
#ifdef ARM_DEMCR
  ARM_DEMCR |= ARM_DEMCR_TRCENA;
#endif
#ifdef ARM_DWT_CTRL
  ARM_DWT_CTRL |= ARM_DWT_CTRL_CYCCNTENA;
#endif

  startTimer();

#ifdef SERIAL_TIMEOUT_US
  while (readTimer() > MICRO_CYCLES * 1000 * 1000);
#endif

  Serial.begin(SERIAL_BAUD_RATE);

#ifdef SERIAL_TIMEOUT_US
  while (!Serial) {
    if (readTimer() > MICRO_CYCLES * SERIAL_TIMEOUT_US) {
      doLoop = 1;
      break;
    }
  } // wait for serial port to connect. Needed for native USB port only

  Serial.print(F("L:Serial init timer exited: doLoop="));
  Serial.println(doLoop);
#endif

  LED_HIGH;

  // Status LED
#ifdef STATUS_PIN
  digitalWrite(STATUS_PIN, LOW);
  pinMode(STATUS_PIN, OUTPUT);
#endif

  // Configure controller pins
#ifdef N64_PIN
  pinMode(N64_PIN, INPUT_PULLUP);
  digitalWrite(N64_PIN, LOW);
#endif

#ifdef SNES_LATCH_PIN
  pinMode(SNES_LATCH_PIN, INPUT);
  pinMode(SNES_CLOCK_PIN, INPUT);
  pinMode(SNES_DATA_PIN, OUTPUT);
  digitalWrite(SNES_DATA_PIN, LOW);
#endif

  // Power
#ifdef POWER_PIN
  digitalWrite(POWER_PIN, LOW);
  pinMode(POWER_PIN, OUTPUT);
#endif

  // VI pulse
#ifdef VI_PIN
  pinMode(VI_PIN, INPUT);
  attachInterrupt(VI_PIN, viInterrupt, FALLING);
#endif

  // Let the controller pins interrupt anything else
#ifdef IRQ_PORTC
  NVIC_SET_PRIORITY(IRQ_PORTC, 0);
#endif

  // Let the VI pin interrupt anything other than controller pins
//  NVIC_SET_PRIORITY(IRQ_PORTC, 16);

#ifndef SD_CONFIG
#define SD_CONFIG BUILTIN_SDCARD
#endif

  // Initialize SD card
  if (!sd.begin(SD_CONFIG)) {
    printSDError("init");
    Serial.println(F("E:SD initialization failed!"));
    return;
  }
  Serial.println(F("L:SD initialization done."));

  // Setup buffer
  initBuffers();

  Serial.println(F("L:Initialization done."));

#ifdef HAVE_EEPROM
  if (doLoop)
    runEEPROM();
#endif
}
