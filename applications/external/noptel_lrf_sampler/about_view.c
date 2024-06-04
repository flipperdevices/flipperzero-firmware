/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.9
 *
 * About view
***/

/*** Includes ***/
#include "common.h"
#include "noptel_lrf_sampler_icons.h" /* Generated from images in assets */

/*** Routines ***/

/** About view enter callback **/
void about_view_enter_callback(void* ctx) {
    App* app = (App*)ctx;

    with_view_model(
        app->about_view,
        AboutModel * about_model,
        {
            /* Start at the first screen */
            about_model->screen = 0;
        },
        false);
}

/** Draw callback for the about view **/
void about_view_draw_callback(Canvas* canvas, void* model) {
    AboutModel* about_model = (AboutModel*)model;

    /* Which screen should we draw? */
    switch(about_model->screen) {
    /* Draw the splash screen with the version number */
    case 0:

        /* Draw the screen's background */
        canvas_draw_icon(canvas, 0, 0, &I_about_splash);

        /* Draw the app's version number */
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 1, 64, "v");
        canvas_draw_str(canvas, 6, 64, VERSION);

        /* Draw a right arrow at the top right */
        canvas_draw_icon(canvas, 124, 0, &I_arrow_right);

        break;

    /* Draw the app purpose description screen */
    case 1:

        /* Draw the title */
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 15, 8, "Noptel LRF Sampler");

        /* Draw the URL */
        canvas_draw_str(canvas, 25, 62, "https://noptel.fi");

        /* Draw the description */
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 10, 29, "Get measurements from a");
        canvas_draw_str(canvas, 15, 40, "Noptel LRF Rangefinder");

        /* Draw a left arrow at the top left */
        canvas_draw_icon(canvas, 0, 0, &I_arrow_left);

        /* Draw a right arrow at the top right */
        canvas_draw_icon(canvas, 124, 0, &I_arrow_right);

        break;

    /* Draw the screen showing the GPIO pin connections */
    case 2:

        /* Draw the screen's background */
        canvas_draw_icon(canvas, 0, 0, &I_about_gpio_pin_connections);

        /* Draw the title */
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 11, 8, "GPIO pin connections");

        /* Draw a left arrow at the top left */
        canvas_draw_icon(canvas, 0, 0, &I_arrow_left);

        canvas_set_font(canvas, FontSecondary);
        canvas_invert_color(canvas);

        /* Draw "LRF" over the LRF side of the pinout diagram */
        canvas_draw_str(canvas, 56, 22, "LRF");

        /* Draw "Flipper Zero" under the Flipper side of the pinout diagram */
        canvas_draw_str(canvas, 39, 62, "Flipper Zero");

        break;
    }
}

/** Input callback for the about view**/
bool about_view_input_callback(InputEvent* evt, void* ctx) {
    App* app = (App*)ctx;
    AboutModel* about_model = view_get_model(app->about_view);
    bool evt_handled = false;

    /* Was the event a button press? */
    if(evt->type == InputTypePress) /* Which button was pressed? */
        switch(evt->key) {
        /* OK button: cycle screens */
        case InputKeyOk:
            FURI_LOG_D(TAG, "OK button pressed");
            about_model->screen = (about_model->screen + 1) % 3;
            evt_handled = true;
            break;

        /* Right button: go to the next screen */
        case InputKeyRight:
            FURI_LOG_D(TAG, "Right button pressed");
            about_model->screen = about_model->screen < 2 ? about_model->screen + 1 :
                                                            about_model->screen;
            evt_handled = true;
            break;

        /* Left button: go to the previous screen */
        case InputKeyLeft:
            FURI_LOG_D(TAG, "Left button pressed");
            about_model->screen = about_model->screen > 0 ? about_model->screen - 1 :
                                                            about_model->screen;
            evt_handled = true;
            break;

        default:
            evt_handled = false;
        }

    /* If we haven't handled this event, return now */
    if(!evt_handled) return false;

    /* Trigger an about view redraw */
    with_view_model(
        app->about_view, AboutModel * _model, { UNUSED(_model); }, true);

    /* We handled the event */
    return true;
}
