/* Copyright (C) 2023 Salvatore Sanfilippo -- All Rights Reserved
 * See the LICENSE file for information about the license. */

#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <math.h>

#define TAG "Asteroids" // Used for logging
#define DEBUG_MSG 1
#define SCREEN_XRES 128
#define SCREEN_YRES 64

#define MAXBUL 10
typedef struct AsteroidsApp {
    /* GUI */
    Gui *gui;
    ViewPort *view_port;     /* We just use a raw viewport and we render
                                everything into the low level canvas. */
    FuriMessageQueue *event_queue;  /* Keypress events go here. */

    /* Game state. */
    int running;            /* Once false exists the app. */
    uint32_t ticks;         /* Game ticks. Increments at each refresh. */
    float shipx;            /* Ship x position. */
    float shipy;            /* Ship y position. */
    float shipa;            /* Ship current angle, 2*PI is a full rotation. */
    float shipvx;           /* x velocity. */
    float shipvy;           /* y velocity. */
    float bulletsx[MAXBUL]; /* Bullets x position. */
    float bulletsy[MAXBUL]; /* Bullets y position. */
    int bullets;            /* Active bullets. */
    uint32_t last_bullet_tick; /* Tick the last bullet was fired. */
    float asteroidsx[MAXBUL];  /* Asteroids x position. */
    float asteroidsy[MAXBUL];  /* Asteroids y position. */
    int asteroids;             /* Active asteroids. */
} AsteroidsApp;

/* Rotate the point X,Y by an angle 'a', with center 0,0. */
void rot2D(float x, float y, float *rx, float *ry, float a) {
    *rx = x*(float)cos(a)-y*(float)sin(a),
    *ry = y*(float)cos(a)+x*(float)sin(a);
}

/* Render the ship at the current position, and rotated by the current
 * angle. */
void render_ship(Canvas *const canvas, float x, float y, float a) {
    struct { float x; float y; } shape[3] = {
        {-3,3}, {0,-6}, {3,3}
    };
    for (int j =0; j < 3; j++) {
        float nx, ny;
        rot2D(shape[j].x, shape[j].y, &nx, &ny, a);
        shape[j].x = nx;
        shape[j].y = ny;
    }

    canvas_set_color(canvas, ColorBlack);
    for (int j =0; j < 4; j++) {
        int a = j%3;
        int b = (j+1)%3;
        canvas_draw_line(canvas,x+shape[a].x,y+shape[a].y,
                                x+shape[b].x,y+shape[b].y);
    }
}

/* Render the current game screen. */
static void render_callback(Canvas *const canvas, void *ctx) {
    AsteroidsApp *app = ctx;

    /* Clear screen. */
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, 0, 0, 127, 63);
    render_ship(canvas,app->shipx,app->shipy,app->shipa);
}

/* Here all we do is putting the events into the queue that will be handled
 * in the while() loop of the app entry point function. */
static void input_callback(InputEvent* input_event, void* ctx)
{
    AsteroidsApp *app = ctx;
    furi_message_queue_put(app->event_queue,input_event,FuriWaitForever);
}

/* Allocate the application state and initialize a number of stuff.
 * This is called in the entry point to create the application state. */
AsteroidsApp* asteroids_app_alloc() {
    AsteroidsApp *app = malloc(sizeof(AsteroidsApp));

    app->gui = furi_record_open(RECORD_GUI);
    app->view_port = view_port_alloc();
    view_port_draw_callback_set(app->view_port, render_callback, app);
    view_port_input_callback_set(app->view_port, input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    app->running = 1;
    app->ticks = 0;
    app->shipx = SCREEN_XRES / 2;
    app->shipy = SCREEN_YRES / 2;
    app->shipa = 0;
    app->shipvx = 0;
    app->shipvy = 0;
    app->bullets = 0;
    app->last_bullet_tick = 0;
    app->asteroids = 0;
    return app;
}

/* Free what the application allocated. It is not clear to me if the
 * Flipper OS, once the application exits, will be able to reclaim space
 * even if we forget to free something here. */
void asteroids_app_free(AsteroidsApp *app) {
    furi_assert(app);

    // View related.
    view_port_enabled_set(app->view_port, false);
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);
    furi_record_close(RECORD_GUI);
    furi_message_queue_free(app->event_queue);
    app->gui = NULL;

    free(app);
}

/* Called periodically. Do signal processing here. Data we process here
 * will be later displayed by the render callback. The side effect of this
 * function is to scan for signals and set DetectedSamples. */
static void timer_callback(void *ctx) {
    AsteroidsApp *app = ctx;

    UNUSED(app);
}

/* Handle keys interaction. */
void asteroids_process_keypress(AsteroidsApp *app, InputEvent input) {
    UNUSED(app);
    UNUSED(input);
}

int32_t asteroids_app_entry(void* p) {
    UNUSED(p);
    AsteroidsApp *app = asteroids_app_alloc();

    /* Create a timer. We do data analysis in the callback. */
    FuriTimer *timer = furi_timer_alloc(timer_callback, FuriTimerTypePeriodic, app);
    furi_timer_start(timer, furi_kernel_get_tick_frequency() / 4);

    /* This is the main event loop: here we get the events that are pushed
     * in the queue by input_callback(), and process them one after the
     * other. The timeout is 100 milliseconds, so if not input is received
     * before such time, we exit the queue_get() function and call
     * view_port_update() in order to refresh our screen content. */
    InputEvent input;
    while(app->running) {
        FuriStatus qstat = furi_message_queue_get(app->event_queue, &input, 100);
        if (qstat == FuriStatusOk) {
            if (DEBUG_MSG) FURI_LOG_E(TAG, "Main Loop - Input: type %d key %u",
                    input.type, input.key);

            /* Handle navigation here. Then handle view-specific inputs
             * in the view specific handling function. */
            if (input.type == InputTypeShort &&
                input.key == InputKeyBack)
            {
                app->running = 0;
            } else {
                asteroids_process_keypress(app,input);
            }
        } else {
            /* Useful to understand if the app is still alive when it
             * does not respond because of bugs. */
            if (DEBUG_MSG) {
                static int c = 0; c++;
                if (!(c % 20)) FURI_LOG_E(TAG, "Loop timeout");
            }
        }
        view_port_update(app->view_port);
    }

    furi_timer_free(timer);
    asteroids_app_free(app);
    return 0;
}
