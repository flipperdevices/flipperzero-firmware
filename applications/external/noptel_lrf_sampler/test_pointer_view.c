/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.9
 *
 * Test pointer view
***/

/*** Includes ***/
#include <furi_hal_infrared.h>
#include <storage/storage.h>

#include "common.h"
#include "noptel_lrf_sampler_icons.h" /* Generated from images in assets */

/*** Routines ***/

/** IR capture callback */
static void ir_capture_callback(void* ctx, bool lvl, uint32_t duration) {
    UNUSED(lvl);
    UNUSED(duration);
    TestPointerModel* testpointer_model = (TestPointerModel*)ctx;

    testpointer_model->ir_received = true;
}

/** IR capture timeout callback */
static void ir_timeout_callback(void* ctx) {
    TestPointerModel* testpointer_model = (TestPointerModel*)ctx;

    testpointer_model->ir_received = false;
}

/** Test pointer view update timer callback **/
static void test_pointer_view_timer_callback(void* ctx) {
    App* app = (App*)ctx;
    TestPointerModel* testpointer_model = view_get_model(app->testpointer_view);

    /* Did the IR receiver change state? */
    if(testpointer_model->ir_received != testpointer_model->ir_received_prev) {
        /* Trigger a test pointer view redraw */
        with_view_model(
            app->testpointer_view, TestPointerModel * _model, { UNUSED(_model); }, true);

        testpointer_model->ir_received_prev = testpointer_model->ir_received;
    }

    /* If an IR signal was received and beeping is enabled, start a beep */
    if(testpointer_model->ir_received && app->config.beep)
        start_beep(&app->speaker_control, test_pointer_view_update_every + 50);
}

/** Pointer control timer callback **/
static void pointer_control_timer_callback(void* ctx) {
    App* app = (App*)ctx;
    TestPointerModel* testpointer_model = view_get_model(app->testpointer_view);

    /* Jiggle the pointer */
    testpointer_model->pointer_is_on = !testpointer_model->pointer_is_on;
    send_lrf_command(
        app->lrf_serial_comm_app, testpointer_model->pointer_is_on ? pointer_on : pointer_off);

    FURI_LOG_T(TAG, "Pointer %s", testpointer_model->pointer_is_on ? "ON" : "OFF");
}

/** Test pointer view enter callback **/
void testpointer_view_enter_callback(void* ctx) {
    App* app = (App*)ctx;
    TestPointerModel* testpointer_model = view_get_model(app->testpointer_view);
    uint32_t period_view_update = furi_ms_to_ticks(test_pointer_view_update_every);
    uint32_t period_jiggle_pointer = furi_ms_to_ticks(test_pointer_jiggle_every);

    /* Make sure the IR receiver isn't busy */
    testpointer_model->ir_busy = furi_hal_infrared_is_busy();
    if(testpointer_model->ir_busy) {
        FURI_LOG_I(TAG, "IR busy");
        return;
    }

    testpointer_model->ir_received_prev = false;
    testpointer_model->ir_received = false;

    /* Reflect the current state of the pointer */
    testpointer_model->pointer_is_on = app->pointer_is_on;

    /* Start the UART at the correct baudrate */
    start_uart(app->lrf_serial_comm_app, app->config.baudrate);

    /* Set up the callback to catch an IR sensor level change */
    furi_hal_infrared_async_rx_set_capture_isr_callback(ir_capture_callback, testpointer_model);

    /* Set up the callback to catch an IR sensor timeout */
    furi_hal_infrared_async_rx_set_timeout_isr_callback(ir_timeout_callback, testpointer_model);

    /* Start the IR receiver and set the timeout */
    furi_hal_infrared_async_rx_start();
    furi_hal_infrared_async_rx_set_timeout(test_pointer_view_update_every * 1000);

    /* Setup and start the view update timer */
    app->test_pointer_view_timer =
        furi_timer_alloc(test_pointer_view_timer_callback, FuriTimerTypePeriodic, ctx);
    furi_timer_start(app->test_pointer_view_timer, period_view_update);

    /* Setup and start the pointer control timer */
    app->test_pointer_control_timer =
        furi_timer_alloc(pointer_control_timer_callback, FuriTimerTypePeriodic, ctx);
    furi_timer_start(app->test_pointer_control_timer, period_jiggle_pointer);

    /* Set the backlight on all the time */
    set_backlight(&app->backlight_control, BL_ON);

    with_view_model(
        app->testpointer_view, TestPointerModel * _model, { UNUSED(_model); }, false);
}

/** Test pointer view exit callback **/
void testpointer_view_exit_callback(void* ctx) {
    App* app = (App*)ctx;
    TestPointerModel* testpointer_model = view_get_model(app->testpointer_view);

    /* If the IR sensor is busy, we have nothing to do */
    if(testpointer_model->ir_busy) return;

    /* Set the backlight back to automatic */
    set_backlight(&app->backlight_control, BL_AUTO);

    /* Stop and free the pointer control timer */
    furi_timer_stop(app->test_pointer_control_timer);
    furi_timer_free(app->test_pointer_control_timer);

    /* Restore the pointer as we found it */
    if(testpointer_model->pointer_is_on != app->pointer_is_on)
        send_lrf_command(app->lrf_serial_comm_app, app->pointer_is_on ? pointer_on : pointer_off);

    /* Stop the UART */
    stop_uart(app->lrf_serial_comm_app);

    /* Unset the IR sensor timeout callback */
    furi_hal_infrared_async_rx_set_timeout_isr_callback(NULL, NULL);

    /* Unset the IR sensor level change callback */
    furi_hal_infrared_async_rx_set_capture_isr_callback(NULL, NULL);

    /* Stop the IR receiver */
    furi_hal_infrared_async_rx_stop();

    /* Stop and free the view update timer */
    furi_timer_stop(app->test_pointer_view_timer);
    furi_timer_free(app->test_pointer_view_timer);
}

/** Draw callback for the test pointer view **/
void testpointer_view_draw_callback(Canvas* canvas, void* model) {
    TestPointerModel* testpointer_model = (TestPointerModel*)model;

    canvas_set_font(canvas, FontPrimary);

    /* Draw the icon to prompt the user to line up the LRF's IR pointer and the
     Flipper's IR port */
    canvas_draw_icon(canvas, 9, 0, &I_align_pointer_and_flipper);

    /* If any IR signal was received, display the laser radiation icon */
    if(testpointer_model->ir_received) {
        canvas_draw_icon(canvas, 0, 22, &I_laser_radiation);
        FURI_LOG_T(TAG, "IR signal received");
    } else
        FURI_LOG_T(TAG, "No IR signal received");

    /* Draw a dividing line between the icons and the bottom line */
    canvas_draw_line(canvas, 0, 48, 128, 48);

    /* If the IR sensor is busy, tell the user and stop */
    if(testpointer_model->ir_busy) {
        canvas_draw_str(canvas, 32, 61, "IR port busy!");
        return;
    }

    /* Prompt the user to line up the LRF's IR pointer and the Flipper's IR port
     at the bottom */
    canvas_draw_str(canvas, 4, 61, "Aim IR pointer at IR port");
}
