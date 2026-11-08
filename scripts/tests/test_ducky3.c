#include "ducky_script_3.h"
#include "ducky_script_i.h"
#include <toolbox/stream/stream.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    const char* text;
    size_t pos;
} MockStream;

struct FuriString {
    char data[256];
};

static char printed[8][64];
static size_t printed_count;

FuriString* furi_string_alloc(void) {
    return calloc(1, sizeof(FuriString));
}
void furi_string_free(FuriString* str) {
    free(str);
}
void furi_string_reset(FuriString* str) {
    str->data[0] = '\0';
}
void furi_string_set_str(FuriString* str, const char* value) {
    snprintf(str->data, sizeof(str->data), "%s", value);
}
const char* furi_string_get_cstr(const FuriString* str) {
    return str->data;
}
size_t furi_string_size(const FuriString* str) {
    return strlen(str->data);
}
void furi_string_cat_str(FuriString* str, const char* value) {
    size_t len = strlen(str->data);
    snprintf(str->data + len, sizeof(str->data) - len, "%s", value);
}
#undef furi_string_trim
void furi_string_trim(FuriString* str, const char chars[]) {
    (void)chars;
    size_t start = 0;
    while(str->data[start] == ' ' || str->data[start] == '\t' || str->data[start] == '\r' ||
          str->data[start] == '\n')
        start++;
    if(start) memmove(str->data, str->data + start, strlen(str->data + start) + 1);
    size_t len = strlen(str->data);
    while(len > 0 && (str->data[len - 1] == ' ' || str->data[len - 1] == '\t' ||
                      str->data[len - 1] == '\r' || str->data[len - 1] == '\n'))
        str->data[--len] = '\0';
}

bool storage_file_seek(File* file, uint32_t offset, bool from_start) {
    MockStream* stream = (MockStream*)file;
    stream->pos = from_start ? offset : stream->pos + offset;
    return true;
}

bool stream_read_line(Stream* stream, FuriString* result) {
    MockStream* mock = (MockStream*)stream;
    if(mock->text[mock->pos] == '\0') return false;
    size_t out = 0;
    while(mock->text[mock->pos] && mock->text[mock->pos] != '\n') {
        result->data[out++] = mock->text[mock->pos++];
    }
    result->data[out] = '\0';
    if(mock->text[mock->pos] == '\n') mock->pos++;
    return true;
}

int32_t ducky_error(BadUsbScript* bad_usb, const char* text, ...) {
    va_list args;
    va_start(args, text);
    vsnprintf(bad_usb->st.error, sizeof(bad_usb->st.error), text, args);
    va_end(args);
    return SCRIPT_STATE_ERROR;
}

bool ducky_string(BadUsbScript* bad_usb, const char* param) {
    (void)bad_usb;
    snprintf(printed[printed_count++], sizeof(printed[0]), "%s", param);
    return true;
}

static int32_t run_line(BadUsbScript* script, size_t index, const char* line) {
    script->st.line_cur = index + 1;
    return ducky3_parse_line(script, line);
}

static void test_expressions(BadUsbScript* script) {
    Ducky3Value value;
    assert(run_line(script, 0, "VAR $x = 2 + 3 * 4") == 0);
    assert(script->ducky3.variables[0].value.number == 14);
    assert(run_line(script, 1, "VAR $y = $x + 2") == 0);
    assert(run_line(script, 2, "VAR $s = \"ab\" + \"cd\"") == 0);
    assert(ducky3_eval_expression(script, "$x >= 14 && $y == 16", &value));
    assert(value.number == 1);
    assert(ducky3_eval_expression(script, "$s == \"abcd\"", &value));
    assert(value.number == 1);
    assert(!ducky3_eval_expression(script, "1 / 0", &value));
    assert(run_line(script, 3, "STRING hello") == SCRIPT_STATE_CMD_UNKNOWN);
    assert(run_line(script, 4, "STRING (hello)") == SCRIPT_STATE_CMD_UNKNOWN);
    assert(run_line(script, 5, "REM comment") == SCRIPT_STATE_CMD_UNKNOWN);
}

static void test_control_flow(BadUsbScript* script) {
    static const char* lines[] = {
        "VAR $x = 0",
        "WHILE $x < 3",
        "VAR $x = $x + 1",
        "IF $x == 2",
        "STRINGLN \"two\"",
        "ELSE",
        "STRINGLN $x",
        "ENDIF",
        "ENDWHILE",
    };
    MockStream stream = {.text =
                             "VAR $x = 0\nWHILE $x < 3\nVAR $x = $x + 1\n"
                             "IF $x == 2\nSTRINGLN \"two\"\nELSE\nSTRINGLN $x\n"
                             "ENDIF\nENDWHILE\n"};
    script->st.line_nb = 9;
    assert(ducky3_preload(script, (File*)&stream));
    assert(script->ducky3.block_count == 2);

    assert(run_line(script, 0, lines[0]) == 0);
    assert(run_line(script, 1, lines[1]) == 0);
    assert(run_line(script, 2, lines[2]) == 0);
    assert(run_line(script, 3, lines[3]) == SCRIPT_STATE_JUMP);
    assert(script->ducky3.jump_line == 6);
    assert(run_line(script, 6, lines[6]) == 0);
    assert(run_line(script, 7, lines[7]) == 0);
    assert(run_line(script, 8, lines[8]) == SCRIPT_STATE_JUMP);
    assert(script->ducky3.jump_line == 1);

    assert(run_line(script, 1, lines[1]) == 0);
    assert(run_line(script, 2, lines[2]) == 0);
    assert(run_line(script, 3, lines[3]) == 0);
    assert(run_line(script, 4, lines[4]) == 0);
    assert(run_line(script, 5, lines[5]) == SCRIPT_STATE_JUMP);
    assert(run_line(script, 8, lines[8]) == SCRIPT_STATE_JUMP);

    assert(run_line(script, 1, lines[1]) == 0);
    assert(run_line(script, 2, lines[2]) == 0);
    assert(run_line(script, 3, lines[3]) == SCRIPT_STATE_JUMP);
    assert(run_line(script, 6, lines[6]) == 0);
    assert(run_line(script, 7, lines[7]) == 0);
    assert(run_line(script, 8, lines[8]) == SCRIPT_STATE_JUMP);
    assert(run_line(script, 1, lines[1]) == SCRIPT_STATE_END);

    assert(printed_count == 3);
    assert(strcmp(printed[0], "1\n") == 0);
    assert(strcmp(printed[1], "two\n") == 0);
    assert(strcmp(printed[2], "3\n") == 0);
}

static void test_invalid_nesting(BadUsbScript* script) {
    MockStream stream = {.text = "IF 1\nELSE\nENDWHILE\n"};
    script->st.line_nb = 3;
    assert(!ducky3_preload(script, (File*)&stream));
}

int main(void) {
    BadUsbScript* script = calloc(1, sizeof(BadUsbScript));
    script->string_print = furi_string_alloc();
    test_expressions(script);
    test_control_flow(script);
    test_invalid_nesting(script);
    furi_string_free(script->string_print);
    free(script);
    puts("Ducky 3.0 Stage 1 regression tests passed");
    return 0;
}
