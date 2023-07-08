#pragma once

#include <gui/view.h>

/** @brief Mode types for FlippPomodoroInfoView
 *
 *  These are the modes that can be used in the FlippPomodoroInfoView
 */
typedef enum {
    FlippPomodoroInfoViewModeStats,
    FlippPomodoroInfoViewModeAbout,
} FlippPomodoroInfoViewMode;

/** @brief Forward declaration of the FlippPomodoroInfoView struct */
typedef struct FlippPomodoroInfoView FlippPomodoroInfoView;

/** @brief User action callback function type
 *
 *  Callback functions of this type are called when a user action is performed.
 */
typedef void (*FlippPomodoroInfoViewUserActionCb)(void* ctx);

/** @brief Allocate a new FlippPomodoroInfoView
 *
 *  Allocates a new FlippPomodoroInfoView and returns a pointer to it.
 *  @return A pointer to a new FlippPomodoroInfoView
 */
FlippPomodoroInfoView* flipp_pomodoro_info_view_alloc();

/** @brief Get the view from a FlippPomodoroInfoView
 *
 *  Returns a pointer to the view associated with a FlippPomodoroInfoView.
 *  @param info_view A pointer to a FlippPomodoroInfoView
 *  @return A pointer to the view of the FlippPomodoroInfoView
 */
View* flipp_pomodoro_info_view_get_view(FlippPomodoroInfoView* info_view);

/** @brief Free a FlippPomodoroInfoView
 *
 *  Frees the memory used by a FlippPomodoroInfoView.
 *  @param info_view A pointer to a FlippPomodoroInfoView
 */
void flipp_pomodoro_info_view_free(FlippPomodoroInfoView* info_view);

/** @brief Set the number of completed pomodoros in the view
 *
 *  Sets the number of completed pomodoros that should be displayed in the view.
 *  @param info_view A pointer to the view
 *  @param pomodoros_completed The number of completed pomodoros
 */
void flipp_pomodoro_info_view_set_pomodoros_completed(View* info_view, uint8_t pomodoros_completed);

/** @brief Set the callback function to be called when the timer should be resumed
 *
 *  Sets the callback function that will be called when the timer should be resumed.
 *  @param info_view A pointer to the FlippPomodoroInfoView
 *  @param user_action_cb The callback function
 *  @param user_action_cb_ctx The context to be passed to the callback function
 */
void flipp_pomodoro_info_view_set_resume_timer_cb(
    FlippPomodoroInfoView* info_view,
    FlippPomodoroInfoViewUserActionCb user_action_cb,
    void* user_action_cb_ctx);

/** @brief Set the mode of the view
 *
 *  Sets the mode that should be used in the view.
 *  @param view A pointer to the view
 *  @param desired_mode The desired mode
 */
void flipp_pomodoro_info_view_set_mode(View* view, FlippPomodoroInfoViewMode desired_mode);
