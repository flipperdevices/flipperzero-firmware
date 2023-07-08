#include "flipp_pomodoro_statistics.h"

FlippPomodoroStatistics* flipp_pomodoro_statistics__new() {
    FlippPomodoroStatistics* statistics = malloc(sizeof(FlippPomodoroStatistics));

    statistics->focus_stages_completed = 0;

    return statistics;
}

// Return the number of completed focus stages
uint8_t
    flipp_pomodoro_statistics__get_focus_stages_completed(FlippPomodoroStatistics* statistics) {
    return statistics->focus_stages_completed;
}

// Increase the number of completed focus stages by one
void flipp_pomodoro_statistics__increase_focus_stages_completed(
    FlippPomodoroStatistics* statistics) {
    statistics->focus_stages_completed++;
}

void flipp_pomodoro_statistics__destroy(FlippPomodoroStatistics* statistics) {
    furi_assert(statistics);
    free(statistics);
};
