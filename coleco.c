#include <furi.h>
#include <furi_hal_gpio.h>
#include <gui/gui.h>
#include <gui/modules/button_panel.h>

const GpioPin* const pin_up = &gpio_ext_pa4;
const GpioPin* const pin_down = &gpio_ext_pb2;
const GpioPin* const pin_right = &gpio_ext_pa7;
const GpioPin* const pin_left = &gpio_ext_pa6;
const GpioPin* const pin_fire = &gpio_ext_pc3;
const GpioPin* const pin_mode = &gpio_ext_pc1;

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
  bool mode;

  bool button_up;
  bool button_down;
  bool button_right;
  bool button_left;
  bool button_fire;
  unsigned int button_code;

  ButtonPanel* button_panel;
  bool dpad;
} Coleco;

static void output_update(Coleco* coleco)
{
  // TODO: check mode, possibly flipped
  if (coleco->mode)
  {
    furi_hal_gpio_write(pin_up, !coleco->button_up);
    furi_hal_gpio_write(pin_down, !coleco->button_down);
    furi_hal_gpio_write(pin_right, !coleco->button_right);
    furi_hal_gpio_write(pin_left, !coleco->button_left);
  }
  else
  {
    // TODO: check ordering of bits
    furi_hal_gpio_write(pin_left, (coleco->button_code & 1));
    furi_hal_gpio_write(pin_down, (coleco->button_code & 2));
    furi_hal_gpio_write(pin_right, (coleco->button_code & 4));
    furi_hal_gpio_write(pin_up, (coleco->button_code & 8));
  }
  furi_hal_gpio_write(pin_fire, !coleco->button_fire);
}

static void mode_isr(void* context)
{
  Coleco* coleco = acquire_mutex((ValueMutex*)context, 25);
  if (coleco == NULL)
  {
    return;
  }

  coleco->mode = furi_hal_gpio_read(&gpio_ext_pc1);
  output_update(coleco);
}

static void render_callback(Canvas* const canvas, void* context)
{
  Coleco* coleco = acquire_mutex((ValueMutex*)context, 25);
  if (coleco == NULL)
  {
    return;
  }

  canvas_set_font(canvas, FontPrimary);
  canvas_draw_str_aligned(canvas, 64, 8, AlignCenter, AlignBottom, "ColecoVision");
  if (coleco->dpad)
  {
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 18, AlignCenter, AlignBottom, "D-pad");
  }

  release_mutex((ValueMutex*)context, coleco);
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue)
{
  furi_assert(event_queue);

  PluginEvent event = {.type = EventTypeKey, .input = *input_event};
  furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static Coleco* coleco_alloc()
{
  Coleco* coleco = malloc(sizeof(Coleco));

  coleco->button_panel = button_panel_alloc();

  coleco->mode = true;
  coleco->button_up = false;
  coleco->button_down = false;
  coleco->button_right = false;
  coleco->button_left = false;
  coleco->button_fire = false;
  coleco->button_code = 0x0F;
  coleco->dpad = false;

  // configure output pins
  furi_hal_gpio_init(pin_up, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
  furi_hal_gpio_init(pin_down, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
  furi_hal_gpio_init(pin_right, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
  furi_hal_gpio_init(pin_left, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
  furi_hal_gpio_init(pin_fire, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);

  output_update(coleco);

  // configure mode pin
  furi_hal_gpio_init(pin_mode, GpioModeInterruptRiseFall, GpioPullNo, GpioSpeedVeryHigh);

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

  // enable mode ISR
  furi_hal_gpio_add_int_callback(pin_mode, mode_isr, &coleco_mutex);

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
                coleco->button_up = true;
              }
              else if (event.input.type == InputTypeRelease)
              {
                coleco->button_up = false;
              }
            }
            else  // FIXME: hack to allow the 1 to be pressed
            {
              if (event.input.type == InputTypePress)
              {
                coleco->button_code = 0x0D;
              }
              else if (event.input.type == InputTypeRelease)
              {
                coleco->button_code = 0x0F;
              }
            }
            break;
          case InputKeyDown:
            if (coleco->dpad)
            {
              if (event.input.type == InputTypePress)
              {
                coleco->button_down = true;
              }
              else if (event.input.type == InputTypeRelease)
              {
                coleco->button_down = false;
              }
            }
            break;
          case InputKeyRight:
            if (coleco->dpad)
            {
              if (event.input.type == InputTypePress)
              {
                coleco->button_right = true;
              }
              else if (event.input.type == InputTypeRelease)
              {
                coleco->button_right = false;
              }
            }
            break;
          case InputKeyLeft:
            if (coleco->dpad)
            {
              if (event.input.type == InputTypePress)
              {
                coleco->button_left = true;
              }
              else if (event.input.type == InputTypeRelease)
              {
                coleco->button_left = false;
              }
            }
            break;
          case InputKeyOk:
            if (coleco->dpad)
            {
              if (event.input.type == InputTypePress)
              {
                coleco->button_fire = true;
              }
              else if (event.input.type == InputTypeRelease)
              {
                coleco->button_fire = false;
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
      }
    }
    else
    {
      FURI_LOG_D("Coleco", "FuriMessageQueue: event timeout");
    }

    output_update(coleco);
    view_port_update(view_port);
    release_mutex(&coleco_mutex, coleco);
  }

  furi_hal_gpio_remove_int_callback(pin_mode);
  view_port_enabled_set(view_port, false);
  gui_remove_view_port(gui, view_port);
  furi_record_close("gui");
  view_port_free(view_port);
  furi_message_queue_free(event_queue);
  delete_mutex(&coleco_mutex);
  coleco_free(coleco);
  return 0;
}
