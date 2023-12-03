#include "../wiegand.h"

FuriTimer *timer = NULL;

static void wiegand_scan_isr_d0(void *context)
{
    UNUSED(context);
    uint32_t time = DWT->CYCCNT;
    bool rise = furi_hal_gpio_read(pinD0);

    data[bit_count] = 0;

    if (rise)
    {
        data_rise[bit_count] = time;
        if (bit_count < MAX_BITS)
        {
            bit_count++;
        }
    }
    else
    {
        data_fall[bit_count] = time;
    }
}

static void wiegand_scan_isr_d1(void *context)
{
    UNUSED(context);
    uint32_t time = DWT->CYCCNT;
    bool rise = furi_hal_gpio_read(pinD1);

    data[bit_count] = 1;

    if (rise)
    {
        data_rise[bit_count] = time;
        if (bit_count < MAX_BITS)
        {
            bit_count++;
        }
    }
    else
    {
        data_fall[bit_count] = time;
    }
}

static void wiegand_start_scan(void *context)
{
    UNUSED(context);
    data_saved = false;
    bit_count = 0;
    furi_hal_gpio_init_simple(pinD0, GpioModeInterruptRiseFall);
    furi_hal_gpio_init_simple(pinD1, GpioModeInterruptRiseFall);
    furi_hal_gpio_add_int_callback(pinD0, wiegand_scan_isr_d0, NULL);
    furi_hal_gpio_add_int_callback(pinD1, wiegand_scan_isr_d1, NULL);
    furi_timer_start(timer, 100);
}

static void wiegand_stop_scan(void *context)
{
    UNUSED(context);
    furi_hal_gpio_remove_int_callback(pinD0);
    furi_hal_gpio_remove_int_callback(pinD1);
    furi_hal_gpio_init_simple(pinD0, GpioModeAnalog);
    furi_hal_gpio_init_simple(pinD1, GpioModeAnalog);
    furi_timer_stop(timer);
}

static void wiegand_scan_found(void *context)
{
    App *app = context;

    FuriHalRtcDateTime datetime;
    furi_hal_rtc_get_datetime(&datetime);
    snprintf(
        app->file_name,
        50,
        "%02d_%02d_%02d_%02d_%02d_%02d",
        datetime.year,
        datetime.month,
        datetime.day,
        datetime.hour,
        datetime.minute,
        datetime.second);
    furi_hal_vibro_on(true);
    furi_delay_ms(50);
    furi_hal_vibro_on(false);
    furi_delay_ms(50);
    wiegand_save(app);
    furi_hal_vibro_on(true);
    furi_delay_ms(50);
    furi_hal_vibro_on(false);
    furi_delay_ms(1000);
    wiegand_start_scan(app);
}

static void wiegand_scan_timer_callback(void *context)
{
    App *app = context;
    uint32_t duration = DWT->CYCCNT;
    const uint32_t one_millisecond = 64000;

    if (bit_count == 0)
    {
        return;
    }

    duration -= data_fall[bit_count - 1];

    bool found = false;
    FURI_CRITICAL_ENTER();
    if (duration > 25 * one_millisecond)
    {
        if (bit_count == 4 || bit_count == 8 || bit_count == 24 || bit_count == 26 ||
            bit_count == 32 || bit_count == 34 || bit_count == 35 || bit_count == 36 ||
            bit_count == 37 || bit_count == 40 || bit_count == 48)
        {
            wiegand_stop_scan(app);
            found = true;
        }
        else
        {
            // No data, clear
            bit_count = 0;
        }
    }
    FURI_CRITICAL_EXIT();

    if (found)
    {
        wiegand_scan_found(app);
    }
}

void wiegand_scan_scene_on_enter(void *context)
{
    App *app = context;
    timer = furi_timer_alloc(wiegand_scan_timer_callback, FuriTimerTypePeriodic, app);
    widget_reset(app->widget);
    widget_add_string_element(app->widget, 0, 0, AlignLeft, AlignTop, FontPrimary, "Scan Wiegand");
    widget_add_string_element(
        app->widget, 0, 20, AlignLeft, AlignTop, FontSecondary, "Files saved automatically.");
    widget_add_string_element(
        app->widget, 0, 30, AlignLeft, AlignTop, FontSecondary, "Vibrates on detection.");
    widget_add_string_element(
        app->widget, 0, 45, AlignLeft, AlignTop, FontSecondary, "D0/Green/A4");
    widget_add_string_element(
        app->widget, 0, 53, AlignLeft, AlignTop, FontSecondary, "D1/White/A7");
    wiegand_start_scan(app);
    view_dispatcher_switch_to_view(app->view_dispatcher, WiegandWidgetView);
}

void wiegand_scan_scene_on_exit(void *context)
{
    App *app = context;
    wiegand_stop_scan(app);
    furi_timer_free(timer);
}
