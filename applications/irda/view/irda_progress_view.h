/**
  * @file irda_progress_view.h
  * Infrared: Custom Infrared view module.
  * It shows popup progress bar during brute force.
  */
#pragma once
#include <gui/view.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Anonumous instance */
typedef struct IrdaProgressView IrdaProgressView;

/** Callback for back button handling */
typedef void (*IrdaProgressViewBackCallback)(void*);

/** Allocate and initialize Infrared view
 *
 * @retval new allocated instance
 */
IrdaProgressView* irda_progress_view_alloc();

/** Free previously allocated Progress view module instance
 *
 * @param instance to free
 */
void irda_progress_view_free(IrdaProgressView* instance);

/** Get progress view module view
 *
 * @param instance view module
 * @retval view
 */
View* irda_progress_view_get_view(IrdaProgressView* instance);

/** Increase progress on progress view module
 *
 * @param instance view module
 * @retval true - value is incremented and maximum is reached,
 *      false - value is incremented and maximum is not reached
 */
bool irda_progress_view_increase_progress(IrdaProgressView* instance);

/** Set maximum progress value
 *
 * @param instance - view module
 * @param progress_max - maximum value of progress
 */
void irda_progress_view_set_progress_total(IrdaProgressView* instance, uint16_t progress_max);

/** Set back button callback
 *
 * @param instance - view module
 * @param callback - callback to call for back button
 * @param context - context to pass to callback
 */
void irda_progress_view_set_back_callback(
    IrdaProgressView* instance,
    IrdaProgressViewBackCallback callback,
    void* context);

#ifdef __cplusplus
}
#endif
