#include "fsfile.h"


bool fs_file_is_valid(FsFile* fsfile) {
    return fsfile && fsfile->valid;
}

bool fs_file_open(FsFile* fsfile, const char* fpath) {
    CHECK_FRESULT(f_stat(fpath, &fsfile->stat));
    CHECK_FRESULT(f_open(&fsfile->file, fpath, FA_OPEN_EXISTING | FA_READ));
    return fsfile->valid = true;
}

void fs_file_close(FsFile* fsfile) {
    if(!fs_file_is_valid(fsfile)) {
        return;
    }

    f_close(&fsfile->file);
    fsfile->valid = false;
}
