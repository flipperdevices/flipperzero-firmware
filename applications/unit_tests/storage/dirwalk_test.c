#include "../minunit.h"
#include <furi.h>
#include <toolbox/dir_walk.h>

static const char* const storage_test_dirwalk_paths[] = {
    "1",
    "11",
    "111",
    "1/2",
    "1/22",
    "1/222",
    "11/2",
    "111/2",
    "111/22",
    "111/22/33",
};

static const char* const storage_test_dirwalk_files[] = {
    "file1.test",
    "file2.test",
    "file3.test",
    "1/file1.test",
    "111/22/33/file1.test",
    "111/22/33/file2.test",
    "111/22/33/file3.test",
    "111/22/33/file4.test",
};

typedef struct {
    const char* path;
    bool is_dir;
    bool visited;
} StorageTestPath;

static StorageTestPath** storage_test_paths_alloc() {
    size_t path_count =
        COUNT_OF(storage_test_dirwalk_paths) + COUNT_OF(storage_test_dirwalk_files);
    StorageTestPath** data = malloc(sizeof(StorageTestPath*) * path_count);

    for(size_t i = 0; i < COUNT_OF(storage_test_dirwalk_paths); i++) {
        data[i] = malloc(sizeof(StorageTestPath));
        data[i]->path = storage_test_dirwalk_paths[i];
        data[i]->is_dir = true;
        data[i]->visited = false;
    }

    for(size_t i = 0; i < COUNT_OF(storage_test_dirwalk_files); i++) {
        data[i + COUNT_OF(storage_test_dirwalk_paths)] = malloc(sizeof(StorageTestPath));
        data[i + COUNT_OF(storage_test_dirwalk_paths)]->path = storage_test_dirwalk_files[i];
        data[i + COUNT_OF(storage_test_dirwalk_paths)]->is_dir = false;
        data[i + COUNT_OF(storage_test_dirwalk_paths)]->visited = false;
    }

    return data;
}

static void storage_test_paths_free(StorageTestPath** data) {
    size_t path_count =
        COUNT_OF(storage_test_dirwalk_paths) + COUNT_OF(storage_test_dirwalk_files);

    for(size_t i = 0; i < path_count; i++) {
        free(data[i]);
    }

    free(data);
}

static bool storage_test_paths_mark(StorageTestPath** data, const char* path, bool is_dir) {
    size_t path_count =
        COUNT_OF(storage_test_dirwalk_paths) + COUNT_OF(storage_test_dirwalk_files);
    bool found = false;

    for(size_t i = 0; i < path_count; i++) {
        if(strcmp(path, data[i]->path) == 0) {
            if(is_dir == data[i]->is_dir) {
                if(data[i]->visited == false) {
                    data[i]->visited = true;
                    found = true;
                    break;
                }
            }
        }
    }

    return found;
}

static bool storage_test_paths_check(StorageTestPath** data) {
    size_t path_count =
        COUNT_OF(storage_test_dirwalk_paths) + COUNT_OF(storage_test_dirwalk_files);
    bool error = false;

    for(size_t i = 0; i < path_count; i++) {
        if(data[i]->visited == false) {
            error = true;
            break;
        }
    }

    return error;
}

static bool write_file_13DA(Storage* storage, const char* path) {
    File* file = storage_file_alloc(storage);
    bool result = false;
    if(storage_file_open(file, path, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        result = storage_file_write(file, "13DA", 4) == 4;
    }
    storage_file_close(file);
    storage_file_free(file);

    return result;
}

static void storage_dirs_create(Storage* storage, const char* base) {
    string_t path;
    string_init(path);

    storage_common_mkdir(storage, base);

    for(size_t i = 0; i < COUNT_OF(storage_test_dirwalk_paths); i++) {
        string_printf(path, "%s/%s", base, storage_test_dirwalk_paths[i]);
        storage_common_mkdir(storage, string_get_cstr(path));
    }

    for(size_t i = 0; i < COUNT_OF(storage_test_dirwalk_files); i++) {
        string_printf(path, "%s/%s", base, storage_test_dirwalk_files[i]);
        write_file_13DA(storage, string_get_cstr(path));
    }

    string_clear(path);
}

MU_TEST_1(test_dirwalk, Storage* storage) {
    StorageTestPath** paths = storage_test_paths_alloc();

    DirWalk* dir_walk = dir_walk_alloc(storage);
    string_t path;
    string_init(path);
    FileInfo fileinfo;

    mu_check(dir_walk_open(dir_walk, "/ext/dirwalk"));
    while(dir_walk_read(dir_walk, path, &fileinfo) == DirWalkOK) {
        string_right(path, strlen("/ext/dirwalk/"));
        mu_check(storage_test_paths_mark(
            paths, string_get_cstr(path), (fileinfo.flags & FSF_DIRECTORY)));
    }

    dir_walk_free(dir_walk);
    string_clear(path);

    mu_check(storage_test_paths_check(paths) == false);

    storage_test_paths_free(paths);
}

MU_TEST_SUITE(test_dirwalk_suite) {
    Storage* storage = furi_record_open("storage");
    storage_dirs_create(storage, "/ext/dirwalk");

    MU_RUN_TEST_1(test_dirwalk, storage);

    storage_simply_remove_recursive(storage, "/ext/dirwalk");
    furi_record_close("storage");
}

int run_minunit_test_dirwalk() {
    MU_RUN_SUITE(test_dirwalk_suite);
    return MU_EXIT_CODE;
}