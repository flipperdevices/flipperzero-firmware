
#include "meal_pager_retekess_td157.h"

static char* genRawDataTD157(int zero, int one, const char* bits) {
    int bitsLen = strlen(bits);
    int lineLen = 256; // Adjust the line length as needed
    char* line = (char*)malloc(lineLen * sizeof(char));

    // Initialize the line with the first part
    char* res = (char*)malloc(lineLen * sizeof(char));
    res[0] = '\0'; // Null-terminate the result string

    customConcat(res, "-6000");

    // Append bits and create the line
    for(int i = 0; i < bitsLen; i++) {
        char c = bits[i];
        int t = (c == '0') ? zero : one;

        if(i % 2 == 0) {
            snprintf(line, lineLen, " %d", t);
        } else {
            snprintf(line, lineLen, " -%d", t);
        }

        // Concatenate the line to the result string
        customConcat(res, line);
    }

    // Append the closing part to the line
    customConcat(res, " 200 -6000");

    free(line); // Free memory allocated for the line

    return res;
}

static void meal_pager_retekess_td157_generate_pager(
    void* context,
    char* stationId,
    uint32_t pager,
    FlipperFormat* ff) {
    Meal_Pager* app = context;
    char pagerId[11];
    char* fullId = (char*)malloc(25 * sizeof(char));
    uint32_t action = 2;
    char actionId[4];
    app->current_pager = pager;
    meal_pager_transmit_model_set_pager(app->meal_pager_transmit, app->current_pager);
    FURI_LOG_D(TAG, "Generating TD157 Data for Pager %lu", pager);
    uint32ToBinaray(pager, pagerId, 10);
    customConcat(fullId, stationId);
    customConcat(fullId, pagerId);
    uint32ToBinaray(action, actionId, 4);
    customConcat(fullId, actionId);
    char* manchester = encManchester(fullId, 0);
    char* rawSignal = genRawDataTD157(200, 600, manchester);
    for(u_int32_t i = 1; app->repeats >= i; i++) {
        flipper_format_write_string_cstr(ff, "RAW_Data", rawSignal);
    }
    free(manchester);
    free(rawSignal);
}

static void
    meal_pager_retekess_td157_generate_station(void* context, uint32_t station, FlipperFormat* ff) {
    Meal_Pager* app = context;
    FURI_LOG_D(
        TAG,
        "Generating TD157 Data for Station %lu. Pagers From %lu to %lu",
        station,
        app->first_pager,
        app->last_pager);
    app->current_station = station;
    app->current_pager = app->first_pager;
    char stationId[14];
    uint32ToBinaray(station, stationId, 10);
    //reverse(stationId);
    meal_pager_transmit_model_set_station(app->meal_pager_transmit, app->current_station);
    for(u_int32_t i = app->current_pager; i <= app->last_pager; i++) {
        meal_pager_retekess_td157_generate_pager(app, stationId, i, ff);
        if(app->stop_transmit) {
            break;
        }
    }
}

bool meal_pager_retekess_td157_generate_all(void* context) {
    Meal_Pager* app = context;

    app->current_pager = 1;
    app->current_station = app->first_station;

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* ff = flipper_format_file_alloc(storage);
    bool success = meal_pager_save_subghz_buffer_file_start(app, ff, storage, MEAL_PAGER_SUBGHZ_FILE_FREQUENCY);

    if(!success) {
        FURI_LOG_D(TAG, "failed to save to buffer");
        meal_pager_save_subghz_buffer_stop(app, ff);
        furi_record_close(RECORD_STORAGE);
        return success;
    }

    for(u_int32_t i = app->current_station; i <= app->last_station; i++) {
        meal_pager_retekess_td157_generate_station(app, i, ff);
        if(app->stop_transmit) {
            break;
        }
    }

    meal_pager_save_subghz_buffer_stop(app, ff);
    furi_record_close(RECORD_STORAGE);
    return success;
}
