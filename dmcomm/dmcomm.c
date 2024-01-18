/*
  DMComm: a project using Arduino to communicate with Digimon toys
  Copyright (c) 2014,2017,2018,2020,2021 BladeSabre

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.
*/

#include "dmcomm.h"

#define BOARD_3V3

//pin assignments

//const byte led_pin = 13;
const GpioPin*  dm_pin_out = &gpio_ext_pc3;
const GpioPin*  dm_pin_notOE = &gpio_ext_pb3;
const GpioPin*  dm_pin_Ain = &gpio_ext_pa4; //no analog pins on flipper but we can still sense v-high
const GpioPin*  probe_pin = &gpio_ext_pb2;


//durations, in microseconds unless otherwise specified

const unsigned int tick_length = 200;
const unsigned long listen_timeout = 3000000;

const unsigned int gofirst_repeat_ms = 5000;
const unsigned int serial_timeout_ms = 6000;
const unsigned int inactive_delay_ms = 250;
const unsigned int ended_capture_ms = 500;


//buffer sizes

#define command_buffer_size 64 //no point having it bigger than system serial buffer
#define log_length 1000
#define HIGH true
#define LOW false


//bytes for log protocol

const byte log_prefix_low   = 0b00000000;
const byte log_prefix_high  = 0b01000000;
const byte log_prefix_again = 0b10000000;
const byte log_prefix_other = 0b11000000;
const byte log_max_count    = 0b00111111;
const byte log_count_bits   = 6;
const byte log_prefix_tick_overrun = 0xF0;
const byte log_max_ticks_missed = 0x0F;

const byte log_opp_enter_wait = 0xC0;
const byte log_opp_init_pulldown = 0xC1;
const byte log_opp_start_bit_high = 0xC2;
const byte log_opp_start_bit_low = 0xC3;
const byte log_opp_bits_begin_high = 0xC4;
const byte log_opp_got_bit_0 = 0xC5;
const byte log_opp_got_bit_1 = 0xC6;
const byte log_opp_exit_ok = 0xC8;
const byte log_opp_exit_fail = 0xC9;

const byte log_self_enter_delay = 0xE0;
const byte log_self_init_pulldown = 0xE1;
const byte log_self_start_bit_high = 0xE2;
const byte log_self_start_bit_low = 0xE3;
const byte log_self_send_bit_0 = 0xE5;
const byte log_self_send_bit_1 = 0xE6;
const byte log_self_release = 0xE7;


//globals

byte logBuf[log_length];
int logIndex;
byte prevSensorLevel;
long ticksSame;
enum circuitTypes {dcom, acom, uncom} circuitType;
enum debugModes {debug_off, debug_digital, debug_analog} debugMode;
byte currentPacketIndex;
byte triggerPacketIndex;

struct dm_times_t {
    int8_t timing_id;
    byte logic_high, logic_low;
    bool invert_bit_read;
    byte sensor_threshold;
    unsigned long pre_high, pre_low;
    unsigned long start_high, start_low, bit1_high, bit1_low, bit0_high, bit0_low, send_recovery;
    unsigned long bit1_high_min;
    unsigned long timeout_reply, timeout_bits, timeout_bit;
} dm_times;


//return integer value of hex digit character, or -1 if not a hex digit
int8_t hex2val(int8_t hexdigit) {
    int8_t value;
    if (hexdigit >= '0' && hexdigit <= '9') {
        value = hexdigit - 0x30;
    } else if (hexdigit >= 'a' && hexdigit <= 'f') {
        value = hexdigit - 0x57;
    } else if (hexdigit >= 'A' && hexdigit <= 'F') {
        value = hexdigit - 0x37;
    } else {
        value = -1;
    }
    return value;
}



//return hex digit character for lowest 4 bits of input byte
int8_t val2hex(int8_t value) {
    value &= 0xF;
    if (value > 9) {
        return value + 0x37;
    } else {
        return value + 0x30;
    }
}


//print number onto serial as hex,
//with specified number of digits up to 4 (with leading zeros);
//if too few digits to display that number, will take the least significant
void serialPrintHex(unsigned int number, byte numDigits) {
    const byte maxDigits = 4;
    int8_t i;
    int8_t digits[maxDigits];
    if (numDigits > maxDigits) {
        numDigits = maxDigits;
    }
    for (i = 0; i < numDigits; i ++) {
        digits[i] = val2hex((byte)number);
        number /= 0x10;
    }
    for (i = numDigits - 1; i >= 0; i --) {
        Serial_writei(digits[i]);
    }
}

//configure durations depending on whether we are using X timings or not
void initDmTimes(int8_t timingID) {
    dm_times.timing_id = timingID;
    dm_times.timeout_reply = 100000;
    dm_times.timeout_bits = 200000; 
    if (timingID == 'V') {
        dm_times.logic_high = HIGH;
        dm_times.logic_low = LOW;
        dm_times.invert_bit_read = false;
        dm_times.pre_high = 3000;
        dm_times.pre_low = 59000;
        dm_times.start_high = 2083;
        dm_times.start_low = 917;
        dm_times.bit1_high = 2667;
        dm_times.bit1_low = 1667;
        dm_times.bit0_high = 1000;
        dm_times.bit0_low = 3167;
        dm_times.send_recovery = 400;
        dm_times.bit1_high_min = 1833;
        dm_times.timeout_bit = 5000;
    } else if (timingID == 'X') {
        dm_times.logic_high = HIGH;
        dm_times.logic_low = LOW;
        dm_times.invert_bit_read = false;
        dm_times.pre_high = 3000;
        dm_times.pre_low = 60000;
        dm_times.start_high = 2200;
        dm_times.start_low = 1600;
        dm_times.bit1_high = 4000;
        dm_times.bit1_low = 1600;
        dm_times.bit0_high = 1600;
        dm_times.bit0_low = 4000;
        dm_times.send_recovery = 400;
        dm_times.bit1_high_min = 2600;
        dm_times.timeout_bit = 7000;
    } else {
        //assuming 'Y'
        dm_times.logic_high = LOW;
        dm_times.logic_low = HIGH;
        dm_times.invert_bit_read = true;
        dm_times.pre_high = 5000;
        dm_times.pre_low = 40000;
        dm_times.start_high = 11000;
        dm_times.start_low = 6000;
        dm_times.bit1_high = 1400;
        dm_times.bit1_low = 4400;
        dm_times.bit0_high = 4000;
        dm_times.bit0_low = 1600;
        dm_times.send_recovery = 200;
        dm_times.bit1_high_min = 3000;
        dm_times.timeout_bit = 20000;
    }
    //this is a 6-bit scale from 0-3.3V
    //keeping this logic separate because it's likely to grow
    if (timingID == 'Y') {
        dm_times.sensor_threshold = 25; //1.3V approx
    } else {
        dm_times.sensor_threshold = 37; //1.9V approx
    }
}

void busDriveLow() {
    digitalWrite(dm_pin_out, dm_times.logic_low);
    digitalWrite(dm_pin_notOE, LOW);
}

void busDriveHigh() {
    digitalWrite(dm_pin_out, dm_times.logic_high);
    digitalWrite(dm_pin_notOE, LOW);
}

void busRelease() {
    digitalWrite(dm_pin_notOE, HIGH);
    digitalWrite(dm_pin_out, dm_times.logic_high);
    //dm_pin_out is "don't care" on D-Com, but matters for A-Com
}

//add specified byte to the log
void addLogByte(byte b) {
    if (currentPacketIndex >= triggerPacketIndex && logIndex < log_length) {
        logBuf[logIndex] = b;
        logIndex ++;
    }
}

//add current sensor level and time to the log (may be multiple bytes)
//and initialize next timing
void addLogTime() {
    if (ticksSame == 0) {
        return;
    }
    if (debugMode == debug_analog) {
        addLogByte(prevSensorLevel);
        ticksSame --;
    } else {
        byte log_prefix = (prevSensorLevel == LOW) ? log_prefix_low : log_prefix_high;
        addLogByte((ticksSame & log_max_count) | log_prefix);
        ticksSame >>= log_count_bits;
    }
    while (ticksSame > 0) {
        addLogByte((ticksSame & log_max_count) | log_prefix_again);
        ticksSame >>= log_count_bits;
    }
}

//add specified byte to the log, making sure current timing information is logged first
void addLogEvent(byte b) {
    addLogTime();
    addLogByte(b);
}

//scale 10-bit ADC to 6 bits 0-3.3V
byte scaleSensorValue(unsigned int sensorValue) {
#ifdef BOARD_3V3
    sensorValue = sensorValue / 16;
#else
    sensorValue = sensorValue * 3 / 32;
#endif
    if (sensorValue > 63) {
        sensorValue = 63;
    }
    return (byte)sensorValue;
}

//read analog input and do logging, clocked by tick_length;
//return current logic level measured
byte doTick(boolean first) {
    static unsigned long prev_micros = 0;
    static int ticks = 0;
    
    unsigned int sensorValue;
    byte sensorLevelScaled;
    byte sensorLevelDigital;
    byte sensorLevelForLog;
    byte ticksMissed = 0;
    
    sensorValue = analogRead(dm_pin_Ain);
    
    //counts missed ticks for log (but delayByTicks does not currently account for them)
    while (ticksMissed <= log_max_ticks_missed && micros() - prev_micros > tick_length) {
        prev_micros += tick_length;
        ticks ++;
        ticksMissed ++;
    }
    if (ticksMissed <= log_max_ticks_missed) {
        while(micros() - prev_micros < tick_length);
        prev_micros += tick_length;
        ticks ++;
    } else {
        prev_micros = micros();
    }
    if (ticksMissed != 0 && !first) {
        if (ticksMissed > log_max_ticks_missed) {
            ticksMissed = 0;
        }
        addLogEvent(log_prefix_tick_overrun | ticksMissed);
    }
    
    if (ticks % 2 == 0) {
        digitalWrite(probe_pin, LOW);
    } else {
        digitalWrite(probe_pin, HIGH);
    }
    
    sensorLevelScaled = scaleSensorValue(sensorValue);
    
    if (sensorLevelScaled >= dm_times.sensor_threshold) {
        sensorLevelDigital = HIGH;
    } else {
        sensorLevelDigital = LOW;
    }
    
    if (debugMode == debug_analog) {
        sensorLevelForLog = sensorLevelScaled;
    } else {
        sensorLevelForLog = sensorLevelDigital;
    }
    
    if (sensorLevelForLog != prevSensorLevel) {
        addLogTime();
        ticksSame = 1;
    } else {
        ticksSame ++;
    }
    prevSensorLevel = sensorLevelForLog;
    
    if (sensorLevelDigital == HIGH) {
        return dm_times.logic_high;
    } else {
        return dm_times.logic_low;
    }
}

//initialize logging for a new run
void startLog() {
    logIndex = 0;
    ticksSame = 0;
    currentPacketIndex = 0;
}

//delay by specified number of microseconds (with resolution of tick_length)
//while logging each tick
void delayByTicks(unsigned long delay_micros) {
    unsigned long delay_ticks = delay_micros / tick_length;
    unsigned long i;
    for (i = 0; i < delay_ticks; i ++) {
        doTick(false);
    }
}

void setup() {     
    //Serial.begin(9600);

    furi_hal_gpio_init(probe_pin, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_init(dm_pin_out, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_init(dm_pin_notOE, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_init(dm_pin_Ain, GpioModeInput, GpioPullNo, GpioSpeedVeryHigh);
    //pinMode(probe_pin, OUTPUT);
    //pinMode(dm_pin_out, OUTPUT);
    //pinMode(dm_pin_notOE, OUTPUT);
    //pinMode(dm_pin_Ain, INPUT);
    ledOn();
    initDmTimes('V');
    busRelease();
}


//receiving

//wait until measured input equals level, or timeout microseconds;
//return time taken in microseconds
unsigned long busWaitTime(byte level, unsigned long timeout) {
    unsigned long timeStart = micros();
    unsigned long time;
    byte logicLevel;
    do {
        logicLevel = doTick(false);
        time = micros() - timeStart;
    } while (logicLevel != level && time <= timeout);
    return time;
}

//wait until measured input equals level, or timeout microseconds;
//return true if timeout was reached, false otherwise
boolean busWait(byte level, unsigned long timeout) {
    unsigned long time = busWaitTime(level, timeout);
    return (time > timeout);
}

//receive one bit, and rotate into bits from the left;
//return 0 on success, 1 on bit error, 2 on error after receiving bit
byte rcvBit(unsigned int * bits) {
    unsigned long time;
    unsigned long timeout = dm_times.timeout_bit;
    boolean bit0 = false;
    time = busWaitTime(LOW, timeout);
    if (time > timeout) {
        return 1;
    }
    if (time > dm_times.bit1_high_min) {
        bit0 = true;
    }
    if (dm_times.invert_bit_read) {
        bit0 = !bit0;
    }
    (*bits) >>= 1;
    if (bit0) {
        addLogEvent(log_opp_got_bit_1);
        (*bits) |= 0x8000;
    } else {
        addLogEvent(log_opp_got_bit_0);
    }
    if (busWait(HIGH, timeout)) {
        return 2;
    }
    return 0;
}

//receive a 16-bit packet,
//with specified timeout in microseconds,
//reporting results on serial and storing into bits parameter;
//return 0 on success, non-zero error code on failure
int rcvPacketGet(unsigned int * bits, unsigned long timeout1) {
    byte i, r;
    if (timeout1 == 0) {
        timeout1 = dm_times.timeout_reply;
    }
    addLogEvent(log_opp_enter_wait);
    if (busWait(LOW, timeout1)) {
        addLogEvent(log_opp_exit_fail);
        Serial_prints(F("t "));
        return -4;
    }
    currentPacketIndex ++;
    addLogEvent(log_opp_init_pulldown);
    if (busWait(HIGH, dm_times.timeout_bits)) {
        addLogEvent(log_opp_exit_fail);
        Serial_prints(F("t:-3 "));
        return -3;
    }
    addLogEvent(log_opp_start_bit_high);
    if (busWait(LOW, dm_times.timeout_bit)) {
        addLogEvent(log_opp_exit_fail);
        Serial_prints(F("t:-2 "));
        return -2;
    }
    addLogEvent(log_opp_start_bit_low);
    if (busWait(HIGH, dm_times.timeout_bit)) {
        addLogEvent(log_opp_exit_fail);
        Serial_prints(F("t:-1 "));
        return -1;
    }
    addLogEvent(log_opp_bits_begin_high);
    for (i = 0; i < 16; i ++) {
        r = rcvBit(bits);
        if (r == 2 && i == 15) {
            //opp didn't release at end of packet
            Serial_prints(F("r:"));
            serialPrintHex(*bits, 4);
            Serial_prints(F("t "));
            addLogEvent(log_opp_exit_fail);
            return 16;
        } else if (r != 0) {
            //packet failed
            r = i + r - 1; //number of bits received
            Serial_prints(F("t:"));
            Serial_printi(i);
            Serial_printi(':');
            serialPrintHex((*bits) >> (16-r), 4);
            Serial_printi(' ');
            addLogEvent(log_opp_exit_fail);
            return r;
        }
    }
    Serial_prints(F("r:"));
    serialPrintHex(*bits, 4);
    Serial_printi(' ');
    addLogEvent(log_opp_exit_ok);
    return 0;
}

//receive a 16-bit packet,
//with specified timeout in microseconds,
//reporting results on serial but discarding the data;
//return 0 on success, non-zero error code on failure
int rcvPacket(unsigned long timeout1) {
    unsigned int bits;
    int result;
    result = rcvPacketGet(&bits, timeout1);
    return result;
}


//sending

//send one bit
void sendBit(unsigned int bit) {
    addLogEvent(bit ? log_self_send_bit_1 : log_self_send_bit_0);
    delayByTicks(bit ? dm_times.bit1_high : dm_times.bit0_high);
    busDriveLow();
    delayByTicks(bit ? dm_times.bit1_low : dm_times.bit0_low);
    busDriveHigh();
}

//send a 16-bit packet
void sendPacket(unsigned int bits) {
    byte i;
    Serial_prints(F("s:"));
    serialPrintHex(bits, 4);
    Serial_printi(' ');
    currentPacketIndex ++;
    addLogEvent(log_self_enter_delay);
    delayByTicks(dm_times.pre_high);
    
    addLogEvent(log_self_init_pulldown);
    busDriveLow();
    delayByTicks(dm_times.pre_low);
    
    addLogEvent(log_self_start_bit_high);
    busDriveHigh();
    delayByTicks(dm_times.start_high);
    
    addLogEvent(log_self_start_bit_low);
    busDriveLow();
    delayByTicks(dm_times.start_low);
    
    busDriveHigh();
    for (i = 0; i < 16; i ++) {
        sendBit(bits & 1);
        bits >>= 1;
    }
    delayByTicks(dm_times.send_recovery);
    
    addLogEvent(log_self_release);
    busRelease();
}


//high-level communication functions

//read next packet from buffer; write bits to send into bitsNew and update checksum
//bitsRcvd is the packet just received (ignored except with ^ token)
//return number of bytes read from buffer on success, 0 if empty, -1 on failure
int makePacket(unsigned int * bitsNew, int8_t * checksum, unsigned int bitsRcvd, byte * buffer) {
    int8_t digits[4];
    int8_t dCur;
    int8_t dCurChk = -1;
    int8_t chkTarget = -1;
    int bufCur = 1;
    byte b1, b2;
    int8_t val1, val2;
    if (buffer[0] == '\0') {
        return 0;
    }
    //require first character dash
    if (buffer[0] != '-') {
        return -1;
    }
    //unpack bitsRcvd into digits
    for (dCur = 3; dCur >= 0; dCur --) {
        digits[dCur] = bitsRcvd & 0xF;
        bitsRcvd >>= 4;
    }
    for (dCur = 0; dCur < 4; dCur ++) {
        b1 = buffer[bufCur];
        if (b1 == '\0') {
            return -1;
        }
        bufCur ++;
        b2 = buffer[bufCur];
        val1 = hex2val(b1);
        val2 = hex2val(b2);
        if (b1 == '@' || b1 == '^') {
            //expect a hex digit to follow
            if (val2 == -1) {
                return -1;
            }
            if (b1 == '@') {
                //here is check digit
                dCurChk = dCur;
                chkTarget = val2;
            } else {
                //xor received bits with new value
                digits[dCur] ^= val2;
            }
            bufCur ++; //extra digit taken
        } else {
            //expect this to be a hex digit
            if (val1 == -1) {
                return -1;
            }
            //store (overwrite) digit
            digits[dCur] = val1;
        }
        if (b1 != '@') {
            //update checksum
            (*checksum) += digits[dCur];
            (*checksum) &= 0xF;
        }
    }
    if (dCurChk != -1) {
        //we have a check digit
        digits[dCurChk] = (chkTarget - (*checksum)) & 0xF;
        (*checksum) = chkTarget;
    }
    //pack digits into bitsNew
    (*bitsNew) = 0;
    for (dCur = 0; dCur < 4; dCur ++) {
        (*bitsNew) <<= 4;
        (*bitsNew) |= digits[dCur];
    }
    return bufCur;
}

//just listen for sequences of incoming messages,
//e.g. if only the first packet is wanted, or listening to 2 toys
void commListen() {
    int result;
    result = rcvPacket(listen_timeout);
    ledOff();
    while (result == 0 || result >= 13) {
        result = rcvPacket(0);
    }
    delayByTicks((long)ended_capture_ms * 1000);
    ledOn();
    Serial_printlns("");
    return;
}

//interact with the toy on the other end of the connection
void commBasic(boolean goFirst, byte * buffer) {
    unsigned int bitsRcvd = 0;
    unsigned int bitsToSend = 0;
    int8_t checksum = 0;
    int bufCur = 2;
    int result;
    if (!goFirst) {
        if (rcvPacketGet(&bitsRcvd, listen_timeout)) {
            Serial_printlns("");
            return;
        }
    }
    ledOff();
    while (1) {
        result = makePacket(&bitsToSend, &checksum, bitsRcvd, buffer + bufCur);
        if (result == 0) {
            //the end
            break;
        }
        if (result == -1) {
            //makePacket error
            Serial_prints(F("s:?"));
            break;
        }
        bufCur += result;
        sendPacket(bitsToSend);
        if (rcvPacketGet(&bitsRcvd, 0)) {
            break;
        }
    }
    delayByTicks((long)ended_capture_ms * 1000);
    ledOn();
    Serial_println();
}


//serial processing

//create trigger if we got a hex digit 1-F, and A/B, for a packet number
//otherwise disable trigger
void setupTrigger(int8_t packetNumChr, int8_t AB) {
    int8_t packetNum = hex2val(packetNumChr);
    boolean OK = (packetNum >= 1);
    triggerPacketIndex = packetNum * 2 - 1;
    if (AB == 'a' || AB == 'A') {
        //do nothing
    } else if (AB == 'b' || AB == 'B') {
        triggerPacketIndex ++;
    } else {
        OK = false;
    }
    if (!OK) {
        triggerPacketIndex = 0;
    }
}

//report trigger packet number with a hex digit and A/B
void serialPrintTrigger() {
    Serial_writei(val2hex((triggerPacketIndex + 1) / 2));
    if (triggerPacketIndex != 0) {
        if (triggerPacketIndex % 2 == 0) {
            Serial_writei('B');
        } else {
            Serial_writei('A');
        }
    }
}

//try to read from serial into buffer;
//read until end-of-line and replace that with a null terminator
//return 0 on failure, or positive integer for number of characters read
byte serialRead(byte * buffer) {
    unsigned long timeStart;
    unsigned long time;
    int incomingInt;
    byte incomingByte;
    byte i = 0;
    
    if (Serial_available() == 0) {
        return 0;
    }
    timeStart = millis();
    do {
        do {
            incomingInt = Serial_read();
            time = millis() - timeStart;
            if (time > serial_timeout_ms) {
                Serial_printlns(F("too late"));
                return 0;
            }
        } while (incomingInt == -1);
        incomingByte = incomingInt;
        if (incomingByte != '\r' && incomingByte != '\n') {
            buffer[i] = incomingByte;
            i += 1;
        }
    } while (incomingByte != '\r' && incomingByte != '\n' && i < command_buffer_size - 1);
    if (incomingByte != '\r' && incomingByte != '\n') {
        Serial_printlns(F("too long"));
        return 0;
    }
    buffer[i] = '\0';
    return i;
}


//main loop
void loop() {
    static boolean active = false;
    static int8_t timingID = 'V';
    static boolean listenOnly;
    static boolean goFirst;
    static byte numPackets;
    static byte buffer[command_buffer_size];

    int i;
    bool info = false;

    //process serial input
    i = serialRead(buffer);
    if (i > 0) {
        if ((buffer[0] == 'i' || buffer[0] == 'I') && buffer[1] == '\0') {
            Serial_printlns(DMCOMM_BUILD_INFO);
            info = true;
        } else {
            Serial_prints(F("got "));
            Serial_printi(i);
            Serial_prints(F(" bytes: "));
            Serial_writeb(buffer, i);
            Serial_prints(F(" -> "));
        }
        if (buffer[0] == 't' || buffer[0] == 'T') {
            Serial_printlns(F("[test voltages]"));
            //scanVoltages(true);
        }
        if ((buffer[0] == 'd' || buffer[0] == 'D') && i >= 2) {
            if (buffer[1] == '0' || buffer[1] == 'o' || buffer[1] == 'O') {
                Serial_prints(F("debug off "));
                debugMode = debug_off;
            } else if (buffer[1] == '1' || buffer[1] == 'd' || buffer[1] == 'D') {
                Serial_prints(F("debug digital "));
                debugMode = debug_digital;
            } else if (buffer[1] == '2' || buffer[1] == 'a' || buffer[1] == 'A') {
                Serial_prints(F("debug analog "));
                debugMode = debug_analog;
            }
            if (i >= 5 && buffer[2] == '-') {
                setupTrigger(buffer[3], buffer[4]);
            } else {
                setupTrigger(' ', ' ');
            }
            if (debugMode != debug_off) {
                Serial_prints(F("trigger="));
                serialPrintTrigger();
                Serial_printi(' ');
            }
        }
        active = true;
        if (buffer[0] == 'v' || buffer[0] == 'V') {
            timingID = 'V';
            Serial_printi('V');
        } else if (buffer[0] == 'x' || buffer[0] == 'X') {
            timingID = 'X';
            Serial_printi('X');
        } else if (buffer[0] == 'y' || buffer[0] == 'Y') {
            timingID = 'Y';
            Serial_printi('Y');
        } else {
            timingID = 'V';
            active = false;
        }
        if (i < 2 || (i < 5 && buffer[1] != '0')) {
            active = false;
        }
        if (active) {
            if (buffer[1] == '0') {
                listenOnly = true;
                goFirst = false;
                Serial_printi('0');
            } else if (buffer[1] == '1') {
                listenOnly = false;
                goFirst = true;
                Serial_printi('1');
            } else if (buffer[1] == '2') {
                listenOnly = false;
                goFirst = false;
                Serial_printi('2');
            } else {
                active = false;
                Serial_printi('?');
            }
        }
        if (i < 7 && !listenOnly) {
            active = false;
        }
        if (active && !listenOnly) {
            numPackets = 0;
            for(i = 2; buffer[i] != '\0'; i ++) {
                if (buffer[i] == '-') {
                    numPackets ++;
                }
            }
            Serial_prints(F("-["));
            Serial_printi(numPackets);
            Serial_prints(F(" packets]"));
        }
        if (!active && !info) {
            Serial_prints(F("(paused)"));
        }
        Serial_printlns("");
        if (active && goFirst) {
            delay(gofirst_repeat_ms);
        }
    }

    //do it
    startLog();
    initDmTimes(timingID);
    busRelease();
    if (active && doTick(true) == HIGH) {
        if (listenOnly) {
            commListen();
        } else {
            commBasic(goFirst, buffer);
        }
        if (debugMode != debug_off) {
            Serial_prints(F("p:timing="));
            Serial_writei(timingID);
            Serial_prints(F(" threshold="));
            Serial_printi(dm_times.sensor_threshold);
            Serial_prints(F(" trigger="));
            serialPrintTrigger();
            Serial_printlns("");
        }
        if (debugMode == debug_digital) {
            Serial_prints(F("d:"));
        }
        if (debugMode == debug_analog) {
            Serial_prints(F("a:"));
        }
        if (debugMode != debug_off) {
            for (i = 0; i < logIndex; i ++) {
                serialPrintHex(logBuf[i], 2);
                Serial_printi(' ');
            }
            Serial_printlns("");
        }
        if (goFirst) {
            delay(gofirst_repeat_ms);
        }
    } else {
        delay(inactive_delay_ms);
    }
}
