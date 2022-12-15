#ifndef __COUNTDOWN_VIEW_H__
#define __COUNTDOWN_VIEW_H__

#include <furi.h>
#include <furi_hal.h>
#include <gui/view.h>
#include <gui/elements.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_CENTER_X (SCREEN_WIDTH / 2)
#define SCREEN_CENTER_Y (SCREEN_HEIGHT / 2)

#define MAX_SCALE 80
#define MAX_MOVE_STEP 16
#define MIN_MOVE_STEP 2

typedef enum {
    CountDownTimerMinuteUp,
    CountDownTimerMinuteDown,
    CountDownTimerSecDown,
    CountDownTimerSecUp,
    CountDownTimerHourUp,
    CountDownTimerHourDown,
    CountDownTimerReset,
    CountDownTimerTimeUp,
    CountDownTimerToggleCounting,
} CountDownViewCmd;

typedef enum {
    CountDownTimerSelectSec,
    CountDownTimerSelectMinute,
    CountDownTimerSelectHour,
} CountDownViewSelect;

typedef struct {
    int32_t sec_expected;
    uint8_t select; // setting
} CountDownModel;

typedef struct {
    View* view;
    FuriTimer* timer; // 1Hz tick timer
    bool counting;

} CountDownTimView;

// functions
// allocate helloworld view
CountDownTimView* helloworld_view_new();

// delete helloworld view
void countdown_timer_view_delete(CountDownTimView* hwv);

// return view
View* countdown_timer_view_get_view(CountDownTimView* hwv);

void countdown_timer_view_state_reset(CountDownTimView* hwv); // set initial state
void countdown_timer_state_toggle(CountDownTimView* hwv);
#endif // __COUNTDOWN_VIEW_H__