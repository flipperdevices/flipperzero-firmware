#include "hal.h"

App* context = NULL;

void setApp(void* v)
{
  App* ctx = v;
  context = ctx;
}

DmcommCallback* serialCallback = NULL;

void setSerialOutputCallback(DmcommCallback *cb)
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
  furi_string_cat_printf(context->dmcomm_output_buffer, "%s", c);
}
void Serial_printlns(const char* c)
{
  FURI_LOG_I(TAG, "%s", c);
  furi_string_cat_printf(context->dmcomm_output_buffer, "%s\n", c);
  FURI_LOG_I(TAG, "SENDING LINE %s", furi_string_get_cstr(context->dmcomm_output_buffer));
  // allocate string, copy data, send string to CB. CB deallocates
  // no locking
  furi_string_reset(context->dmcomm_output_buffer);
}

void Serial_println(void)
{
  furi_string_cat_printf(context->dmcomm_output_buffer, "\n");
  FURI_LOG_I(TAG, "SENDING LINE %s", furi_string_get_cstr(context->dmcomm_output_buffer));
  // allocate string, copy data, send string to CB. CB deallocates
  // no locking
  furi_string_reset(context->dmcomm_output_buffer);
}

void Serial_printi(const int c)
{
  FURI_LOG_I(TAG, "Serial_printi %d", c);
  furi_string_cat_printf(context->dmcomm_output_buffer, "%d", c);
}
void Serial_printf(const float c, int acc)
{
  FURI_LOG_I(TAG, "Serial_printf %f %d", (double)c, acc);
  UNUSED(c);
  UNUSED(acc);
  //furi_string_cat_printf(dmcomm_output_buffer, "%d", c);
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
  furi_string_push_back(context->dmcomm_output_buffer, (char)((i >> 24) & 0xFF));
  furi_string_push_back(context->dmcomm_output_buffer, (char)((i >> 16) & 0xFF));
  furi_string_push_back(context->dmcomm_output_buffer, (char)((i >> 8) & 0xFF));
  furi_string_push_back(context->dmcomm_output_buffer, (char)(i & 0xFF));
}

int Serial_available(void)
{
  // We're using an always available buffer
  return 1;
}

int Serial_read(void)
{
  int ret = -1;

  furi_check(furi_mutex_acquire(context->dmcomm_mutex, FuriWaitForever) == FuriStatusOk);
  
  char s;
  size_t recieved = 1;
  recieved = furi_stream_buffer_receive(
      context->dmcomm_stream_buffer,
      &s,
      1,
      1);
    
  if(recieved > 0)
  {
    ret = (int)s;
  }
  furi_check(furi_mutex_release(context->dmcomm_mutex) == FuriStatusOk);

  return ret;
}

void Serial_writeb(const byte* data, int len)
{
  FURI_LOG_I(TAG, "Serial_writeb %d", len);
  for(int i = 0; i < len; i++)
    furi_string_push_back(context->dmcomm_output_buffer, (char)data[i]);
}