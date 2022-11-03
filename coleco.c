#include <furi.h>
#include <furi_hal_gpio.h>
#include <furi_hal_power.h>
#include <gui/gui.h>
#include <gui/modules/button_panel.h>

#define CODE_0 0x0A
#define CODE_1 0x0D
#define CODE_2 0x07
#define CODE_3 0x0C
#define CODE_4 0x02
#define CODE_5 0x03
#define CODE_6 0x0E
#define CODE_7 0x05
#define CODE_8 0x01
#define CODE_9 0x0B
#define CODE_H 0x06
#define CODE_S 0x09
#define CODE_N 0x0F

const GpioPin* const pin_up = &gpio_ext_pa6;
const GpioPin* const pin_down = &gpio_ext_pc0;
const GpioPin* const pin_right = &gpio_ext_pb2;
const GpioPin* const pin_left = &gpio_ext_pc3;
const GpioPin* const pin_code0 = &gpio_ext_pa7;
const GpioPin* const pin_code1 = &gpio_ext_pa4;
const GpioPin* const pin_code2 = &ibutton_gpio;
const GpioPin* const pin_code3 = &gpio_ext_pc1;
const GpioPin* const pin_fire = &gpio_ext_pb3;

typedef enum
{
  EventTypeTick,
  EventTypeKey,
} EventType;

typedef struct
{
  EventType type;
  InputEvent input;
} PluginEvent;

typedef struct
{
  ButtonPanel* button_panel;
  bool dpad;
} Coleco;

static void render_callback(Canvas* const canvas, void* context)
{
  Coleco* coleco = acquire_mutex((ValueMutex*)context, 25);
  if (coleco == NULL)
  {
    return;
  }

  canvas_set_font(canvas, FontPrimary);
  canvas_draw_str_aligned(canvas, 64, 8, AlignCenter, AlignBottom, "ColecoVision");

  canvas_set_font(canvas, FontSecondary);

  if (coleco->dpad)
  {
    canvas_draw_str_aligned(canvas, 64, 18, AlignCenter, AlignBottom, "d-pad");
  }
  else
  {
    canvas_draw_str_aligned(canvas, 64, 18, AlignCenter, AlignBottom, "numbers");
  }

  release_mutex((ValueMutex*)context, coleco);
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue)
{
  furi_assert(event_queue);

  PluginEvent event = {.type = EventTypeKey, .input = *input_event};
  furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void coleco_write_code(unsigned int code)
{
  furi_hal_gpio_write(pin_code0, (code & 1));
  furi_hal_gpio_write(pin_code1, (code & 2));
  furi_hal_gpio_write(pin_code2, (code & 4));
  furi_hal_gpio_write(pin_code3, (code & 8));
}

static Coleco* coleco_alloc()
{
  Coleco* coleco = malloc(sizeof(Coleco));

  coleco->button_panel = button_panel_alloc();

  coleco->dpad = false;

  // configure output pins
  furi_hal_gpio_init(pin_up, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
  furi_hal_gpio_init(pin_down, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
  furi_hal_gpio_init(pin_right, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
  furi_hal_gpio_init(pin_left, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
  furi_hal_gpio_init(pin_code0, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
  furi_hal_gpio_init(pin_code1, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
  furi_hal_gpio_init(pin_code2, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
  furi_hal_gpio_init(pin_code3, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
  furi_hal_gpio_init(pin_fire, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);

  furi_hal_gpio_write(pin_up, true);
  furi_hal_gpio_write(pin_down, true);
  furi_hal_gpio_write(pin_right, true);
  furi_hal_gpio_write(pin_left, true);
  furi_hal_gpio_write(pin_fire, true);

  coleco_write_code(CODE_N);

  return coleco;
}

static void coleco_free(Coleco* coleco)
{
  furi_assert(coleco);

  button_panel_free(coleco->button_panel);

  free(coleco);
}

int32_t coleco_app(void* p)
{
  UNUSED(p);

  FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));

  Coleco* coleco = coleco_alloc();

  ValueMutex coleco_mutex;
  if (!init_mutex(&coleco_mutex, coleco, sizeof(Coleco)))
  {
    FURI_LOG_E("Coleco", "cannot create mutex\r\n");
    coleco_free(coleco);
    return 255;
  }

  // set system callbacks
  ViewPort* view_port = view_port_alloc();
  view_port_draw_callback_set(view_port, render_callback, &coleco_mutex);
  view_port_input_callback_set(view_port, input_callback, event_queue);

  // open GUI and register view_port
  Gui* gui = furi_record_open("gui");
  gui_add_view_port(gui, view_port, GuiLayerFullscreen);

  furi_hal_power_enable_otg();

  PluginEvent event;
  for (bool processing = true; processing;)
  {
    FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);

    Coleco* coleco = (Coleco*)acquire_mutex_block(&coleco_mutex);

    if (event_status == FuriStatusOk)
    {
      // press events
      if (event.type == EventTypeKey)
      {
        switch (event.input.key)
        {
          case InputKeyUp:
            if (coleco->dpad)
            {
              if (event.input.type == InputTypePress)
              {
                furi_hal_gpio_write(pin_up, false);
              }
              else if (event.input.type == InputTypeRelease)
              {
                furi_hal_gpio_write(pin_up, true);
              }
            }
            else  // FIXME: hack to allow the 1 to be pressed
            {
              if (event.input.type == InputTypePress)
              {
                coleco_write_code(CODE_1);
              }
              else if (event.input.type == InputTypeRelease)
              {
                coleco_write_code(CODE_N);
              }
            }
            break;
          case InputKeyDown:
            if (coleco->dpad)
            {
              if (event.input.type == InputTypePress)
              {
                furi_hal_gpio_write(pin_down, false);
              }
              else if (event.input.type == InputTypeRelease)
              {
                furi_hal_gpio_write(pin_down, true);
              }
            }
            else  // FIXME: hack to allow the 2 to be pressed
            {
              if (event.input.type == InputTypePress)
              {
                coleco_write_code(CODE_2);
              }
              else if (event.input.type == InputTypeRelease)
              {
                coleco_write_code(CODE_N);
              }
            }
            break;
          case InputKeyRight:
            if (coleco->dpad)
            {
              if (event.input.type == InputTypePress)
              {
                furi_hal_gpio_write(pin_right, false);
              }
              else if (event.input.type == InputTypeRelease)
              {
                furi_hal_gpio_write(pin_right, true);
              }
            }
            else  // FIXME: hack to allow the 3 to be pressed
            {
              if (event.input.type == InputTypePress)
              {
                coleco_write_code(CODE_3);
              }
              else if (event.input.type == InputTypeRelease)
              {
                coleco_write_code(CODE_N);
              }
            }
            break;
          case InputKeyLeft:
            if (coleco->dpad)
            {
              if (event.input.type == InputTypePress)
              {
                furi_hal_gpio_write(pin_left, false);
              }
              else if (event.input.type == InputTypeRelease)
              {
                furi_hal_gpio_write(pin_left, true);
              }
            }
            else  // FIXME: hack to allow the 4 to be pressed
            {
              if (event.input.type == InputTypePress)
              {
                coleco_write_code(CODE_4);
              }
              else if (event.input.type == InputTypeRelease)
              {
                coleco_write_code(CODE_N);
              }
            }
            break;
          case InputKeyOk:
            if (coleco->dpad)
            {
              if (event.input.type == InputTypePress)
              {
                furi_hal_gpio_write(pin_fire, false);
              }
              else if (event.input.type == InputTypeRelease)
              {
                furi_hal_gpio_write(pin_fire, true);
              }
            }
            else
            {
              coleco->dpad = true;
            }
            break;
          case InputKeyBack:
            if (event.input.type == InputTypePress)
            {
              if (coleco->dpad)
              {
                coleco->dpad = false;
              }
              else
              {
                processing = false;
              }
            }
            break;
          default:
            break;
        }

        view_port_update(view_port);
      }
    }
    else
    {
      FURI_LOG_D("Coleco", "FuriMessageQueue: event timeout");
    }

    release_mutex(&coleco_mutex, coleco);
  }

  furi_hal_power_disable_otg();

  view_port_enabled_set(view_port, false);
  gui_remove_view_port(gui, view_port);
  furi_record_close("gui");
  view_port_free(view_port);
  furi_message_queue_free(event_queue);
  delete_mutex(&coleco_mutex);
  coleco_free(coleco);
  return 0;
}
