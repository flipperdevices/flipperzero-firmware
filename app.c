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
#ifndef PI
#define PI 3.14159265358979f
#endif

#define MAXBUL 10   /* Max bullets on the screen. */
#define MAXAST 8    /* Max asteroids on the screen. */
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
    int bullets_num;        /* Active bullets. */
    uint32_t last_bullet_tick; /* Tick the last bullet was fired. */
    struct {
        float x, y, vx, vy, size;
        uint8_t shape_seed;
    } asteroids[MAXAST];        /* Asteroids state. */
    int asteroids_num;          /* Active asteroids. */
    uint32_t pressed[InputKeyMAX]; /* pressed[id] is true if pressed.
                                      Each array item contains the time
                                      in milliseconds the key was pressed. */
    bool fire;                 /* Short press detected: fire a bullet. */
} AsteroidsApp;

/* This structure represents a polygon of at most POLY_MAX points.
 * The function draw_poly() is able to render it on the screen, rotated
 * by the amount specified. */
#define POLY_MAX 8
typedef struct Poly {
    float x[POLY_MAX];
    float y[POLY_MAX];
    uint32_t points; /* Number of points actually populated. */
} Poly;

/* Define the polygons we use. */
Poly ShipPoly = {
    {-3, 0, 3},
    {-3, 6, -3},
    3
};

/* Rotate the point of the poligon 'poly' and store the new rotated
 * polygon in 'rot'. The polygon is rotated by an angle 'a', with
 * center at 0,0. */
void rotate_poly(Poly *rot, Poly *poly, float a) {
    /* We want to compute sin(a) and cos(a) only one time
     * for every point to rotate. It's a slow operation. */
    float sin_a = (float)sin(a);
    float cos_a = (float)cos(a);
    for (uint32_t j = 0; j < poly->points; j++) {
        rot->x[j] = poly->x[j]*cos_a - poly->y[j]*sin_a;
        rot->y[j] = poly->y[j]*cos_a + poly->x[j]*sin_a;
    }
    rot->points = poly->points;
}

#if 0
/* This is an 8 bit LFSR we use to generate a predictable and fast
 * pseudorandom sequence of numbers, to give a different shape to
 * each asteroid. */
static void lfsr_next(unsigned char *prev) {
    unsigned char lsb = *prev & 1;
    *prev = *prev >> 1;
    if (lsb == 1) *prev ^= 0b11000111;
}
#endif

/* Render the polygon 'poly' at x,y, rotated by the specified angle. */
void draw_poly(Canvas *const canvas, Poly *poly, uint8_t x, uint8_t y, float a)
{
    Poly rot;
    rotate_poly(&rot,poly,a);
    canvas_set_color(canvas, ColorBlack);
    for (uint32_t j = 0; j < rot.points; j++) {
        uint32_t a = j;
        uint32_t b = j+1;
        if (b == rot.points) b = 0;
        canvas_draw_line(canvas,x+rot.x[a],y+rot.y[a],
                                x+rot.x[b],y+rot.y[b]);
    }
}

/* Render the current game screen. */
static void render_callback(Canvas *const canvas, void *ctx) {
    AsteroidsApp *app = ctx;

    /* Clear screen. */
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, 0, 0, 127, 63);

    /* Draw ship and asteroids. */
    draw_poly(canvas,&ShipPoly,app->shipx,app->shipy,app->shipa);
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
    app->shipa = PI; /* Start headed towards top. */
    app->shipvx = 0;
    app->shipvy = 0;
    app->bullets_num = 0;
    app->last_bullet_tick = 0;
    app->asteroids_num = 0;
    memset(app->pressed,0,sizeof(app->pressed));
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

/* Thi is the main game execution function, called 10 times for
 * second (with the Flipper screen latency, an higher FPS does not
 * make sense). In this function we update the position of objects based
 * on velocity. Detect collisions. Update the score and so forth.
 *
 * Each time this function is called, app->tick is incremented. */
static void game_tick(void *ctx) {
    AsteroidsApp *app = ctx;
    if (app->pressed[InputKeyLeft]) app->shipa -= .2;
    if (app->pressed[InputKeyRight]) app->shipa += .2;
    if (app->pressed[InputKeyOk]) {
        app->shipvx -= 0.15*(float)sin(app->shipa);
        app->shipvy += 0.15*(float)cos(app->shipa);
    }
    
    /* Update ship position according to its velocity. */
    app->shipx += app->shipvx;
    app->shipy += app->shipvy;

    /* Return back from one side to the other of the screen. */
    if (app->shipx >= SCREEN_XRES) app->shipx = 0;
    else if (app->shipx < 0) app->shipx = SCREEN_XRES-1;
    if (app->shipy >= SCREEN_YRES) app->shipy = 0;
    else if (app->shipy < 0) app->shipy = SCREEN_YRES-1;

    app->ticks++;
    view_port_update(app->view_port);
}

/* Handle keys interaction. */
void asteroids_update_keypress_state(AsteroidsApp *app, InputEvent input) {
    if (input.type == InputTypePress) {
        app->pressed[input.key] = furi_get_tick();
    } else if (input.type == InputTypeRelease) {
        uint32_t dur = furi_get_tick() - app->pressed[input.key];
        app->pressed[input.key] = 0;
        if (dur < 100 && input.key == InputKeyOk) app->fire = true;
    }
}

int32_t asteroids_app_entry(void* p) {
    UNUSED(p);
    AsteroidsApp *app = asteroids_app_alloc();

    /* Create a timer. We do data analysis in the callback. */
    FuriTimer *timer = furi_timer_alloc(game_tick, FuriTimerTypePeriodic, app);
    furi_timer_start(timer, furi_kernel_get_tick_frequency() / 10);

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
                asteroids_update_keypress_state(app,input);
            }
        } else {
            /* Useful to understand if the app is still alive when it
             * does not respond because of bugs. */
            if (DEBUG_MSG) {
                static int c = 0; c++;
                if (!(c % 20)) FURI_LOG_E(TAG, "Loop timeout");
            }
        }
    }

    furi_timer_free(timer);
    asteroids_app_free(app);
    return 0;
}
