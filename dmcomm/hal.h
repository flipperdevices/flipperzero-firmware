#ifndef HAL_HEADERS
#define HAL_HEADERS

#include "flipper.h"
#include "../app_state.h"

typedef uint8_t byte;
typedef bool boolean;

void setApp(void* ctx);

void boilerplate_led_set_rgb(int red, int green, int blue);

void boilerplate_led_reset();

void ledOn();
void ledOff();

void digitalWrite(const GpioPin* pin, bool level);

void Serial_prints(const char* c);
void Serial_printlns(const char* c);
void Serial_println(void);
void Serial_printi(const int c);
void Serial_printf(const float c, int acc);
const char *F(const char* i);
void delay(int ms);
int analogRead(const GpioPin* pin);
uint32_t micros();
uint32_t millis();
void Serial_writei(int i);
int Serial_available(void);
int Serial_read(void);
void Serial_writeb(const byte* data, int len);
#endif