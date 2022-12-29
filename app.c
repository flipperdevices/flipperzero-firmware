#include <furi.h>
#include <furi_hal.h>
#include <lib/flipper_format/flipper_format.h>
#include <input/input.h>
#include <gui/gui.h>
#include <stdlib.h>
#include "app.h"
#include "app_buffer.h"

#define FREQ 433920000

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
void render_signal(ProtoViewApp *app, Canvas *const canvas, RawSamplesBuffer *buf, uint32_t idx) {
    canvas_set_color(canvas, ColorBlack);

    int rows = 8;
    uint32_t time_per_pixel = app->us_scale;
    bool level = 0;
    uint32_t dur = 0;
    for (int row = 0; row < rows ; row++) {
        for (int x = 0; x < 128; x++) {
            int y = 3 + row*8;
            if (dur < time_per_pixel/2) {
                /* Get more data. */
                raw_samples_get(buf, idx++, &level, &dur);
            }

            canvas_draw_line(canvas, x,y,x,y-(level*3));

            /* Remove from the current level duration the time we
             * just plot. */
            if (dur > time_per_pixel)
                dur -= time_per_pixel;
            else
                dur = 0;
        }
    }
}

/* Return the time difference between a and b, always >= 0 since
 * the absolute value is returned. */
uint32_t duration_delta(uint32_t a, uint32_t b) {
    return a > b ? a - b : b -a;
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
uint32_t search_coherent_signal(RawSamplesBuffer *s, uint32_t idx) {
    struct {
        uint32_t dur[2];     /* dur[0] = low, dur[1] = high */
        uint32_t count[2];   /* Associated observed frequency. */
    } classes[SEARCH_CLASSES];

    memset(classes,0,sizeof(classes));
    uint32_t minlen = 40, maxlen = 4000; /* Depends on data rate, here we
                                            allow for high and low. */
    uint32_t len = 0; /* Observed len of coherent samples. */
    s->short_pulse_dur = 0;
    for (uint32_t j = idx; j < idx+100; j++) {
        bool level;
        uint32_t dur;
        raw_samples_get(s, j, &level, &dur);
        if (dur < minlen || dur > maxlen) break; /* return. */

        /* Let's see if it matches a class we already have or if we
         * can populate a new (yet empty) class. */
        uint32_t k;
        for (k = 0; k < SEARCH_CLASSES; k++) {
            if (classes[k].count[level] == 0) {
                classes[k].dur[level] = dur;
                classes[k].count[level] = 1;
                break; /* Sample accepted. */
            } else {
                uint32_t classavg = classes[k].dur[level];
                uint32_t count = classes[k].count[level];
                uint32_t delta = duration_delta(dur,classavg);
                if (delta < classavg/10) {
                    /* It is useful to compute the average of the class
                     * we are observing. We know how many samples we got so
                     * far, so we can recompute the average easily.
                     * By always having a better estimate of the pulse len
                     * we can avoid missing next samples in case the first
                     * observed samples are too off. */
                    classavg = ((classavg * count) + dur) / (count+1);
                    classes[k].dur[level] = classavg;
                    classes[k].count[level]++;
                    break; /* Sample accepted. */
                }
            }
        }

        if (k == SEARCH_CLASSES) break; /* No match, return. */

        /* If we are here, we accepted this sample. Try with the next
         * one. */
        len++;
    }

    /* Update the buffer setting the shortest pulse we found
     * among the three classes. This will be used when scaling
     * for visualization. */
    for (int j = 0; j < SEARCH_CLASSES; j++) {
        for (int level = 0; level < 2; level++) {
            if (classes[j].dur[level] == 0) continue;
            if (classes[j].count[level] < 3) continue;
            if (s->short_pulse_dur == 0 ||
                s->short_pulse_dur > classes[j].dur[level])
            {
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
void scan_for_signal(ProtoViewApp *app) {
    /* We need to work on a copy: the RawSamples buffer is populated
     * by the background thread receiving data. */
    RawSamplesBuffer *copy = raw_samples_alloc();
    raw_samples_copy(copy,RawSamples);

    /* Try to seek on data that looks to have a regular high low high low
     * pattern. */
    uint32_t minlen = 13;           /* Min run of coherent samples. Up to
                                       12 samples it's very easy to mistake
                                       noise for signal. */

    for (uint32_t i = 0; i < copy->total-1; i++) {
        uint32_t thislen = search_coherent_signal(copy,i);
        if (thislen > minlen && thislen > app->signal_bestlen) {
            app->signal_bestlen = thislen;
            raw_samples_copy(DetectedSamples,copy);
            DetectedSamples->idx = (DetectedSamples->idx+i)%
                                   DetectedSamples->total;
            FURI_LOG_E(TAG, "Displayed sample updated (%d samples)",
                (int)thislen);
        }
    }
    raw_samples_free(copy);
}

/* Draw some white text with a black border. */
void canvas_draw_str_with_border(Canvas* canvas, uint8_t x, uint8_t y, const char* str)
{
    struct {
        uint8_t x; uint8_t y;
    } dir[8] = {
        {-1,-1},
        {0,-1},
        {1,-1},
        {1,0},
        {1,1},
        {0,1},
        {-1,1},
        {-1,0}
    };

    /* Rotate in all the directions writing the same string in black
     * to create a border, then write the actaul string in white in the
     * middle. */
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);
    for (int j = 0; j < 8; j++)
        canvas_draw_str(canvas,x+dir[j].x,y+dir[j].y,str);
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_str(canvas,x,y,str);
}

static void render_callback(Canvas *const canvas, void *ctx) {
    ProtoViewApp *app = ctx;

    /* Clear screen. */
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, 0, 0, 127, 63);

    /* Show signal. */
    render_signal(app, canvas, DetectedSamples, 0);

    /* Show signal information. */
    char buf[64];
    snprintf(buf,sizeof(buf),"%luus", (unsigned long)DetectedSamples->short_pulse_dur);
    canvas_draw_str_with_border(canvas, 100, 63, buf);
}

/* Here all we do is putting the events into the queue that will be handled
 * in the while() loop of the app entry point function. */
static void input_callback(InputEvent* input_event, void* ctx)
{
    ProtoViewApp *app = ctx;

    if (input_event->type == InputTypePress) {
        furi_message_queue_put(app->event_queue,input_event,FuriWaitForever);
        FURI_LOG_E(TAG, "INPUT CALLBACK %d", (int)input_event->key);
    }
}

ProtoViewApp* protoview_app_alloc() {
    ProtoViewApp *app = malloc(sizeof(ProtoViewApp));

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

    // Signal found and visualization defaults
    app->signal_bestlen = 0;
    app->us_scale = 100;

    //init Worker & Protocol
    app->txrx = malloc(sizeof(ProtoViewTxRx));
    app->txrx->preset = malloc(sizeof(SubGhzRadioPreset));
    app->txrx->preset->name = furi_string_alloc();

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

    furi_hal_power_suppress_charge_enter();
    app->running = 1;

    return app;
}

void protoview_app_free(ProtoViewApp *app) {
    furi_assert(app);

    //CC1101 off
    radio_sleep(app);

    // View
    view_port_enabled_set(app->view_port, false);
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);
    furi_record_close(RECORD_GUI);
    furi_message_queue_free(app->event_queue);
    app->gui = NULL;

    //setting
    subghz_setting_free(app->setting);

    //Worker
    subghz_receiver_free(app->txrx->receiver);
    subghz_environment_free(app->txrx->environment);
    subghz_worker_free(app->txrx->worker);
    furi_string_free(app->txrx->preset->name);
    free(app->txrx->preset);
    free(app->txrx);

    furi_hal_power_suppress_charge_exit();
    raw_samples_free(RawSamples);
    raw_samples_free(DetectedSamples);

    free(app);
}

/* Called 10 times per second. Do signal processing here. Data we process here
 * will be later displayed by the render callback. The side effect of this
 * function is to scan for signals and set DetectedSamples. */
static void timer_callback(void *ctx) {
    ProtoViewApp *app = ctx;
    scan_for_signal(app);
}

int32_t protoview_app_entry(void* p) {
    UNUSED(p);
    ProtoViewApp *app = protoview_app_alloc();

    /* Create a timer. We do data analysis in the callback. */
    FuriTimer *timer = furi_timer_alloc(timer_callback, FuriTimerTypePeriodic, app);
    furi_timer_start(timer, furi_kernel_get_tick_frequency() / 10);

    radio_begin(app);
    radio_rx(app, FREQ);

    InputEvent input;
    while(app->running) {
        FuriStatus qstat = furi_message_queue_get(app->event_queue, &input, 100);
        if (qstat == FuriStatusOk) {
            if (input.key == InputKeyBack) {
                app->running = 0;
            } else if (input.key == InputKeyOk) {
                app->signal_bestlen = 0;
                raw_samples_reset(DetectedSamples);
                raw_samples_reset(RawSamples);
            } else if (input.key == InputKeyDown) {
                uint32_t scale_step = app->us_scale >= 50 ? 50 : 10;
                if (app->us_scale < 500) app->us_scale += scale_step;
            } else if (input.key == InputKeyUp) {
                uint32_t scale_step = app->us_scale > 50 ? 50 : 10;
                if (app->us_scale > 10) app->us_scale -= scale_step;
            }

            FURI_LOG_E(TAG, "Main Loop - Input: %u", input.key);
        } else {
            static int c = 0;
            c++;
            if (!(c % 20)) FURI_LOG_E(TAG, "Loop timeout");
        }
        view_port_update(app->view_port);
    }

    if (app->txrx->txrx_state == TxRxStateRx) {
        FURI_LOG_E(TAG, "Putting CC1101 to sleep before exiting.");
        radio_rx_end(app);
        radio_sleep(app);
    }

    furi_timer_free(timer);
    protoview_app_free(app);
    return 0;
}
