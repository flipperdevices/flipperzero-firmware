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
    boilerplate_led_set_rgb(255, 0, 0);
}

void ledOff() {
  boilerplate_led_reset();
}

void Serial_prints(const char* c)
{
  size_t sent = furi_stream_buffer_send(
    context->dmcomm_output_stream,
    c,
    strlen(c),
    0);
  UNUSED(sent);
}

void Serial_printlns(const char* c)
{
  size_t sent = furi_stream_buffer_send(
    context->dmcomm_output_stream,
    c,
    strlen(c),
    0);

  sent = furi_stream_buffer_send(
    context->dmcomm_output_stream,
    "\n",
    1,
    0);
  UNUSED(sent);

  if(serialCallback != NULL)
    (*serialCallback)(context);
}

void Serial_println(void)
{
  size_t sent = furi_stream_buffer_send(
    context->dmcomm_output_stream,
    "\n",
    1,
    0);
  UNUSED(sent);

  if(serialCallback != NULL)
    serialCallback(context);
}

void Serial_printi(const int c)
{
  char str[10];
  snprintf(str, 10, "%c", (char)c);
  size_t sent = furi_stream_buffer_send(
    context->dmcomm_output_stream,
    str,
    strlen(str),
    0);
  UNUSED(sent);
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
  return DWT->CYCCNT / furi_hal_cortex_instructions_per_microsecond();
}

uint32_t millis()
{
  return micros() / 1000;
}

void Serial_writei(int i)
{
  char str[10];
  snprintf(str, 10, "%c", (char)i);
  size_t sent = furi_stream_buffer_send(
    context->dmcomm_output_stream,
    str,
    strlen(str),
    0);
  UNUSED(sent);
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

  recieved = furi_stream_buffer_receive(
      context->dmcomm_input_stream,
      &s,
      1,
      1);

  if(recieved > 0)
  {
    ret = (int)s;
  }

  return ret;
}

void Serial_writeb(const byte* data, int len)
{
  char str[10];
  size_t sent;
  for(int i = 0; i < len; i++)
  {
    snprintf(str, 10, "%c", (char)data[i]);
    sent = furi_stream_buffer_send(
      context->dmcomm_output_stream,
      str,
      1,
      0);
  }
  UNUSED(sent);
}