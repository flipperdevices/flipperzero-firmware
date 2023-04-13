#include "sequential_file.h"

SequentialFile* sequential_file_create(Storage* storage, const char* dir, const char* prefix, const char* extension) {
    if (storage == NULL || dir == NULL || prefix == NULL || extension == NULL) {
        return NULL;
    }
    char file_path[256];
    File* file = storage_file_alloc(storage);

    int file_index = 0;
    do {
        snprintf(file_path, sizeof(file_path),
                 "%s/%s_%d.%s",
                 dir, prefix, file_index, extension);
        file_index++;
    } while(storage_file_exists(storage, file_path));

    if(!storage_file_open(file, file_path, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        storage_file_free(file);
        return NULL;
    }

    SequentialFile* sequential_file = (SequentialFile*) malloc(sizeof(SequentialFile));
    sequential_file->file = file;
    sequential_file->path = (char*) malloc(strlen(file_path) + 1);
    strcpy(sequential_file->path, file_path);
    sequential_file->file_index = file_index - 1;

    return sequential_file;
}


uint16_t sequential_file_write(SequentialFile* sequential_file, const void *buffer, uint16_t bytes_to_write) {
    if (sequential_file == NULL || sequential_file->file == NULL) {
        return 0;
    }
    return storage_file_write(sequential_file->file, buffer, bytes_to_write);
}

void sequential_file_free_destroy_file(SequentialFile* sequential_file) {
    if (sequential_file != NULL) {
        if (sequential_file->file != NULL) {
            storage_file_close(sequential_file->file);
            storage_file_free(sequential_file->file);
        }
        free(sequential_file->path);
        free(sequential_file);
    }
}

void sequential_file_free_close_file(SequentialFile* sequential_file) {
    if (sequential_file != NULL) {
        if (sequential_file->file != NULL) {
            storage_file_close(sequential_file->file);
        }
        free(sequential_file->path);
        free(sequential_file);
    }
}