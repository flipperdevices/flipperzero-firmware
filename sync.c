#include "sync.h"

/* Search sync results commencing at *syncString, which starts from element *syncStringIndex
   The found token has its type stored in *tokenItem and value in tokenValue.
   Returns TRUE if a token was found, FALSE otherwise.
*/
bool syncNextToken(uint8_t **syncString, GravitySyncItem *tokenItem, char *tokenValue) {
    /* *syncString begins with '(', use it to determine whether we've reached the end */
    if (*syncString[0] != '(') {
        return false;
    }
    uint8_t *tokStart = *syncString + 1;
    int tokLen = 0;
    for ( ; tokStart[tokLen] != ':'; ++tokLen) { }
    /* Create somewhere to store string repreesentation of the token */
    uint8_t *tokItem = malloc(sizeof(uint8_t) * (tokLen + 1)); // Include '\0' for strtol
    if (tokItem == NULL) {
        // TODO: error dialogue
        return false;
    }
    memcpy(tokItem, tokStart, tokLen);
    tokItem[tokLen] = '\0';
    *tokenItem = strtol((char *)tokItem, NULL, 10);
    /* Update syncString */
    *syncString = tokStart + tokLen;
    /* Copy the token value, starting with ':' & terminated by ')' */
    tokStart = *syncString + 1;
    for (tokLen = 0; tokStart[tokLen] != ')'; ++tokLen) {
        tokenValue[tokLen] = tokStart[tokLen];
    }
    tokenValue[tokLen] = '\0';
    /* Update syncString once again */
    *syncString = tokStart + tokLen + 1; // +1 to skip ')'

    return true;
}

/* Process settings and state information stored in syncBuffer.
   Methods of synchronisation:
    HOP_ON: "hop on" "hop off" "hop sequential" "hop random"
    ALL OTHERS: set
   Where appropriate, replace "GET" menuItem options with the current value.
*/
bool syncProcessResponse(UART_TerminalApp *app) {
    uint8_t *nextToken = app->syncBuffer;
    int tokenInt;
    GravitySyncItem tokenItem = 0;
    char tokenValue[18] = "";
    char *newGet = NULL;

    /* Skip over any extraneous data */
    while (nextToken[0] != '(' && nextToken[0] != '\0') {
        ++nextToken;
    }

    /* syncNextToken will set nextIndex to -1 after last token returned */
    while (syncNextToken(&nextToken, &tokenItem, tokenValue)) {
        switch (tokenItem) {
            case GRAVITY_SYNC_HOP_ON:;
                HopStatus status = strtol(tokenValue, NULL, 10);
                HopStatus newStatus = OPTIONS_HOP_DEFAULT;
                switch (status) {
                    case HOP_STATUS_ON:
                        newStatus = OPTIONS_HOP_ON;
                        break;
                    case HOP_STATUS_OFF:
                        newStatus = OPTIONS_HOP_OFF;
                        break;
                    case HOP_STATUS_DEFAULT:
                        newStatus = OPTIONS_HOP_DEFAULT;
                        break;
                }
                app->selected_menu_options[GRAVITY_MENU_SETTINGS][SETTINGS_MENU_HOP_STATUS] = newStatus;
                break;
            case GRAVITY_SYNC_SSID_MIN:
                /* If the "Get" label has already been replaced free the existing string */
                if (strcmp(settings[SETTINGS_MENU_SSID_MIN].options_menu[OPTIONS_SSID_MIN_GET], STRINGS_GET)) {
                    free(settings[SETTINGS_MENU_SSID_MIN].options_menu[OPTIONS_SSID_MIN_GET]);
                }
                /* Allocate non-transient memory to store the new label */
                newGet = malloc(sizeof(char) * (strlen(tokenValue) + 1));
                if (newGet == NULL) {
                    // TODO: Panic!
                    break;
                }
                strcpy(newGet, tokenValue);
                settings[SETTINGS_MENU_SSID_MIN].options_menu[OPTIONS_SSID_MIN_GET] = newGet;
                break;
            case GRAVITY_SYNC_SSID_MAX:
                /* If label has already been replaced then free the contents */
                if (strcmp(settings[SETTINGS_MENU_SSID_MAX].options_menu[OPTIONS_SSID_MAX_GET], STRINGS_GET)) {
                    free(settings[SETTINGS_MENU_SSID_MAX].options_menu[OPTIONS_SSID_MAX_GET]);
                }
                newGet = malloc(sizeof(char) * (strlen(tokenValue) + 1));
                if (newGet == NULL) {
                    // TODO: error
                    break;
                }
                strcpy(newGet, tokenValue);
                settings[SETTINGS_MENU_SSID_MAX].options_menu[OPTIONS_SSID_MAX_GET] = newGet;
                break;
            case GRAVITY_SYNC_SSID_COUNT:
                /* If label has already been replaced then free it */
                if (strcmp(settings[SETTINGS_MENU_SSID_DEFAULT].options_menu[OPTIONS_SSID_DEFAULT_GET], STRINGS_GET)) {
                    free(settings[SETTINGS_MENU_SSID_DEFAULT].options_menu[OPTIONS_SSID_DEFAULT_GET]);
                }
                newGet = malloc(sizeof(char) * (strlen(tokenValue) + 1));
                if (newGet == NULL) {
                    // TODO: Panic
                    break;
                }
                strcpy(newGet, tokenValue);
                settings[SETTINGS_MENU_SSID_DEFAULT].options_menu[OPTIONS_SSID_DEFAULT_GET] = newGet;
                break;
            case GRAVITY_SYNC_CHANNEL:
                // set channel
                app->channel = strtol(tokenValue, NULL, 10);
                app->selected_menu_options[GRAVITY_MENU_SETTINGS][SETTINGS_MENU_CHANNEL] = app->channel;
                break;
            case GRAVITY_SYNC_MAC: ;
                // set MAC
                mac_string_to_bytes(tokenValue, app->mac_bytes);
                break;
            case GRAVITY_SYNC_ATTACK_MILLIS:
                /* If 'get' label has already been replaced by sync, free that memory */
                if (strcmp(settings[SETTINGS_MENU_ATTACK_MILLIS].options_menu[OPTIONS_ATTACK_MILLIS_GET], STRINGS_GET)) {
                    free(settings[SETTINGS_MENU_ATTACK_MILLIS].options_menu[OPTIONS_ATTACK_MILLIS_GET]);
                }
                newGet = malloc(sizeof(char) * (strlen(tokenValue) + 1));
                if (newGet == NULL) {
                    // TODO: Panic
                    break;
                }
                strcpy(newGet, tokenValue);
                settings[SETTINGS_MENU_ATTACK_MILLIS].options_menu[OPTIONS_ATTACK_MILLIS_GET] = newGet;
                break;
            case GRAVITY_SYNC_MAC_RAND:
                // set value
                tokenInt = strtol(tokenValue, NULL, 10);
                if ((bool)tokenInt) {
                    app->selected_menu_options[GRAVITY_MENU_SETTINGS][SETTINGS_MENU_MAC_RAND] = OPTIONS_MAC_RAND_ON;
                    app->mac_rand = true;
                } else {
                    app->selected_menu_options[GRAVITY_MENU_SETTINGS][SETTINGS_MENU_MAC_RAND] = OPTIONS_MAC_RAND_OFF;
                    app->mac_rand = false;
                }
                break;
            case GRAVITY_SYNC_PKT_EXPIRY:
                /* Free the label if it's already been changed by sync */
                if (strcmp(settings[SETTINGS_MENU_PKT_EXPIRY].options_menu[OPTIONS_PKT_EXPIRY_GET], STRINGS_GET)) {
                    free(settings[SETTINGS_MENU_PKT_EXPIRY].options_menu[OPTIONS_PKT_EXPIRY_GET]);
                }
                newGet = malloc(sizeof(char) * (strlen(tokenValue) + 1));
                if (newGet == NULL) {
                    // TODO: Panic
                    break;
                }
                strcpy(newGet, tokenValue);
                settings[SETTINGS_MENU_PKT_EXPIRY].options_menu[OPTIONS_PKT_EXPIRY_GET] = newGet;
                break;
            case GRAVITY_SYNC_HOP_MODE:
                // set 
                tokenInt = strtol(tokenValue, NULL, 10);
                int currentHopMode = 0;
                switch (tokenInt) {
                    case HOP_MODE_SEQUENTIAL:
                        currentHopMode = OPTIONS_HOP_MODE_SEQUENTIAL;
                        break;
                    case HOP_MODE_RANDOM:
                        currentHopMode = OPTIONS_HOP_MODE_RANDOM;
                        break;
                    default:
                        // TODO: Error
                        break;
                }
                app->hopMode = tokenInt;
                app->selected_menu_options[GRAVITY_MENU_SETTINGS][SETTINGS_MENU_HOP_MODE] = currentHopMode;
                break;
            case GRAVITY_SYNC_DICT_DISABLED:
                // set
                tokenInt = strtol(tokenValue, NULL, 10);
                bool scrambled = (bool)tokenInt;
                int newVal;
                if (scrambled) {
                    newVal = OPTIONS_DICT_CHARS;
                } else {
                    newVal = OPTIONS_DICT_WORDS;
                }
                app->dict_disabled = scrambled;
                app->selected_menu_options[GRAVITY_MENU_SETTINGS][SETTINGS_MENU_DICT_DISABLE] = newVal;
                break;
            case GRAVITY_SYNC_PURGE_STRAT:
                // set
                tokenInt = strtol(tokenValue, NULL, 10);
                app->purgeStrategy = tokenInt;
                if ((tokenInt & GRAVITY_PURGE_AGE) == GRAVITY_PURGE_AGE) {
                    app->selected_menu_options[GRAVITY_MENU_PURGE][PURGE_MENU_AGE_ON] = OPTIONS_PURGE_ON;
                } else {
                    app->selected_menu_options[GRAVITY_MENU_PURGE][PURGE_MENU_AGE_ON] = OPTIONS_PURGE_OFF;
                }
                if ((tokenInt & GRAVITY_PURGE_RSSI) == GRAVITY_PURGE_RSSI) {
                    app->selected_menu_options[GRAVITY_MENU_PURGE][PURGE_MENU_RSSI_ON] = OPTIONS_PURGE_ON;
                } else {
                    app->selected_menu_options[GRAVITY_MENU_PURGE][PURGE_MENU_RSSI_ON] = OPTIONS_PURGE_OFF;
                }
                if ((tokenInt & GRAVITY_PURGE_UNSELECTED) == GRAVITY_PURGE_UNSELECTED) {
                    app->selected_menu_options[GRAVITY_MENU_PURGE][PURGE_MENU_UNSELECTED_ON] = OPTIONS_PURGE_ON;
                } else {
                    app->selected_menu_options[GRAVITY_MENU_PURGE][PURGE_MENU_UNSELECTED_ON] = OPTIONS_PURGE_OFF;
                }
                if ((tokenInt & GRAVITY_PURGE_UNNAMED) == GRAVITY_PURGE_UNNAMED) {
                    app->selected_menu_options[GRAVITY_MENU_PURGE][PURGE_MENU_UNNAMED_ON] = OPTIONS_PURGE_ON;
                } else {
                    app->selected_menu_options[GRAVITY_MENU_PURGE][PURGE_MENU_UNNAMED_ON] = OPTIONS_PURGE_OFF;
                }
                break;
            case GRAVITY_SYNC_PURGE_RSSI_MAX:
                /* This can be set to any value but Flipper offers discrete values. Find the nearest
                   matching option that is less than that returned */
                // Loop through purgeMenu.num_options_menu in purgeMenu.actual_commands[] until
                // option is > tokenValue. Then take the option prior to that.
                tokenInt = strtol(tokenValue, NULL, 10);
                app->purgeRSSI = tokenInt;
                int rssiIdx = 0;
                for (; rssiIdx < purgeMenu[PURGE_MENU_RSSI].num_options_menu &&
                        strtol(purgeMenu[PURGE_MENU_RSSI].actual_commands[rssiIdx], NULL, 10) <= tokenInt; ++rssiIdx) { }
                if (rssiIdx == purgeMenu[PURGE_MENU_RSSI].num_options_menu) {
                    // Not found - TODO: Error (modal dialogue?)
                    rssiIdx = 0;
                } else {
                    /* Take the element prior to that greater than tokenInt (<= tokenInt) */
                    --rssiIdx;
                }
                app->selected_menu_options[GRAVITY_MENU_PURGE][PURGE_MENU_RSSI] = rssiIdx;
                break;
            case GRAVITY_SYNC_PURGE_AGE_MIN:
                /* This can be set to any value but Flipper offers discrete values. Find the nearest
                   matching option that is greater than that returned */
                // Loop through age options while option < tokenValue, setting selected option as
                // the first option >= tokenValue
                tokenInt = strtol(tokenValue, NULL, 10);
                app->purgeAge = tokenInt;
                int ageIdx = 0;
                for (; ageIdx < purgeMenu[PURGE_MENU_AGE].num_options_menu &&
                        strtol(purgeMenu[PURGE_MENU_AGE].actual_commands[ageIdx], NULL, 10) < tokenInt; ++ageIdx) { }
                if (ageIdx == purgeMenu[PURGE_MENU_AGE].num_options_menu) {
                    // Not found - TODO: Error (modal dialogue?)
                    ageIdx = 0;
                }
                app->selected_menu_options[GRAVITY_MENU_PURGE][PURGE_MENU_AGE] = ageIdx;
                break;
            default:
                // display modal error
                break;
        }
    }
    return true;
}

/* Handle synchronisation response from Flipper */
void uart_terminal_sync_rx_data_cb(uint8_t* buf, size_t len, void* context) {
     furi_assert(context);
     UART_TerminalApp* app = context;

     /* Append buf and len to log file */
    Storage *storage = furi_record_open(RECORD_STORAGE);
    File *file = storage_file_alloc(storage);
    if (!storage_file_open(file, APP_DATA_PATH("gravity.log"), FSAM_WRITE, FSOM_OPEN_APPEND)) {
        FURI_LOG_E("SYNC", "Failed to open gravity.log for writing");
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);
    } else {
        storage_file_write(file, "In callback with args:\n", strlen("In callback with args:\n"));
        storage_file_write(file, (char *)buf, len);
        storage_file_write(file, "\n\n", 2);
        // storage_file_sync(file);
        // storage_file_close(file);
        // storage_file_free(file);
        // furi_record_close(RECORD_STORAGE);
    }

     if (len == 0 || buf == NULL) {
        storage_file_write(file, "empty buffer\n", strlen("empty buffer\n"));
        storage_file_sync(file);
        storage_file_close(file);
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);
         return;
     }

    /* Ensure syncBuffer has enough space for buf */
     int copyLen = app->syncBufLen + len;
     if (copyLen >= SYNC_BUFFER_SIZE) {
    //     /* Buffer too small, copy as much as we can
    //        YAGNI: Process buffer contents then clear all bar the last sync element
    //        if it is incomplete. Then add the remainder of buf (buffer is plenty
    //        large enough)
    //     */
         copyLen = SYNC_BUFFER_SIZE - app->syncBufLen - 1;
     } else {
         copyLen = len;
     }
    /* Append buf to syncBuffer */
    memcpy(app->syncBuffer + app->syncBufLen, buf, copyLen);
    app->syncBufLen += copyLen;

    /* Wait until all data has been received before processing anything, otherwise
       sync transmission will be interrupted by responses to "set" commands
       issued by the sync process
    */

    /* Sync is complete when a command prompt is encountered (and it's
       not at the beginning of the response) */
    uint16_t promptIdx = 1;
    for ( ; promptIdx < len && buf[promptIdx] != '>'; ++promptIdx) { }
    if (promptIdx < len) {
        /* If app->syncBuffer contains 'z' (for "Unrecognized command") call
           sync a second time */
        uint16_t zIdx = 0;
        for ( ; zIdx < app->syncBufLen && app->syncBuffer[zIdx] != 'z'; ++zIdx) { }
        if (zIdx < app->syncBufLen) {
            memset(app->syncBuffer, '\0', SYNC_BUFFER_SIZE);
            app->syncBufLen = 0;
            uart_terminal_uart_tx((uint8_t *)"sync\n", 5);
        } else {
             app->syncComplete = true;
             /* Process sync elements */
             if (!syncProcessResponse(app)) {
                 // TODO: Display modal dialogue reporting failure
             }
             memset(app->syncBuffer, '\0', SYNC_BUFFER_SIZE);
             app->syncBufLen = 0;
             /* De-register the sync callback */
             uart_terminal_uart_set_handle_rx_data_cb(app->uart, NULL);
        }
     }

     storage_file_sync(file);
     storage_file_close(file);
     storage_file_free(file);
     furi_record_close(RECORD_STORAGE);
}

/* Free options labels that have been modified by Sync to replace the "Get" option label */
void syncCleanup() {
    if (strcmp(settings[SETTINGS_MENU_SSID_MIN].options_menu[OPTIONS_SSID_MIN_GET], STRINGS_GET)) {
        free(settings[SETTINGS_MENU_SSID_MIN].options_menu[OPTIONS_SSID_MIN_GET]);
    }
    if (strcmp(settings[SETTINGS_MENU_SSID_MAX].options_menu[OPTIONS_SSID_MAX_GET], STRINGS_GET)) {
        free(settings[SETTINGS_MENU_SSID_MAX].options_menu[OPTIONS_SSID_MAX_GET]);
    }
    if (strcmp(settings[SETTINGS_MENU_SSID_DEFAULT].options_menu[OPTIONS_SSID_DEFAULT_GET], STRINGS_GET)) {
        free(settings[SETTINGS_MENU_SSID_DEFAULT].options_menu[OPTIONS_SSID_DEFAULT_GET]);
    }
    if (strcmp(settings[SETTINGS_MENU_ATTACK_MILLIS].options_menu[OPTIONS_ATTACK_MILLIS_GET], STRINGS_GET)) {
        free(settings[SETTINGS_MENU_ATTACK_MILLIS].options_menu[OPTIONS_ATTACK_MILLIS_GET]);
    }
    if (strcmp(settings[SETTINGS_MENU_PKT_EXPIRY].options_menu[OPTIONS_PKT_EXPIRY_GET], STRINGS_GET)) {
        free(settings[SETTINGS_MENU_PKT_EXPIRY].options_menu[OPTIONS_PKT_EXPIRY_GET]);
    }
}

void do_sync(UART_TerminalApp *app) {
//    if (!app->syncComplete) {
        /* Initialise sync buffer */
        memset(app->syncBuffer, '\0', SYNC_BUFFER_SIZE);
        app->syncBufLen = 0;
        /* Init */
        uart_terminal_uart_set_handle_rx_data_cb(app->uart, NULL);
        uart_terminal_uart_tx((uint8_t *)"\n", 1);
        
        /* Register callback to receive data */
        uart_terminal_uart_set_handle_rx_data_cb(app->uart, uart_terminal_sync_rx_data_cb);
        /* Execute Sync */
        uart_terminal_uart_tx((uint8_t *)"sync\n", 5);

        // perhaps start a timer here of 1 or 2 seconds. If syncBuffer contains "Unrecognized" then sync again

        //char purgeString[] = "(0:2)(1:8)(2:32)(3:20)(4:1)(5:40:91:51:BB:AC:7D)(6:5)(7:1)(8:0.000000)(9:0)(10:0)(11:11)(12:-95)(13:90)\n";
        //uart_terminal_sync_rx_data_cb((uint8_t *)purgeString, strlen(purgeString), app);
//    }
}