#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PathHelper PathHelper;

PathHelper* path_helper_alloc();

PathHelper* path_helper_alloc_apps_data();

void path_helper_free(PathHelper* path_helper);

void path_helper_append(PathHelper* path_helper, const char* path);

const char* path_helper_get(PathHelper* path_helper);

#ifdef __cplusplus
}
#endif