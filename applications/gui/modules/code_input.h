/**
 * @file code_input.h
 * GUI: CodeInput keyboard view module API
 */

#pragma once

#include <gui/view.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Code input anonymous structure  */
typedef struct CodeInput CodeInput;

/** callback that is executed on save button press */
typedef void (*CodeInputCallback)(void* context);

/** callback that is executed when code buffer is changed */
typedef void (*CodeChangedCallback)(void* context);

/** Allocate and initialize code input. This code input is used to enter codes.
 *
 * @return     CodeInput instance pointer
 */
CodeInput* code_input_alloc();

/** Deinitialize and free code input
 *
 * @param      code_input  Code input instance
 */
void code_input_free(CodeInput* code_input);

/** Get code input view
 *
 * @param      code_input  code input instance
 *
 * @return     View instance that can be used for embedding
 */
View* code_input_get_view(CodeInput* code_input);

/** Set code input result callback
 *
 * @param      code_input        code input instance
 * @param      input_callback    input callback fn
 * @param      changed_callback  changed callback fn
 * @param      callback_context  callback context
 * @param      codes             buffer to use
 * @param      codes_count       buffer length
 */
void code_input_set_result_callback(
    CodeInput* code_input,
    CodeInputCallback input_callback,
    CodeChangedCallback changed_callback,
    void* callback_context,
    uint8_t* codes,
    uint8_t codes_count);

/** Set code input header text
 *
 * @param      code_input  code input instance
 * @param      text        text to be shown
 */
void code_input_set_header_text(CodeInput* code_input, const char* text);

#ifdef __cplusplus
}
#endif