/**
 * @file arg_parser.h
 * @brief Library for parsing command line arguments.
 */
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <furi/core/string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ArgParserOptions is used to describe a flag/argument option submitted
 * when the program is run.
 */
typedef struct {
    const char identifier; /**< Argument identifier.*/
    const char* access_letters /**< Access letters for an argument.*/;
    const char* access_name; /**< Access name for an argument.*/
    const char* value_name; /**< Value name of an argument.*/
    const char* description; /**< Description of an argument.*/
} ArgParserOptions;

/**
 * @brief ArgParser opaque type defenition.
 */
typedef struct ArgParser ArgParser;

/**
 * @brief Allocate ArgParser instance.
 *
 * @param[in] options pointer to ArgParserOptions array with arguments.
 * @param[in] option_count size of options array
 * @param[in] command pointer to FuriString instance with command to be parsed.
 * @return pointer to ArgParser instance.
 */
ArgParser* arg_parser_alloc(
    const ArgParserOptions* options,
    size_t option_count,
    const FuriString* command);

/**
 * @brief Free ArgParser instance.
 *
 * @param[in] instance pointer to ArgParser instance.
 */
void arg_parser_free(ArgParser* instance);

/**
 * @brief Fetches an option from the argument list.
 *
 * This function fetches a single option from the argument list. Information
 * can be extracted from the context after the item has been fetched.
 * The arguments will be re-ordered, which means that non-option arguments will
 * be moved to the end of the argument list.
 *
 * @param[in, out] instance pointer to ArgParser instance.
 * @return Returns true if there was another option or false if the end is
 * reached.
 */
bool arg_parser_fetch(ArgParser* instance);

/**
 * @brief Gets the identifier of the option.
 *
 * This function gets the identifier of the option, which should be unique to
 * this option and can be used to determine what kind of option this is.
 *
 * @param[in, out] instance pointer to ArgParser instance.
 * @return Returns the identifier of the option.
 */
char arg_parser_get_identifier(ArgParser* instance);

/**
 * @brief Gets the value from the option.
 *
 * This function gets the value from the option, if any. If the option does not
 * contain a value, this function will return NULL.
 *
 * @param[in, out] instance pointer to ArgParser instance.
 * @return Returns a pointer to the value or NULL if there is no value.
 */
const char* arg_parser_get_value(ArgParser* instance);

/**
 * @brief Prints the error associated with the invalid option to the specified destination.
 *
 * This function prints information about the error associated with the invalid option
 * to the FuriString instance. It helps in displaying the error of the current context.
 *
 * @param[in, out] instance pointer to ArgParser instance.
 * @param[in] error_str Pointer to FuriString instance where the error information will be printed.
 */
void arg_parser_get_error_message(ArgParser* instance, FuriString* error_str);

/**
 * @brief Prints all options to the specified destination.
 *
 * This can be used to generate the output for a "--help" option.
 *
 * @param[in, out] instance pointer to ArgParser instance.
 * @param[in] help_str Pointer to FuriString instance where the help information will be printed.
 */
void arg_parser_get_help_message(ArgParser* instance, FuriString* help_str);

/**
 * @brief Gets non-specified arguments.
 *
 * Call this function after arg_parser_fetch() returns false, which means that all specified
 * arguments are parsed. The call of this function returns the pointer to non-specified
 * arguments or NULL if there are no more arguments left.
 *
 * @param[in, out] instance pointer to ArgParser instance.
 * @return Returns a pointer to the non-specified argument or NULL if there is no argument left.
 */
const char* arg_parser_get_next_argument(ArgParser* instance);

#ifdef __cplusplus
}
#endif
