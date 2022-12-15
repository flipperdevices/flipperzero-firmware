#include "countdown_view.h"
#include "../utils/utils.h"

// internal
static void handle_cmd(CountDownTimView* hw, CountDownViewCmd cmd);
static void handle_time_setting_updown(CountDownTimView* hwv, CountDownViewCmd cmd);
static void handle_time_setting_select(InputKey key, CountDownTimView* hwv);
static void draw_selection(Canvas* canvas, CountDownViewSelect selection);

// callbacks
static void countdown_timer_view_on_enter(void* ctx);
static void countdown_timer_view_on_draw(Canvas* canvas, void* ctx);
static bool countdown_timer_view_on_input(InputEvent* event, void* ctx);
static void timer_cb(void* ctx);

CountDownTimView* helloworld_view_new() {
    CountDownTimView* hwv = (CountDownTimView*)(malloc(sizeof(CountDownTimView)));

    hwv->view = view_alloc();

    hwv->timer = furi_timer_alloc(timer_cb, FuriTimerTypePeriodic, hwv);

    hwv->counting = false;

    view_set_context(hwv->view, hwv);

    view_allocate_model(hwv->view, ViewModelTypeLocking, sizeof(CountDownModel));

    view_set_draw_callback(hwv->view, countdown_timer_view_on_draw);
    view_set_input_callback(hwv->view, countdown_timer_view_on_input);
    view_set_enter_callback(hwv->view, countdown_timer_view_on_enter);

    return hwv;
}

void countdown_timer_view_delete(CountDownTimView* hwv) {
    furi_assert(hwv);

    view_free(hwv->view);
    furi_timer_stop(hwv->timer);
    furi_timer_free(hwv->timer);

    free(hwv);
}

View* countdown_timer_view_get_view(CountDownTimView* hwv) {
    return hwv->view;
}

void countdown_timer_view_state_reset(CountDownTimView* hwv) {
    hwv->counting = false;
    with_view_model(
        hwv->view, CountDownModel * model, { model->sec_expected = 10; }, true);
}

void countdown_timer_state_toggle(CountDownTimView* hwv) {
    bool on = hwv->counting;
    if(!on) {
        furi_timer_start(hwv->timer, furi_kernel_get_tick_frequency() * 1); // 1s
    } else {
        furi_timer_stop(hwv->timer);
        notification_off();
    }

    hwv->counting = !on;
}

// on enter callback, CountDownTimView as ctx
static void countdown_timer_view_on_enter(void* ctx) {
    furi_assert(ctx);

    CountDownTimView* hwv = (CountDownTimView*)ctx;
    countdown_timer_view_state_reset(hwv);
}

// view draw callback, CountDownModel as ctx
static void countdown_timer_view_on_draw(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    CountDownModel* model = (CountDownModel*)ctx;

    char buffer[64];

    int32_t sec = model->sec_expected;
    CountDownViewSelect select = model->select;

    elements_frame(canvas, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    canvas_set_font(canvas, FontBigNumbers);
    draw_selection(canvas, select);

    snprintf(
        buffer,
        sizeof(buffer),
        "%02ld:%02ld:%02ld",
        (sec % (60 * 60 * 24)) / (60 * 60), // hour
        (sec % (60 * 60)) / 60, // minute
        sec % 60); // second
    canvas_draw_str_aligned(
        canvas, SCREEN_CENTER_X, SCREEN_CENTER_Y, AlignCenter, AlignCenter, buffer);
}

// keys input event callback, CountDownTimView as ctx
static bool countdown_timer_view_on_input(InputEvent* event, void* ctx) {
    furi_assert(ctx);

    CountDownTimView* hw = (CountDownTimView*)ctx;

    if(event->type == InputTypeShort || event->type == InputTypeRepeat) {
        switch(event->key) {
        case InputKeyUp:
        case InputKeyDown:
        case InputKeyRight:
        case InputKeyLeft:
            handle_time_setting_select(event->key, hw);
            break;

        case InputKeyOk:
            if(event->type == InputTypeShort) {
                handle_cmd(hw, CountDownTimerToggleCounting);
            }
            break;

        default:
            break;
        }

        return true;
    }

    if(event->type == InputTypeLong) {
        switch(event->key) {
        case InputKeyOk:
            handle_cmd(hw, CountDownTimerReset);
            break;

        case InputKeyBack:
            return false;
            break;

        default:
            break;
        }

        return true;
    }

    return false;
}

static void timer_cb(void* ctx) {
    furi_assert(ctx);

    CountDownTimView* hwv = (CountDownTimView*)ctx;

    int32_t sec;
    bool timeup = false;

    // decrement counter
    with_view_model(
        hwv->view,
        CountDownModel * model,
        {
            sec = model->sec_expected;
            sec--;

            // check timeup
            if(sec <= 0) {
                sec = 0;
                timeup = true;
            }

            model->sec_expected = sec;
        },
        true);

    if(timeup) {
        handle_cmd(hwv, CountDownTimerTimeUp);
    }
}

static void handle_time_setting_updown(CountDownTimView* hwv, CountDownViewCmd cmd) {
    int32_t sec_expected;

    with_view_model(
        hwv->view,
        CountDownModel * model,
        {
            sec_expected = model->sec_expected;
            switch(cmd) {
            case CountDownTimerMinuteUp:
                sec_expected += 60;
                break;
            case CountDownTimerMinuteDown:
                sec_expected -= 60;
                break;
            case CountDownTimerHourDown:
                sec_expected -= 3600;
                break;
            case CountDownTimerHourUp:
                sec_expected += 3600;
                break;
            case CountDownTimerSecUp:
                sec_expected++;
                break;
            case CountDownTimerSecDown:
                sec_expected--;
                break;
            default:
                break;
            }

            if(sec_expected < 0) {
                sec_expected = 0;
            }

            model->sec_expected = sec_expected;
        },
        true);
}

static void handle_cmd(CountDownTimView* hw, CountDownViewCmd cmd) {
    switch(cmd) {
    case CountDownTimerTimeUp:
        notification_timeup();
        break;

    case CountDownTimerReset:
        furi_timer_stop(hw->timer);
        countdown_timer_view_state_reset(hw);
        notification_off();

        break;

    case CountDownTimerToggleCounting:
        countdown_timer_state_toggle(hw);
        break;

    default:
        break;
    }

    return;
}

static void handle_time_setting_select(InputKey key, CountDownTimView* hwv) {
    bool counting = hwv->counting;
    CountDownViewCmd setting_cmd = CountDownTimerSecUp;
    CountDownViewSelect selection;

    if(counting) {
        return;
    }

    // load current selection from model context
    with_view_model(
        hwv->view, CountDownModel * model, { selection = model->select; }, false);

    // select
    switch(key) {
    case InputKeyUp:
        switch(selection) {
        case CountDownTimerSelectSec:
            setting_cmd = CountDownTimerSecUp;
            break;
        case CountDownTimerSelectMinute:
            setting_cmd = CountDownTimerMinuteUp;
            break;
        case CountDownTimerSelectHour:
            setting_cmd = CountDownTimerHourUp;
            break;
        }

        handle_time_setting_updown(hwv, setting_cmd);
        break;

    case InputKeyDown:
        switch(selection) {
        case CountDownTimerSelectSec:
            setting_cmd = CountDownTimerSecDown;
            break;
        case CountDownTimerSelectMinute:
            setting_cmd = CountDownTimerMinuteDown;
            break;
        case CountDownTimerSelectHour:
            setting_cmd = CountDownTimerHourDown;
            break;
        }

        handle_time_setting_updown(hwv, setting_cmd);
        break;

    case InputKeyRight:
        selection--;
        selection = selection % 3;
        break;

    case InputKeyLeft:
        selection++;
        selection = selection % 3;
        break;

    default:
        break;
    }

    // save selection to model context
    with_view_model(
        hwv->view, CountDownModel * model, { model->select = selection; }, false);
}

static void draw_selection(Canvas* canvas, CountDownViewSelect selection) {
    switch(selection) {
    case CountDownTimerSelectSec:
        elements_slightly_rounded_box(canvas, SCREEN_CENTER_X + 25, SCREEN_CENTER_Y + 11, 21, 2);
        break;
    case CountDownTimerSelectMinute:
        elements_slightly_rounded_box(canvas, SCREEN_CENTER_X - 10, SCREEN_CENTER_Y + 11, 21, 2);
        break;
    case CountDownTimerSelectHour:
        elements_slightly_rounded_box(canvas, SCREEN_CENTER_X - 47, SCREEN_CENTER_Y + 11, 21, 2);
        break;
    }
}