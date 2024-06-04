#include "flipenigma_text.h"
#include "flipenigma_haptic.h"

#define ENIGMA_IMPLEMENTATION
#include "../enigma/enigma.h"

#define TEXT_PLUGBOARD_DISABLED "Plugboard disabled"
#define TEXT_PLUGBOARD_ERROR          \
    "Error parsing plugboard!\n"      \
    "The plugboard text must be\n"    \
    "an even number of unique \n"     \
    "letters. These are interpret-\n" \
    "ed as connected pairs.\n"        \
    "There is a maximum of 10\n"      \
    "connections (20 letters).\n"     \
    "To disable the plugboard,\n"     \
    "input the letter 'X'."

void text_string_to_uppercase(char* input) {
    int i;
    for(i = 0; input[i] != '\0'; i++) {
        if(input[i] >= 'a' && input[i] <= 'z') {
            input[i] = input[i] - 32;
        } else {
            input[i] = input[i];
        }
    }
}

bool text_are_all_characters_unique(const char* input) {
    bool char_set[256] = {false}; // Assuming ASCII character set

    size_t len = strlen(input);
    for(size_t i = 0; i < len; ++i) {
        unsigned char val = (unsigned char)input[i];
        if(char_set[val]) {
            return false; // Character already found
        }
        char_set[val] = true;
    }

    return true; // All characters are unique
}

// Remove all whitespace from a string and insert a space every Nth character
void text_normalize_spacing(const char* input, char* output, size_t n) {
    // Remove all existing whitespace
    size_t len = strlen(input);
    size_t j = 0;
    for(size_t i = 0; i < len; ++i) {
        if(!isspace((unsigned char)input[i])) {
            output[j++] = input[i];
        }
    }
    output[j] = '\0'; // Null-terminate the intermediate result

    // Insert spaces every Nth character
    size_t input_index = 0;
    size_t output_index = 0;
    char* temp = strdup(output); // Duplicate the intermediate result for further processing
    if(temp == NULL) {
        return; // Memory allocation failed
    }

    for(size_t i = 0; i < j; ++i) {
        if(i > 0 && i % n == 0) {
            output[output_index++] = ' ';
        }
        output[output_index++] = temp[input_index++];
    }
    output[output_index] = '\0'; // Null-terminate the final result

    free(temp); // Free the duplicated intermediate result
}

size_t text_validate_and_convert_plugboard(const char* input, uint8_t output[10][2]) {
    size_t len = strlen(input);

    // Check if all characters are unique
    if(!text_are_all_characters_unique(input)) {
        return 0; // Dupes found
    }

    // Check if the length is even
    if(len % 2 != 0 || len > (PLUGBOARD_SIZE * 2)) {
        return 0; // Invalid input length
    }

    // Check if each character is an uppercase letter and convert
    for(size_t i = 0; i < len; ++i) {
        // Only A-Z characters are valid
        if(!(input[i] >= 'A' && input[i] <= 'Z')) {
            return 0; // Invalid character found
        }
        output[i / 2][i % 2] = input[i];
    }

    return len / 2; // Successful validation and conversion
}

void text_build_output_ciphertext(FlipEnigma* app, char* input, char* output) {
    Enigma* e = init_enigma(
        app->rotors_model,
        app->rotor_positions,
        app->rotor_ring_settings,
        app->reflector_model,
        app->plugboard_switches,
        app->plugboard_size);

    size_t out = 0;
    size_t len = strlen(input);
    for(size_t in = 0; in < len; ++in) {
        // Encrypt A-Z characters only
        char plaintext[2] = "X";
        char ciphertext[2] = "X";
        if(input[in] >= 'A' && input[in] <= 'Z') {
            // Copy the current input char to new object
            plaintext[0] = input[in];
            // Encrypt this single character
            enigma_encrypt(e, plaintext, 1, ciphertext);
            // Set output at position
            output[out] = ciphertext[0];
        } else {
            // Passthrough non A-Z char at position
            output[out] = input[in];
        }
        out++;
    }
    //output[out] = '\n';
    //out++;
    output[out] = '\0';

    destroy_enigma(e);
}

void text_input_callback(void* context) {
    furi_assert(context);
    FlipEnigma* app = context;
    bool show_text_box = false;

    if(app->input_state == FlipEnigmaTextInputMessage) {
        // Check that there is text in the input
        if(strlen(app->input_message_text) > 0) {
            // Convert the text to uppercase
            text_string_to_uppercase(app->input_message_text);
            // Do the actual work of encrypting the message
            text_normalize_spacing(app->input_message_text, app->plain_text, 5);
            text_build_output_ciphertext(app, app->plain_text, app->cipher_text);
            // Populate text box with cipher text
            text_box_set_text(app->text_box, app->cipher_text);
            // Set show_text_box boolean
            show_text_box = true;

            flipenigma_play_happy_bump(app);
        }
    } else if(app->input_state == FlipEnigmaTextInputPlugboard) {
        // Check that there is text in the input
        if(strlen(app->input_plugboard_text) > 0) {
            // Convert the text to uppercase
            text_string_to_uppercase(app->input_plugboard_text);

            // Check for disable case
            if(strcmp(app->input_plugboard_text, "X") == 0) {
                app->plugboard_size = 0;
                // Populate text box with info text
                text_box_set_text(app->text_box, TEXT_PLUGBOARD_DISABLED);
                // Set show_text_box boolean
                show_text_box = true;

                flipenigma_play_happy_bump(app);
            }
            // Do the actual work of parsing the plugboard
            else {
                app->plugboard_size = text_validate_and_convert_plugboard(
                    app->input_plugboard_text, app->plugboard_switches);
                if(app->plugboard_size == 0) {
                    // Populate text box with error text
                    text_box_set_text(app->text_box, TEXT_PLUGBOARD_ERROR);
                    // Set show_text_box boolean
                    show_text_box = true;

                    flipenigma_play_bad_bump(app);
                } else {
                    text_normalize_spacing(app->input_plugboard_text, app->plain_text, 2);
                    // Populate text box with plugboard text
                    text_box_set_text(app->text_box, app->plain_text);
                    // Set show_text_box boolean
                    show_text_box = true;

                    flipenigma_play_happy_bump(app);
                }
                // text_input_set_header_text(app->plugboard_input, app->input_plugboard_text);
            }
        }
    }

    // Reset input state
    app->input_state = FlipEnigmaTextInputDefault;
    if(show_text_box) {
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipEnigmaViewIdTextBox);
    } else {
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipEnigmaViewIdMenu);
    }
}
