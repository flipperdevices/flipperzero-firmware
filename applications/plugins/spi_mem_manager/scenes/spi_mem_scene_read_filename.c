#include "../spi_mem_app.h"

void spi_mem_scene_read_filename_view_result_callback(void* context) {
    SPIMemApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, SPIMemCustomEventTextEditResult);
}

void spi_mem_scene_read_set_random_filename(SPIMemApp* app) {
    FuriString* file_name = furi_string_alloc();
    if(furi_string_end_with(app->file_path, SPI_MEM_FILE_EXTENSION)) {
        path_extract_filename(app->file_path, file_name, true);
    }
    set_random_name(app->text_buffer, SPI_MEM_TEXT_BUFFER_SIZE);
    furi_string_free(file_name);
}

void spi_mem_scene_read_filename_on_enter(void* context) {
    SPIMemApp* app = context;
    spi_mem_scene_read_set_random_filename(app);
    text_input_set_header_text(app->text_input, "Name the dump");
    text_input_set_result_callback(
        app->text_input,
        spi_mem_scene_read_filename_view_result_callback,
        app,
        app->text_buffer,
        SPI_MEM_FILE_NAME_SIZE,
        true);
    view_dispatcher_switch_to_view(app->view_dispatcher, SPIMemViewTextInput);
}

bool spi_mem_scene_read_filename_on_event(void* context, SceneManagerEvent event) {
    SPIMemApp* app = context;
    UNUSED(app);
    bool success = false;
    if(event.type == SceneManagerEventTypeCustom) {
        success = true;
        if(event.event == SPIMemCustomEventTextEditResult) {
            if(spi_mem_file_create(app, app->text_buffer)) {
                // wtart worker in read mode
                scene_manager_next_scene(app->scene_manager, SPIMemSceneRead);
            } else {
                scene_manager_search_and_switch_to_previous_scene(
                    app->scene_manager, SPIMemSceneChipDetected);
            }
        }
    }
    return success;
}
void spi_mem_scene_read_filename_on_exit(void* context) {
    SPIMemApp* app = context;
    text_input_reset(app->text_input);
}
