#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <lib/toolbox/name_generator.h>
#include <storage/filesystem_api_defines.h>
#include <storage/storage.h>

#define I2CTOOLS_FILE_NAME_SIZE 100
#define I2CTOOLS_TEXT_STORE_SIZE 1024

#define I2CTOOLS_APP_FOLDER "/ext/i2cTools/"
#define I2CTOOLS_APP_EXTENSION ".csv"
#define I2CTOOLS_APP_FILE_TYPE "Flipper i2cTools records"

#define TIMESTAMP_FORMAT "%.4d-%.2d-%.2d_%.2d-%.2d"

bool i2c_save_file(const char* content);