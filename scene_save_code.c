/*
Takes the code in result_code and opens a file name KB

Saves the code into that file and then goes to the send code
dialog and clears app state

TODO:

Codes need @ and ^ to denote checksum and xor characters

Codes should save using the special characters listed on 
https://humulos.com/digimon/0NL1NE/converter/

In order to do this I should save a code-type

When loading a code type, I can assume saving response codes as the same type.

If there is no code type on load, and there are no special chars, I can have the user
specify the code type.

Code type should be specified on save as well for manual add
*/

#include "flipper.h"
#include "app_state.h"
#include "scenes.h"
#include "scene_save_code.h"

void save_text_input_callback(void* context) {
    App* app = context;
    FURI_LOG_I(TAG, "save_text_input_callback %s", app->state->file_name_tmp);

    FlipperFormat* fff_file = flipper_format_file_alloc(app->storage);
    FuriString* path = furi_string_alloc();
    
    furi_string_printf(path, "%s/%s",APP_DIRECTORY_PATH, app->state->file_name_tmp);

    const char* path_cstr = furi_string_get_cstr(path);

    if(storage_file_exists(app->storage, path_cstr)) {
        storage_simply_remove(app->storage, path_cstr);
    }

    // Open File, create if not exists
    if(!storage_common_stat(app->storage, path_cstr, NULL) == FSE_OK) {
        FURI_LOG_D(TAG, "Config file %s is not found. Will create new.", path_cstr);
        if(storage_common_stat(app->storage, APP_DIRECTORY_PATH, NULL) == FSE_NOT_EXIST) {
            FURI_LOG_D(
                TAG,
                "Directory %s doesn't exist. Will create new.",
                APP_DIRECTORY_PATH);
            if(!storage_simply_mkdir(app->storage, APP_DIRECTORY_PATH)) {
                FURI_LOG_E(TAG, "Error creating directory %s", APP_DIRECTORY_PATH);
            }
        }
    }

    do {

        if(!flipper_format_file_open_new(fff_file, path_cstr)) break;
        if(!flipper_format_write_header_cstr(fff_file, "F-Com Code File", 0)) break;
        if(!flipper_format_write_comment_cstr(fff_file, "Digimon DMCommm Code")) break;
        if(!flipper_format_write_string_cstr(fff_file, "Code", app->state->result_code)) break;

    // signal that the file was written successfully
    } while(0);

    flipper_format_file_close(fff_file);
    flipper_format_free(fff_file);

    // After we save, switch to main menu
    scene_manager_search_and_switch_to_previous_scene(app->scene_manager, FcomMainMenuScene);
}


void fcom_save_code_scene_on_enter(void* context) {
    FURI_LOG_I(TAG, "fcom_save_code_scene_on_enter");
    App* app = context;

    text_input_reset(app->text_input);
    text_input_set_header_text(app->text_input, "Set filename");

    ValidatorIsFile* validator_is_file = validator_is_file_alloc_init(
        "apppath",
        ".txt",
        "code.txt");
    text_input_set_validator(app->text_input, validator_is_file_callback, validator_is_file);
    
    text_input_set_result_callback(app->text_input,
                                   save_text_input_callback,
                                   app,
                                   app->state->file_name_tmp,
                                   MAX_FILENAME_LEN,
                                   true);
    view_dispatcher_switch_to_view(app->view_dispatcher, FcomKeyboardView);
}

bool fcom_save_code_scene_on_event(void* context, SceneManagerEvent event) {
    FURI_LOG_I(TAG, "fcom_save_code_scene_on_event");
    UNUSED(context);
    UNUSED(event);

    return false; //consumed event
}

void fcom_save_code_scene_on_exit(void* context) {
    FURI_LOG_I(TAG, "fcom_save_code_scene_on_exit");
    UNUSED(context);
}


