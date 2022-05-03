#pragma once
#include <storage/storage.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DirWalk DirWalk;

typedef enum {
    DirWalkOK, /**< OK */
    DirWalkError, /**< Error */
    DirWalkLast, /**< Last element */
} DirWalkResult;

typedef enum {
    DirWalkForward, /**< Forward direction (parent folder first) */
    DirWalkBackward, /**< Backward direction (parent folder last) */
} DirWalkMode;

/**
 * Allocate DirWalk
 * @param storage 
 * @return DirWalk* 
 */
DirWalk* dir_walk_alloc(Storage* storage);

/**
 * Free DirWalk
 * @param dir_walk 
 */
void dir_walk_free(DirWalk* dir_walk);

/**
 * Open directory 
 * @param dir_walk 
 * @param path 
 * @param mode 
 * @return true 
 * @return false 
 */
bool dir_walk_open(DirWalk* dir_walk, const char* path, DirWalkMode mode);

/**
 * Get error id
 * @param dir_walk 
 * @return FS_Error 
 */
FS_Error dir_walk_get_error(DirWalk* dir_walk);

/**
 * Read next element from directory
 * @param dir_walk 
 * @param return_path 
 * @param fileinfo 
 * @return DirWalkResult 
 */
DirWalkResult dir_walk_read(DirWalk* dir_walk, string_t return_path, FileInfo* fileinfo);

/**
 * Close directory
 * @param dir_walk 
 */
void dir_walk_close(DirWalk* dir_walk);

#ifdef __cplusplus
}
#endif