#pragma once

#include <gui/view.h>
// #include "mag_validators.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Text input anonymous structure */
typedef struct Mag_TextInput Mag_TextInput;
typedef void (*Mag_TextInputCallback)(void* context);
// typedef bool (*Mag_TextInputValidatorCallback)(const char* text, FuriString* error, void* context);

/** Allocate and initialize text input 
 * 
 * This text input is used to enter string
 *
 * @return     Mag_TextInput instance
 */
Mag_TextInput* mag_text_input_alloc();

/** Deinitialize and free text input
 *
 * @param      mag_text_input  Mag_TextInput instance
 */
void mag_text_input_free(Mag_TextInput* mag_text_input);

/** Clean text input view Note: this function does not free memory
 *
 * @param      mag_text_input  Text input instance
 */
void mag_text_input_reset(Mag_TextInput* mag_text_input);

/** Get text input view
 *
 * @param      mag_text_input  Mag_TextInput instance
 *
 * @return     View instance that can be used for embedding
 */
View* mag_text_input_get_view(Mag_TextInput* mag_text_input);

/** Set text input result callback
 *
 * @param      mag_text_input          Mag_TextInput instance
 * @param      callback            callback fn
 * @param      callback_context    callback context
 * @param      text_buffer         pointer to YOUR text buffer, that we going
 *                                 to modify
 * @param      text_buffer_size    YOUR text buffer size in bytes. Max string
 *                                 length will be text_buffer_size-1.
 * @param      clear_default_text  clear text from text_buffer on first OK
 *                                 event
 */
void mag_text_input_set_result_callback(
    Mag_TextInput* mag_text_input,
    Mag_TextInputCallback callback,
    void* callback_context,
    char* text_buffer,
    size_t text_buffer_size,
    bool clear_default_text);

/* void mag_text_input_set_validator(
    Mag_TextInput* mag_text_input,
    Mag_TextInputValidatorCallback callback,
    void* callback_context);

Mag_TextInputValidatorCallback
    mag_text_input_get_validator_callback(Mag_TextInput* mag_text_input);

void* mag_text_input_get_validator_callback_context(Mag_TextInput* mag_text_input); */

/** Set text input header text
 *
 * @param      mag_text_input  Mag_TextInput instance
 * @param      text        text to be shown
 */
void mag_text_input_set_header_text(Mag_TextInput* mag_text_input, const char* text);

#ifdef __cplusplus
}
#endif
