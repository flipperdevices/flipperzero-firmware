#include "ducky_script_3.h"
#include "ducky_script_i.h"

#include <ctype.h>
#include <stdlib.h>
#include <toolbox/stream/stream.h>

#define DUCKY3_NO_LINE ((size_t)-1)

typedef struct {
    BadUsbScript* bad_usb;
    const char* start;
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

static Ducky3Variable* ducky3_find_define(BadUsbScript* bad_usb, const char* name) {
    for(size_t i = 0; i < bad_usb->ducky3.define_count; i++) {
        if(strcmp(bad_usb->ducky3.defines[i].name, name) == 0) {
            return &bad_usb->ducky3.defines[i];
        }
    }
    return NULL;
}

static bool ducky3_add_define(BadUsbScript* bad_usb, const char* name, const Ducky3Value* value) {
    if(ducky3_find_define(bad_usb, name) || bad_usb->ducky3.define_count >= DUCKY3_MAX_DEFINES) {
        return false;
    }
    Ducky3Variable* define = &bad_usb->ducky3.defines[bad_usb->ducky3.define_count++];
    ducky3_copy_string(define->name, DUCKY3_MAX_VAR_NAME, name);
    define->value = *value;
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

static int32_t ducky3_jump(BadUsbScript* bad_usb, size_t target_line);

static Ducky3Function* ducky3_find_function(BadUsbScript* bad_usb, const char* name) {
    for(size_t i = 0; i < bad_usb->ducky3.function_count; i++) {
        if(strcmp(bad_usb->ducky3.functions[i].name, name) == 0) {
            return &bad_usb->ducky3.functions[i];
        }
    }
    return NULL;
}

static bool ducky3_begin_call(
    Ducky3ExpressionParser* parser,
    Ducky3Function* function,
    const char* name) {
    BadUsbScript* bad_usb = parser->bad_usb;
    if(bad_usb->ducky3.call_depth >= DUCKY3_MAX_CALL_DEPTH) {
        parser->error = true;
        return false;
    }

    Ducky3CallFrame* frame = &bad_usb->ducky3.call_stack[bad_usb->ducky3.call_depth++];
    ducky3_copy_string(frame->name, DUCKY3_MAX_VAR_NAME, name);
    frame->return_line = bad_usb->st.line_cur - 1;
    ducky3_copy_string(frame->expression, DUCKY3_MAX_EXPRESSION, parser->start);

    ducky3_copy_string(
        bad_usb->ducky3.deferred_function, DUCKY3_MAX_VAR_NAME, function->name);
    ducky3_copy_string(
        bad_usb->ducky3.deferred_expression, DUCKY3_MAX_EXPRESSION, parser->start);
    bad_usb->ducky3.has_deferred = true;
    bad_usb->ducky3.has_return_value = false;
    bad_usb->ducky3.jump_line = function->start_line + 1;
    return true;
}

static int32_t ducky3_return_value(BadUsbScript* bad_usb, Ducky3Value value) {
    if(bad_usb->ducky3.call_depth == 0) {
        return ducky_error(bad_usb, "RETURN outside a function");
    }

    Ducky3CallFrame* frame = &bad_usb->ducky3.call_stack[--bad_usb->ducky3.call_depth];
    bad_usb->ducky3.deferred_value = value;
    bad_usb->ducky3.has_return_value = true;
    bad_usb->ducky3.has_deferred = false;

    size_t return_line = frame->return_line;
    ducky3_copy_string(bad_usb->ducky3.deferred_function, DUCKY3_MAX_VAR_NAME, frame->name);
    ducky3_copy_string(
        bad_usb->ducky3.deferred_expression, DUCKY3_MAX_EXPRESSION, frame->expression);
    return ducky3_jump(bad_usb, return_line);
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
        parser->input = ducky3_skip_spaces(parser->input);

        if(strcmp(name, "RANDINT") == 0 && *parser->input == '(') {
            parser->input++;
            Ducky3Value lower = ducky3_parse_expression(parser);
            parser->input = ducky3_skip_spaces(parser->input);
            if(*parser->input != ',') {
                parser->error = true;
                return result;
            }
            parser->input++;
            Ducky3Value upper = ducky3_parse_expression(parser);
            parser->input = ducky3_skip_spaces(parser->input);
            if(*parser->input != ')') {
                parser->error = true;
                return result;
            }
            parser->input++;
            if(lower.is_string || upper.is_string) {
                parser->error = true;
                return result;
            }
            if(lower.number > upper.number) {
                Ducky3Value swap = lower;
                lower = upper;
                upper = swap;
            }
            uint32_t span = (uint32_t)upper.number - (uint32_t)lower.number + 1;
            int32_t random_value = lower.number;
            if(span != 0) random_value += (int32_t)(furi_hal_random_get() % span);
            ducky3_value_number(&result, random_value);
            return result;
        }

        if(strncmp(parser->input, "()", 2) == 0) {
            if(parser->bad_usb->ducky3.has_return_value &&
               strcmp(parser->bad_usb->ducky3.deferred_function, name) == 0 &&
               strcmp(
                   ducky3_skip_spaces(parser->bad_usb->ducky3.deferred_expression),
                   ducky3_skip_spaces(parser->start)) == 0) {
                result = parser->bad_usb->ducky3.deferred_value;
                parser->bad_usb->ducky3.has_return_value = false;
                parser->bad_usb->ducky3.has_deferred = false;
                parser->bad_usb->ducky3.deferred_function[0] = '\0';
                parser->bad_usb->ducky3.deferred_expression[0] = '\0';
                parser->input += 2;
                return result;
            }

            Ducky3Function* function = ducky3_find_function(parser->bad_usb, name);
            if(!function) {
                parser->error = true;
            } else {
                parser->input += 2;
                ducky3_begin_call(parser, function, name);
            }
            return result;
        }

        if(strcmp(name, "_RANDOM_INT") == 0) {
            uint32_t min = parser->bad_usb->ducky3.random_min;
            uint32_t max = parser->bad_usb->ducky3.random_max;
            uint32_t span = max >= min ? max - min + 1 : 1;
            ducky3_value_number(&result, (int32_t)(min + furi_hal_random_get() % span));
            return result;
        }
        if(strcmp(name, "_RANDOM_MIN") == 0) {
            ducky3_value_number(&result, parser->bad_usb->ducky3.random_min);
            return result;
        }
        if(strcmp(name, "_RANDOM_MAX") == 0) {
            ducky3_value_number(&result, parser->bad_usb->ducky3.random_max);
            return result;
        }
        if(strcmp(name, "_CURRENT_VID") == 0) {
            ducky3_value_number(&result, parser->bad_usb->hid_cfg->usb.vid);
            return result;
        }
        if(strcmp(name, "_CURRENT_PID") == 0) {
            ducky3_value_number(&result, parser->bad_usb->hid_cfg->usb.pid);
            return result;
        }
        if(strcmp(name, "_CURRENT_ATTACKMODE") == 0) {
            ducky3_value_number(&result, parser->bad_usb->ducky3.attack_mode);
            return result;
        }
        if(strcmp(name, "_EXFIL_MODE_ENABLED") == 0) {
            ducky3_value_number(&result, parser->bad_usb->ducky3.exfil_enabled);
            return result;
        }
        if(strcmp(name, "_EXFIL_LEDS_ENABLED") == 0) {
            ducky3_value_number(&result, parser->bad_usb->ducky3.exfil_leds_enabled);
            return result;
        }
        if(strcmp(name, "_SAVED_CAPSLOCK_ON") == 0 || strcmp(name, "_SAVED_NUMLOCK_ON") == 0 ||
           strcmp(name, "_SAVED_SCROLLLOCK_ON") == 0) {
            uint8_t mask = strcmp(name, "_SAVED_CAPSLOCK_ON") == 0 ? HID_KB_LED_CAPS :
                           strcmp(name, "_SAVED_NUMLOCK_ON") == 0  ? HID_KB_LED_NUM :
                                                                     HID_KB_LED_SCROLL;
            ducky3_value_number(&result, (parser->bad_usb->ducky3.saved_lock_state & mask) != 0);
            return result;
        }
        if(strcmp(name, "_CAPSLOCK_ON") == 0 || strcmp(name, "_NUMLOCK_ON") == 0 ||
           strcmp(name, "_SCROLLLOCK_ON") == 0) {
            uint8_t led_state = parser->bad_usb->hid->get_led_state(parser->bad_usb->hid_inst);
            uint8_t mask = strcmp(name, "_CAPSLOCK_ON") == 0 ? HID_KB_LED_CAPS :
                           strcmp(name, "_NUMLOCK_ON") == 0  ? HID_KB_LED_NUM :
                                                               HID_KB_LED_SCROLL;
            ducky3_value_number(&result, (led_state & mask) != 0);
            return result;
        }

        if(strcmp(name, "TRUE") == 0) {
            ducky3_value_number(&result, 1);
            return result;
        }
        if(strcmp(name, "FALSE") == 0) {
            ducky3_value_number(&result, 0);
            return result;
        }

        Ducky3Variable* define = ducky3_find_define(parser->bad_usb, name);
        if(define) {
            result = define->value;
            return result;
        }

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
        .start = expression,
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
    if(bad_usb->ducky3.has_deferred) return ducky3_jump(bad_usb, bad_usb->ducky3.jump_line);

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

    if(strcmp(name, "_EXFIL_LEDS_ENABLED") == 0) {
        Ducky3Value enabled;
        if(!ducky3_eval_expression(bad_usb, input, &enabled)) {
            return ducky_error(bad_usb, "Invalid EXFIL LED mode %s", input);
        }
        bad_usb->ducky3.exfil_leds_enabled = ducky3_value_truthy(&enabled);
        return 0;
    }

    if(strcmp(name, "_EXFIL_MODE_ENABLED") == 0) {
        Ducky3Value enabled;
        if(!ducky3_eval_expression(bad_usb, input, &enabled)) {
            return ducky_error(bad_usb, "Invalid exfil mode %s", input);
        }
        bad_usb->ducky3.exfil_enabled = ducky3_value_truthy(&enabled);
        return 0;
    }

    bool is_random_bound = strcmp(name, "_RANDOM_MIN") == 0 || strcmp(name, "_RANDOM_MAX") == 0;
    if(is_random_bound) {
        char* end = NULL;
        long bound = strtol(input, &end, 0);
        if(end == input || *end != '\0' || bound < 0 || bound > 65535) {
            return ducky_error(bad_usb, "Invalid random bound %s", input);
        }
        if(strcmp(name, "_RANDOM_MIN") == 0) {
            bad_usb->ducky3.random_min = (uint16_t)bound;
        } else {
            bad_usb->ducky3.random_max = (uint16_t)bound;
        }
        return 0;
    }

    Ducky3Value value;
    if(!ducky3_eval_expression(bad_usb, input, &value)) {
        return ducky_error(bad_usb, "Invalid expression %s", input);
    }
    if(bad_usb->ducky3.has_deferred) return ducky3_jump(bad_usb, bad_usb->ducky3.jump_line);
    if(!ducky3_set_variable(bad_usb, name, &value)) {
        return ducky_error(bad_usb, "Too many variables");
    }
    return 0;
}

static bool ducky3_if_chain_taken(BadUsbScript* bad_usb, Ducky3Block* block) {
    while(block) {
        if(block->condition == 1) return true;
        if(block->parent_line == DUCKY3_NO_LINE) return false;
        Ducky3Block* parent = NULL;
        for(size_t i = 0; i < bad_usb->ducky3.block_count; i++) {
            if(bad_usb->ducky3.blocks[i].start_line == block->parent_line) {
                parent = &bad_usb->ducky3.blocks[i];
                break;
            }
        }
        block = parent;
    }
    return false;
}

int32_t ducky3_parse_line(BadUsbScript* bad_usb, const char* line) {
    size_t current_line = bad_usb->st.line_cur - 1;
    if(bad_usb->ducky3.has_deferred && bad_usb->ducky3.call_depth > 0 &&
       current_line != bad_usb->ducky3.call_stack[bad_usb->ducky3.call_depth - 1].return_line) {
        bad_usb->ducky3.has_deferred = false;
    }

    if(ducky3_is_keyword(line, "DEFINE")) return 0;
    if(ducky3_is_keyword(line, "VAR")) return ducky3_parse_var(bad_usb, line);

    if(ducky3_is_keyword(line, "IF")) {
        Ducky3Block* block = ducky3_find_block(bad_usb, Ducky3BlockIf, current_line, false);
        if(!block) return ducky_error(bad_usb, "Unexpected IF");
        Ducky3Value condition;
        const char* expression = ducky3_skip_spaces(line + 2);
        if(!ducky3_eval_expression(bad_usb, expression, &condition)) {
            return ducky_error(bad_usb, "Invalid IF expression %s", expression);
        }
        if(bad_usb->ducky3.has_deferred) return ducky3_jump(bad_usb, bad_usb->ducky3.jump_line);
        block->condition = ducky3_value_truthy(&condition) ? 1 : 2;
        if(ducky3_value_truthy(&condition)) return 0;
        if(block->else_line != DUCKY3_NO_LINE) return ducky3_jump(bad_usb, block->else_line + 1);
        return ducky3_jump(bad_usb, block->end_line + 1);
    }

    if(ducky3_is_keyword(line, "ELSE IF")) {
        Ducky3Block* block = NULL;
        Ducky3Block* parent = NULL;
        for(size_t i = 0; i < bad_usb->ducky3.block_count; i++) {
            if(bad_usb->ducky3.blocks[i].start_line == current_line) {
                block = &bad_usb->ducky3.blocks[i];
            }
        }
        if(block) {
            for(size_t i = 0; i < bad_usb->ducky3.block_count; i++) {
                if(bad_usb->ducky3.blocks[i].start_line == block->parent_line) {
                    parent = &bad_usb->ducky3.blocks[i];
                    break;
                }
            }
        }
        if(!block || !parent) return ducky_error(bad_usb, "Unexpected ELSE IF");
        if(parent->condition == 1) return ducky3_jump(bad_usb, block->end_line + 1);

        Ducky3Value condition;
        const char* expression = ducky3_skip_spaces(line + 7);
        if(!ducky3_eval_expression(bad_usb, expression, &condition)) {
            return ducky_error(bad_usb, "Invalid ELSE IF expression %s", expression);
        }
        if(bad_usb->ducky3.has_deferred) return ducky3_jump(bad_usb, bad_usb->ducky3.jump_line);
        block->condition = ducky3_value_truthy(&condition) ? 1 : 2;
        if(ducky3_value_truthy(&condition)) return 0;
        if(block->else_line != DUCKY3_NO_LINE) return ducky3_jump(bad_usb, block->else_line + 1);
        return ducky3_jump(bad_usb, block->end_line + 1);
    }

    if(ducky3_is_keyword(line, "ELSE")) {
        Ducky3Block* block = ducky3_find_block(bad_usb, Ducky3BlockIf, current_line, true);
        if(!block) return ducky_error(bad_usb, "Unexpected ELSE");
        if(ducky3_if_chain_taken(bad_usb, block)) return ducky3_jump(bad_usb, block->end_line + 1);
        return 0;
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
        if(bad_usb->ducky3.has_deferred) return ducky3_jump(bad_usb, bad_usb->ducky3.jump_line);
        if(ducky3_value_truthy(&condition)) return 0;
        return ducky3_jump(bad_usb, block->end_line + 1);
    }

    if(ducky3_is_keyword(line, "ENDWHILE")) {
        Ducky3Block* block = ducky3_find_end_block(bad_usb, Ducky3BlockWhile, current_line);
        if(!block) return ducky_error(bad_usb, "Unexpected ENDWHILE");
        return ducky3_jump(bad_usb, block->start_line);
    }

    if(ducky3_is_keyword(line, "FUNCTION")) {
        Ducky3Function* function = NULL;
        for(size_t i = 0; i < bad_usb->ducky3.function_count; i++) {
            if(bad_usb->ducky3.functions[i].start_line == current_line) {
                function = &bad_usb->ducky3.functions[i];
                break;
            }
        }
        if(!function) return ducky_error(bad_usb, "Unexpected FUNCTION");
        return ducky3_jump(bad_usb, function->end_line + 1);
    }

    if(ducky3_is_keyword(line, "END_FUNCTION")) {
        Ducky3Value value;
        ducky3_value_number(&value, 0);
        return ducky3_return_value(bad_usb, value);
    }

    if(ducky3_is_keyword(line, "RETURN")) {
        Ducky3Value value;
        const char* expression = ducky3_skip_spaces(line + 6);
        if(*expression == '\0') {
            ducky3_value_number(&value, 0);
        } else if(!ducky3_eval_expression(bad_usb, expression, &value)) {
            return ducky_error(bad_usb, "Invalid RETURN expression %s", expression);
        }
        if(bad_usb->ducky3.has_deferred) return ducky3_jump(bad_usb, bad_usb->ducky3.jump_line);
        return ducky3_return_value(bad_usb, value);
    }

    if(ducky3_is_keyword(line, "EXFIL")) {
        const char* expression = ducky3_skip_spaces(line + 5);
        Ducky3Value value;
        if(!ducky3_eval_expression(bad_usb, expression, &value)) {
            return ducky_error(bad_usb, "Invalid EXFIL expression %s", expression);
        }
        if(bad_usb->ducky3.has_deferred) return ducky3_jump(bad_usb, bad_usb->ducky3.jump_line);

        File* loot = storage_file_alloc(furi_record_open(RECORD_STORAGE));
        if(!storage_file_open(loot, "/ext/loot.bin", FSAM_WRITE, FSOM_OPEN_APPEND)) {
            storage_file_free(loot);
            return ducky_error(bad_usb, "Unable to open loot.bin");
        }
        size_t written = 0;
        if(value.is_string) {
            written = storage_file_write(loot, (const uint8_t*)value.string, strlen(value.string));
        } else {
            uint32_t number = (uint32_t)value.number;
            written = storage_file_write(loot, (const uint8_t*)&number, sizeof(number));
        }
        storage_file_close(loot);
        storage_file_free(loot);
        if(written == 0) return ducky_error(bad_usb, "Unable to write loot.bin");
        return 0;
    }

    if(ducky3_is_keyword(line, "SAVE_HOST_KEYBOARD_LOCK_STATE")) {
        bad_usb->ducky3.saved_lock_state = bad_usb->hid->get_led_state(bad_usb->hid_inst);
        return 0;
    }

    if(ducky3_is_keyword(line, "RESTORE_HOST_KEYBOARD_LOCK_STATE")) {
        return ducky_error(bad_usb, "LED state restoration is not supported");
    }

    if(ducky3_is_keyword(line, "WAIT_FOR_SCROLL_CHANGE")) {
        const uint8_t initial_state = bad_usb->hid->get_led_state(bad_usb->hid_inst);
        while(bad_usb->hid->get_led_state(bad_usb->hid_inst) == initial_state) {
            furi_delay_ms(10);
        }
        return 0;
    }

    if(ducky3_is_keyword(line, "SAVE_ATTACKMODE")) {
        bad_usb->ducky3.saved_attack_mode = bad_usb->ducky3.attack_mode;
        bad_usb->ducky3.saved_hid_config = *bad_usb->hid_cfg;
        bad_usb->ducky3.has_saved_attack_mode = true;
        return 0;
    }

    if(ducky3_is_keyword(line, "RESTORE_ATTACKMODE")) {
        if(!bad_usb->ducky3.has_saved_attack_mode) {
            return ducky_error(bad_usb, "No saved ATTACKMODE");
        }
        bad_usb->ducky3.attack_mode = bad_usb->ducky3.saved_attack_mode;
        *bad_usb->hid_cfg = bad_usb->ducky3.saved_hid_config;
        if(bad_usb->ducky3.attack_mode == 0) {
            furi_hal_usb_disable();
        } else {
            furi_hal_usb_enable();
        }
        return 0;
    }

    if(ducky3_is_keyword(line, "ATTACKMODE")) {
        const char* mode = ducky3_skip_spaces(line + 10);
        if(strcmp(mode, "OFF") == 0) {
            bad_usb->ducky3.attack_mode = 0;
            furi_hal_usb_disable();
            return 0;
        }
        if(strcmp(mode, "HID") == 0) {
            bad_usb->ducky3.attack_mode = 1;
            furi_hal_usb_enable();
            return 0;
        }
        return ducky_error(bad_usb, "Unsupported ATTACKMODE %s", mode);
    }

    static const char random_commands[] = "RANDOM_";
    if(strncmp(line, random_commands, sizeof(random_commands) - 1) == 0) {
        const char* characters = NULL;
        if(strcmp(line, "RANDOM_LOWERCASE_LETTER") == 0)
            characters = "abcdefghijklmnopqrstuvwxyz";
        else if(strcmp(line, "RANDOM_UPPERCASE_LETTER") == 0)
            characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        else if(strcmp(line, "RANDOM_LETTER") == 0)
            characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        else if(strcmp(line, "RANDOM_NUMBER") == 0)
            characters = "0123456789";
        else if(strcmp(line, "RANDOM_SPECIAL") == 0)
            characters = "!@#$%^&*()";
        else if(strcmp(line, "RANDOM_CHAR") == 0)
            characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()";
        if(!characters) return ducky_error(bad_usb, "Unknown random command %s", line);
        size_t count = strlen(characters);
        char output[2] = {characters[furi_hal_random_get() % count], '\0'};
        ducky_string(bad_usb, output);
        return 0;
    }

    char function_name[DUCKY3_MAX_VAR_NAME];
    const char* function_input = line;
    if(ducky3_parse_variable_name(&function_input, function_name, sizeof(function_name))) {
        function_input = ducky3_skip_spaces(function_input);
        if(strncmp(function_input, "()", 2) == 0 && function_input[2] == '\0') {
            Ducky3Function* function = ducky3_find_function(bad_usb, function_name);
            if(!function) return ducky_error(bad_usb, "Unknown function %s", function_name);
            if(bad_usb->ducky3.has_return_value &&
               strcmp(bad_usb->ducky3.deferred_function, function_name) == 0 &&
               strcmp(ducky3_skip_spaces(bad_usb->ducky3.deferred_expression),
                      ducky3_skip_spaces(line)) == 0) {
                bad_usb->ducky3.has_return_value = false;
                bad_usb->ducky3.has_deferred = false;
                bad_usb->ducky3.deferred_function[0] = '\0';
                bad_usb->ducky3.deferred_expression[0] = '\0';
                return 0;
            }
            Ducky3ExpressionParser parser = {
                .bad_usb = bad_usb,
                .start = line,
                .input = line,
                .error = false,
            };
            if(!ducky3_begin_call(&parser, function, function_name)) {
                return ducky_error(bad_usb, "Function call depth exceeded");
            }
            return ducky3_jump(bad_usb, bad_usb->ducky3.jump_line);
        }
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
    bad_usb->ducky3.function_count = 0;
    bad_usb->ducky3.define_count = 0;
    bad_usb->ducky3.random_min = 0;
    bad_usb->ducky3.random_max = 9;
    bad_usb->ducky3.attack_mode = 1;
    bad_usb->ducky3.has_saved_attack_mode = false;
    bad_usb->ducky3.exfil_enabled = false;
    bad_usb->ducky3.exfil_leds_enabled = false;
    bad_usb->ducky3.call_depth = 0;
    bad_usb->ducky3.has_deferred = false;
    bad_usb->ducky3.has_return_value = false;
    storage_file_seek(script_file, 0, true);
    while(stream_read_line((Stream*)script_file, line)) {
        furi_string_trim(line);
        const char* text = furi_string_get_cstr(line);

        if(ducky3_is_keyword(text, "DEFINE")) {
            const char* input = ducky3_skip_spaces(text + 6);
            char name[DUCKY3_MAX_VAR_NAME];
            if(!ducky3_parse_variable_name(&input, name, sizeof(name))) {
                success = false;
            } else {
                input = ducky3_skip_spaces(input);
                Ducky3Value value;
                char* end = NULL;
                long number = strtol(input, &end, 0);
                if(end != input && *end == '\0') {
                    ducky3_value_number(&value, (int32_t)number);
                } else if(strcmp(input, "TRUE") == 0) {
                    ducky3_value_number(&value, 1);
                } else if(strcmp(input, "FALSE") == 0) {
                    ducky3_value_number(&value, 0);
                } else {
                    ducky3_value_string(&value, input);
                }
                success = ducky3_add_define(bad_usb, name, &value);
            }
        } else if(ducky3_is_keyword(text, "FUNCTION")) {
            const char* input = ducky3_skip_spaces(text + 8);
            char name[DUCKY3_MAX_VAR_NAME];
            if(!ducky3_parse_variable_name(&input, name, sizeof(name)) ||
               bad_usb->ducky3.function_count >= DUCKY3_MAX_FUNCTIONS) {
                success = false;
            } else {
                input = ducky3_skip_spaces(input);
                if(strncmp(input, "()", 2) != 0 || input[2] != '\0') {
                    success = false;
                } else {
                    Ducky3Function* function =
                        &bad_usb->ducky3.functions[bad_usb->ducky3.function_count++];
                    ducky3_copy_string(function->name, DUCKY3_MAX_VAR_NAME, name);
                    function->start_line = current_line;
                    function->end_line = DUCKY3_NO_LINE;
                }
            }
        } else if(ducky3_is_keyword(text, "END_FUNCTION")) {
            if(bad_usb->ducky3.function_count == 0) {
                success = false;
            } else {
                bad_usb->ducky3.functions[bad_usb->ducky3.function_count - 1].end_line =
                    current_line;
            }
        } else if(ducky3_is_keyword(text, "IF")) {
            success = ducky3_push_block(bad_usb, stack, &depth, Ducky3BlockIf, current_line);
        } else if(ducky3_is_keyword(text, "ELSE IF")) {
            if(depth == 0) {
                success = false;
            } else {
                Ducky3Block* parent = &bad_usb->ducky3.blocks[stack[depth - 1]];
                if(parent->type != Ducky3BlockIf || parent->else_line != DUCKY3_NO_LINE) {
                    success = false;
                } else {
                    parent->else_line = current_line;
                    success = ducky3_push_block(bad_usb, stack, &depth, Ducky3BlockIf, current_line);
                    if(success) {
                        Ducky3Block* block =
                            &bad_usb->ducky3.blocks[bad_usb->ducky3.block_count - 1];
                        block->parent_line = parent->start_line;
                        depth--;
                        stack[depth - 1] = bad_usb->ducky3.block_count - 1;
                    }
                }
            }
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
                    for(size_t i = 0; i < bad_usb->ducky3.block_count; i++) {
                        Ducky3Block* chain = &bad_usb->ducky3.blocks[i];
                        if(chain->type == Ducky3BlockIf && chain->end_line == DUCKY3_NO_LINE) {
                            chain->end_line = current_line;
                        }
                    }
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
    if(success) {
        for(size_t i = 0; i < bad_usb->ducky3.function_count; i++) {
            if(bad_usb->ducky3.functions[i].end_line == DUCKY3_NO_LINE) {
                bad_usb->st.error_line = current_line;
                success = false;
                break;
            }
        }
    }
    furi_string_free(line);
    storage_file_seek(script_file, 0, true);
    return success;
}

void ducky3_reset(BadUsbScript* bad_usb) {
    bad_usb->ducky3.variable_count = 0;
    bad_usb->ducky3.random_min = 0;
    bad_usb->ducky3.random_max = 9;
    bad_usb->ducky3.attack_mode = 1;
    bad_usb->ducky3.has_saved_attack_mode = false;
    bad_usb->ducky3.exfil_enabled = false;
    bad_usb->ducky3.exfil_leds_enabled = false;
    bad_usb->ducky3.jump_line = 0;
    bad_usb->ducky3.call_depth = 0;
    bad_usb->ducky3.has_deferred = false;
    bad_usb->ducky3.has_return_value = false;
    bad_usb->ducky3.deferred_function[0] = '\0';
    bad_usb->ducky3.deferred_expression[0] = '\0';
}
