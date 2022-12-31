/* Copyright (C) 2022-2023 Salvatore Sanfilippo -- All Rights Reserved
 * See the LICENSE file for information about the license. */

#include <furi.h>
#include <furi_hal.h>
#include <lib/flipper_format/flipper_format.h>
#include <input/input.h>
#include <gui/gui.h>
#include <stdlib.h>
#include "app.h"
#include "app_buffer.h"

RawSamplesBuffer *RawSamples, *DetectedSamples;
extern const SubGhzProtocolRegistry protoview_protocol_registry;

/* Render the received signal.
 *
 * The screen of the flipper is 128 x 64. Even using 4 pixels per line
 * (where low level signal is one pixel high, high level is 4 pixels
 * high) and 4 pixels of spacing between the different lines, we can
 * plot comfortably 8 lines.
 *
 * The 'idx' argument is the first sample to render in the circular
 * buffer. */
void render_signal(ProtoViewApp* app, Canvas* const canvas, RawSamplesBuffer* buf, uint32_t idx) {
    canvas_set_color(canvas, ColorBlack);

    int rows = 8;
    uint32_t time_per_pixel = app->us_scale;
    bool level = 0;
    uint32_t dur = 0, sample_num = 0;
    for(int row = 0; row < rows; row++) {
        for(int x = 0; x < 128; x++) {
            int y = 3 + row * 8;
            if(dur < time_per_pixel / 2) {
                /* Get more data. */
                raw_samples_get(buf, idx++, &level, &dur);
                sample_num++;
            }

            canvas_draw_line(canvas, x, y, x, y - (level * 3));

            /* Write a small triangle under the last sample detected. */
            if(app->signal_bestlen != 0 && sample_num == app->signal_bestlen + 1) {
                canvas_draw_dot(canvas, x, y + 2);
                canvas_draw_dot(canvas, x - 1, y + 3);
                canvas_draw_dot(canvas, x, y + 3);
                canvas_draw_dot(canvas, x + 1, y + 3);
                sample_num++; /* Make sure we don't mark the next, too. */
            }

            /* Remove from the current level duration the time we
             * just plot. */
            if(dur > time_per_pixel)
                dur -= time_per_pixel;
            else
                dur = 0;
        }
    }
}

/* Return the time difference between a and b, always >= 0 since
 * the absolute value is returned. */
uint32_t duration_delta(uint32_t a, uint32_t b) {
    return a > b ? a - b : b - a;
}

/* This function starts scanning samples at offset idx looking for the
 * longest run of pulses, either high or low, that are among 10%
 * of each other, for a maximum of three classes. The classes are
 * counted separtely for high and low signals (RF on / off) because
 * many devices tend to have different pulse lenghts depending on
 * the level of the pulse.
 *
 * For instance Oregon2 sensors, in the case of protocol 2.1 will send
 * pulses of ~400us (RF on) VS ~580us (RF off). */
#define SEARCH_CLASSES 3
uint32_t search_coherent_signal(RawSamplesBuffer* s, uint32_t idx) {
    struct {
        uint32_t dur[2]; /* dur[0] = low, dur[1] = high */
        uint32_t count[2]; /* Associated observed frequency. */
    } classes[SEARCH_CLASSES];

    memset(classes, 0, sizeof(classes));
    uint32_t minlen = 40, maxlen = 4000; /* Depends on data rate, here we
                                            allow for high and low. */
    uint32_t len = 0; /* Observed len of coherent samples. */
    s->short_pulse_dur = 0;
    for(uint32_t j = idx; j < idx + 500; j++) {
        bool level;
        uint32_t dur;
        raw_samples_get(s, j, &level, &dur);
        if(dur < minlen || dur > maxlen) break; /* return. */

        /* Let's see if it matches a class we already have or if we
         * can populate a new (yet empty) class. */
        uint32_t k;
        for(k = 0; k < SEARCH_CLASSES; k++) {
            if(classes[k].count[level] == 0) {
                classes[k].dur[level] = dur;
                classes[k].count[level] = 1;
                break; /* Sample accepted. */
            } else {
                uint32_t classavg = classes[k].dur[level];
                uint32_t count = classes[k].count[level];
                uint32_t delta = duration_delta(dur, classavg);
                if(delta < classavg / 10) {
                    /* It is useful to compute the average of the class
                     * we are observing. We know how many samples we got so
                     * far, so we can recompute the average easily.
                     * By always having a better estimate of the pulse len
                     * we can avoid missing next samples in case the first
                     * observed samples are too off. */
                    classavg = ((classavg * count) + dur) / (count + 1);
                    classes[k].dur[level] = classavg;
                    classes[k].count[level]++;
                    break; /* Sample accepted. */
                }
            }
        }

        if(k == SEARCH_CLASSES) break; /* No match, return. */

        /* If we are here, we accepted this sample. Try with the next
         * one. */
        len++;
    }

    /* Update the buffer setting the shortest pulse we found
     * among the three classes. This will be used when scaling
     * for visualization. */
    for(int j = 0; j < SEARCH_CLASSES; j++) {
        for(int level = 0; level < 2; level++) {
            if(classes[j].dur[level] == 0) continue;
            if(classes[j].count[level] < 3) continue;
            if(s->short_pulse_dur == 0 || s->short_pulse_dur > classes[j].dur[level]) {
                s->short_pulse_dur = classes[j].dur[level];
            }
        }
    }
    return len;
}

/* Search the buffer with the stored signal (last N samples received)
 * in order to find a coherent signal. If a signal that does not appear to
 * be just noise is found, it is set in DetectedSamples global signal
 * buffer, that is what is rendered on the screen. */
void scan_for_signal(ProtoViewApp* app) {
    /* We need to work on a copy: the RawSamples buffer is populated
     * by the background thread receiving data. */
    RawSamplesBuffer* copy = raw_samples_alloc();
    raw_samples_copy(copy, RawSamples);

    /* Try to seek on data that looks to have a regular high low high low
     * pattern. */
    uint32_t minlen = 13; /* Min run of coherent samples. Up to
                                       12 samples it's very easy to mistake
                                       noise for signal. */

    uint32_t i = 0;
    while(i < copy->total - 1) {
        uint32_t thislen = search_coherent_signal(copy, i);
        if(thislen > minlen && thislen > app->signal_bestlen) {
            app->signal_bestlen = thislen;
            raw_samples_copy(DetectedSamples, copy);
            DetectedSamples->idx = (DetectedSamples->idx + i) % DetectedSamples->total;
            FURI_LOG_E(TAG, "Displayed sample updated (%d samples)", (int)thislen);
        }
        i += thislen ? thislen : 1;
    }
    raw_samples_free(copy);
}

/* Draw some text with a border. If the outside color is black and the inside
 * color is white, it just writes the border of the text, but the function can
 * also be used to write a bold variation of the font setting both the
 * colors to black, or alternatively to write a black text with a white
 * border so that it is visible if there are black stuff on the background. */
void canvas_draw_str_with_border(
    Canvas* canvas,
    uint8_t x,
    uint8_t y,
    const char* str,
    Color text_color,
    Color border_color) {
    struct {
        uint8_t x;
        uint8_t y;
    } dir[8] = {{-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}};

    /* Rotate in all the directions writing the same string to create a
     * border, then write the actual string in the other color in the
     * middle. */
    canvas_set_color(canvas, border_color);
    for(int j = 0; j < 8; j++) canvas_draw_str(canvas, x + dir[j].x, y + dir[j].y, str);
    canvas_set_color(canvas, text_color);
    canvas_draw_str(canvas, x, y, str);
    canvas_set_color(canvas, ColorBlack);
}

/* Raw pulses rendering. This is our default view. */
void render_view_raw_pulses(Canvas* const canvas, ProtoViewApp* app) {
    /* Show signal. */
    render_signal(app, canvas, DetectedSamples, 0);

    /* Show signal information. */
    char buf[64];
    snprintf(buf, sizeof(buf), "%luus", (unsigned long)DetectedSamples->short_pulse_dur);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_with_border(canvas, 97, 63, buf, ColorWhite, ColorBlack);
}

/* Renders a single view with frequency and modulation setting. However
 * this are logically two different views, and only one of the settings
 * will be highlighted. */
void render_view_settings(Canvas* const canvas, ProtoViewApp* app) {
    UNUSED(app);
    canvas_set_font(canvas, FontPrimary);
    if(app->current_view == ViewFrequencySettings)
        canvas_draw_str_with_border(canvas, 1, 10, "Frequency", ColorWhite, ColorBlack);
    else
        canvas_draw_str(canvas, 1, 10, "Frequency");

    if(app->current_view == ViewModulationSettings)
        canvas_draw_str_with_border(canvas, 70, 10, "Modulation", ColorWhite, ColorBlack);
    else
        canvas_draw_str(canvas, 70, 10, "Modulation");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 10, 61, "Use up and down to modify");

    /* Show frequency. We can use big numbers font since it's just a number. */
    if(app->current_view == ViewFrequencySettings) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%.2f", (double)app->frequency / 1000000);
        canvas_set_font(canvas, FontBigNumbers);
        canvas_draw_str(canvas, 30, 40, buf);
    } else if(app->current_view == ViewModulationSettings) {
        int current = app->modulation;
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 33, 39, ProtoViewModulations[current].name);
    }
}

/* The callback actually just passes the control to the actual active
 * view callback, after setting up basic stuff like cleaning the screen
 * and setting color to black. */
static void render_callback(Canvas* const canvas, void* ctx) {
    ProtoViewApp* app = ctx;

    /* Clear screen. */
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, 0, 0, 127, 63);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);

    /* Call who is in charge right now. */
    switch(app->current_view) {
    case ViewRawPulses:
        render_view_raw_pulses(canvas, app);
        break;
    case ViewFrequencySettings:
    case ViewModulationSettings:
        render_view_settings(canvas, app);
        break;
    case ViewLast:
        furi_crash(TAG " ViewLast selected");
        break;
    }
}

/* Here all we do is putting the events into the queue that will be handled
 * in the while() loop of the app entry point function. */
static void input_callback(InputEvent* input_event, void* ctx) {
    ProtoViewApp* app = ctx;

    if(input_event->type == InputTypePress) {
        furi_message_queue_put(app->event_queue, input_event, FuriWaitForever);
        FURI_LOG_E(TAG, "INPUT CALLBACK %d", (int)input_event->key);
    }
}

/* Allocate the application state and initialize a number of stuff.
 * This is called in the entry point to create the application state. */
ProtoViewApp* protoview_app_alloc() {
    ProtoViewApp* app = malloc(sizeof(ProtoViewApp));

    // Init shared data structures
    RawSamples = raw_samples_alloc();
    DetectedSamples = raw_samples_alloc();

    //init setting
    app->setting = subghz_setting_alloc();
    subghz_setting_load(app->setting, EXT_PATH("protoview/settings.txt"));

    // GUI
    app->gui = furi_record_open(RECORD_GUI);
    app->view_port = view_port_alloc();
    view_port_draw_callback_set(app->view_port, render_callback, app);
    view_port_input_callback_set(app->view_port, input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->current_view = ViewRawPulses;

    // Signal found and visualization defaults
    app->signal_bestlen = 0;
    app->us_scale = 100;

    //init Worker & Protocol
    app->txrx = malloc(sizeof(ProtoViewTxRx));

    /* Setup rx worker and environment. */
    app->txrx->worker = subghz_worker_alloc();
    app->txrx->environment = subghz_environment_alloc();
    subghz_environment_set_protocol_registry(
        app->txrx->environment, (void*)&protoview_protocol_registry);
    app->txrx->receiver = subghz_receiver_alloc_init(app->txrx->environment);

    subghz_receiver_set_filter(app->txrx->receiver, SubGhzProtocolFlag_Decodable);
    subghz_worker_set_overrun_callback(
        app->txrx->worker, (SubGhzWorkerOverrunCallback)subghz_receiver_reset);
    subghz_worker_set_pair_callback(
        app->txrx->worker, (SubGhzWorkerPairCallback)subghz_receiver_decode);
    subghz_worker_set_context(app->txrx->worker, app->txrx->receiver);

    app->frequency = subghz_setting_get_default_frequency(app->setting);
    app->modulation = 0; /* Defaults to ProtoViewModulations[0]. */

    furi_hal_power_suppress_charge_enter();
    app->running = 1;

    return app;
}

/* Free what the application allocated. It is not clear to me if the
 * Flipper OS, once the application exits, will be able to reclaim space
 * even if we forget to free something here. */
void protoview_app_free(ProtoViewApp* app) {
    furi_assert(app);

    // Put CC1101 on sleep.
    radio_sleep(app);

    // View related.
    view_port_enabled_set(app->view_port, false);
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);
    furi_record_close(RECORD_GUI);
    furi_message_queue_free(app->event_queue);
    app->gui = NULL;

    // Frequency setting.
    subghz_setting_free(app->setting);

    // Worker stuff.
    subghz_receiver_free(app->txrx->receiver);
    subghz_environment_free(app->txrx->environment);
    subghz_worker_free(app->txrx->worker);
    free(app->txrx);

    // Raw samples buffers.
    raw_samples_free(RawSamples);
    raw_samples_free(DetectedSamples);
    furi_hal_power_suppress_charge_exit();

    free(app);
}

/* Called periodically. Do signal processing here. Data we process here
 * will be later displayed by the render callback. The side effect of this
 * function is to scan for signals and set DetectedSamples. */
static void timer_callback(void* ctx) {
    ProtoViewApp* app = ctx;
    scan_for_signal(app);
}

/* Handle input for the raw pulses view. */
void process_input_raw_pulses(ProtoViewApp* app, InputEvent input) {
    if(input.key == InputKeyOk) {
        /* Reset the current sample to capture the next. */
        app->signal_bestlen = 0;
        raw_samples_reset(DetectedSamples);
        raw_samples_reset(RawSamples);
    } else if(input.key == InputKeyDown) {
        /* Rescaling. The set becomes finer under 50us per pixel. */
        uint32_t scale_step = app->us_scale >= 50 ? 50 : 10;
        if(app->us_scale < 500) app->us_scale += scale_step;
    } else if(input.key == InputKeyUp) {
        uint32_t scale_step = app->us_scale > 50 ? 50 : 10;
        if(app->us_scale > 10) app->us_scale -= scale_step;
    }
}

/* Handle input for the settings view. */
void process_input_settings(ProtoViewApp* app, InputEvent input) {
    /* Here we handle only up and down. Avoid any work if the user
     * pressed something else. */
    if(input.key != InputKeyDown && input.key != InputKeyUp) return;

    if(app->current_view == ViewFrequencySettings) {
        size_t curidx = 0, i;
        size_t count = subghz_setting_get_frequency_count(app->setting);

        /* Scan the list of frequencies to check for the index of the
         * currently set frequency. */
        for(i = 0; i < count; i++) {
            uint32_t freq = subghz_setting_get_frequency(app->setting, i);
            if(freq == app->frequency) {
                curidx = i;
                break;
            }
        }
        if(i == count) return; /* Should never happen. */

        if(input.key == InputKeyUp) {
            curidx = (curidx + 1) % count;
        } else if(input.key == InputKeyDown) {
            curidx = curidx == 0 ? count - 1 : curidx - 1;
        }
        app->frequency = subghz_setting_get_frequency(app->setting, curidx);
    } else if(app->current_view == ViewModulationSettings) {
        uint32_t count = 0;
        uint32_t modid = app->modulation;

        while(ProtoViewModulations[count].name != NULL) count++;
        if(input.key == InputKeyUp) {
            modid = (modid + 1) % count;
        } else if(input.key == InputKeyDown) {
            modid = modid == 0 ? count - 1 : modid - 1;
        }
        app->modulation = modid;
    }

    /* Apply changes. */
    FURI_LOG_E(
        TAG,
        "Setting view, setting frequency/modulation to %lu %s",
        app->frequency,
        ProtoViewModulations[app->modulation].name);
    radio_rx_end(app);
    radio_begin(app);
    radio_rx(app);
}

int32_t protoview_app_entry(void* p) {
    UNUSED(p);
    ProtoViewApp* app = protoview_app_alloc();

    /* Create a timer. We do data analysis in the callback. */
    FuriTimer* timer = furi_timer_alloc(timer_callback, FuriTimerTypePeriodic, app);
    furi_timer_start(timer, furi_kernel_get_tick_frequency() / 4);

    /* Start listening to signals immediately. */
    radio_begin(app);
    radio_rx(app);

    /* This is the main event loop: here we get the events that are pushed
     * in the queue by input_callback(), and process them one after the
     * other. The timeout is 100 milliseconds, so if not input is received
     * before such time, we exit the queue_get() function and call
     * view_port_update() in order to refresh our screen content. */
    InputEvent input;
    while(app->running) {
        FuriStatus qstat = furi_message_queue_get(app->event_queue, &input, 100);
        if(qstat == FuriStatusOk) {
            FURI_LOG_E(TAG, "Main Loop - Input: %u", input.key);

            /* Handle navigation here. Then handle view-specific inputs
             * in the view specific handling function. */
            if(input.key == InputKeyBack) {
                /* Exit the app. */
                app->running = 0;
            } else if(input.key == InputKeyRight) {
                /* Go to the next view. */
                app->current_view++;
                if(app->current_view == ViewLast) app->current_view = 0;
            } else if(input.key == InputKeyLeft) {
                /* Go to the previous view. */
                if(app->current_view == 0)
                    app->current_view = ViewLast - 1;
                else
                    app->current_view--;
            } else {
                /* This is where we pass the control to the currently
                 * active view input processing. */
                switch(app->current_view) {
                case ViewRawPulses:
                    process_input_raw_pulses(app, input);
                    break;
                case ViewFrequencySettings:
                case ViewModulationSettings:
                    process_input_settings(app, input);
                    break;
                case ViewLast:
                    furi_crash(TAG " ViewLast selected");
                    break;
                }
            }
        } else {
            /* Useful to understand if the app is still alive when it
             * does not respond because of bugs. */
            static int c = 0;
            c++;
            if(!(c % 20)) FURI_LOG_E(TAG, "Loop timeout");
        }
        view_port_update(app->view_port);
    }

    /* App no longer running. Shut down and free. */
    if(app->txrx->txrx_state == TxRxStateRx) {
        FURI_LOG_E(TAG, "Putting CC1101 to sleep before exiting.");
        radio_rx_end(app);
        radio_sleep(app);
    }

    furi_timer_free(timer);
    protoview_app_free(app);
    return 0;
}
