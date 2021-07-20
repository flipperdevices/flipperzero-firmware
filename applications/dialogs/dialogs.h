#pragma once
#include <furi.h>
#include <gui/canvas.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DialogsApp DialogsApp;

/**
 * Shows and processes the file selection dialog
 * @param context api pointer
 * @param path path to directory
 * @param extension file extension to be offered for selection
 * @param selected_filename buffer where the selected filename will be saved
 * @param selected_filename_size and the size of this buffer
 * @param preselected_filename filename to be preselected
 * @return bool whether a file was selected
 */
bool dialog_show_file_select(
    DialogsApp* context,
    const char* path,
    const char* extension,
    char* result,
    uint8_t result_size,
    const char* preselected_filename);

/**
 * Message result type
 */
typedef enum {
    DialogMessageButtonBack,
    DialogMessageButtonLeft,
    DialogMessageButtonCenter,
    DialogMessageButtonRight,
} DialogMessageButton;

/**
 * Message struct
 */
typedef struct {
    const char* header_text;
    uint8_t header_text_x;
    uint8_t header_text_y;
    Align header_horizontal;
    Align header_vertical;
    const char* dialog_text;
    uint8_t dialog_text_x;
    uint8_t dialog_text_y;
    Align dialog_text_horizontal;
    Align dialog_text_vertical;
    const Icon* icon;
    uint8_t icon_x;
    uint8_t icon_y;
    const char* left_button_text;
    const char* center_button_text;
    const char* right_button_text;
} DialogMessage;

/**
 * Allocate and fill message struct
 * @return DialogMessage* 
 */
DialogMessage* dialog_allocate_message();

/**
 * Free message struct
 * @param message message struct pointer
 */
void dialog_free_message(DialogMessage* message);

/**
 * Show message from filled struct
 * @param context api pointer
 * @param message message struct pointer to be shown
 * @return DialogMessageButton type
 */
DialogMessageButton dialog_show_message(DialogsApp* context, const DialogMessage* message);

#ifdef __cplusplus
}
#endif