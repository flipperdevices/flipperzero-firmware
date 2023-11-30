
#include "meal_pager_retekess_t119.h"

void customConcat(char* dest, const char* src) {
    //FURI_LOG_D(TAG, "adding %s to %s", src, dest);
    // Find the end of the destination string
    while (*dest != '\0') {
        dest++;
    }

    // Copy characters from src to dest
    while (*src != '\0') {
        *dest = *src;
        dest++;
        src++;
    }

    // Null-terminate the concatenated string
    *dest = '\0';
}

char* genRawData(int zero, int one, const char* bits) {
    int bitsLen = strlen(bits);
    int lineLen = 256; // Adjust the line length as needed
    char* line = (char*)malloc(lineLen * sizeof(char));

    //FURI_LOG_D(TAG, "bitLen = %u", bitsLen);
    // Initialize the line with the first part
    //snprintf(line, lineLen, "-6000");

    //char* res = (char*)malloc((bitsLen * 4) * sizeof(char));
    char* res = (char*)malloc(lineLen * sizeof(char));
    res[0] = '\0'; // Null-terminate the result string

    //customConcat(res, line);
    customConcat(res, "-6000");

    // Append bits and create the line
    for (int i = 0; i < bitsLen; i++) {
        char c = bits[i];
        //char p = (i > 0) ? bits[i - 1] : '0';
        int t = (c == '0') ? zero : one;

        if (i % 2 == 0) {
            snprintf(line, lineLen, " %d", t);
        } else {
            snprintf(line, lineLen, " -%d", t);
        }

        // Concatenate the line to the result string
        //strncat(res, line, bitsLen * 4);
        customConcat(res, line);
    }

    // Append the closing part to the line
    //strncat(line, " -6000\n", lineLen);
    customConcat(res, " 200 -6000");
    //FURI_LOG_D(TAG, "res is: %s", res);

    free(line); // Free memory allocated for the line

    return res;
}

char* encManchester(const char* bits, int mode) {
    // Allocate memory for the result string
    char* res = (char*)malloc((strlen(bits) * 2 + 1) * sizeof(char));

    int index = 0;
    for (int i = 0; bits[i] != '\0'; i++) {
        char c = bits[i];
        if (c == '0') {
            if (mode) {
                res[index++] = '1';
                res[index++] = '0';
            } else {
                res[index++] = '0';
                res[index++] = '1';
            }
        } else if (c == '1') {
            if (mode) {
                res[index++] = '0';
                res[index++] = '1';
            } else {
                res[index++] = '1';
                res[index++] = '0';
            }
        } else {
            // Handle 'EE' case (error)
            res[index++] = 'E';
            res[index++] = 'E';
        }
    }

    // Null-terminate the result string
    res[index] = '\0';

    return res;
}

void uint32ToBinaray(uint32_t number, char* str, int8_t length) {
    int i = 0;
    length--; // count length without 0
    for (i = length; i >= 0; i--) {
        // Bitwise AND extration of the i-th bit
        int bit = (number >> i) & 1;
        // convert the bit to a character of 1 or 0
        str[length - i] = bit + '0';
    }
    // Terminate the string
    str[length+1] = '\0';
}

void reverse(char* str) {
    int length = strlen(str);
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}
 
static void meal_pager_retekess_t119_generate_pager(void* context, char* stationId, uint32_t pager, FlipperFormat* ff) {
    Meal_Pager* app = context;
    char pagerId[11];
    //char stationPagerId[25];
    //char fullId[25];
    char* fullId = (char*)malloc(25 * sizeof(char));
    uint32_t action = 0; // 0 = ring, 1 = mute
    char actionId[2];
    //char action[2];
    //action[0] = '0'; // 0 = ring, 1 = mute
    //action[1] = '\0';
    FURI_LOG_D(TAG, "Generating T119 Data for Pager %lu", pager);
    app->current_pager = pager;
    meal_pager_transmit_model_set_pager(app->meal_pager_transmit, app->current_pager);
    uint32ToBinaray(pager, pagerId, 10);
    uint32ToBinaray(action, actionId, 1);
    reverse(pagerId);
    reverse(actionId);
    //FURI_LOG_D(TAG, "Station Bin: %s", stationId);
    //FURI_LOG_D(TAG, "Pager Bin: %s", pagerId);
    //FURI_LOG_D(TAG, "Action Bin: %s", actionId);
    customConcat(fullId, stationId);
    customConcat(fullId, pagerId);
    FURI_LOG_D(TAG, "Result %s", fullId);
    //FURI_LOG_D(TAG, "Station & Pager: %s", stationPagerId);
    //FURI_LOG_D(TAG, "Station & Pager: %s", stationPagerId);
    customConcat(fullId, actionId);
    FURI_LOG_D(TAG, "CustomConcat: %s", fullId);
    //FURI_LOG_D(TAG, "Station & Pager & Action: %s", fullId);
    char* manchester = encManchester(fullId, 0);
    FURI_LOG_D(TAG, "Manchester: %s", manchester);
    char* rawSignal = genRawData(200, 600, manchester);
    FURI_LOG_D(TAG, "RAW_Data: %s", rawSignal);
    flipper_format_write_string_cstr(ff, "RAW_Data", rawSignal);
    free(manchester);
    free(rawSignal);
}

static void meal_pager_retekess_t119_generate_station(void* context, uint32_t station, FlipperFormat* ff) {
    Meal_Pager* app = context;
    FURI_LOG_D(TAG, "Generating T119 Data for Station %lu", station);
    app->current_station = station;
    app->current_pager = app->first_pager;
    char stationId[14];
    uint32ToBinaray(station, stationId, 13);
    reverse(stationId);
    meal_pager_transmit_model_set_station(app->meal_pager_transmit, app->current_station);
    for (u_int32_t i = app->current_pager;i <= app->last_pager; i++) {
        meal_pager_retekess_t119_generate_pager(app, stationId, i, ff);
        //furi_thread_flags_wait(0, FuriFlagWaitAny, 1);
        if (app->stop_transmit) {
            break;
        }
    }
}

void meal_pager_retekess_t119_generate_all(void* context) {
    Meal_Pager* app = context;

    app->current_pager = 1;
    app->current_station = app->first_station;

    FlipperFormat* ff = meal_pager_save_subghz_buffer_file_start(app);

    for (u_int32_t i = app->current_station;i <= app->last_station; i++) {
        meal_pager_retekess_t119_generate_station(app, i, ff);
        //furi_thread_flags_wait(0, FuriFlagWaitAny, 100);
        if (app->stop_transmit) {
            break;
        }
    }

    meal_pager_save_subghz_buffer_stop(app, ff);
}

