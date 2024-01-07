/**
 * @file int_input.h
 * GUI: Integer keyboard view module API
 */

#pragma once

#include <gui/view.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Int input anonymous structure  */
typedef struct IntInput IntInput;

/** callback that is executed on save button press */
typedef void (*IntInputCallback)(void* context);

/** callback that is executed when byte buffer is changed */
typedef void (*IntChangedCallback)(void* context);

/** Allocate and initialize Int input. This Int input is used to enter Ints.
 *
 * @return     IntInput instance pointer
 */
IntInput* int_input_alloc();

/** Deinitialize and free byte input
 *
 * @param      int_input  Int input instance
 */
void int_input_free(IntInput* int_input);

/** Get byte input view
 *
 * @param      int_input  byte input instance
 *
 * @return     View instance that can be used for embedding
 */
View* int_input_get_view(IntInput* int_input);

/** Set byte input result callback
 *
 * @param      int_input          byte input instance
 * @param      input_callback     input callback fn
 * @param      changed_callback   changed callback fn
 * @param      callback_context   callback context
 * @param      text_buffer        buffer to use
 * @param      text_buffer_size   buffer length
 * @param      clear_default_text clear previous entry
 */

void int_input_set_result_callback(
    IntInput* int_input,
    IntInputCallback input_callback,
    void* callback_context,
    char* text_buffer,
    size_t text_buffer_size,
    bool clear_default_text);

/** Set byte input header text
 *
 * @param      int_input  byte input instance
 * @param      text        text to be shown
 */
void int_input_set_header_text(IntInput* int_input, const char* text);

#ifdef __cplusplus
}
#endif
