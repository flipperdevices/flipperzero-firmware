#include "hal.h"

App* context = NULL;

void setApp(void* v)
{
  App* ctx = v;
  context = ctx;
}

DmcommCallback serialCallback = NULL;

void setSerialOutputCallback(DmcommCallback cb)
{
  serialCallback = cb;
}

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
    furi_hal_gpio_write(pin, level);
}

int analogRead(const GpioPin* pin)
{
  if(furi_hal_gpio_read(pin))
  {
    return 600;
  }
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
  furi_check(furi_mutex_acquire(context->dmcomm_output_mutex, FuriWaitForever) == FuriStatusOk);
  furi_string_cat_printf(context->dmcomm_output_buffer, "%s", c);
  furi_check(furi_mutex_release(context->dmcomm_output_mutex) == FuriStatusOk);
}
void Serial_printlns(const char* c)
{
  furi_check(furi_mutex_acquire(context->dmcomm_output_mutex, FuriWaitForever) == FuriStatusOk);
  furi_string_cat_printf(context->dmcomm_output_buffer, "%s\n", c);
  furi_check(furi_mutex_release(context->dmcomm_output_mutex) == FuriStatusOk);

  if(serialCallback != NULL)
    (*serialCallback)(context);
}

void Serial_println(void)
{
  furi_check(furi_mutex_acquire(context->dmcomm_output_mutex, FuriWaitForever) == FuriStatusOk);
  furi_string_cat_printf(context->dmcomm_output_buffer, "\n");
  furi_check(furi_mutex_release(context->dmcomm_output_mutex) == FuriStatusOk);

  if(serialCallback != NULL)
    serialCallback(context);
}

void Serial_printi(const int c)
{
  furi_check(furi_mutex_acquire(context->dmcomm_output_mutex, FuriWaitForever) == FuriStatusOk);
  furi_string_cat_printf(context->dmcomm_output_buffer, "%c", (char)c);
  furi_check(furi_mutex_release(context->dmcomm_output_mutex) == FuriStatusOk);
}
/*
void Serial_printf(const float c, int acc)
{
  furi_check(furi_mutex_acquire(context->dmcomm_output_mutex, FuriWaitForever) == FuriStatusOk);
  FURI_LOG_I(TAG, "Serial_printf %f %d", (double)c, acc);
  furi_check(furi_mutex_release(context->dmcomm_output_mutex) == FuriStatusOk);
}
*/
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
  return DWT->CYCCNT / furi_hal_cortex_instructions_per_microsecond();
}

uint32_t millis()
{
  return micros() / 1000;
}

void Serial_writei(int i)
{
  furi_check(furi_mutex_acquire(context->dmcomm_output_mutex, FuriWaitForever) == FuriStatusOk);
  furi_string_cat_printf(context->dmcomm_output_buffer, "%c", (char)i);
  furi_check(furi_mutex_release(context->dmcomm_output_mutex) == FuriStatusOk);
}

int Serial_available(void)
{
  // We're using an always available buffer
  return 1;
}

int Serial_read(void)
{
  int ret = -1;
  char s;
  size_t recieved = 1;

  furi_check(furi_mutex_acquire(context->dmcomm_input_mutex, FuriWaitForever) == FuriStatusOk);
  recieved = furi_stream_buffer_receive(
      context->dmcomm_stream_buffer,
      &s,
      1,
      1);
  furi_check(furi_mutex_release(context->dmcomm_input_mutex) == FuriStatusOk);

  if(recieved > 0)
  {
    ret = (int)s;
  }

  return ret;
}

void Serial_writeb(const byte* data, int len)
{
  furi_check(furi_mutex_acquire(context->dmcomm_output_mutex, FuriWaitForever) == FuriStatusOk);
  for(int i = 0; i < len; i++)
    furi_string_push_back(context->dmcomm_output_buffer, (char)data[i]);
  furi_check(furi_mutex_release(context->dmcomm_output_mutex) == FuriStatusOk);
}