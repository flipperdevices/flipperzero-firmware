#include "genie_send.h"

#include "genie_app.h"
#include "genie_submenu.h"
#include "genie_subghz_receive.h"
#include "genie_file.h"

#include <flipper_format/flipper_format_i.h>
#include <lib/subghz/transmitter.h>

#ifdef TAG
#undef TAG
#endif
#define TAG "GenieSend"

typedef struct GenieSend GenieSend;

struct GenieSend {
    View* view;
};

typedef struct {
    GenieApp* app;
    GenieFile* file;
    bool loaded;
} GenieRefs;

static void
    set_genie(FlipperFormat* flipper_format, uint32_t key_hi, uint32_t key_lo, uint32_t repeat) {
    FuriString* genie_settings = furi_string_alloc_printf(
        "Protocol: Genie\n"
        "Bit: 64\n"
        "Key: %02X %02X %02X %02X %02X %02X %02X %02X\n"
        "Increment: 0\n"
        "Repeat: %lu\n",
        (uint8_t)((key_hi >> 24) & 0xFFU),
        (uint8_t)((key_hi >> 16) & 0xFFU),
        (uint8_t)((key_hi >> 8) & 0xFFU),
        (uint8_t)((key_hi) & 0xFFU),
        (uint8_t)((key_lo >> 24) & 0xFFU),
        (uint8_t)((key_lo >> 16) & 0xFFU),
        (uint8_t)((key_lo >> 8) & 0xFFU),
        (uint8_t)((key_lo) & 0xFFU),
        repeat);
    Stream* stream = flipper_format_get_raw_stream(flipper_format);
    stream_clean(stream);
    stream_write_cstring(stream, furi_string_get_cstr(genie_settings));
    stream_seek(stream, 0, StreamOffsetFromStart);
    furi_string_free(genie_settings);
}

const SubGhzProtocol* send_subghz_protocol_registry_items[] = {
    &subghz_protocol_genie,
};

const SubGhzProtocolRegistry send_subghz_protocol_registry = {
    .items = send_subghz_protocol_registry_items,
    .size = COUNT_OF(send_subghz_protocol_registry_items)};

static void send_genie(uint32_t key_hi, uint32_t key_lo, uint32_t frequency) {
    uint32_t repeat = 5;
    const SubGhzDevice* device;

    subghz_devices_init();
    device = subghz_devices_get_by_name(SUBGHZ_DEVICE_CC1101_INT_NAME);
    SubGhzEnvironment* environment = subghz_environment_alloc();
    subghz_environment_set_protocol_registry(environment, (void*)&send_subghz_protocol_registry);
    SubGhzTransmitter* transmitter = subghz_transmitter_alloc_init(environment, "Genie");
    FlipperFormat* flipper_format = flipper_format_string_alloc();
    set_genie(flipper_format, key_hi, key_lo, repeat);
    subghz_transmitter_deserialize(transmitter, flipper_format);
    subghz_devices_begin(device);
    subghz_devices_reset(device);
    subghz_devices_load_preset(device, FuriHalSubGhzPresetOok650Async, NULL);
    frequency = subghz_devices_set_frequency(device, frequency);

    // Send
    furi_hal_power_suppress_charge_enter();
    if(subghz_devices_start_async_tx(device, subghz_transmitter_yield, transmitter)) {
        while(!(subghz_devices_is_async_complete_tx(device))) {
            furi_delay_ms(100);
        }
        subghz_devices_stop_async_tx(device);
    }

    // Cleanup
    subghz_devices_sleep(device);
    subghz_devices_end(device);
    subghz_devices_deinit();
    furi_hal_power_suppress_charge_exit();
    flipper_format_free(flipper_format);
    subghz_transmitter_free(transmitter);
    subghz_environment_free(environment);
}

static void genie_send_draw_callback(Canvas* canvas, void* model) {
    GenieRefs* refs = (GenieRefs*)model;
    GenieFile* file = refs->file;
    char buffer[36] = {0};

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 5, 10, "Genie Sub-Ghz Sender!!!");
    canvas_set_font(canvas, FontSecondary);
    if(refs->loaded) {
        if(!file) {
            return;
        }
        snprintf(
            buffer,
            COUNT_OF(buffer),
            "KEY: %08lx %08lx",
            genie_file_get_key_hi(file),
            genie_file_get_key_lo(file));
        canvas_draw_str(canvas, 1, 23, buffer);

        if(genie_file_get_rec_count(file) < 0xFFFD) {
            snprintf(
                buffer,
                COUNT_OF(buffer),
                "Last sent: %d of %d",
                genie_file_get_last_sent(file),
                genie_file_get_rec_count(file));
            canvas_draw_str(canvas, 1, 33, buffer);

            snprintf(buffer, COUNT_OF(buffer), "Remote not fully captured!");
            canvas_draw_str(canvas, 1, 49, buffer);
        } else {
            snprintf(buffer, COUNT_OF(buffer), "Last sent: %d", genie_file_get_last_sent(file));
            canvas_draw_str(canvas, 1, 33, buffer);
        }

        // TODO: Is this off by 1?
        if(genie_file_get_last_sent(file) < genie_file_get_rec_count(file)) {
            canvas_draw_str(canvas, 1, 63, "Press OK to send next code.");
        } else {
            canvas_draw_str(canvas, 1, 61, "Long press OK to reset code.");
        }
    } else {
        canvas_draw_str(canvas, 20, 36, "NO .GNE FILE LOADED");
    }
}

static void genie_send_refresh_file(View* view) {
    with_view_model(
        view,
        GenieRefs * refs,
        {
            const char* path = genie_app_get_file_path(refs->app);
            if(refs->file) {
                genie_file_free(refs->file);
                refs->file = NULL;
            }
            refs->file = genie_file_load(path);
        },
        true);
}

static bool genie_send_input_callback(InputEvent* event, void* context) {
    View* view = (View*)context;
    genie_send_refresh_file(view);

    if(event->type == InputTypeShort) {
        if(event->key == InputKeyOk) {
            with_view_model(
                view,
                GenieRefs * refs,
                {
                    if(refs->file) {
                        if(genie_file_get_last_sent(refs->file) <
                           genie_file_get_rec_count(refs->file)) {
                            send_genie(
                                genie_file_get_key_hi(refs->file),
                                genie_file_get_key_lo(refs->file),
                                genie_app_get_frequency(refs->app));

                            genie_file_set_last_sent(
                                genie_app_get_file_path(refs->app),
                                genie_file_get_last_sent(refs->file) + 1);
                            genie_send_refresh_file(view);
                        }
                    }
                },
                false);
            return true;
        }
    } else if(event->type == InputTypeLong) {
        with_view_model(
            view,
            GenieRefs * refs,
            {
                if(refs->file) {
                    if(genie_file_get_last_sent(refs->file) >=
                       genie_file_get_rec_count(refs->file)) {
                        genie_file_set_last_sent(genie_app_get_file_path(refs->app), 0);
                        genie_send_refresh_file(view);
                    }
                }
            },
            true);
        return true;
    }

    return false;
}

static void genie_send_enter_callback(void* context) {
    View* view = (View*)context;

    with_view_model(
        view,
        GenieRefs * refs,
        {
            const char* path = genie_app_get_file_path(refs->app);
            refs->file = genie_file_load(path);
            refs->loaded = refs->file != NULL;
        },
        false);
}

GenieSend* genie_send_alloc(GenieApp* app) {
    UNUSED(app);
    GenieSend* genie_send = malloc(sizeof(GenieSend));
    View* view = view_alloc();
    genie_send->view = view;
    view_set_draw_callback(view, genie_send_draw_callback);
    view_set_input_callback(view, genie_send_input_callback);
    view_set_previous_callback(view, genie_navigation_submenu_callback);
    view_set_enter_callback(view, genie_send_enter_callback);
    view_set_context(view, view);
    view_allocate_model(view, ViewModelTypeLockFree, sizeof(GenieRefs));
    GenieRefs* refs = (GenieRefs*)view_get_model(view);
    refs->app = app;
    refs->file = NULL;
    refs->loaded = false;

    return genie_send;
}

void genie_send_free(GenieSend* genie_send) {
    furi_assert(genie_send);
    furi_assert(genie_send->view);
    with_view_model(
        genie_send->view,
        GenieRefs * refs,
        {
            genie_file_free(refs->file);
            refs->file = NULL;
        },
        false);
    view_free(genie_send->view);
    genie_send->view = NULL;
    free(genie_send);
}

View* genie_send_get_view(GenieSend* genie_send) {
    furi_assert(genie_send);
    furi_assert(genie_send->view);
    return genie_send->view;
}