#include "hal.h"

App* context = NULL;

const char* getPinName(const GpioPin* pin)
{
  if(pin == &gpio_ext_pc3)
    return "out";
  if(pin == &gpio_ext_pb3)
    return "notOE";
  if(pin == &gpio_ext_pa4)
    return "Ain";
  if(pin == &gpio_ext_pb2)
    return "probe";
  return "unknown";
}

void setApp(void* v)
{
  App* ctx = v;
  context = ctx;
}

void boilerplate_led_set_rgb(int red, int green, int blue) {
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

void boilerplate_led_reset() {
    App* app = context;
    notification_message(app->notification, &sequence_reset_red);
    notification_message(app->notification, &sequence_reset_green);
    notification_message(app->notification, &sequence_reset_blue);
    
    furi_thread_flags_wait(0, FuriFlagWaitAny, 300); //Delay, prevent removal from RAM before LED value set    
}

void digitalWrite(const GpioPin* pin, bool level)
{
    FURI_LOG_I(TAG, "digitalWrite %s %d", getPinName(pin), level);
    furi_hal_gpio_write(pin, level);
}

int analogRead(const GpioPin* pin)
{
  if(furi_hal_gpio_read(pin))
  {
    FURI_LOG_I(TAG, "analogRead %s 1", getPinName(pin));
    return 500;
  }
  FURI_LOG_I(TAG, "analogRead %s 0", getPinName(pin));
  return 0;
}

void ledOn() {
  FURI_LOG_I(TAG, "ledOn");
    boilerplate_led_set_rgb(255, 0, 0);
}

void ledOff() {
  FURI_LOG_I(TAG, "ledOff");
    boilerplate_led_reset();
}


void Serial_prints(const char* c)
{
  FURI_LOG_I(TAG, "Serial_prints %s", c);
  UNUSED(c);
}
void Serial_printlns(const char* c)
{
  FURI_LOG_I(TAG, "%s", c);
  UNUSED(c);
}
void Serial_println(void)
{
}
void Serial_printi(const int c)
{
  FURI_LOG_I(TAG, "Serial_printi %d", c);
  UNUSED(c);
}
void Serial_printf(const float c, int acc)
{
  FURI_LOG_I(TAG, "Serial_printf %f %d", (double)c, acc);
  UNUSED(c);
  UNUSED(acc);
}

const char *F(const char* i)
{
  return i;
}

void delay(int ms)
{
  //FURI_LOG_I(TAG, "delay %d", ms);
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
  FURI_LOG_I(TAG, "%d", i);
  UNUSED(i);
}

int Serial_available(void)
{
  //FURI_LOG_I(TAG, "Serial_available");
  return 1;
}

int Serial_read(void)
{
  int ret = 0;
  if(!furi_string_empty(context->dmcomm_input_buffer))
  {
    FURI_LOG_I(TAG, "Serial_read");
    const char s = furi_string_get_char(context->dmcomm_input_buffer, 0);
    FURI_LOG_I(TAG, " reading from: %c", s);
    ret = (int)s;
    furi_string_right(context->dmcomm_input_buffer, 1);
  }
  return ret;
}

void Serial_writeb(const byte* data, int len)
{
  FURI_LOG_I(TAG, "Serial_writeb %d", len);
  UNUSED(data);
  UNUSED(len);
}