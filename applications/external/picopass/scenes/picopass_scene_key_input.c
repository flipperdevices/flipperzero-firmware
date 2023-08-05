#include "../picopass_i.h"
#include <lib/toolbox/random_name.h>
#include <gui/modules/validators.h>
#include <toolbox/path.h>

void picopass_scene_key_input_text_input_callback(void* context) {
    Picopass* picopass = context;

    picopass->dev->dev_data.pacs.elite_kdf = true;
    memcpy(picopass->dev->dev_data.pacs.key, picopass->byte_input_store, RFAL_PICOPASS_BLOCK_LEN);
    view_dispatcher_send_custom_event(picopass->view_dispatcher, PicopassCustomEventByteInputDone);
}

void picopass_scene_key_input_on_enter(void* context) {
    Picopass* picopass = context;

    ByteInput* byte_input = picopass->byte_input;
    byte_input_set_header_text(byte_input, "Enter The Key In Hex");
    byte_input_set_result_callback(
        byte_input,
        picopass_scene_key_input_text_input_callback,
        NULL,
        picopass,
        picopass->byte_input_store,
        RFAL_PICOPASS_BLOCK_LEN);
    view_dispatcher_switch_to_view(picopass->view_dispatcher, PicopassViewByteInput);
}

bool picopass_scene_key_input_on_event(void* context, SceneManagerEvent event) {
    Picopass* picopass = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == PicopassCustomEventByteInputDone) {
            scene_manager_next_scene(picopass->scene_manager, PicopassSceneWriteKey);
            consumed = true;
        }
    }
    return consumed;
}

void picopass_scene_key_input_on_exit(void* context) {
    Picopass* picopass = context;

    // Clear view
    byte_input_set_result_callback(picopass->byte_input, NULL, NULL, NULL, NULL, 0);
    byte_input_set_header_text(picopass->byte_input, "");
}
