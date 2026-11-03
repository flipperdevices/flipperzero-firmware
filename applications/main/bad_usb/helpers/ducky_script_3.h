#pragma once

#include "ducky_script.h"
#include <storage/storage.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DUCKY3_MAX_BLOCKS       (8U)
#define DUCKY3_MAX_VARIABLES    (8U)
#define DUCKY3_MAX_VAR_NAME     (32U)
#define DUCKY3_MAX_STRING_VALUE (48U)
#define DUCKY3_MAX_FUNCTIONS    (4U)
#define DUCKY3_MAX_CALL_DEPTH   (4U)
#define DUCKY3_MAX_EXPRESSION   (64U)
#define DUCKY3_MAX_DEFINES      (4U)

typedef enum {
    Ducky3BlockIf,
    Ducky3BlockWhile,
} Ducky3BlockType;

typedef struct {
    Ducky3BlockType type;
    size_t start_line;
    size_t else_line;
    size_t end_line;
    size_t parent_line;
    uint8_t condition;
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
    char name[DUCKY3_MAX_VAR_NAME];
    size_t start_line;
    size_t end_line;
} Ducky3Function;

typedef struct {
    char name[DUCKY3_MAX_VAR_NAME];
    size_t return_line;
    char expression[DUCKY3_MAX_EXPRESSION];
    Ducky3Value return_value;
} Ducky3CallFrame;

typedef struct {
    Ducky3Block blocks[DUCKY3_MAX_BLOCKS];
    size_t block_count;
    Ducky3Variable variables[DUCKY3_MAX_VARIABLES];
    size_t variable_count;
    Ducky3Variable defines[DUCKY3_MAX_DEFINES];
    size_t define_count;
    uint16_t random_min;
    uint16_t random_max;
    uint8_t attack_mode;
    uint8_t saved_attack_mode;
    BadUsbHidConfig saved_hid_config;
    bool has_saved_attack_mode;
    size_t jump_line;
    Ducky3Function functions[DUCKY3_MAX_FUNCTIONS];
    size_t function_count;
    Ducky3CallFrame call_stack[DUCKY3_MAX_CALL_DEPTH];
    size_t call_depth;
    bool has_deferred;
    bool has_return_value;
    char deferred_function[DUCKY3_MAX_VAR_NAME];
    char deferred_expression[DUCKY3_MAX_EXPRESSION];
    Ducky3Value deferred_value;
} Ducky3Context;

bool ducky3_preload(BadUsbScript* bad_usb, File* script_file);
void ducky3_reset(BadUsbScript* bad_usb);
int32_t ducky3_parse_line(BadUsbScript* bad_usb, const char* line);
bool ducky3_eval_expression(BadUsbScript* bad_usb, const char* expression, Ducky3Value* result);

#ifdef __cplusplus
}
#endif
