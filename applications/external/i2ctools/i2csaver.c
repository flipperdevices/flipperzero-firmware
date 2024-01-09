#include "i2csaver.h"

bool i2c_save_file(const char* content) {
    bool to_return = false;
    // We need a storage struct (gain accesso to the filesystem API )
    Storage* storage = furi_record_open(RECORD_STORAGE);
    // storage_file_alloc gives to us a File pointer using the Storage API.
    File* file = storage_file_alloc(storage);

    // Get timestamp
    FuriHalRtcDateTime datetime;
    furi_hal_rtc_get_datetime(&datetime);
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

    // Create random file name with date
    char* file_name = (char*)malloc(sizeof(char) * I2CTOOLS_FILE_NAME_SIZE);
    name_generator_make_random(file_name, I2CTOOLS_FILE_NAME_SIZE);

    // Create save path
    size_t savePathSize =
        sizeof(char) * sizeof(I2CTOOLS_APP_FOLDER) + sizeof(char) * sizeof(timeStamp) +
        sizeof(char) * I2CTOOLS_FILE_NAME_SIZE + sizeof(char) * sizeof(I2CTOOLS_APP_EXTENSION);

    char* savePath = (char*)malloc(savePathSize);

    snprintf(savePath, savePathSize, "%s%s%s", I2CTOOLS_APP_FOLDER, timeStamp, file_name);

    // Open path in write mode
    bool result = storage_file_open(file, savePath, FSAM_WRITE, FSOM_OPEN_ALWAYS);

    if(result) {
        // Write the file
        storage_file_write(file, content, strlen(content));
        to_return = true;
    }
    // Closing the "file descriptor"
    storage_file_close(file);

    // Freeing up memory
    storage_file_free(file);
    free(file_name);
    free(savePath);
    return to_return;
}