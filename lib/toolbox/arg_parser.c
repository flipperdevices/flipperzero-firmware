// Adapted from https://github.com/likle/cargs

#include "arg_parser.h"

#include <furi/furi.h>
#include <toolbox/args.h>
#include <m-array.h>

#define ARG_PARSER_PRINT_DISTANCE 4
#define ARG_PARSER_PRINT_MIN_INDENTION 20

ARRAY_DEF(ArgParserArgumentArray, FuriString*, FURI_STRING_OPLIST);

struct ArgParser {
    const ArgParserOptions* options;
    FuriString* arg_str;
    ArgParserArgumentArray_t arg_arr;

    size_t option_count;
    int argc;
    int index;
    int inner_index;
    int error_index;
    char error_letter;
    bool forced_end;
    char identifier;
    const char* value;
};

static void arg_parser_argument_array_alloc(ArgParser* instance, const FuriString* command) {
    ArgParserArgumentArray_init(instance->arg_arr);

    FuriString* command_copy = furi_string_alloc_set(command);
    furi_string_trim(command_copy);

    while(true) {
        size_t next_space_offset = furi_string_search_char(command_copy, ' ');
        if(next_space_offset == FURI_STRING_FAILURE) {
            FuriString* arg = furi_string_alloc_set(command_copy);
            ArgParserArgumentArray_push_back(instance->arg_arr, arg);
            break;
        }

        FuriString* arg = furi_string_alloc();
        furi_string_set_n(arg, command_copy, 0, next_space_offset);
        ArgParserArgumentArray_push_back(instance->arg_arr, arg);

        furi_string_right(command_copy, next_space_offset);
        furi_string_trim(command_copy);
    }

    furi_string_free(command_copy);
}

static void arg_parser_argument_array_free(ArgParser* instance) {
    ArgParserArgumentArray_it_t iter;

    for(ArgParserArgumentArray_it(iter, instance->arg_arr); !ArgParserArgumentArray_end_p(iter);
        ArgParserArgumentArray_next(iter)) {
        FuriString* arg = *ArgParserArgumentArray_ref(iter);
        furi_string_free(arg);
    }
    ArgParserArgumentArray_clear(instance->arg_arr);
}

ArgParser* arg_parser_alloc(
    const ArgParserOptions* options,
    size_t option_count,
    const FuriString* command) {
    ArgParser* instance = malloc(sizeof(ArgParser));
    arg_parser_argument_array_alloc(instance, command);

    // This just initialized the values to the beginning of all the arguments.
    instance->argc = ArgParserArgumentArray_size(instance->arg_arr);
    instance->options = options;
    instance->option_count = option_count;
    instance->index = 0;
    instance->inner_index = 0;
    instance->forced_end = false;
    instance->error_index = -1;
    instance->error_letter = 0;

    return instance;
}

void arg_parser_free(ArgParser* instance) {
    furi_check(instance);

    arg_parser_argument_array_free(instance);
}

static const ArgParserOptions*
    arg_parser_find_by_name(ArgParser* instance, const char* name, size_t name_size) {
    const ArgParserOptions* option;
    size_t i;

    // We loop over all the available options and stop as soon as we have found
    // one. We don't use any hash map table, since there won't be that many
    // arguments anyway.
    for(i = 0; i < instance->option_count; ++i) {
        option = &instance->options[i];

        // The option might not have an item name, we can just skip those.
        if(option->access_name == NULL) {
            continue;
        }

        // Try to compare the name of the access name. We can use the name_size or
        // this comparison, since we are guaranteed to have null-terminated access
        // names.
        if(strncmp(option->access_name, name, name_size) == 0) {
            return option;
        }
    }

    return NULL;
}

static const ArgParserOptions* arg_parser_find_by_letter(ArgParser* instance, char letter) {
    const ArgParserOptions* option;
    size_t i;

    // We loop over all the available options and stop as soon as we have found
    // one. We don't use any look up table, since there won't be that many
    // arguments anyway.
    for(i = 0; i < instance->option_count; ++i) {
        option = &instance->options[i];

        // If this option doesn't have any access letters we will skip them.
        if(option->access_letters == NULL) {
            continue;
        }

        // Verify whether this option has the access letter in it's access letter
        // string. If it does, then this is our option.
        if(strchr(option->access_letters, letter) != NULL) {
            return option;
        }
    }

    return NULL;
}

static void
    arg_parser_parse_value(ArgParser* instance, const ArgParserOptions* option, const char** c) {
    // And now let's check whether this option is supposed to have a value, which
    // is the case if there is a value name set. The value can be either submitted
    // with a '=' sign or a space, which means we would have to jump over to the
    // next argv index. This is somewhat ugly, but we do it to behave the same as
    // the other option parsers.
    if(option->value_name != NULL) {
        if(**c == '=') {
            instance->value = ++(*c);
        } else {
            // If the next index is larger or equal to the argument count, then the
            // parameter for this option is missing. The user will know about this,
            // since the value pointer of the context will be NULL because we don't
            // set it here in that case.
            if(instance->argc > instance->index + 1) {
                // We consider this argv to be the value, no matter what the contents
                // are.
                ++instance->index;
                FuriString* arg = *ArgParserArgumentArray_get(instance->arg_arr, instance->index);
                *c = furi_string_get_cstr(arg);
                instance->value = *c;
            }
        }

        // Move c to the end of the value, to not confuse the caller about our
        // position.
        while(**c) {
            ++(*c);
        }
    }
}

static void arg_parser_parse_access_name(ArgParser* instance, const char** c) {
    const ArgParserOptions* option;
    const char* n;

    // Now we need to extract the access name, which is any symbol up to a '=' or
    // a '\0'.
    n = *c;
    while(**c && **c != '=') {
        ++*c;
    }

    // Now this will obviously always be true, but we are paranoid. Sometimes. It
    // doesn't hurt to check.
    furi_check(*c >= n);

    // Figure out which option this name belongs to. This might return NULL if the
    // name is not registered, which means the user supplied an unknown option. In
    // that case we return true to indicate that we finished with this option. We
    // have to skip the value parsing since we don't know whether the user thinks
    // this option has one or not. Since we don't set any identifier specifically,
    // it will remain '?' within the context.
    option = arg_parser_find_by_name(instance, n, (size_t)(*c - n));
    if(option != NULL) {
        // We found an option and now we can specify the identifier within the
        // context.
        instance->identifier = option->identifier;

        // And now we try to parse the value. This function will also check whether
        // this option is actually supposed to have a value.
        arg_parser_parse_value(instance, option, c);
    } else {
        // Remember the error index so that we can print a error message.
        instance->error_index = instance->index;
    }

    // And finally we move on to the next index.
    ++instance->index;
}

static void arg_parser_parse_access_letter(ArgParser* instance, const char** c) {
    const ArgParserOptions* option;
    const char *n, *v;
    char letter;

    n = *c;

    // Figure out which option this letter belongs to. This might return NULL if
    // the letter is not registered, which means the user supplied an unknown
    // option. In that case we return true to indicate that we finished with this
    // option. We have to skip the value parsing since we don't know whether the
    // user thinks this option has one or not. Since we don't set any identifier
    // specifically, it will remain '?' within the context.
    letter = n[instance->inner_index];
    option = arg_parser_find_by_letter(instance, letter);
    v = &n[++instance->inner_index];
    if(option == NULL) {
        instance->error_index = instance->index;
        instance->error_letter = letter;
    } else {
        // We found an option and now we can specify the identifier within the
        // context.
        instance->identifier = option->identifier;

        // And now we try to parse the value. This function will also check whether
        // this option is actually supposed to have a value.
        arg_parser_parse_value(instance, option, &v);
    }

    // Check whether we reached the end of this option argument.
    if(*v == '\0') {
        ++instance->index;
        instance->inner_index = 0;
    }
}

static void arg_parser_shift(ArgParser* instance, int start, int option, int end) {
    FuriString* tmp;
    int a_index, shift_index, left_shift, right_shift, target_index, source_index;

    // The block between start and option will be shifted to the end, and the order
    // of everything will be preserved. Left shift is the amount of indexes the block
    // between option and end will shift towards the start, and right shift is the
    // amount of indexes the block between start and option will be shifted towards
    // the end.
    left_shift = option - start;
    right_shift = end - option;

    // There is no shift is required if the start and the option have the same
    // index.
    if(left_shift == 0) { //-V1051
        return;
    }

    // Let's loop through the option strings first, which we will move towards the
    // beginning.
    for(a_index = option; a_index < end; ++a_index) {
        // First remember the current option value, because we will have to save
        // that later at the beginning.
        tmp = *ArgParserArgumentArray_get(instance->arg_arr, a_index);

        // Let's loop over all option values and shift them one towards the end.
        // This will override the option value we just stored temporarily.
        for(shift_index = 0; shift_index < left_shift; ++shift_index) {
            target_index = a_index - shift_index;
            source_index = a_index - shift_index - 1;
            FuriString* source_arg = *ArgParserArgumentArray_get(instance->arg_arr, source_index);
            ArgParserArgumentArray_set_at(instance->arg_arr, target_index, source_arg);
        }

        // Now restore the saved option value at the beginning.
        ArgParserArgumentArray_set_at(instance->arg_arr, a_index - left_shift, tmp);
    }

    // The new index will be before all non-option values, in such a way that they
    // all will be moved again in the next fetch call.
    instance->index = end - left_shift;

    // The error index may have changed, we need to fix that as well.
    if(instance->error_index >= start) { //-V1051
        if(instance->error_index < option) {
            instance->error_index += right_shift;
        } else if(instance->error_index < end) {
            instance->error_index -= left_shift;
        }
    }
}

static bool arg_parser_is_argument_string(const char* c) {
    return *c == '-' && *(c + 1) != '\0';
}

static int arg_parser_find_next(ArgParser* instance) {
    // Prepare to search the next option at the next index.
    int next_index;
    const char* c;
    FuriString* arg;

    next_index = instance->index;

    // Let's verify that it is not the end If it is
    // the end we have to return -1 to indicate that we finished.
    if(next_index >= instance->argc) {
        return -1;
    }

    // Grab a pointer to the argument string.
    arg = *ArgParserArgumentArray_get(instance->arg_arr, next_index);
    c = furi_string_get_cstr(arg);
    if(instance->forced_end || c == NULL) {
        return -1;
    }

    // Check whether it is a '-'. We need to find the next option - and an option
    // always starts with a '-'. If there is a string "-\0", we don't consider it
    // as an option neither.
    while(!arg_parser_is_argument_string(c)) {
        if(++next_index >= instance->argc) {
            // We reached the end and did not find any argument anymore. Let's tell
            // our caller that we reached the end.
            return -1;
        }

        arg = *ArgParserArgumentArray_get(instance->arg_arr, next_index);
        c = furi_string_get_cstr(arg);
        if(c == NULL) {
            return -1;
        }
    }

    // Indicate that we found an option which can be processed. The index of the
    // next option will be returned.
    return next_index;
}

bool arg_parser_fetch(ArgParser* instance) {
    furi_check(instance);

    const char* c;
    FuriString* arg;
    int old_index, new_index;

    // Reset our identifier to a question mark, which indicates an "unknown"
    // option. The value is set to NULL, to make sure we are not carrying the
    // parameter from the previous option to this one.
    instance->identifier = '?';
    instance->value = NULL;
    instance->error_index = -1;
    instance->error_letter = 0;

    // Check whether there are any options left to parse and remember the old
    // index as well as the new index. In the end we will move the option junk to
    // the beginning, so that non option arguments can be read.
    old_index = instance->index;
    new_index = arg_parser_find_next(instance);
    if(new_index >= 0) {
        instance->index = new_index;
    } else {
        return false;
    }

    // Grab a pointer to the beginning of the option. At this point, the next
    // character must be a '-', since if it was not the prepare function would
    // have returned false. We will skip that symbol and proceed.
    arg = *ArgParserArgumentArray_get(instance->arg_arr, instance->index);
    c = furi_string_get_cstr(arg);
    furi_check(*c == '-');
    ++c;

    // Check whether this is a long option, starting with a double "--".
    if(*c == '-') {
        ++c;

        // This might be a double "--" which indicates the end of options. If this
        // is the case, we will not move to the next index. That ensures that
        // another call to the fetch function will not skip the "--".
        if(*c == '\0') {
            instance->forced_end = true;
        } else {
            // We parse now the access name. All information about it will be written
            // to the context.
            arg_parser_parse_access_name(instance, &c);
        }
    } else {
        // This is no long option, so we can just parse an access letter.
        arg_parser_parse_access_letter(instance, &c);
    }

    // Move the items so that the options come first followed by non-option
    // arguments.
    arg_parser_shift(instance, old_index, new_index, instance->index);

    return instance->forced_end == false;
}

char arg_parser_get_identifier(ArgParser* instance) {
    furi_check(instance);

    return instance->identifier;
}

const char* arg_parser_get_value(ArgParser* instance) {
    furi_check(instance);

    return instance->value;
}

void arg_parser_get_error_message(ArgParser* instance, FuriString* error_str) {
    furi_check(instance);
    furi_check(error_str);

    int error_index;
    char error_letter;

    error_index = instance->error_index;
    if(error_index < 0) {
        return;
    }

    error_letter = instance->error_letter;
    FuriString* error_arg = *ArgParserArgumentArray_get(instance->arg_arr, error_index);
    if(error_letter) {
        furi_string_printf(
            error_str,
            "Unknown option '%c' in '%s'.\n",
            error_letter,
            furi_string_get_cstr(error_arg));
    } else {
        furi_string_printf(error_str, "Unknown option '%s'.\n", furi_string_get_cstr(error_arg));
    }
}

static void arg_parser_print_value(
    const ArgParserOptions* option,
    size_t* accessor_length,
    FuriString* str) {
    if(option->value_name != NULL) {
        *accessor_length += furi_string_cat_printf(str, "=%s", option->value_name);
    }
}

static void arg_parser_print_letters(
    const ArgParserOptions* option,
    bool* first,
    size_t* accessor_length,
    FuriString* str) {
    const char* access_letter;
    access_letter = option->access_letters;
    if(access_letter != NULL) {
        while(*access_letter) {
            if(*first) {
                *accessor_length += furi_string_cat_printf(str, "-%c", *access_letter);
                *first = false;
            } else {
                *accessor_length += furi_string_cat_printf(str, ", -%c", *access_letter);
            }
            ++access_letter;
        }
    }
}

static void arg_parser_print_name(
    const ArgParserOptions* option,
    bool* first,
    size_t* accessor_length,
    FuriString* str) {
    if(option->access_name != NULL) {
        if(*first) {
            *accessor_length += furi_string_cat_printf(str, "--%s", option->access_name);
        } else {
            *accessor_length += furi_string_cat_printf(str, ", --%s", option->access_name);
        }
    }
}

static size_t arg_parser_get_print_indention(ArgParser* instance) {
    size_t option_index, indention, result;
    const ArgParserOptions* option;

    result = ARG_PARSER_PRINT_MIN_INDENTION;

    for(option_index = 0; option_index < instance->option_count; ++option_index) {
        indention = ARG_PARSER_PRINT_DISTANCE;
        option = &instance->options[option_index];
        if(option->access_letters != NULL && *option->access_letters) {
            indention += strlen(option->access_letters) * 4 - 2;
            if(option->access_name != NULL) {
                indention += strlen(option->access_name) + 4;
            }
        } else if(option->access_name != NULL) {
            indention += strlen(option->access_name) + 2;
        }

        if(option->value_name != NULL) {
            indention += strlen(option->value_name) + 1;
        }

        if(indention > result) {
            result = indention;
        }
    }

    return result;
}

void arg_parser_get_help_message(ArgParser* instance, FuriString* help_str) {
    furi_check(instance);
    furi_check(help_str);

    size_t option_index, indention, i, accessor_length;
    const ArgParserOptions* option;
    bool first;

    indention = arg_parser_get_print_indention(instance);
    furi_string_reset(help_str);

    for(option_index = 0; option_index < instance->option_count; ++option_index) {
        option = &instance->options[option_index];
        accessor_length = 0;
        first = true;

        furi_string_cat_printf(help_str, "  ");

        arg_parser_print_letters(option, &first, &accessor_length, help_str);
        arg_parser_print_name(option, &first, &accessor_length, help_str);
        arg_parser_print_value(option, &accessor_length, help_str);

        for(i = accessor_length; i < indention; ++i) {
            furi_string_cat_printf(help_str, " ");
        }

        furi_string_cat_printf(help_str, " %s\r\n", option->description);
    }
}

const char* arg_parser_get_next_argument(ArgParser* instance) {
    furi_check(instance);

    const char* next_arg = NULL;
    if(instance->index < instance->argc) {
        FuriString* arg = *ArgParserArgumentArray_get(instance->arg_arr, instance->index++);
        next_arg = furi_string_get_cstr(arg);
    }

    return next_arg;
}
