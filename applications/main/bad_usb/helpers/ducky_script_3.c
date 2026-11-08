#include "ducky_script_3.h"
#include "ducky_script_i.h"

#include <ctype.h>
#include <stdlib.h>
#include <toolbox/stream/stream.h>

#define DUCKY3_NO_LINE ((size_t)-1)

typedef struct {
    BadUsbScript* bad_usb;
    const char* input;
    bool error;
} Ducky3ExpressionParser;

static const char* ducky3_skip_spaces(const char* input) {
    while(*input == ' ' || *input == '\t') input++;
    return input;
}

static void ducky3_copy_string(char* destination, size_t destination_size, const char* source) {
    if(destination_size == 0) return;
    size_t len = strlen(source);
    if(len >= destination_size) len = destination_size - 1;
    memcpy(destination, source, len);
    destination[len] = '\0';
}

static bool ducky3_is_keyword(const char* line, const char* keyword) {
    size_t len = strlen(keyword);
    return strncmp(line, keyword, len) == 0 &&
           (line[len] == '\0' || line[len] == ' ' || line[len] == '\t');
}

static void ducky3_value_number(Ducky3Value* value, int32_t number) {
    value->is_string = false;
    value->number = number;
    value->string[0] = '\0';
}

static void ducky3_value_string(Ducky3Value* value, const char* string) {
    value->is_string = true;
    value->number = 0;
    ducky3_copy_string(value->string, DUCKY3_MAX_STRING_VALUE, string);
}

static bool ducky3_value_truthy(const Ducky3Value* value) {
    return value->is_string ? value->string[0] != '\0' : value->number != 0;
}

static void ducky3_value_to_string(const Ducky3Value* value, char* output, size_t output_size) {
    if(value->is_string) {
        ducky3_copy_string(output, output_size, value->string);
    } else {
        snprintf(output, output_size, "%ld", (long)value->number);
    }
}

static Ducky3Variable* ducky3_find_variable(BadUsbScript* bad_usb, const char* name) {
    for(size_t i = 0; i < bad_usb->ducky3.variable_count; i++) {
        if(strcmp(bad_usb->ducky3.variables[i].name, name) == 0) {
            return &bad_usb->ducky3.variables[i];
        }
    }
    return NULL;
}

static bool ducky3_set_variable(BadUsbScript* bad_usb, const char* name, const Ducky3Value* value) {
    Ducky3Variable* variable = ducky3_find_variable(bad_usb, name);
    if(!variable) {
        if(bad_usb->ducky3.variable_count >= DUCKY3_MAX_VARIABLES) return false;
        variable = &bad_usb->ducky3.variables[bad_usb->ducky3.variable_count++];
        ducky3_copy_string(variable->name, DUCKY3_MAX_VAR_NAME, name);
    }
    variable->value = *value;
    return true;
}

static bool ducky3_parse_variable_name(const char** input, char* name, size_t name_size) {
    const char* start = *input;
    if(*start != '$' && !isalpha((unsigned char)*start) && *start != '_') return false;
    if(*start == '$') start++;

    size_t len = 0;
    while(isalnum((unsigned char)start[len]) || start[len] == '_') {
        if(len + 1 >= name_size) return false;
        name[len] = start[len];
        len++;
    }
    if(len == 0) return false;
    name[len] = '\0';
    *input = start + len;
    return true;
}

static Ducky3Value ducky3_parse_expression(Ducky3ExpressionParser* parser);

static Ducky3Value ducky3_parse_primary(Ducky3ExpressionParser* parser) {
    Ducky3Value result;
    ducky3_value_number(&result, 0);
    parser->input = ducky3_skip_spaces(parser->input);

    if(*parser->input == '(') {
        parser->input++;
        result = ducky3_parse_expression(parser);
        parser->input = ducky3_skip_spaces(parser->input);
        if(*parser->input != ')') {
            parser->error = true;
        } else {
            parser->input++;
        }
        return result;
    }

    if(*parser->input == '"' || *parser->input == '\'') {
        char quote = *parser->input++;
        char output[DUCKY3_MAX_STRING_VALUE];
        size_t pos = 0;
        while(*parser->input && *parser->input != quote) {
            char chr = *parser->input++;
            if(chr == '\\') {
                chr = *parser->input++;
                if(chr == 'n')
                    chr = '\n';
                else if(chr == 't')
                    chr = '\t';
                else if(chr == '\0') {
                    parser->error = true;
                    break;
                }
            }
            if(pos + 1 >= sizeof(output)) {
                parser->error = true;
                break;
            }
            output[pos++] = chr;
        }
        if(*parser->input != quote) {
            parser->error = true;
        } else {
            parser->input++;
        }
        output[pos] = '\0';
        ducky3_value_string(&result, output);
        return result;
    }

    if(isdigit((unsigned char)*parser->input)) {
        char* end = NULL;
        long number = strtol(parser->input, &end, 0);
        if(end == parser->input) {
            parser->error = true;
        } else {
            parser->input = end;
            ducky3_value_number(&result, (int32_t)number);
        }
        return result;
    }

    char name[DUCKY3_MAX_VAR_NAME];
    if(ducky3_parse_variable_name(&parser->input, name, sizeof(name))) {
        Ducky3Variable* variable = ducky3_find_variable(parser->bad_usb, name);
        if(!variable) {
            parser->error = true;
        } else {
            result = variable->value;
        }
        return result;
    }

    parser->error = true;
    return result;
}

static Ducky3Value ducky3_parse_unary(Ducky3ExpressionParser* parser) {
    parser->input = ducky3_skip_spaces(parser->input);
    if(*parser->input == '!') {
        parser->input++;
        Ducky3Value value = ducky3_parse_unary(parser);
        ducky3_value_number(&value, !ducky3_value_truthy(&value));
        return value;
    }
    if(*parser->input == '-') {
        parser->input++;
        Ducky3Value value = ducky3_parse_unary(parser);
        if(value.is_string) {
            parser->error = true;
        } else {
            value.number = -value.number;
        }
        return value;
    }
    if(*parser->input == '+') {
        parser->input++;
        return ducky3_parse_unary(parser);
    }
    return ducky3_parse_primary(parser);
}

static Ducky3Value ducky3_parse_multiplicative(Ducky3ExpressionParser* parser) {
    Ducky3Value left = ducky3_parse_unary(parser);
    while(!parser->error) {
        parser->input = ducky3_skip_spaces(parser->input);
        char operation = *parser->input;
        if(operation != '*' && operation != '/' && operation != '%') break;
        parser->input++;
        Ducky3Value right = ducky3_parse_unary(parser);
        if(left.is_string || right.is_string) {
            parser->error = true;
            break;
        }
        if((operation == '/' || operation == '%') && right.number == 0) {
            parser->error = true;
            break;
        }
        if(operation == '*')
            left.number *= right.number;
        else if(operation == '/')
            left.number /= right.number;
        else
            left.number %= right.number;
    }
    return left;
}

static Ducky3Value ducky3_parse_additive(Ducky3ExpressionParser* parser) {
    Ducky3Value left = ducky3_parse_multiplicative(parser);
    while(!parser->error) {
        parser->input = ducky3_skip_spaces(parser->input);
        char operation = *parser->input;
        if(operation != '+' && operation != '-') break;
        parser->input++;
        Ducky3Value right = ducky3_parse_multiplicative(parser);

        if(operation == '+' && (left.is_string || right.is_string)) {
            char left_string[DUCKY3_MAX_STRING_VALUE];
            char right_string[DUCKY3_MAX_STRING_VALUE];
            char output[DUCKY3_MAX_STRING_VALUE];
            ducky3_value_to_string(&left, left_string, sizeof(left_string));
            ducky3_value_to_string(&right, right_string, sizeof(right_string));
            if(snprintf(output, sizeof(output), "%s%s", left_string, right_string) >=
               (int)sizeof(output)) {
                parser->error = true;
            } else {
                ducky3_value_string(&left, output);
            }
        } else if(left.is_string || right.is_string) {
            parser->error = true;
        } else if(operation == '+') {
            left.number += right.number;
        } else {
            left.number -= right.number;
        }
    }
    return left;
}

static bool ducky3_match_operator(Ducky3ExpressionParser* parser, const char* operation) {
    parser->input = ducky3_skip_spaces(parser->input);
    size_t len = strlen(operation);
    if(strncmp(parser->input, operation, len) != 0) return false;
    parser->input += len;
    return true;
}

static Ducky3Value ducky3_parse_relational(Ducky3ExpressionParser* parser) {
    Ducky3Value left = ducky3_parse_additive(parser);
    while(!parser->error) {
        const char* operation = NULL;
        if(ducky3_match_operator(parser, "<="))
            operation = "<=";
        else if(ducky3_match_operator(parser, ">="))
            operation = ">=";
        else if(ducky3_match_operator(parser, "<"))
            operation = "<";
        else if(ducky3_match_operator(parser, ">"))
            operation = ">";
        else
            break;

        Ducky3Value right = ducky3_parse_additive(parser);
        if(left.is_string || right.is_string) {
            parser->error = true;
            break;
        }
        if(strcmp(operation, "<=") == 0)
            left.number = left.number <= right.number;
        else if(strcmp(operation, ">=") == 0)
            left.number = left.number >= right.number;
        else if(strcmp(operation, "<") == 0)
            left.number = left.number < right.number;
        else
            left.number = left.number > right.number;
    }
    return left;
}

static Ducky3Value ducky3_parse_equality(Ducky3ExpressionParser* parser) {
    Ducky3Value left = ducky3_parse_relational(parser);
    while(!parser->error) {
        bool negate = false;
        if(ducky3_match_operator(parser, "==")) {
            negate = false;
        } else if(ducky3_match_operator(parser, "!=")) {
            negate = true;
        } else {
            break;
        }

        Ducky3Value right = ducky3_parse_relational(parser);
        bool equal = false;
        if(left.is_string || right.is_string) {
            if(left.is_string && right.is_string) {
                equal = strcmp(left.string, right.string) == 0;
            }
        } else {
            equal = left.number == right.number;
        }
        ducky3_value_number(&left, negate ? !equal : equal);
    }
    return left;
}

static Ducky3Value ducky3_parse_and(Ducky3ExpressionParser* parser) {
    Ducky3Value left = ducky3_parse_equality(parser);
    while(!parser->error && ducky3_match_operator(parser, "&&")) {
        Ducky3Value right = ducky3_parse_equality(parser);
        ducky3_value_number(&left, ducky3_value_truthy(&left) && ducky3_value_truthy(&right));
    }
    return left;
}

static Ducky3Value ducky3_parse_expression(Ducky3ExpressionParser* parser) {
    Ducky3Value left = ducky3_parse_and(parser);
    while(!parser->error && ducky3_match_operator(parser, "||")) {
        Ducky3Value right = ducky3_parse_and(parser);
        ducky3_value_number(&left, ducky3_value_truthy(&left) || ducky3_value_truthy(&right));
    }
    return left;
}

bool ducky3_eval_expression(BadUsbScript* bad_usb, const char* expression, Ducky3Value* result) {
    Ducky3ExpressionParser parser = {
        .bad_usb = bad_usb,
        .input = expression,
        .error = false,
    };
    *result = ducky3_parse_expression(&parser);
    parser.input = ducky3_skip_spaces(parser.input);
    return !parser.error && *parser.input == '\0';
}

static int32_t ducky3_output_string(BadUsbScript* bad_usb, const char* expression, bool newline) {
    Ducky3Value value;
    if(!ducky3_eval_expression(bad_usb, expression, &value)) {
        return ducky_error(bad_usb, "Invalid expression %s", expression);
    }

    char output[DUCKY3_MAX_STRING_VALUE];
    ducky3_value_to_string(&value, output, sizeof(output));
    furi_string_set_str(bad_usb->string_print, output);
    if(newline) furi_string_cat(bad_usb->string_print, "\n");

    if(bad_usb->stringdelay == 0 && bad_usb->defstringdelay == 0) {
        ducky_string(bad_usb, furi_string_get_cstr(bad_usb->string_print));
        return 0;
    }
    return SCRIPT_STATE_STRING_START;
}

static Ducky3Block* ducky3_find_block(
    BadUsbScript* bad_usb,
    Ducky3BlockType type,
    size_t line,
    bool else_block) {
    for(size_t i = 0; i < bad_usb->ducky3.block_count; i++) {
        Ducky3Block* block = &bad_usb->ducky3.blocks[i];
        if(block->type != type) continue;
        if(else_block) {
            if(block->else_line == line) return block;
        } else if(block->start_line == line) {
            return block;
        }
    }
    return NULL;
}

static Ducky3Block* ducky3_find_end_block(
    BadUsbScript* bad_usb,
    Ducky3BlockType type,
    size_t line) {
    for(size_t i = 0; i < bad_usb->ducky3.block_count; i++) {
        Ducky3Block* block = &bad_usb->ducky3.blocks[i];
        if(block->type == type && block->end_line == line) return block;
    }
    return NULL;
}

static int32_t ducky3_jump(BadUsbScript* bad_usb, size_t target_line) {
    if(target_line >= bad_usb->st.line_nb) return SCRIPT_STATE_END;
    bad_usb->ducky3.jump_line = target_line;
    return SCRIPT_STATE_JUMP;
}

static int32_t ducky3_parse_var(BadUsbScript* bad_usb, const char* line) {
    const char* input = ducky3_skip_spaces(line + 3);
    char name[DUCKY3_MAX_VAR_NAME];
    if(!ducky3_parse_variable_name(&input, name, sizeof(name))) {
        return ducky_error(bad_usb, "Invalid variable declaration %s", line);
    }
    input = ducky3_skip_spaces(input);
    if(*input != '=') return ducky_error(bad_usb, "Expected '=' after %s", name);
    input++;

    Ducky3Value value;
    if(!ducky3_eval_expression(bad_usb, input, &value)) {
        return ducky_error(bad_usb, "Invalid expression %s", input);
    }
    if(!ducky3_set_variable(bad_usb, name, &value)) {
        return ducky_error(bad_usb, "Too many variables");
    }
    return 0;
}

int32_t ducky3_parse_line(BadUsbScript* bad_usb, const char* line) {
    size_t current_line = bad_usb->st.line_cur - 1;

    if(ducky3_is_keyword(line, "VAR")) return ducky3_parse_var(bad_usb, line);

    if(ducky3_is_keyword(line, "IF")) {
        Ducky3Block* block = ducky3_find_block(bad_usb, Ducky3BlockIf, current_line, false);
        if(!block) return ducky_error(bad_usb, "Unexpected IF");
        Ducky3Value condition;
        const char* expression = ducky3_skip_spaces(line + 2);
        if(!ducky3_eval_expression(bad_usb, expression, &condition)) {
            return ducky_error(bad_usb, "Invalid IF expression %s", expression);
        }
        if(ducky3_value_truthy(&condition)) return 0;
        if(block->else_line != DUCKY3_NO_LINE) return ducky3_jump(bad_usb, block->else_line + 1);
        return ducky3_jump(bad_usb, block->end_line + 1);
    }

    if(ducky3_is_keyword(line, "ELSE")) {
        Ducky3Block* block = ducky3_find_block(bad_usb, Ducky3BlockIf, current_line, true);
        if(!block) return ducky_error(bad_usb, "Unexpected ELSE");
        return ducky3_jump(bad_usb, block->end_line + 1);
    }

    if(ducky3_is_keyword(line, "ENDIF")) {
        if(!ducky3_find_end_block(bad_usb, Ducky3BlockIf, current_line)) {
            return ducky_error(bad_usb, "Unexpected ENDIF");
        }
        return 0;
    }

    if(ducky3_is_keyword(line, "WHILE")) {
        Ducky3Block* block = ducky3_find_block(bad_usb, Ducky3BlockWhile, current_line, false);
        if(!block) return ducky_error(bad_usb, "Unexpected WHILE");
        Ducky3Value condition;
        const char* expression = ducky3_skip_spaces(line + 5);
        if(!ducky3_eval_expression(bad_usb, expression, &condition)) {
            return ducky_error(bad_usb, "Invalid WHILE expression %s", expression);
        }
        if(ducky3_value_truthy(&condition)) return 0;
        return ducky3_jump(bad_usb, block->end_line + 1);
    }

    if(ducky3_is_keyword(line, "ENDWHILE")) {
        Ducky3Block* block = ducky3_find_end_block(bad_usb, Ducky3BlockWhile, current_line);
        if(!block) return ducky_error(bad_usb, "Unexpected ENDWHILE");
        return ducky3_jump(bad_usb, block->start_line);
    }

    bool stringln = ducky3_is_keyword(line, "STRINGLN");
    if(ducky3_is_keyword(line, "STRING") || stringln) {
        const char* expression = ducky3_skip_spaces(line + (stringln ? 8 : 6));
        if(*expression == '$' || *expression == '"' || *expression == '\'') {
            return ducky3_output_string(bad_usb, expression, stringln);
        }
    }

    return SCRIPT_STATE_CMD_UNKNOWN;
}

static bool ducky3_push_block(
    BadUsbScript* bad_usb,
    size_t* stack,
    size_t* depth,
    Ducky3BlockType type,
    size_t line) {
    if(*depth >= DUCKY3_MAX_BLOCKS || bad_usb->ducky3.block_count >= DUCKY3_MAX_BLOCKS) {
        return false;
    }
    Ducky3Block block = {
        .type = type,
        .start_line = line,
        .else_line = DUCKY3_NO_LINE,
        .end_line = DUCKY3_NO_LINE,
    };
    stack[(*depth)++] = bad_usb->ducky3.block_count;
    bad_usb->ducky3.blocks[bad_usb->ducky3.block_count++] = block;
    return true;
}

bool ducky3_preload(BadUsbScript* bad_usb, File* script_file) {
    FuriString* line = furi_string_alloc();
    size_t stack[DUCKY3_MAX_BLOCKS];
    size_t depth = 0;
    size_t current_line = 0;
    bool success = true;

    bad_usb->ducky3.block_count = 0;
    storage_file_seek(script_file, 0, true);
    while(stream_read_line((Stream*)script_file, line)) {
        furi_string_trim(line);
        const char* text = furi_string_get_cstr(line);

        if(ducky3_is_keyword(text, "IF")) {
            success = ducky3_push_block(bad_usb, stack, &depth, Ducky3BlockIf, current_line);
        } else if(ducky3_is_keyword(text, "ELSE")) {
            if(depth == 0) {
                success = false;
            } else {
                Ducky3Block* block = &bad_usb->ducky3.blocks[stack[depth - 1]];
                if(block->type != Ducky3BlockIf || block->else_line != DUCKY3_NO_LINE) {
                    success = false;
                } else {
                    block->else_line = current_line;
                }
            }
        } else if(ducky3_is_keyword(text, "ENDIF")) {
            if(depth == 0) {
                success = false;
            } else {
                Ducky3Block* block = &bad_usb->ducky3.blocks[stack[depth - 1]];
                if(block->type != Ducky3BlockIf) {
                    success = false;
                } else {
                    block->end_line = current_line;
                    depth--;
                }
            }
        } else if(ducky3_is_keyword(text, "WHILE")) {
            success = ducky3_push_block(bad_usb, stack, &depth, Ducky3BlockWhile, current_line);
        } else if(ducky3_is_keyword(text, "ENDWHILE")) {
            if(depth == 0) {
                success = false;
            } else {
                Ducky3Block* block = &bad_usb->ducky3.blocks[stack[depth - 1]];
                if(block->type != Ducky3BlockWhile) {
                    success = false;
                } else {
                    block->end_line = current_line;
                    depth--;
                }
            }
        }

        if(!success) {
            bad_usb->st.error_line = current_line + 1;
            break;
        }
        current_line++;
    }

    if(success && depth != 0) {
        bad_usb->st.error_line = current_line;
        success = false;
    }
    furi_string_free(line);
    storage_file_seek(script_file, 0, true);
    return success;
}

void ducky3_reset(BadUsbScript* bad_usb) {
    bad_usb->ducky3.variable_count = 0;
    bad_usb->ducky3.jump_line = 0;
}
