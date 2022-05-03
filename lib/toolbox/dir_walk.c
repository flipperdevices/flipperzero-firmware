#include "dir_walk.h"
#include <m-list.h>

LIST_DEF(DirIndexList, uint32_t);

struct DirWalk {
    File* file;
    string_t path;
    DirIndexList_t index_list;
    uint32_t current_index;
    DirWalkMode mode;
};

DirWalk* dir_walk_alloc(Storage* storage) {
    DirWalk* dir_walk = malloc(sizeof(DirWalk));
    string_init(dir_walk->path);
    dir_walk->file = storage_file_alloc(storage);
    DirIndexList_init(dir_walk->index_list);
    return dir_walk;
}

void dir_walk_free(DirWalk* dir_walk) {
    storage_file_free(dir_walk->file);
    string_clear(dir_walk->path);
    DirIndexList_clear(dir_walk->index_list);
    free(dir_walk);
}

bool dir_walk_open(DirWalk* dir_walk, const char* path, DirWalkMode mode) {
    string_set(dir_walk->path, path);
    dir_walk->current_index = 0;
    dir_walk->mode = mode;
    return storage_dir_open(dir_walk->file, path);
}

static DirWalkResult
    dir_walk_iter_forward(DirWalk* dir_walk, string_t return_path, FileInfo* fileinfo) {
    DirWalkResult result = DirWalkError;
    char* name = malloc(256);
    FileInfo info;
    bool end = false;

    while(!end) {
        storage_dir_read(dir_walk->file, &info, name, 255);

        if(storage_file_get_error(dir_walk->file) == FSE_OK) {
            result = DirWalkOK;
            dir_walk->current_index++;

            if(return_path != NULL) {
                string_printf(return_path, "%s/%s", string_get_cstr(dir_walk->path), name);
            }

            if(fileinfo != NULL) {
                memcpy(fileinfo, &info, sizeof(FileInfo));
            }

            if(info.flags & FSF_DIRECTORY) {
                // step into

                DirIndexList_push_back(dir_walk->index_list, dir_walk->current_index);
                dir_walk->current_index = 0;
                storage_dir_close(dir_walk->file);

                string_cat_printf(dir_walk->path, "/%s", name);
                storage_dir_open(dir_walk->file, string_get_cstr(dir_walk->path));
            }

            end = true;
        } else if(storage_file_get_error(dir_walk->file) == FSE_NOT_EXIST) {
            if(DirIndexList_size(dir_walk->index_list) == 0) {
                result = DirWalkLast;
                end = true;
            } else {
                // step out
                uint32_t index;
                DirIndexList_pop_back(&index, dir_walk->index_list);
                dir_walk->current_index = 0;

                storage_dir_close(dir_walk->file);

                size_t last_char = string_search_rchar(dir_walk->path, '/');
                if(last_char != STRING_FAILURE) {
                    string_left(dir_walk->path, last_char);
                }

                storage_dir_open(dir_walk->file, string_get_cstr(dir_walk->path));

                // rewind
                while(true) {
                    if(index == dir_walk->current_index) {
                        result = DirWalkOK;
                        break;
                    }

                    if(!storage_dir_read(dir_walk->file, &info, name, 255)) {
                        result = DirWalkError;
                        end = true;
                        break;
                    }

                    dir_walk->current_index++;
                }
            }
        } else {
            result = DirWalkError;
            end = true;
        }
    }

    free(name);
    return result;
}

static DirWalkResult
    dir_walk_iter_backward(DirWalk* dir_walk, string_t return_path, FileInfo* fileinfo) {
    DirWalkResult result = DirWalkError;
    char* name = malloc(256);
    FileInfo info;
    bool end = false;

    while(!end) {
        storage_dir_read(dir_walk->file, &info, name, 255);

        if(storage_file_get_error(dir_walk->file) == FSE_OK) {
            result = DirWalkOK;
            dir_walk->current_index++;

            if(info.flags & FSF_DIRECTORY) {
                // step into
                DirIndexList_push_back(dir_walk->index_list, dir_walk->current_index);
                dir_walk->current_index = 0;
                storage_dir_close(dir_walk->file);

                string_cat_printf(dir_walk->path, "/%s", name);
                storage_dir_open(dir_walk->file, string_get_cstr(dir_walk->path));
            } else {
                if(return_path != NULL) {
                    string_printf(return_path, "%s/%s", string_get_cstr(dir_walk->path), name);
                }

                if(fileinfo != NULL) {
                    memcpy(fileinfo, &info, sizeof(FileInfo));
                }

                end = true;
            }
        } else if(storage_file_get_error(dir_walk->file) == FSE_NOT_EXIST) {
            if(DirIndexList_size(dir_walk->index_list) == 0) {
                result = DirWalkLast;
                end = true;
            } else {
                // step out
                uint32_t index;
                DirIndexList_pop_back(&index, dir_walk->index_list);
                dir_walk->current_index = 0;

                end = true;

                storage_dir_close(dir_walk->file);

                size_t last_char = string_search_rchar(dir_walk->path, '/');
                if(last_char != STRING_FAILURE) {
                    string_left(dir_walk->path, last_char);
                }

                storage_dir_open(dir_walk->file, string_get_cstr(dir_walk->path));

                // rewind
                while(true) {
                    // there will be at least one read before that, because popped index > 0
                    if(index == dir_walk->current_index) {
                        if(return_path != NULL) {
                            string_printf(
                                return_path, "%s/%s", string_get_cstr(dir_walk->path), name);
                        }

                        if(fileinfo != NULL) {
                            memcpy(fileinfo, &info, sizeof(FileInfo));
                        }

                        result = DirWalkOK;
                        break;
                    }

                    if(!storage_dir_read(dir_walk->file, &info, name, 255)) {
                        result = DirWalkError;
                        end = true;
                        break;
                    }

                    dir_walk->current_index++;
                }
            }
        } else {
            result = DirWalkError;
            end = true;
        }
    }

    free(name);
    return result;
}

FS_Error dir_walk_get_error(DirWalk* dir_walk) {
    return storage_file_get_error(dir_walk->file);
}

DirWalkResult dir_walk_read(DirWalk* dir_walk, string_t return_path, FileInfo* fileinfo) {
    if(dir_walk->mode == DirWalkForward) {
        return dir_walk_iter_forward(dir_walk, return_path, fileinfo);
    } else {
        return dir_walk_iter_backward(dir_walk, return_path, fileinfo);
    }
}

void dir_walk_close(DirWalk* dir_walk) {
    if(storage_file_is_open(dir_walk->file)) {
        storage_dir_close(dir_walk->file);
    }

    DirIndexList_reset(dir_walk->index_list);
    string_reset(dir_walk->path);
    dir_walk->current_index = 0;
}
