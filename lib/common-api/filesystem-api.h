#pragma once
#include "flipper.h"
#include "flipper_v2.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Access mode flags
 */
typedef enum {
    FSAM_READ = (1 << 0), /**< Read access */
    FSAM_WRITE = (1 << 1), /**< Write access */
} FS_AccessMode;

/**
 * Open mode flags
 */
typedef enum {
    FSOM_OPEN_EXISTING = 1, /**< Open file, fail if file doesn't exist */
    FSOM_OPEN_ALWAYS = 2, /**< Open file. Create new file if not exist */
    FSOM_OPEN_APPEND = 4, /**< Open file. Create new file if not exist. Set R/W pointer to EOF */
    FSOM_CREATE_NEW = 8, /**< Creates a new file. Fails if the file is exist */
    FSOM_CREATE_ALWAYS = 16, /**< Creates a new file. If file exist, truncate to zero size */
} FS_OpenMode;

/**
 * API errors enumeration
 */
typedef enum {
    FSE_OK, /**< No error */
    FSE_NOT_READY, /**< FS not ready */
    FSE_EXIST, /**< File/Dir alrady exist */
    FSE_NOT_EXIST, /**< File/Dir does not exist */
    FSE_INVALID_PARAMETER, /**< Invalid API parameter */
    FSE_DENIED, /**< Access denied */
    FSE_INVALID_NAME, /**< Invalid name/path */
    FSE_INTERNAL, /**< Internal error */
    FSE_NOT_IMPLEMENTED, /**< Functon not implemented */
} FS_Error;

/**
 * FileInfo flags
 */
typedef enum {
    FSF_READ_ONLY = (1 << 0), /**< Readonly */
    FSF_HIDDEN = (1 << 1), /**< Hidden */
    FSF_SYSTEM = (1 << 2), /**< System */
    FSF_DIRECTORY = (1 << 3), /**< Directory */
    FSF_ARCHIVE = (1 << 4), /**< Archive */
} FS_Flags;

/** 
 *  Structure that hold file index and returned api errors 
 */
typedef struct {
    uint32_t file_id; /**< File ID for internal references */
    FS_Error error_id; /**< Standart API error from FS_Error enum */
    uint32_t internal_error_id; /**< Internal API error value */
} File;

// TODO: solve year 2107 problem
/** 
 *  Structure that hold packed date values 
 */
typedef struct __attribute__((packed)) {
    uint16_t month_day : 5; /**< month day */
    uint16_t month : 4; /**< month index */
    uint16_t year : 7; /**< year, year + 1980 to get actual value */
} FileDate;

/** 
 *  Structure that hold packed time values 
 */
typedef struct __attribute__((packed)) {
    uint16_t second : 5; /**< second, second * 2 to get actual value  */
    uint16_t minute : 6; /**< minute */
    uint16_t hour : 5; /**< hour */
} FileTime;

/** 
 *  Union of simple date and real value 
 */
typedef union {
    FileDate simple; /**< simple access to date */
    uint16_t value; /**< real date value */
} FileDateUnion;

/** 
 *  Union of simple time and real value 
 */
typedef union {
    FileTime simple; /**< simple access to time */
    uint16_t value; /**< real time value */
} FileTimeUnion;

/** 
 *  Structure that hold file info
 */
typedef struct {
    uint8_t flags; /**< flags from FS_Flags enum */
    uint64_t size; /**< file size */
    FileDateUnion date; /**< file date */
    FileTimeUnion time; /**< file time */
} FileInfo;

/* File api */
typedef struct {
    bool (*open)(
        File* file,
        const char* path,
        FS_AccessMode access_mode,
        FS_OpenMode open_mode);
    bool (*close)(File* file);
    uint16_t (*read)(File* file, void* buff, uint16_t bytes_to_read);
    uint16_t (*write)(File* file, void* buff, uint16_t bytes_to_write);
    bool (*seek)(File* file, uint32_t offset, bool from_start);
    uint64_t (*tell)(File* file);
    bool (*truncate)(File* file);
    uint64_t (*size)(File* file);
    bool (*sync)(File* file);
    bool (*eof)(File* file);
} FS_File_Api;

/* Dir api */
typedef struct {
    bool (*open)(File* file, const char* path);
    bool (*close)(File* file);
    bool (*read)(File* file, FileInfo* fileinfo, char* name, uint16_t name_length);
    bool (*rewind)(File* file);
} FS_Dir_Api;

/* Common api */
typedef struct {
    FS_Error (*info)(const char* path, FileInfo* fileinfo, char* name, const uint16_t name_length);
    FS_Error (*remove)(const char* path);
    FS_Error (*rename)(const char* old_path, const char* new_path);
    FS_Error (*set_attr)(const char* path, uint8_t attr, uint8_t mask);
    FS_Error (*mkdir)(const char* path);
    FS_Error (*set_time)(const char* path, FileDateUnion date, FileTimeUnion time);
    FS_Error (*get_fs_info)(uint64_t* total_space, uint64_t* free_space);
} FS_Common_Api;

/* Errors api */
typedef struct {
    const char* (*get_desc)(FS_Error error_id);
    const char* (*get_internal_desc)(uint32_t internal_error_id);
} FS_Error_Api;

/* full api set */
typedef struct {
    FS_File_Api file;
    FS_Dir_Api dir;
    FS_Common_Api common;
    FS_Error_Api error;
} FS_Api;

#ifdef __cplusplus
}
#endif