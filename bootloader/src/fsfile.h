#pragma once

#include <fatfs.h>

#define CHECK_FRESULT(result)   \
    {                           \
        if((result) != FR_OK) { \
            return false;       \
        }                       \
    }

typedef struct {
    FILINFO stat;
    FIL file;
    bool valid;
} FsFile;


bool fs_file_is_valid(FsFile* fsfile);

bool fs_file_open(FsFile* fsfile, const char* fpath);

void fs_file_close(FsFile* fsfile);
