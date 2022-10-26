#pragma once
#include "spi_mem_app.h"

void spi_mem_file_create_folder(SPIMemApp* app);
bool spi_mem_file_select(SPIMemApp* app);
bool spi_mem_file_create(SPIMemApp* app, const char* file_name);
bool spi_mem_file_delete(SPIMemApp* app);
