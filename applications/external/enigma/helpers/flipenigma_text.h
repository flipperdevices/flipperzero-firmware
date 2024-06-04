#include "../flipenigma.h"

void text_string_to_uppercase(char* input);
bool text_are_all_characters_unique(const char* input);
void text_normalize_spacing(const char* input, char* output, size_t n);
size_t text_validate_and_convert_plugboard(const char* input, uint8_t output[10][2]);
void text_build_output_ciphertext(FlipEnigma* app, char* input, char* output);
void text_input_callback(void* context);
