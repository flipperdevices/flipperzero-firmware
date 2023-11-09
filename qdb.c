#include "qdb.h"
#include <furi_hal.h>

q_char** q_shuffle(const uint8_t group_number) {
    FuriHalRtcDateTime curr_dt;
    furi_hal_rtc_get_datetime(&curr_dt);
    const uint32_t current_timestamp = furi_hal_rtc_datetime_to_timestamp(&curr_dt);
    srand(current_timestamp);
    const uint32_t group_size = Q_GROUPS[group_number].size;
    q_char** res_arr = malloc(group_size * sizeof(q_char*));
    // copy first
    if(group_number < NUMBER_OF_GROUPS - 1) { // not "ALL"
        q_char** group_arr = Q_GROUPS[group_number].questions;
        for(uint16_t i = 0; i < group_size; i++) {
            res_arr[i] = group_arr[i];
        }
    } else {
        uint32_t i = 0;
        for(uint8_t g = 0; g < NUMBER_OF_GROUPS - 1; g++) {
            const QuestionList current = Q_GROUPS[g];
            for(uint16_t j = 0; j < current.size; j++) {
                res_arr[i++] = current.questions[j];
            }
        }
    }
    // then shuffle
    for(uint16_t i = 0; i < group_size; i++) {
        uint16_t j = i + rand() / (RAND_MAX / (group_size - i) + 1);
        q_char* temp = res_arr[i];
        res_arr[i] = res_arr[j];
        res_arr[j] = temp;
    }
    return res_arr;
}