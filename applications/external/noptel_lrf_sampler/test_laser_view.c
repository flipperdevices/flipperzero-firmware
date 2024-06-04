/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.9
 *
 * Test laser view
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
    TestLaserModel* testlaser_model = (TestLaserModel*)ctx;

    testlaser_model->ir_received = true;
}

/** IR capture timeout callback */
static void ir_timeout_callback(void* ctx) {
    TestLaserModel* testlaser_model = (TestLaserModel*)ctx;

    testlaser_model->ir_received = false;
}

/** Test laser view update timer callback **/
static void test_laser_view_timer_callback(void* ctx) {
    App* app = (App*)ctx;
    TestLaserModel* testlaser_model = view_get_model(app->testlaser_view);

    /* Did the IR receiver change state? */
    if(testlaser_model->ir_received != testlaser_model->ir_received_prev) {
        /* Trigger a test laser view redraw */
        with_view_model(
            app->testlaser_view, TestLaserModel * _model, { UNUSED(_model); }, true);

        testlaser_model->ir_received_prev = testlaser_model->ir_received;
    }

    /* If an IR signal was received and beeping is enabled, start a beep */
    if(testlaser_model->ir_received && app->config.beep)
        start_beep(&app->speaker_control, test_laser_view_update_every + 50);
}

/** CMM restart timer callback **/
static void restart_cmm_timer_callback(void* ctx) {
    App* app = (App*)ctx;
    TestLaserModel* testlaser_model = view_get_model(app->testlaser_view);

    /* Restart CMM if needed */
    if(testlaser_model->restart_cmm) {
        send_lrf_command(app->lrf_serial_comm_app, cmm_10hz);
        FURI_LOG_T(TAG, "CMM restart");
    }

    /* Set ourselves up to restart CMM the next time we get called: if any sample
     is received before that, the flag will be cleared and we won't */
    testlaser_model->restart_cmm = true;
}

/** LRF sample handler
    Called when a LRF sample is available from the LRF serial
    communication app **/
static void lrf_sample_handler(LRFSample* lrf_sample, void* ctx) {
    App* app = (App*)ctx;
    TestLaserModel* testlaser_model = view_get_model(app->testlaser_view);

    /* We don't care about the sample, just that it was received */
    UNUSED(lrf_sample);
    testlaser_model->restart_cmm = false;
}

/** Test laser view enter callback **/
void testlaser_view_enter_callback(void* ctx) {
    App* app = (App*)ctx;
    TestLaserModel* testlaser_model = view_get_model(app->testlaser_view);
    uint32_t period_view_update = furi_ms_to_ticks(test_laser_view_update_every);
    uint32_t period_restart_cmm = furi_ms_to_ticks(test_laser_restart_cmm_every);

    /* Make sure the IR receiver isn't busy */
    testlaser_model->ir_busy = furi_hal_infrared_is_busy();
    if(testlaser_model->ir_busy) {
        FURI_LOG_I(TAG, "IR busy");
        return;
    }

    testlaser_model->ir_received_prev = false;
    testlaser_model->ir_received = false;

    /* Start the UART at the correct baudrate */
    start_uart(app->lrf_serial_comm_app, app->config.baudrate);

    /* Set up the callback to catch an IR sensor level change */
    furi_hal_infrared_async_rx_set_capture_isr_callback(ir_capture_callback, testlaser_model);

    /* Set up the callback to catch an IR sensor timeout */
    furi_hal_infrared_async_rx_set_timeout_isr_callback(ir_timeout_callback, testlaser_model);

    /* Start the IR receiver and set the timeout */
    furi_hal_infrared_async_rx_start();
    furi_hal_infrared_async_rx_set_timeout(test_laser_view_update_every * 1000);

    /* Setup the callback to receive decoded LRF samples */
    set_lrf_sample_handler(app->lrf_serial_comm_app, lrf_sample_handler, app);

    /* Start CMM rightaway */
    send_lrf_command(app->lrf_serial_comm_app, cmm_10hz);
    FURI_LOG_T(TAG, "CMM start");

    /* Setup and start the view update timer */
    app->test_laser_view_timer =
        furi_timer_alloc(test_laser_view_timer_callback, FuriTimerTypePeriodic, ctx);
    furi_timer_start(app->test_laser_view_timer, period_view_update);

    /* Setup and start the CMM restart timer */
    app->test_laser_restart_cmm_timer =
        furi_timer_alloc(restart_cmm_timer_callback, FuriTimerTypePeriodic, ctx);
    furi_timer_start(app->test_laser_restart_cmm_timer, period_restart_cmm);

    /* Set the backlight on all the time */
    set_backlight(&app->backlight_control, BL_ON);

    with_view_model(
        app->testlaser_view, TestLaserModel * _model, { UNUSED(_model); }, false);
}

/** Test laser view exit callback **/
void testlaser_view_exit_callback(void* ctx) {
    App* app = (App*)ctx;
    TestLaserModel* testlaser_model = view_get_model(app->testlaser_view);

    /* If the IR sensor is busy, we have nothing to do */
    if(testlaser_model->ir_busy) return;

    /* Set the backlight back to automatic */
    set_backlight(&app->backlight_control, BL_AUTO);

    /* Stop and free the CMM restart timer */
    furi_timer_stop(app->test_laser_restart_cmm_timer);
    furi_timer_free(app->test_laser_restart_cmm_timer);

    /* Send a CMM-break command unconditionally - 3 times to be sure */
    send_lrf_command(app->lrf_serial_comm_app, cmm_break);
    send_lrf_command(app->lrf_serial_comm_app, cmm_break);
    send_lrf_command(app->lrf_serial_comm_app, cmm_break);
    app->pointer_is_on = false; /* A CMM break turns the pointer off */

    /* Unset the callback to receive decoded LRF samples */
    set_lrf_sample_handler(app->lrf_serial_comm_app, NULL, app);

    /* Stop the UART */
    stop_uart(app->lrf_serial_comm_app);

    /* Unset the IR sensor timeout callback */
    furi_hal_infrared_async_rx_set_timeout_isr_callback(NULL, NULL);

    /* Unset the IR sensor level change callback */
    furi_hal_infrared_async_rx_set_capture_isr_callback(NULL, NULL);

    /* Stop the IR receiver */
    furi_hal_infrared_async_rx_stop();

    /* Stop and free the view update timer */
    furi_timer_stop(app->test_laser_view_timer);
    furi_timer_free(app->test_laser_view_timer);
}

/** Draw callback for the test laser view **/
void testlaser_view_draw_callback(Canvas* canvas, void* model) {
    TestLaserModel* testlaser_model = (TestLaserModel*)model;

    canvas_set_font(canvas, FontPrimary);

    /* Draw the icon to prompt the user to line up the LRF's laser transmitter
     and the Flipper's IR port */
    canvas_draw_icon(canvas, 9, 0, &I_align_laser_and_flipper);

    /* If any IR signal was received, display the laser radiation icon */
    if(testlaser_model->ir_received) {
        canvas_draw_icon(canvas, 0, 22, &I_laser_radiation);
        FURI_LOG_T(TAG, "IR signal received");
    } else
        FURI_LOG_T(TAG, "No IR signal received");

    /* Draw a dividing line between the icons and the bottom line */
    canvas_draw_line(canvas, 0, 48, 128, 48);

    /* If the IR sensor is busy, tell the user and stop */
    if(testlaser_model->ir_busy) {
        canvas_draw_str(canvas, 32, 61, "IR port busy!");
        return;
    }

    /* Prompt the user to line up the LRF's laser transmitter and the Flipper's
     IR port at the bottom */
    canvas_draw_str(canvas, 4, 61, "Aim LRX laser at IR port");
}
