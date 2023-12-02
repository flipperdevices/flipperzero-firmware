#pragma once
#include <furi_hal.h>

/** @brief FlippPomodoroStatistics structure
 *
 *  This structure is used to keep track of completed focus stages.
 */
typedef struct {
    uint8_t focus_stages_completed;
} FlippPomodoroStatistics;

/** @brief Allocate and initialize a new FlippPomodoroStatistics
 *
 *  This function allocates a new FlippPomodoroStatistics structure, initializes its members
 *  and returns a pointer to it.
 *
 *  @return A pointer to a new FlippPomodoroStatistics structure
 */
FlippPomodoroStatistics* flipp_pomodoro_statistics__new();

/** @brief Get the number of completed focus stages
 *
 *  This function retrieves the number of completed focus stages in a FlippPomodoroStatistics structure.
 *
 *  @param statistics A pointer to a FlippPomodoroStatistics structure
 *  @return The number of completed focus stages
 */
uint8_t flipp_pomodoro_statistics__get_focus_stages_completed(FlippPomodoroStatistics* statistics);

/** @brief Increase the number of completed focus stages
 *
 *  This function increases the count of the completed focus stages by one in a FlippPomodoroStatistics structure.
 *
 *  @param statistics A pointer to a FlippPomodoroStatistics structure
 */
void flipp_pomodoro_statistics__increase_focus_stages_completed(
    FlippPomodoroStatistics* statistics);

/** @brief Free a FlippPomodoroStatistics structure
 *
 *  This function frees the memory used by a FlippPomodoroStatistics structure.
 *
 *  @param statistics A pointer to a FlippPomodoroStatistics structure
 */
void flipp_pomodoro_statistics__destroy(FlippPomodoroStatistics* state);