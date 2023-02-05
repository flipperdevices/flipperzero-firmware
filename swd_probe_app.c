#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include <dolphin/dolphin.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

#include "swd_probe_app.h"
#include "jep106.h"

#define COUNT(x) (sizeof(x) / sizeof((x)[0]))

static const GpioPin* gpios[] = {
    &gpio_ext_pc0,
    &gpio_ext_pc1,
    &gpio_ext_pc3,
    &gpio_ext_pb2,
    &gpio_ext_pb3,
    &gpio_ext_pa4,
    &gpio_ext_pa6,
    &gpio_ext_pa7};

static const char* gpio_names[] = {"PC0", "PC1", "PC3", "PB2", "PB3", "PA4", "PA6", "PA7"};

/* bit set: clock, else data */
static const uint8_t gpio_direction_mask[6] =
    {0b10101010, 0b01010101, 0b11001100, 0b00110011, 0b11110000, 0b00001111};
static const uint8_t gpio_direction_ind[6] = "-\\||/-";

static bool has_multiple_bits(uint8_t x) {
    return (x & (x - 1)) != 0;
}

static int get_bit_num(uint8_t x) {
    return __builtin_ctz(x);
}

static const char* gpio_name(uint8_t mask) {
    if(has_multiple_bits(mask)) {
        return "Pxx";
    }
    int io = get_bit_num(mask);
    if(io > COUNT(gpio_names)) {
        return "Pxx";
    }

    return gpio_names[io];
}

static void swd_configure_pins(AppFSM* const ctx, bool output) {
    for(int io = 0; io < 8; io++) {
        uint8_t bitmask = 1 << io;

        if(ctx->current_mask & bitmask) {
            /* set for clock */
            furi_hal_gpio_init(gpios[io], GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
        } else {
            /* set for data */
            if(!output) {
                furi_hal_gpio_init(gpios[io], GpioModeInput, GpioPullUp, GpioSpeedVeryHigh);
            } else {
                furi_hal_gpio_init(
                    gpios[io], GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedVeryHigh);
            }
        }
    }
}

static void swd_set_clock(AppFSM* const ctx, const uint8_t level) {
    for(int io = 0; io < 8; io++) {
        uint8_t bitmask = 1 << io;

        if(ctx->current_mask & bitmask) {
            furi_hal_gpio_write(gpios[io], level);
        }
    }
}

static void swd_set_data(AppFSM* const ctx, const uint8_t level) {
    for(int io = 0; io < 8; io++) {
        uint8_t bitmask = 1 << io;

        if(!(ctx->current_mask & bitmask)) {
            furi_hal_gpio_write(gpios[io], level);
        }
    }
}

static uint8_t swd_get_data(AppFSM* const ctx) {
    uint8_t bits = 0;
    for(int io = 0; io < 8; io++) {
        bits >>= 1;
        bits |= furi_hal_gpio_read(gpios[io]) ? 0x80 : 0;
    }
    return bits;
}

static void swd_write_bit(AppFSM* const ctx, bool level) {
    swd_set_clock(ctx, 0);
    swd_set_data(ctx, level);
    furi_delay_us(5);
    swd_set_clock(ctx, 1);
    furi_delay_us(5);
    swd_set_clock(ctx, 0);
}

static uint8_t swd_read_bit(AppFSM* const ctx) {
    swd_set_clock(ctx, 0);
    furi_delay_us(5);
    swd_set_clock(ctx, 1);
    furi_delay_us(5);
    uint8_t bits = swd_get_data(ctx);

    swd_set_clock(ctx, 0);

    return bits;
}

/* send a byte or less LSB-first */
static void swd_write_byte(AppFSM* const ctx, const uint8_t data, size_t bits) {
    for(size_t pos = 0; pos < bits; pos++) {
        swd_write_bit(ctx, data & (1 << pos));
    }
}

/* send a sequence of bytes LSB-first */
static void swd_write(AppFSM* const ctx, const uint8_t* data, size_t bits) {
    size_t byte_pos = 0;
    while(bits > 0) {
        size_t remain = (bits > 8) ? 8 : bits;
        swd_write_byte(ctx, data[byte_pos++], remain);
        bits -= remain;
    }
}

static uint32_t swd_detect(AppFSM* const ctx) {
    swd_configure_pins(ctx, true);

    uint8_t data[] = {0xA5};
    swd_write(ctx, data, sizeof(data) * 8);

    /* turnaround cycle */
    swd_configure_pins(ctx, false);

    uint8_t ack_bits[3];
    uint8_t rdata[32];

    /* receive 3 ACK bits */
    for(int pos = 0; pos < 3; pos++) {
        ack_bits[pos] = swd_read_bit(ctx);
    }

    /* receive 32 RDATA bits */
    for(int pos = 0; pos < 32; pos++) {
        rdata[pos] = swd_read_bit(ctx);
    }

    /* receive parity bit */
    uint8_t parity = swd_read_bit(ctx);

    for(int io = 0; io < 8; io++) {
        uint8_t bitmask = 1 << io;

        /* skip if it's a clock */
        if(ctx->current_mask & bitmask) {
            continue;
        }

        uint8_t ack = 0;
        for(int pos = 0; pos < 3; pos++) {
            ack >>= 1;
            ack |= (ack_bits[pos] & bitmask) ? 4 : 0;
        }

        uint32_t dpidr = 0;
        bool dpdidr_parity = false;
        for(int pos = 0; pos < 32; pos++) {
            bool bit_set = (rdata[pos] & bitmask);
            dpidr >>= 1;
            dpidr |= bit_set ? 0x80000000 : 0;
            dpdidr_parity ^= bit_set;
        }

        if(ack == 1) {
            bool received_parity = (parity & bitmask);
            if(dpdidr_parity == received_parity) {
                ctx->dpidr = dpidr;
                ctx->detected = true;
                ctx->io_swd = bitmask;
                ctx->io_swc &= ctx->current_mask;
            }
        }
    }

    return 0;
}

/* A line reset is achieved by holding the data signal HIGH for at least 50 clock cycles, followed by at least two idle cycles. */
static void swd_line_reset(AppFSM* const ctx) {
    for(int bitcount = 0; bitcount < 50; bitcount += 8) {
        swd_write_byte(ctx, 0xFF, 8);
    }
    swd_write_byte(ctx, 0, 8);
}

static void swd_scan(AppFSM* const ctx) {
    /* To switch SWJ-DP from JTAG to SWD operation:
        1. Send at least 50 SWCLKTCK cycles with SWDIOTMS HIGH. This ensures that the current interface is in its reset state. The JTAG interface only detects the 16-bit JTAG-to-SWD sequence starting from the Test-Logic-Reset state.
        2. Send the 16-bit JTAG-to-SWD select sequence 0x79e7 on SWDIOTMS.
        3. Send at least 50 SWCLKTCK cycles with SWDIOTMS HIGH. This ensures that if SWJ-DP was already in SWD operation before sending the select sequence, the SWD interface enters line reset state.
    */
    swd_configure_pins(ctx, true);

    /* reset JTAG interface */
    for(int bitcount = 0; bitcount < 50; bitcount += 8) {
        swd_write_byte(ctx, 0xFF, 8);
    }

    /* Send the 16-bit JTAG-to-SWD select sequence */
    swd_write_byte(ctx, 0x9E, 8);
    swd_write_byte(ctx, 0xE7, 8);

    /* resynchronize SWD */
    swd_line_reset(ctx);

    swd_detect(ctx);
}

static void render_callback(Canvas* const canvas, void* cb_ctx) {
    AppFSM* ctx = acquire_mutex((ValueMutex*)cb_ctx, 25);
    if(ctx == NULL) {
        return;
    }

    char buffer[64];
    int y = 10;

    if(ctx->detected_device) {
        /* if seen less than a quarter second ago */
        if((ctx->detected_timeout + TIMER_HZ / 4) >= TIMER_HZ * TIMEOUT) {
            snprintf(buffer, sizeof(buffer), "FOUND!");
        } else {
            /* if it was seen more than a quarter second ago, show countdown */
            snprintf(
                buffer, sizeof(buffer), "FOUND! (%lus)", (ctx->detected_timeout / TIMER_HZ) + 1);
        }
    } else {
        snprintf(
            buffer, sizeof(buffer), "Searching... %c", gpio_direction_ind[ctx->current_mask_id]);
    }

    canvas_draw_frame(canvas, 0, 0, 128, 64);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, y, AlignCenter, AlignBottom, buffer);
    y += 10;

    if(ctx->detected_device) {
        canvas_set_font(canvas, FontKeyboard);
        snprintf(
            buffer,
            sizeof(buffer),
            "SWC/SWD: %s/%s",
            gpio_name(ctx->io_swc),
            gpio_name(ctx->io_swd));
        canvas_draw_str_aligned(canvas, 5, y, AlignLeft, AlignBottom, buffer);
        y += 10;
        snprintf(buffer, sizeof(buffer), "DPIDR 0x%08lX", ctx->dpidr);
        canvas_draw_str_aligned(canvas, 5, y, AlignLeft, AlignBottom, buffer);
        y += 10;

        uint16_t rev = (ctx->dpidr >> 28) & 0x0F;
        uint16_t part = (ctx->dpidr >> 20) & 0xFF;
        uint16_t dap = (ctx->dpidr >> 12) & 0x0F;
        uint16_t designer_id = (ctx->dpidr >> 1) & 0x3FF;
        const char* designer = jep106_manufacturer(designer_id);

        snprintf(buffer, sizeof(buffer), "Part %02X Rev %X DAPv%d", part, rev, dap);
        canvas_draw_str_aligned(canvas, 5, y, AlignLeft, AlignBottom, buffer);
        y += 10;

        canvas_set_font(canvas, FontSecondary);
        snprintf(buffer, sizeof(buffer), "Des.: %s", designer);
        canvas_draw_str_aligned(canvas, 5, y, AlignLeft, AlignBottom, buffer);
        y += 10;
    }

    release_mutex((ValueMutex*)cb_ctx, ctx);
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    AppEvent event = {.type = EventKeyPress, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void timer_tick_callback(FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    AppEvent event = {.type = EventTimerTick};
    furi_message_queue_put(event_queue, &event, 0);
}

static void app_init(AppFSM* const ctx, FuriMessageQueue* event_queue) {
    ctx->_event_queue = event_queue;
    FuriTimer* timer =
        furi_timer_alloc(timer_tick_callback, FuriTimerTypePeriodic, ctx->_event_queue);
    furi_timer_start(timer, furi_kernel_get_tick_frequency() / TIMER_HZ);
    ctx->_timer = timer;

    ctx->current_mask_id = 0;
    ctx->current_mask = gpio_direction_mask[ctx->current_mask_id];
    ctx->io_swd = 0xFF;
    ctx->io_swc = 0xFF;

    strcpy(ctx->state_string, "none");
}

static void app_deinit(AppFSM* const ctx) {
    furi_timer_free(ctx->_timer);
}

static void on_timer_tick(AppFSM* ctx) {
    /* reset after timeout */
    if(ctx->detected_timeout == 0) {
        ctx->detected_device = false;
        ctx->io_swd = 0xFF;
        ctx->io_swc = 0xFF;
    } else {
        ctx->detected_timeout--;
    }

    ctx->detected = false;
    ctx->current_mask = gpio_direction_mask[ctx->current_mask_id];

    /* when SWD was already detected, set it to data pin regardless of the mask */
    if(ctx->detected_device) {
        ctx->current_mask &= ~ctx->io_swd;
    }

    /* do the scan */
    swd_scan(ctx);

    /* now when detected a device, set the timeout */
    if(ctx->detected) {
        ctx->detected_device = true;
        ctx->detected_timeout = TIMER_HZ * TIMEOUT;
    }

    ctx->current_mask_id = (ctx->current_mask_id + 1) % COUNT(gpio_direction_mask);
}

int32_t swd_probe_app_main(void* p) {
    UNUSED(p);

    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(AppEvent));
    AppFSM* ctx = malloc(sizeof(AppFSM));
    app_init(ctx, event_queue);

    ValueMutex state_mutex;
    if(!init_mutex(&state_mutex, ctx, sizeof(AppFSM))) {
        FURI_LOG_E(TAG, "cannot create mutex\r\n");
        free(ctx);
        return 255;
    }

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, &state_mutex);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);
    ctx->notification = furi_record_open(RECORD_NOTIFICATION);
    notification_message_block(ctx->notification, &sequence_display_backlight_enforce_on);

    DOLPHIN_DEED(DolphinDeedPluginGameStart);

    AppEvent event;
    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);

        AppFSM* ctx = (AppFSM*)acquire_mutex_block(&state_mutex);

        if(event_status == FuriStatusOk) {
            if(event.type == EventKeyPress) {
                if(event.input.type == InputTypePress) {
                    switch(event.input.key) {
                    case InputKeyUp:
                        ctx->last_key = KeyUp;
                        break;
                    case InputKeyDown:
                        ctx->last_key = KeyDown;
                        break;
                    case InputKeyRight:
                        ctx->last_key = KeyRight;
                        break;
                    case InputKeyLeft:
                        ctx->last_key = KeyLeft;
                        break;
                    case InputKeyOk:
                        ctx->last_key = KeyOK;
                        break;
                    case InputKeyBack:
                        processing = false;
                        break;
                    default:
                        break;
                    }
                }
            } else if(event.type == EventTimerTick) {
                FURI_CRITICAL_ENTER();
                on_timer_tick(ctx);
                FURI_CRITICAL_EXIT();
            }
        } else {
            /* timeout */
        }

        view_port_update(view_port);

        bool beep = false;

        if(ctx->detected_device && !ctx->detected_notified) {
            ctx->detected_notified = true;
            beep = true;
        }
        if(!ctx->detected_device && ctx->detected_notified) {
            ctx->detected_notified = false;
        }

        release_mutex(&state_mutex, ctx);

        if(beep) {
            notification_message_block(ctx->notification, &seq_c_minor);
        }
    }

    // Wait for all notifications to be played and return backlight to normal state
    app_deinit(ctx);

    notification_message_block(ctx->notification, &sequence_display_backlight_enforce_auto);

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    delete_mutex(&state_mutex);
    free(ctx);

    return 0;
}
