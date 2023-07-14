#include "i2csniffer.h"
#include <lib/toolbox/random_name.h>
#include <toolbox/path.h>
#include <storage/filesystem_api_defines.h>
#include <storage/storage.h>
#include <dolphin/dolphin.h>

#define I2CTOOLS_FILE_NAME_SIZE 100
#define I2CTOOLS_TEXT_STORE_SIZE 1024

#define I2CTOOLS_APP_FOLDER "/ext/i2cTools/"
#define I2CTOOLS_APP_EXTENSION ".csv"
#define I2CTOOLS_APP_FILE_TYPE "Flipper i2c record"

#define TIMESTAMP_FORMAT "%.4d-%.2d-%.2d_%.2d-%.2d"

typedef struct {
    FuriString* file_path;
    char text_store[I2CTOOLS_TEXT_STORE_SIZE + 1];
    i2cSniffer* i2c_data;
    bool saving;
    bool saved;
} i2cSaver;

i2cSaver* i2c_saver_alloc();
void i2c_saver_free(i2cSaver* i2c_save);
void i2c_save_file();
//void i2csniffer_save_name_on_enter(void* context);