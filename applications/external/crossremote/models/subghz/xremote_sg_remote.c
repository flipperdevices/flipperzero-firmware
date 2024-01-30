#include "xremote_sg_remote.h"

#define TAG "Xremote"

struct SubGhzRemote {
    FuriString* name;
    FuriString* filename;
    FuriString* path;
};

const char* xremote_sg_remote_get_name(SubGhzRemote* remote) {
    return furi_string_get_cstr(remote->name);
}

const char* xremote_sg_remote_get_filename(SubGhzRemote* remote) {
    return furi_string_get_cstr(remote->filename);
}

SubGhzRemote* xremote_sg_remote_alloc() {
    SubGhzRemote* remote = malloc(sizeof(SubGhzRemote));
    remote->name = furi_string_alloc();
    remote->filename = furi_string_alloc();
    remote->path = furi_string_alloc();

    return remote;
}

void xremote_sg_remote_free(SubGhzRemote* remote) {
    furi_string_free(remote->path);
    furi_string_free(remote->name);
    furi_string_free(remote->filename);

    free(remote);
}

bool xremote_sg_remote_load(SubGhzRemote* remote, FuriString* path) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* ff = flipper_format_buffered_file_alloc(storage);

    FuriString* buf;
    buf = furi_string_alloc();

    FURI_LOG_I(TAG, "loading SG Remote: \'%s\'", furi_string_get_cstr(path));
    bool success = false;

    do {
        if(!flipper_format_buffered_file_open_existing(ff, furi_string_get_cstr(path))) break;
        const char* fullPath = furi_string_get_cstr(path);
        char* fileName = strrchr(fullPath, '/') + 1;
        furi_string_set_str(remote->filename, fullPath);
        char* dotPosition = strrchr(fileName, '.');
        if(dotPosition != NULL) { // check if there is a dot in the file name
            *dotPosition = '\0'; // set the dot position to NULL character to truncate the string
        }
        //remote->name = fileName;
        furi_string_set_str(remote->name, fileName);
        //furi_string_set_str(remote->filename, fileName);
        //free(fileName);
        uint32_t version;
        if(!flipper_format_read_header(ff, buf, &version)) break;
        if(!furi_string_equal(buf, "Flipper SubGhz RAW File") || (version != 1)) break;

        success = true;
    } while(false);

    furi_string_free(buf);
    flipper_format_buffered_file_close(ff);
    flipper_format_free(ff);
    furi_record_close(RECORD_STORAGE);
    return success;
}