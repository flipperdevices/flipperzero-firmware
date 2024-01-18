#ifndef HAL_HEADERS
#define HAL_HEADERS

#include "flipper.h"

typedef uint8_t byte;
typedef bool boolean;

void boilerplate_led_set_rgb(void* context, int red, int green, int blue);

void boilerplate_led_reset(void* context);

void ledOn();
void ledOff();

void digitalWrite(const GpioPin* pin, bool level);

void Serial_prints(const char* c)
{
  UNUSED(c);
}
void Serial_printlns(const char* c)
{
  UNUSED(c);
}
void Serial_println(void)
{
}
void Serial_printi(const int c)
{
  UNUSED(c);
}
void Serial_printf(const float c, int acc)
{
  UNUSED(c);
  UNUSED(acc);
}

const char *F(const char* i)
{
  return i;
}

void delay(int ms)
{
  furi_delay_ms(ms);
}

int analogRead(const GpioPin* pin);

uint32_t micros()
{
  return furi_get_tick() * 1000 / furi_ms_to_ticks(1);
}

uint32_t millis()
{
  return furi_get_tick() / furi_ms_to_ticks(1);
}

void Serial_writei(int i)
{
  UNUSED(i);
}

int Serial_available(void)
{
  return 0;
}

int Serial_read(void)
{
  return 0;
}

void Serial_writeb(const byte* data, int len)
{
  UNUSED(data);
  UNUSED(len);
}
#endif