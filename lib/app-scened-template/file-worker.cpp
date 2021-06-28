#include "file-worker.h"

FileWorker::FileWorker()
    : fs_api{"sdcard"}
    , sd_ex_api{"sdcard-ex"} {
    string_init(error_string);
}

FileWorker::~FileWorker() {
    string_clear(error_string);
}

bool FileWorker::open(const char* filename, FS_AccessMode access_mode, FS_OpenMode open_mode) {
    bool result = fs_api.get()->file.open(&file, filename, access_mode, open_mode);

    if(!result) {
        show_error_message("Cannot open\nfile");
        close();
        return false;
    }

    return check_common_errors();
}

bool FileWorker::close() {
    fs_api.get()->file.close(&file);

    return check_common_errors();
}

bool FileWorker::mkdir(const char* dirname) {
    FS_Error fs_result = fs_api.get()->common.mkdir(dirname);

    if(fs_result != FSE_OK && fs_result != FSE_EXIST) {
        show_error_message("Cannot create\nfolder");
        return false;
    };

    return check_common_errors();
}

bool FileWorker::remove(const char* filename) {
    FS_Error fs_result = fs_api.get()->common.remove(filename);
    if(fs_result != FSE_OK && fs_result != FSE_NOT_EXIST) {
        show_error_message("Cannot remove\nold file");
        return false;
    };

    return check_common_errors();
}

bool FileWorker::read(void* buffer, uint16_t bytes_to_read) {
    if(!read_internal(buffer, bytes_to_read)) {
        return false;
    }

    return check_common_errors();
}

bool FileWorker::read_until(string_t str_result, char separator) {
    string_clean(str_result);
    const uint8_t buffer_size = 32;
    uint8_t buffer[buffer_size];

    do {
        uint16_t read_count = fs_api.get()->file.read(&file, buffer, buffer_size);
        if(file.error_id != FSE_OK) {
            show_error_message("Cannot read\nfile");
            fs_api.get()->file.close(&file);
            return false;
        }

        bool result = false;
        for(uint16_t i = 0; i < read_count; i++) {
            if(buffer[i] == separator) {
                uint64_t position;
                if(!tell_internal(&position)) {
                    return false;
                }

                position = position - read_count + i + 1;

                if(!seek_internal(position, true)) {
                    return false;
                }

                result = true;
                break;
            } else {
                string_push_back(str_result, buffer[i]);
            }
        }

        if(result || read_count == 0) {
            break;
        }
    } while(true);

    return check_common_errors();
}

bool FileWorker::read_hex(uint8_t* buffer, uint16_t bytes_to_read) {
    uint8_t text[2];

    for(uint8_t i = 0; i < bytes_to_read; i++) {
        if(!read_internal(text, 2)) {
            return false;
        }

        // convert hex value to byte
        buffer[i] = strtol(reinterpret_cast<char*>(text), NULL, 16);
    }

    return check_common_errors();
}

bool FileWorker::tell(uint64_t* position) {
    if(!tell_internal(position)) {
        return false;
    }

    return check_common_errors();
}

bool FileWorker::seek(uint64_t position, bool from_start) {
    if(!seek_internal(position, from_start)) {
        return false;
    }

    return check_common_errors();
}

bool FileWorker::check_common_errors() {
    sd_ex_api.get()->check_error(sd_ex_api.get()->context);
    return true;
}

void FileWorker::show_error_message(const char* error_text) {
    string_set_str(error_string, error_text);
    sd_ex_api.get()->show_error(sd_ex_api.get()->context, string_get_cstr(error_string));
}

bool FileWorker::read_internal(void* buffer, uint16_t bytes_to_read) {
    uint16_t read_count = fs_api.get()->file.read(&file, buffer, bytes_to_read);

    if(file.error_id != FSE_OK || read_count != bytes_to_read) {
        show_error_message("Cannot read\nfile");
        fs_api.get()->file.close(&file);
        return false;
    }

    return true;
}

bool FileWorker::tell_internal(uint64_t* position) {
    *position = fs_api.get()->file.tell(&file);

    if(file.error_id != FSE_OK) {
        show_error_message("Cannot tell\nfile offset");
        return false;
    }

    return true;
}

bool FileWorker::seek_internal(uint64_t position, bool from_start) {
    fs_api.get()->file.seek(&file, position, from_start);
    if(file.error_id != FSE_OK) {
        show_error_message("Cannot seek\nfile");
        return false;
    }

    return true;
}
