#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*ClockTimerCallback)(void* context);

void clock_timer_start(ClockTimerCallback callback, void* context, float period);

void clock_timer_stop(void);

#ifdef __cplusplus
}
#endif