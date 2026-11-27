#pragma once

#include "ducky_script.h"
#include <storage/storage.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DUCKY3_MAX_BLOCKS       (8U)
#define DUCKY3_MAX_VARIABLES    (8U)
#define DUCKY3_MAX_VAR_NAME     (16U)
#define DUCKY3_MAX_STRING_VALUE (48U)

typedef enum {
    Ducky3BlockIf,
    Ducky3BlockWhile,
} Ducky3BlockType;

typedef struct {
    Ducky3BlockType type;
    size_t start_line;
    size_t else_line;
    size_t end_line;
} Ducky3Block;

typedef struct {
    bool is_string;
    int32_t number;
    char string[DUCKY3_MAX_STRING_VALUE];
} Ducky3Value;

typedef struct {
    char name[DUCKY3_MAX_VAR_NAME];
    Ducky3Value value;
} Ducky3Variable;

typedef struct {
    Ducky3Block blocks[DUCKY3_MAX_BLOCKS];
    size_t block_count;
    Ducky3Variable variables[DUCKY3_MAX_VARIABLES];
    size_t variable_count;
    size_t jump_line;
} Ducky3Context;

bool ducky3_preload(BadUsbScript* bad_usb, File* script_file);
void ducky3_reset(BadUsbScript* bad_usb);
int32_t ducky3_parse_line(BadUsbScript* bad_usb, const char* line);
bool ducky3_eval_expression(BadUsbScript* bad_usb, const char* expression, Ducky3Value* result);

#ifdef __cplusplus
}
#endif
