/*
Abstraction layer to convert arduino functions into their flipper zero alternatives.

Kinda janky but works well enough for this
*/
#ifndef HAL_HEADERS
#define HAL_HEADERS

#include "flipper.h"
#include "../app_state.h"

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

#define HIGH true
#define LOW false
#define INPUT 1
#define OUTPUT 0

typedef uint8_t byte;
typedef bool boolean;

typedef void (*DmcommCallback)();

void setSerialOutputCallback(DmcommCallback cb);
void setApp(void* ctx);

void boilerplate_led_set_rgb(int red, int green, int blue);
void boilerplate_led_reset();

void ledOn();
void ledOff();

void digitalWrite(const GpioPin* pin, bool level);

void Serial_prints(const char* c);
void Serial_printlns(const char* c);
void Serial_println(void);
void Serial_printc(const char c);
void Serial_printi(const int c);
EXTERNC const char *F(const char* i);
EXTERNC void delay(int ms);
EXTERNC void delayMicroseconds(int us);
int analogRead(const GpioPin* pin);
EXTERNC uint32_t micros();
EXTERNC uint32_t millis();
void Serial_writei(int i);
int Serial_available(void);
int Serial_read(void);
void Serial_writeb(const byte* data, int len);


#undef EXTERNC
#endif