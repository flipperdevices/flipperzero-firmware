#include "i2csaver.h"

i2cSaver* i2c_saver_alloc() {
    i2cSaver* i2c_save = malloc(sizeof(i2cSaver));
    i2c_save->file_path = furi_string_alloc();
    i2c_save->saved = false;
    i2c_save->saving = false;
    return i2c_save;
}

void i2c_saver_free(i2cSaver* i2c_save) {
    furi_assert(i2c_save);
    free(i2c_save);
}

void i2c_save_file() {
    // We need a storage struct (gain accesso to the filesystem API )
    Storage* storage = furi_record_open(RECORD_STORAGE);
    // storage_file_alloc gives to us a File pointer using the Storage API.
    File* file = storage_file_alloc(storage);

    // Get timestamp
    FuriHalRtcDateTime datetime;
    furi_hal_rtc_get_datetime(&datetime);
    //uint32_t curr_ts = furi_hal_rtc_datetime_to_timestamp(&datetime);
    char timeStamp[30];
    snprintf(
        timeStamp,
        30,
        TIMESTAMP_FORMAT,
        datetime.year,
        datetime.month,
        datetime.day,
        datetime.hour,
        datetime.minute);

    //printf(timeStamp);
    // Create random file name with date
    char* file_name = (char*)malloc(sizeof(char) * I2CTOOLS_FILE_NAME_SIZE);
    set_random_name(file_name, I2CTOOLS_FILE_NAME_SIZE);

    // Create save path
    char* savePath = (char*)malloc(
        sizeof(char) * sizeof(I2CTOOLS_APP_FOLDER) + sizeof(char) * sizeof(timeStamp) +
        sizeof(char) * I2CTOOLS_FILE_NAME_SIZE + sizeof(char) * sizeof(I2CTOOLS_APP_EXTENSION));

    // Add app folder to savePath
    strcpy(savePath, I2CTOOLS_APP_FOLDER);
    // Add timestamp to path
    strcat(savePath, timeStamp);
    // Add filename to path
    strcat(savePath, file_name);
    // Add extension to path
    strcat(savePath, I2CTOOLS_APP_EXTENSION);

    // Open path in write mode
    bool result = storage_file_open(file, savePath, FSAM_WRITE, FSOM_OPEN_ALWAYS);

    // Create content to write
    char* content = (char*)malloc(sizeof(char) * I2CTOOLS_TEXT_STORE_SIZE);
    content = "This is an Awesome test !!\n";
    if(result) {
        // Write the file
        storage_file_write(file, content, strlen(content));

        // Closing the "file descriptor"
        storage_file_close(file);

        // Freeing up memory
        storage_file_free(file);
    }
    free(savePath);
}
/*
static void i2csniffer_save_name_text_input_callback(void* context) {
    UNUSED(context);
    printf("save");
}
*/