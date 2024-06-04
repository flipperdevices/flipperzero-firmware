/***
 * HC-11 wireless modem emulator for the Flipper Zero
 * Version: 0.1
 *
 * About view
***/

/*** Includes ***/
#include "common.h"
#include "hc11_modem_icons.h" /* Generated from images in assets */

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

        /* Draw "modem" and the app's version number */
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 90, 15, "modem");
        canvas_draw_str(canvas, 90, 28, "v");
        canvas_draw_str(canvas, 96, 28, VERSION);

        /* Draw a right arrow at the top right */
        canvas_draw_icon(canvas, 124, 0, &I_arrow_right);

        break;

    /* Draw the app's purpose description screen */
    case 1:

        /* Draw the title */
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 29, 8, "HC-11 modem");

        /* Draw the URL */
        canvas_draw_str(canvas, 7, 62, "https://www.hc01.com/");

        /* Draw the description */
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 7, 29, "Send data to/from a HC-11");
        canvas_draw_str(canvas, 11, 40, "wireless RF UART module ");

        /* Draw a left arrow at the top left */
        canvas_draw_icon(canvas, 0, 0, &I_arrow_left);

        /* Draw a right arrow at the top right */
        canvas_draw_icon(canvas, 124, 0, &I_arrow_right);

        break;

    /* Draw the warning about the app's limitation */
    case 2:

        /* Draw the title */
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 38, 8, "WARNING");

        /* Draw the description */
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 6, 29, "Currently only transmissions");
        canvas_draw_str(canvas, 0, 40, "mode 3 (FU3) works reliably &");
        canvas_draw_str(canvas, 3, 51, "mode 1 (FU1) in reception only");

        /* Draw a left arrow at the top left */
        canvas_draw_icon(canvas, 0, 0, &I_arrow_left);

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
