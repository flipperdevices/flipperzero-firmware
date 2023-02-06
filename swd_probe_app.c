#include <furi.h>
#include <gui/gui.h>
#include <gui/elements.h>
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
    if(io >= COUNT(gpio_names)) {
        return "Pxx";
    }

    return gpio_names[io];
}

static void swd_configure_pins(AppFSM* const ctx, bool output) {
    if(ctx->mode_page != 0 && ctx->io_num_swc < 8 && ctx->io_num_swd < 8) {
        furi_hal_gpio_init(
            gpios[ctx->io_num_swc], GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
        if(!output) {
            furi_hal_gpio_init(
                gpios[ctx->io_num_swd], GpioModeInput, GpioPullUp, GpioSpeedVeryHigh);
        } else {
            furi_hal_gpio_init(
                gpios[ctx->io_num_swd], GpioModeOutputOpenDrain, GpioPullUp, GpioSpeedVeryHigh);
        }
        return;
    }

    for(int io = 0; io < 8; io++) {
        uint8_t bitmask = 1 << io;

        /* if neither candidate for SWC nor SWD then skip */
        if(!(ctx->io_swc & bitmask) && !(ctx->io_swd & bitmask)) {
            furi_hal_gpio_init(gpios[io], GpioModeInput, GpioPullUp, GpioSpeedVeryHigh);
            continue;
        }

        if(ctx->current_mask & bitmask) {
            /* set for clock */
            furi_hal_gpio_init(gpios[io], GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
        } else {
            /* set for data */
            if(!output) {
                furi_hal_gpio_init(gpios[io], GpioModeInput, GpioPullUp, GpioSpeedVeryHigh);
            } else {
                furi_hal_gpio_init(
                    gpios[io], GpioModeOutputOpenDrain, GpioPullUp, GpioSpeedVeryHigh);
            }
        }
    }
}

static void swd_set_clock(AppFSM* const ctx, const uint8_t level) {
    if(ctx->mode_page != 0 && ctx->io_num_swc < 8) {
        furi_hal_gpio_write(gpios[ctx->io_num_swc], level);
        return;
    }

    for(int io = 0; io < 8; io++) {
        uint8_t bitmask = 1 << io;

        /* if no candidate for SWC then skip */
        if(!(ctx->io_swc & bitmask)) {
            continue;
        }

        if(ctx->current_mask & bitmask) {
            furi_hal_gpio_write(gpios[io], level);
        }
    }
}

static void swd_set_data(AppFSM* const ctx, const uint8_t level) {
    if(ctx->mode_page != 0 && ctx->io_num_swd < 8) {
        furi_hal_gpio_write(gpios[ctx->io_num_swd], level);
        return;
    }

    for(int io = 0; io < 8; io++) {
        uint8_t bitmask = 1 << io;

        /* if no candidate for SWD then skip */
        if(!(ctx->io_swd & bitmask)) {
            continue;
        }

        if(!(ctx->current_mask & bitmask)) {
            furi_hal_gpio_write(gpios[io], level);
        }
    }
}

static uint8_t swd_get_data(AppFSM* const ctx) {
    if(ctx->mode_page != 0 && ctx->io_num_swd < 8) {
        return furi_hal_gpio_read(gpios[ctx->io_num_swd]);
    }

    uint8_t bits = 0;
    for(int io = 0; io < 8; io++) {
        uint8_t bitmask = 1 << io;

        /* if no candidate for SWD then skip */
        if(!(ctx->io_swd & bitmask)) {
            continue;
        }
        bits |= furi_hal_gpio_read(gpios[io]) ? bitmask : 0;
    }
    return bits;
}

static void swd_write_bit(AppFSM* const ctx, bool level) {
    swd_set_clock(ctx, 0);
    swd_set_data(ctx, level);
    furi_delay_us(SWD_DELAY_US);
    swd_set_clock(ctx, 1);
    furi_delay_us(SWD_DELAY_US);
    swd_set_clock(ctx, 0);
}

static uint8_t swd_read_bit(AppFSM* const ctx) {
    swd_set_clock(ctx, 1);
    furi_delay_us(SWD_DELAY_US);
    swd_set_clock(ctx, 0);
    uint8_t bits = swd_get_data(ctx);
    furi_delay_us(SWD_DELAY_US);
    swd_set_clock(ctx, 1);

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

static uint8_t swd_transfer(AppFSM* const ctx, bool ap, bool write, uint8_t a23, uint32_t* data) {
    swd_set_data(ctx, false);
    swd_configure_pins(ctx, true);

    swd_write_byte(ctx, 0, 8);
    uint8_t request[] = {0};

    request[0] |= 0x01; /* start bit*/
    request[0] |= ap ? 0x02 : 0; /* APnDP */
    request[0] |= write ? 0 : 0x04; /* operation */
    request[0] |= (a23 & 0x01) ? 0x08 : 0; /* A[2:3] */
    request[0] |= (a23 & 0x02) ? 0x10 : 0; /* A[2:3] */
    request[0] |= 0x80; /* park bit */
    request[0] |= __builtin_parity(request[0]) ? 0x20 : 0; /* parity */

    swd_write(ctx, request, sizeof(request) * 8);

    /* turnaround cycle */
    swd_configure_pins(ctx, false);

    uint8_t ack = 0;

    /* receive 3 ACK bits */
    for(int pos = 0; pos < 3; pos++) {
        ack >>= 1;
        ack |= swd_read_bit(ctx) ? 0x04 : 0;
    }

    /* force ABORT/CTRL to always work */
    if(!ap && a23 == 0) {
        ack = 1;
    }

    if(ack != 0x01) {
        return ack;
    }

    if(write) {
        swd_write_bit(ctx, 0);
        swd_configure_pins(ctx, true);

        /* send 32 WDATA bits */
        for(int pos = 0; pos < 32; pos++) {
            swd_write_bit(ctx, *data & (1 << pos));
        }

        /* send parity bit */
        swd_write_bit(ctx, __builtin_parity(*data));
    } else {
        *data = 0;
        /* receive 32 RDATA bits */
        for(int pos = 0; pos < 32; pos++) {
            *data >>= 1;
            *data |= swd_read_bit(ctx) ? 0x80000000 : 0;
        }

        /* receive parity bit */
        bool parity = swd_read_bit(ctx);

        if(parity != __builtin_parity(*data)) {
            return 8;
        }
    }
    swd_set_data(ctx, false);
    swd_configure_pins(ctx, true);

    return ack;
}

/* A line reset is achieved by holding the data signal HIGH for at least 50 clock cycles, followed by at least two idle cycles. */
static void swd_line_reset(AppFSM* const ctx) {
    for(int bitcount = 0; bitcount < 50; bitcount += 8) {
        swd_write_byte(ctx, 0xFF, 8);
    }
    swd_write_byte(ctx, 0, 8);
}

static void swd_abort(AppFSM* const ctx) {
    uint32_t dpidr;

    /* first reset the line */
    swd_line_reset(ctx);
    swd_transfer(ctx, false, false, 0, &dpidr);
    uint32_t abort = 0x0E;
    swd_transfer(ctx, false, true, 0, &abort);
}

static void swd_abort_simple(AppFSM* const ctx) {
    uint32_t abort = 0x0E;
    swd_transfer(ctx, false, true, 0, &abort);

    uint32_t dpidr;
    if(swd_transfer(ctx, false, false, 0, &dpidr) != 1) {
        swd_abort(ctx);
    }
}

static uint8_t swd_select(AppFSM* const ctx, uint8_t ap_sel, uint8_t ap_bank, uint8_t dp_bank) {
    uint32_t bank_reg = (ap_sel << 24) | ((ap_bank & 0x0F) << 4) | (dp_bank & 0x0F);

    uint8_t ret = swd_transfer(ctx, false, true, 2, &bank_reg);
    if(ret != 1) {
        furi_log_print_format(FuriLogLevelDefault, TAG, "swd_select: failed: %d", ret);
    }
    return ret;
}

static uint8_t
    swd_read_dpbank(AppFSM* const ctx, uint8_t dp_off, uint8_t dp_bank, uint32_t* data) {
    /* select target bank */
    uint8_t ret = swd_select(ctx, 0, 0, dp_bank);
    if(ret != 1) {
        furi_log_print_format(FuriLogLevelDefault, TAG, "swd_read_dpbank: swd_select failed");
        return ret;
    }

    /* read data from it */
    *data = 0;
    ret = swd_transfer(ctx, false, false, dp_off, data);
    if(ret != 1) {
        furi_log_print_format(FuriLogLevelDefault, TAG, "swd_read_dpbank: failed: %d", ret);
        return ret;
    }
    /* reset bank to zero again */
    ret = swd_select(ctx, 0, 0, 0);
    return ret;
}

static uint8_t swd_read_ap(AppFSM* const ctx, uint8_t ap, uint8_t ap_off, uint32_t* data) {
    uint8_t ret = swd_select(ctx, ap, (ap_off >> 4) & 0x0F, 0);
    if(ret != 1) {
        furi_log_print_format(FuriLogLevelDefault, TAG, "swd_read_ap: swd_select failed");
        return ret;
    }
    ret = swd_transfer(ctx, true, false, (ap_off >> 2) & 3, data);
    *data = 0;
    ret = swd_transfer(ctx, true, false, (ap_off >> 2) & 3, data);
    if(ret != 1) {
        furi_log_print_format(FuriLogLevelDefault, TAG, "swd_read_ap: failed: %d", ret);
        return ret;
    }
    return ret;
}

static uint8_t swd_read_ap_single(AppFSM* const ctx, uint8_t ap, uint8_t ap_off, uint32_t* data) {
    uint8_t ret = swd_select(ctx, ap, (ap_off >> 4) & 0x0F, 0);
    if(ret != 1) {
        furi_log_print_format(FuriLogLevelDefault, TAG, "swd_read_ap_single: swd_select failed");
        return ret;
    }
    *data = 0;
    ret = swd_transfer(ctx, true, false, (ap_off >> 2) & 3, data);
    if(ret != 1) {
        furi_log_print_format(FuriLogLevelDefault, TAG, "swd_read_ap_single: failed: %d", ret);
        return ret;
    }
    return ret;
}

static uint8_t swd_write_ap(AppFSM* const ctx, uint8_t ap, uint8_t ap_off, uint32_t data) {
    uint8_t ret = swd_select(ctx, ap, (ap_off >> 4) & 0x0F, 0);
    if(ret != 1) {
        furi_log_print_format(FuriLogLevelDefault, TAG, "swd_write_ap: swd_select failed");
        return ret;
    }
    ret = swd_transfer(ctx, true, true, (ap_off >> 2) & 3, &data);
    if(ret != 1) {
        furi_log_print_format(FuriLogLevelDefault, TAG, "swd_write_ap: failed: %d", ret);
        return ret;
    }
    return ret;
}

static uint8_t swd_read_memory(AppFSM* const ctx, uint8_t ap, uint32_t address, uint32_t* data) {
    uint8_t ret = 0;

    ret |= swd_write_ap(ctx, ap, MEMAP_CSW, 0x03000012);
    ret |= swd_write_ap(ctx, ap, MEMAP_TAR, address);
    ret |= swd_read_ap(ctx, ap, MEMAP_DRW, data);

    if(ret != 1) {
        swd_abort(ctx);
    }
    return ret;
}

static uint8_t swd_read_memory_cont(AppFSM* const ctx, uint8_t ap, uint32_t* data) {
    uint8_t ret = 0;

    ret |= swd_read_ap_single(ctx, ap, MEMAP_DRW, data);

    if(ret != 1) {
        swd_abort(ctx);
    }
    return ret;
}

static uint32_t swd_detect(AppFSM* const ctx) {
    swd_set_data(ctx, false);
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
        for(int pos = 0; pos < 32; pos++) {
            dpidr >>= 1;
            dpidr |= (rdata[pos] & bitmask) ? 0x80000000 : 0;
        }

        if(ack == 1 && dpidr != 0 && dpidr != 0xFFFFFFFF) {
            bool received_parity = (parity & bitmask);
            if(__builtin_parity(dpidr) == received_parity) {
                ctx->dp_regs.dpidr = dpidr;
                ctx->dp_regs.dpidr_ok = true;
                ctx->detected = true;
                ctx->io_swd = bitmask;
                ctx->io_swc &= ctx->current_mask;
                furi_log_print_format(
                    FuriLogLevelDefault,
                    TAG,
                    "swd_detect: data: %08lX, io_swd %02X, io_swc %02X",
                    dpidr,
                    ctx->io_swd,
                    ctx->io_swc);

                if(!has_multiple_bits(ctx->io_swc)) {
                    ctx->io_num_swd = get_bit_num(ctx->io_swd);
                    ctx->io_num_swc = get_bit_num(ctx->io_swc);
                }
            }
        }
    }
    swd_set_data(ctx, false);
    swd_configure_pins(ctx, true);

    return 0;
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

    canvas_draw_frame(canvas, 0, 0, 128, 64);
    canvas_set_font(canvas, FontPrimary);

    if(ctx->detected_device) {
        /* if seen less than a quarter second ago */
        switch(ctx->mode_page) {
        case 0: {
            if((ctx->detected_timeout + TIMER_HZ / 4) >= TIMER_HZ * TIMEOUT) {
                snprintf(buffer, sizeof(buffer), "FOUND!");
            } else {
                /* if it was seen more than a quarter second ago, show countdown */
                snprintf(
                    buffer,
                    sizeof(buffer),
                    "FOUND! (%lus)",
                    (ctx->detected_timeout / TIMER_HZ) + 1);
            }
            canvas_draw_str_aligned(canvas, 64, y, AlignCenter, AlignBottom, buffer);
            y += 10;
            canvas_set_font(canvas, FontKeyboard);

            snprintf(
                buffer,
                sizeof(buffer),
                "SWC/SWD: %s/%s",
                gpio_name(ctx->io_swc),
                gpio_name(ctx->io_swd));
            canvas_draw_str_aligned(canvas, 5, y, AlignLeft, AlignBottom, buffer);
            y += 10;
            snprintf(buffer, sizeof(buffer), "DPIDR 0x%08lX", ctx->dp_regs.dpidr);
            canvas_draw_str_aligned(canvas, 5, y, AlignLeft, AlignBottom, buffer);
            y += 10;

            snprintf(
                buffer,
                sizeof(buffer),
                "Part %02X Rev %X DAPv%d",
                ctx->dpidr_info.partno,
                ctx->dpidr_info.revision,
                ctx->dpidr_info.version);
            canvas_draw_str_aligned(canvas, 5, y, AlignLeft, AlignBottom, buffer);
            y += 10;

            canvas_set_font(canvas, FontSecondary);
            snprintf(buffer, sizeof(buffer), "%s", jep106_manufacturer(ctx->dpidr_info.designer));
            canvas_draw_str_aligned(canvas, 5, y, AlignLeft, AlignBottom, buffer);
            y += 10;

            canvas_set_font(canvas, FontSecondary);
            elements_button_right(canvas, "DP Regs");

            break;
        }
        case 1: {
            canvas_draw_str_aligned(canvas, 64, y, AlignCenter, AlignBottom, "DP Registers");
            y += 10;
            canvas_set_font(canvas, FontKeyboard);
            if(ctx->dp_regs.dpidr_ok) {
                snprintf(buffer, sizeof(buffer), "DPIDR %08lX", ctx->dp_regs.dpidr);
                canvas_draw_str_aligned(canvas, 5, y, AlignLeft, AlignBottom, buffer);
            }
            y += 10;

            if(ctx->dp_regs.ctrlstat_ok) {
                snprintf(buffer, sizeof(buffer), "CTRL  %08lX", ctx->dp_regs.ctrlstat);
                canvas_draw_str_aligned(canvas, 5, y, AlignLeft, AlignBottom, buffer);
            }
            y += 10;

            if(ctx->dp_regs.targetid_ok) {
                snprintf(buffer, sizeof(buffer), "TGTID %08lX", ctx->dp_regs.targetid);
                canvas_draw_str_aligned(canvas, 5, y, AlignLeft, AlignBottom, buffer);
            }
            y += 10;

            if(ctx->dp_regs.eventstat_ok) {
                snprintf(buffer, sizeof(buffer), "EVTST %08lX", ctx->dp_regs.eventstat);
                canvas_draw_str_aligned(canvas, 5, y, AlignLeft, AlignBottom, buffer);
            }
            y += 10;
            canvas_set_font(canvas, FontSecondary);
            elements_button_left(canvas, "Scan");
            elements_button_right(canvas, "DPID");
            break;
        }
        case 2: {
            canvas_draw_str_aligned(canvas, 64, y, AlignCenter, AlignBottom, "DP ID Register");
            y += 10;
            canvas_set_font(canvas, FontKeyboard);
            if(ctx->dpidr_info.version != 2) {
                snprintf(buffer, sizeof(buffer), "TARGETID not supported");
                canvas_draw_str_aligned(canvas, 5, y, AlignLeft, AlignBottom, buffer);
                y += 10;
            } else {
                if(ctx->dp_regs.targetid_ok) {
                    snprintf(buffer, sizeof(buffer), "TGTID %08lX", ctx->dp_regs.targetid);
                    canvas_draw_str_aligned(canvas, 5, y, AlignLeft, AlignBottom, buffer);
                    y += 10;

                    snprintf(buffer, sizeof(buffer), "Part No. %04X", ctx->targetid_info.partno);
                    canvas_draw_str_aligned(canvas, 5, y, AlignLeft, AlignBottom, buffer);
                    y += 10;
                    snprintf(
                        buffer,
                        sizeof(buffer),
                        "%s",
                        jep106_manufacturer(ctx->targetid_info.designer));
                    canvas_draw_str_aligned(canvas, 5, y, AlignLeft, AlignBottom, buffer);
                    y += 10;
                }
            }
            canvas_set_font(canvas, FontSecondary);
            elements_button_left(canvas, "DP Regs");
            elements_button_right(canvas, "APs");
            break;
        }
        case 3: {
            canvas_draw_str_aligned(canvas, 64, y, AlignCenter, AlignBottom, "AP Menu");
            y += 10;
            canvas_set_font(canvas, FontKeyboard);

            char state = ' ';
            if(ctx->ap_pos >= ctx->ap_scanned && ctx->ap_pos <= ctx->ap_scanned + 10) {
                state = '*';
            }

            if(!ctx->apidr_info[ctx->ap_pos].ok) {
                snprintf(buffer, sizeof(buffer), "[%d]%c<none>", ctx->ap_pos, state);
                canvas_draw_str_aligned(canvas, 5, y, AlignLeft, AlignBottom, buffer);
                y += 10;

                if(ctx->ap_pos == 0) {
                    for(int pos = 0; pos < COUNT(ctx->apidr_info); pos++) {
                        if(ctx->apidr_info[pos].ok) {
                            ctx->ap_pos = pos;
                        }
                    }
                }
            } else {
                const char* class = "";

                switch(ctx->apidr_info[ctx->ap_pos].class) {
                case 0:
                    class = "und";
                    break;
                case 1:
                    class = "COM";
                    break;
                case 8:
                    class = "MEM";
                    break;
                default:
                    class = "unk";
                    break;
                }

                const char* types[] = {
                    "COM-AP",
                    "AHB3",
                    "APB2 or APB3",
                    "Type unknown",
                    "AXI3 or AXI4",
                    "AHB5",
                    "APB4 and APB5",
                    "AXI5",
                    "AHB5 enh.",
                };
                const char* type = "Type unk";

                if(ctx->apidr_info[ctx->ap_pos].type < COUNT(types)) {
                    type = types[ctx->apidr_info[ctx->ap_pos].type];
                }

                snprintf(buffer, sizeof(buffer), "[%d]%c%s, %s", ctx->ap_pos, state, class, type);
                canvas_draw_str_aligned(canvas, 5, y, AlignLeft, AlignBottom, buffer);
                y += 10;

                snprintf(
                    buffer, sizeof(buffer), "Base 0x%08lX", ctx->apidr_info[ctx->ap_pos].base);
                canvas_draw_str_aligned(canvas, 5, y, AlignLeft, AlignBottom, buffer);
                y += 10;

                snprintf(
                    buffer,
                    sizeof(buffer),
                    "Rev %d Var %d",
                    ctx->apidr_info[ctx->ap_pos].revision,
                    ctx->apidr_info[ctx->ap_pos].variant);
                canvas_draw_str_aligned(canvas, 5, y, AlignLeft, AlignBottom, buffer);
                y += 10;

                snprintf(
                    buffer,
                    sizeof(buffer),
                    "%s",
                    jep106_manufacturer(ctx->apidr_info[ctx->ap_pos].designer));
                canvas_draw_str_aligned(canvas, 5, y, AlignLeft, AlignBottom, buffer);
                y += 10;

                elements_button_center(canvas, "Show");
            }
            canvas_set_font(canvas, FontSecondary);
            elements_button_left(canvas, "DPID");
            elements_scrollbar_pos(
                canvas, 4, 10, 40, ctx->ap_pos / 32, COUNT(ctx->apidr_info) / 32);
        } break;

            /* hex dump view */
        case 4: {
            canvas_draw_str_aligned(canvas, 64, y, AlignCenter, AlignBottom, "Hex dump");
            y += 10;
            canvas_set_font(canvas, FontKeyboard);

            canvas_draw_str_aligned(canvas, 5, y, AlignLeft, AlignBottom, "Addr:");

            snprintf(buffer, sizeof(buffer), "%08lX", ctx->hex_addr);
            canvas_draw_str_aligned(canvas, 38, y, AlignLeft, AlignBottom, buffer);
            uint32_t font_width = canvas_glyph_width(canvas, '0');
            uint32_t x = 37 + (7 - ctx->hex_select) * font_width;

            /* draw selection */
            canvas_draw_line(canvas, x, y + 1, x + font_width, y + 1);
            y += 10;

            uint32_t byte_num = 0;
            for(int line = 0; line < 4; line++) {
                uint32_t x_pos = 5;

                for(int byte_pos = 0; byte_pos < 8; byte_pos++) {
                    if(ctx->hex_buffer_valid[byte_num / 4]) {
                        snprintf(buffer, sizeof(buffer), "%02X", ctx->hex_buffer[byte_num]);
                    } else {
                        snprintf(buffer, sizeof(buffer), "--");
                    }
                    byte_num++;
                    canvas_draw_str_aligned(canvas, x_pos, y, AlignLeft, AlignBottom, buffer);
                    x_pos += font_width * 2 + font_width / 2;
                }
                y += 10;
            }

            break;
        }
        }
    } else {
        snprintf(
            buffer, sizeof(buffer), "Searching... %c", gpio_direction_ind[ctx->current_mask_id]);
        canvas_draw_str_aligned(canvas, 64, y, AlignCenter, AlignBottom, buffer);
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
    ctx->hex_addr = 0x40002800;

    strcpy(ctx->state_string, "none");
}

static void app_deinit(AppFSM* const ctx) {
    furi_timer_free(ctx->_timer);
}

static void on_timer_tick(AppFSM* ctx) {
    switch(ctx->mode_page) {
    case 0: {
        /* reset after timeout */
        if(ctx->detected_timeout == 0) {
            ctx->detected_device = false;
            ctx->io_swd = 0xFF;
            ctx->io_swc = 0xFF;
            ctx->io_num_swd = 0xFF;
            ctx->io_num_swc = 0xFF;
            ctx->ap_scanned = 0;
            memset(&ctx->dp_regs, 0x00, sizeof(ctx->dp_regs));
            memset(&ctx->targetid_info, 0x00, sizeof(ctx->targetid_info));
            memset(&ctx->apidr_info, 0x00, sizeof(ctx->apidr_info));
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

            /* update DPIDR fields */
            ctx->dpidr_info.revision = (ctx->dp_regs.dpidr >> 28) & 0x0F;
            ctx->dpidr_info.partno = (ctx->dp_regs.dpidr >> 20) & 0xFF;
            ctx->dpidr_info.version = (ctx->dp_regs.dpidr >> 12) & 0x0F;
            ctx->dpidr_info.designer = (ctx->dp_regs.dpidr >> 1) & 0x3FF;

            if(!has_multiple_bits(ctx->io_swc)) {
                /* reset error */
                uint8_t ack = swd_transfer(ctx, false, false, 1, &ctx->dp_regs.ctrlstat);
                furi_log_print_format(
                    FuriLogLevelDefault, TAG, "stat %02lX %d", ctx->dp_regs.ctrlstat, ack);

                if(ack != 1 || (ctx->dp_regs.ctrlstat & STAT_ERROR_FLAGS)) {
                    furi_log_print_format(FuriLogLevelDefault, TAG, "send ABORT");
                    swd_abort(ctx);
                }
                ctx->dp_regs.ctrlstat_ok = swd_read_dpbank(ctx, 1, 0, &ctx->dp_regs.ctrlstat) == 1;

                if(ctx->dpidr_info.version >= 1) {
                    ctx->dp_regs.dlcr_ok = swd_read_dpbank(ctx, 1, 1, &ctx->dp_regs.dlcr) == 1;
                }

                if(ctx->dpidr_info.version >= 2) {
                    ctx->dp_regs.targetid_ok =
                        swd_read_dpbank(ctx, 1, 2, &ctx->dp_regs.targetid) == 1;
                    ctx->dp_regs.eventstat_ok =
                        swd_read_dpbank(ctx, 1, 4, &ctx->dp_regs.eventstat) == 1;
                    ctx->dp_regs.dlpidr_ok = swd_read_dpbank(ctx, 1, 3, &ctx->dp_regs.dlpidr) == 1;
                }

                if(ctx->dp_regs.targetid_ok) {
                    ctx->targetid_info.revision = (ctx->dp_regs.targetid >> 28) & 0x0F;
                    ctx->targetid_info.partno = (ctx->dp_regs.targetid >> 12) & 0xFFFF;
                    ctx->targetid_info.designer = (ctx->dp_regs.targetid >> 1) & 0x3FF;
                }
            }
        }

        ctx->current_mask_id = (ctx->current_mask_id + 1) % COUNT(gpio_direction_mask);
        break;
    }
    case 1:
    case 2:
    case 3: {
        /* set debug enable request */
        if(!(ctx->dp_regs.ctrlstat & (CSYSPWRUPREQ | CDBGPWRUPREQ))) {
            /* fetch current CTRL/STAT */
            swd_transfer(ctx, false, false, 1, &ctx->dp_regs.ctrlstat);
            ctx->dp_regs.ctrlstat |= CDBGPWRUPREQ;
            ctx->dp_regs.ctrlstat |= CSYSPWRUPREQ;
            furi_log_print_format(FuriLogLevelDefault, TAG, "no (CSYSPWRUPREQ | CDBGPWRUPREQ)");
            swd_transfer(ctx, false, true, 1, &ctx->dp_regs.ctrlstat);
            break;
        }
        if(!(ctx->dp_regs.ctrlstat & CDBGPWRUPACK)) {
            /* fetch current CTRL/STAT */
            swd_transfer(ctx, false, false, 1, &ctx->dp_regs.ctrlstat);
            furi_log_print_format(FuriLogLevelDefault, TAG, "no CDBGPWRUPACK");
            break;
        }

        /* only scan a few APs at once to stay responsive */
        for(int pos = 0; pos < 5; pos++) {
            if(ctx->ap_scanned == 0) {
                for(int reset_ap = 0; reset_ap < COUNT(ctx->apidr_info); reset_ap++) {
                    ctx->apidr_info[reset_ap].tested = false;
                }
            }

            int ap = ctx->ap_scanned++;

            if(ctx->apidr_info[ap].tested) {
                continue;
            }
            ctx->apidr_info[ap].tested = true;

            uint32_t data = 0;
            uint32_t base = 0;
            if(swd_read_ap(ctx, ap, AP_IDR, &data) != 1) {
                swd_abort(ctx);
                continue;
            }
            if(data == 0) {
                continue;
            }
            furi_log_print_format(FuriLogLevelDefault, TAG, "AP%d detected", ap);
            ctx->apidr_info[ap].ok = true;
            ctx->apidr_info[ap].revision = (data >> 24) & 0x0F;
            ctx->apidr_info[ap].designer = (data >> 17) & 0x3FF;
            ctx->apidr_info[ap].class = (data >> 13) & 0x0F;
            ctx->apidr_info[ap].variant = (data >> 4) & 0x0F;
            ctx->apidr_info[ap].type = (data >> 0) & 0x0F;

            if(swd_read_ap(ctx, ap, AP_BASE, &ctx->apidr_info[ap].base) != 1) {
                swd_abort(ctx);
            }

            break;
        }
        break;
    }
    case 4: {
        if(ctx->hex_read_delay++ < 10) {
            break;
        }
        ctx->hex_read_delay = 0;

        memset(ctx->hex_buffer, 0xEE, sizeof(ctx->hex_buffer));

        uint32_t addr = ctx->hex_addr;
        uint32_t data = 0;
        for(int pos = 0; pos < sizeof(ctx->hex_buffer) / 4; pos++) {
            ctx->hex_buffer_valid[pos] = swd_read_memory(ctx, ctx->ap_pos, addr, &data) == 1;
            if(ctx->hex_buffer_valid[pos]) {
                memcpy(&ctx->hex_buffer[pos * 4], &data, 4);
            } else {
                swd_abort_simple(ctx);
            }
            addr += 4;
        }
    }
    }
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
                        switch(ctx->mode_page) {
                        default:
                            break;
                        case 3:
                            if(ctx->ap_pos > 0) {
                                ctx->ap_pos--;
                            }
                            break;
                        case 4: {
                            ctx->hex_addr +=
                                ((ctx->hex_select) ? 1 : 8) * (1 << (4 * ctx->hex_select));
                            break;
                        }
                        }
                        break;

                    case InputKeyDown:
                        ctx->last_key = KeyDown;
                        switch(ctx->mode_page) {
                        default:
                            break;
                        case 3:
                            if(ctx->ap_pos + 1 < COUNT(ctx->apidr_info)) {
                                ctx->ap_pos++;
                            }
                            break;
                        case 4: {
                            ctx->hex_addr -=
                                ((ctx->hex_select) ? 1 : 8) * (1 << (4 * ctx->hex_select));
                            break;
                        }
                        }

                        break;

                    case InputKeyRight:
                        ctx->last_key = KeyRight;
                        if(ctx->mode_page == 4) {
                            if(ctx->hex_select > 0) {
                                ctx->hex_select--;
                            }
                        } else {
                            if(ctx->mode_page + 1 < MODE_PAGES) {
                                ctx->mode_page++;
                            }
                        }
                        break;

                    case InputKeyLeft:
                        ctx->last_key = KeyLeft;

                        if(ctx->mode_page == 4) {
                            if(ctx->hex_select < 7) {
                                ctx->hex_select++;
                            }
                        } else {
                            if(ctx->mode_page > 0) {
                                ctx->mode_page--;
                            }
                        }
                        break;

                    case InputKeyOk:
                        ctx->last_key = KeyOK;

                        if(ctx->mode_page == 3 && ctx->apidr_info[ctx->ap_pos].ok) {
                            ctx->mode_page = 4;
                        }
                        break;

                    case InputKeyBack:
                        if(ctx->mode_page == 4) {
                            ctx->mode_page = 3;
                        } else if(ctx->mode_page != 0) {
                            ctx->mode_page = 0;
                        } else {
                            processing = false;
                        }
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
