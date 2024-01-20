#include "hal.h"
#include "../app_state.h"

void digitalWrite(const GpioPin* pin, bool level)
{
    furi_hal_gpio_write(pin, level);
}

int analogRead(const GpioPin* pin)
{
  if(furi_hal_gpio_read(pin))
    return 500;
  return 0;
}

void boilerplate_led_set_rgb(void* context, int red, int green, int blue) {
    App* app = context;

    NotificationMessage notification_led_message_1;
    notification_led_message_1.type = NotificationMessageTypeLedRed;
    NotificationMessage notification_led_message_2;
    notification_led_message_2.type = NotificationMessageTypeLedGreen;
    NotificationMessage notification_led_message_3;
    notification_led_message_3.type = NotificationMessageTypeLedBlue;

    notification_led_message_1.data.led.value = red;
    notification_led_message_2.data.led.value = green;
    notification_led_message_3.data.led.value = blue;
    const NotificationSequence notification_sequence = {
        &notification_led_message_1,
        &notification_led_message_2,
        &notification_led_message_3,
        &message_do_not_reset,
        NULL,
    };
    notification_message(app->notification, &notification_sequence);
    furi_thread_flags_wait(0, FuriFlagWaitAny, 10); //Delay, prevent removal from RAM before LED value set    
}

void boilerplate_led_reset(void* context) {
    App* app = context;
    notification_message(app->notification, &sequence_reset_red);
    notification_message(app->notification, &sequence_reset_green);
    notification_message(app->notification, &sequence_reset_blue);
    
    furi_thread_flags_wait(0, FuriFlagWaitAny, 300); //Delay, prevent removal from RAM before LED value set    
}


void ledOn() {
    boilerplate_led_set_rgb(NULL, 255, 0, 0);
}

void ledOff() {
    boilerplate_led_reset(NULL);
}


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