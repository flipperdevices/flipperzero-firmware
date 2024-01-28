#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*ClockTimerCallback)(void* context);

void clock_timer_init(ClockTimerCallback callback, void* context, float period);

void clock_timer_start(void);

void clock_timer_stop(void);

void clock_timer_deinit(void);

#ifdef __cplusplus
}
#endif