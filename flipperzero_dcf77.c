#include <furi_hal.h>
#include <furi_hal_pwm.h>
#include <gui/gui.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

#include "dcf77.h"

#define SCREEN_SIZE_X 128
#define SCREEN_SIZE_Y 64
#define DCF77_FREQ    77500
#define DCF77_OFFSET  61
#define SYNC_DELAY    50
#define UPDATES       8

char *WEEKDAYS[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

typedef struct {
    FuriHalRtcDateTime dt;
    bool               is_dst;
} AppData;

static void app_draw_callback(Canvas *canvas, void *context)
{
  AppData *app_data = (AppData *)context;

  char buffer[64];

  snprintf(buffer, sizeof(buffer), "%02u:%02u:%02u", app_data->dt.hour, app_data->dt.minute, app_data->dt.second);

  canvas_set_font(canvas, FontBigNumbers);
  canvas_draw_str_aligned(canvas, SCREEN_SIZE_X / 2, SCREEN_SIZE_Y / 2, AlignCenter, AlignCenter, buffer);

  const char *dow_str = WEEKDAYS[(app_data->dt.weekday - 1) % 7];
  const char *dst_str = app_data->is_dst ? "CEST" : "CET";
  snprintf(buffer, sizeof(buffer), "%s %02u-%02u-%04u %s", dow_str, app_data->dt.day, app_data->dt.month, app_data->dt.year, dst_str);

  canvas_set_font(canvas, FontSecondary);
  canvas_draw_str_aligned(canvas, SCREEN_SIZE_X / 2, 0, AlignCenter, AlignTop, buffer);

  if (app_data->dt.second < 59) {
    char *data = get_dcf77_data(app_data->dt.second);
    canvas_draw_str_aligned(canvas, SCREEN_SIZE_X, SCREEN_SIZE_Y, AlignRight, AlignBottom, data);
  }
}

static void app_input_callback(InputEvent *input_event, void *ctx)
{
  furi_assert(ctx);
  FuriMessageQueue *event_queue = ctx;
  furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

int flipperzero_dcf77_app_main(void *p)
{
  UNUSED(p);

  AppData    app_data;
  InputEvent event;

  app_data.is_dst = false;
  furi_hal_rtc_get_datetime(&app_data.dt);
  set_dcf77_time(&app_data.dt, app_data.is_dst);

  ViewPort         *view_port = view_port_alloc();
  FuriMessageQueue *event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

  view_port_draw_callback_set(view_port, app_draw_callback, &app_data);
  view_port_input_callback_set(view_port, app_input_callback, event_queue);

  Gui *gui = furi_record_open(RECORD_GUI);
  gui_add_view_port(gui, view_port, GuiLayerFullscreen);

  NotificationApp *notification = furi_record_open(RECORD_NOTIFICATION);
  notification_message_block(notification, &sequence_display_backlight_enforce_on);

  bool running = false;
  bool exit = false;
  int  sec = app_data.dt.second;
  while (!exit) {
    int silence_ms = 0;
    // wait next second
    while (app_data.dt.second == sec)
      furi_hal_rtc_get_datetime(&app_data.dt);

    if (app_data.dt.second < 59) {
      furi_hal_light_set(LightRed | LightGreen | LightBlue, 0);
      if (running) {
        furi_hal_rfid_tim_read_stop();
        furi_hal_pwm_stop(FuriHalPwmOutputIdLptim2PA4);
        furi_hal_gpio_init(&gpio_ext_pa4, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
      }
      silence_ms = get_dcf77_bit(app_data.dt.second) ? 200 : 100;
      furi_delay_ms(silence_ms);
      furi_hal_rfid_tim_read_start(DCF77_FREQ, 0.5);
      furi_hal_pwm_start(FuriHalPwmOutputIdLptim2PA4, DCF77_FREQ, 50);
      running = true;
      furi_hal_light_set(LightBlue, 0xFF);
    }
    else {
      FuriHalRtcDateTime dcf_dt; // next minute
      furi_hal_rtc_timestamp_to_datetime(furi_hal_rtc_datetime_to_timestamp(&app_data.dt) + DCF77_OFFSET, &dcf_dt);
      // fix forgotten weekday
      dcf_dt.weekday = (app_data.dt.day == dcf_dt.day) ? app_data.dt.weekday : (app_data.dt.weekday % 7) + 1;
      set_dcf77_time(&dcf_dt, app_data.is_dst);
    }

    sec = app_data.dt.second;
    int wait_ms = (1000 - silence_ms - SYNC_DELAY) / UPDATES;
    for (int i = 0; i < UPDATES; i++) {
      if (furi_message_queue_get(event_queue, &event, wait_ms) == FuriStatusOk) {
        if ((event.type == InputTypePress) || (event.type == InputTypeRepeat)) {
          switch (event.key) {
          case InputKeyOk:
            app_data.is_dst = !app_data.is_dst;
            break;
          case InputKeyBack:
            exit = true;
            break;
          default:
            break;
          }
        }
      }
      view_port_update(view_port);
      if (exit)
        break;
    }
  }

  if (running) {
    furi_hal_rfid_tim_read_stop();
    furi_hal_pwm_stop(FuriHalPwmOutputIdLptim2PA4);
    furi_hal_light_set(LightRed | LightGreen | LightBlue, 0);
  }

  notification_message_block(notification, &sequence_display_backlight_enforce_auto);

  view_port_enabled_set(view_port, false);
  gui_remove_view_port(gui, view_port);
  furi_record_close(RECORD_NOTIFICATION);
  furi_record_close(RECORD_GUI);
  furi_message_queue_free(event_queue);
  view_port_free(view_port);

  return 0;
}