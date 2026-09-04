/**
 * @file ndef_text_input.h
 *
 * Text input view forked from gui/modules/text_input. Adds a SHIFT key that
 * toggles a second keyboard layout containing URL/Wi-Fi-friendly punctuation
 * (`.` `/` `:` `-` `?` `=` `&` `#` `@` `+` `_` `!` `~` `,` `*` `;` `(` `)` `'`
 * `<` `>` `"` `%` `$` `[` `]`). The stock TextInput on F0 only exposes
 * `a-z 0-9 _`, which makes URL/SSID entry impossible.
 *
 * Public API mirrors text_input.h so existing patterns translate 1:1.
 */
#pragma once

#include <gui/view.h>
#include <gui/modules/validators.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NdefTextInput NdefTextInput;
typedef void (*NdefTextInputCallback)(void* context);
typedef bool (*NdefTextInputValidatorCallback)(const char* text, FuriString* error, void* context);

NdefTextInput* ndef_text_input_alloc(void);
void ndef_text_input_free(NdefTextInput* text_input);
void ndef_text_input_reset(NdefTextInput* text_input);
View* ndef_text_input_get_view(NdefTextInput* text_input);

void ndef_text_input_set_result_callback(
    NdefTextInput* text_input,
    NdefTextInputCallback callback,
    void* callback_context,
    char* text_buffer,
    size_t text_buffer_size,
    bool clear_default_text);

void ndef_text_input_set_minimum_length(NdefTextInput* text_input, size_t minimum_length);

void ndef_text_input_set_validator(
    NdefTextInput* text_input,
    NdefTextInputValidatorCallback callback,
    void* callback_context);

NdefTextInputValidatorCallback ndef_text_input_get_validator_callback(NdefTextInput* text_input);

void* ndef_text_input_get_validator_callback_context(NdefTextInput* text_input);

void ndef_text_input_set_header_text(NdefTextInput* text_input, const char* text);

#ifdef __cplusplus
}
#endif
