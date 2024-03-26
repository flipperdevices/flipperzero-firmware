#include <scenes/doc_scene.h>
#include <scenes/doc_scene_common.h>
#include <helpers/dolphin_state.h>

void doc_scene_current_on_enter(void* ctx) {
    doc_scene_description_on_enter(DescriptionSceneCurrent, ctx);
}

bool doc_scene_current_on_event(void* ctx, SceneManagerEvent event) {
    return doc_scene_description_on_event(DescriptionSceneCurrent, ctx, event);
}

void doc_scene_current_on_exit(void* ctx) {
    doc_scene_description_on_exit(DescriptionSceneCurrent, ctx);
}

//! ------------------ Main ------------------

void doc_scene_description_current_change_text(Doc* app) {
    FuriString* text = doc_description_get_string(app->description);
    furi_string_reset(text);

    furi_string_cat_printf(text, "Experience:\t%lu\n", app->dolphin->data.icounter);
    furi_string_cat_printf(text, "Level:\t%u | %hhu\n", doc_dolphin_get_level(app->dolphin->data.icounter), MAX_LEVEL);

    const uint32_t next_level = doc_dolphin_state_xp_to_levelup(app->dolphin->data.icounter);
    if(next_level == (uint32_t) - 1) {
        furi_string_cat_printf(text, "Next Level:\t/\n");
    } else {
        furi_string_cat_printf(text, "Next lvl in:\t%lu\n", next_level);
    }

    furi_string_cat_printf(text, "Butthurt:\t%lu | %hhu\n", app->dolphin->data.butthurt, MAX_BUTTHURT);

    doc_description_set_size(app->description, 3);
}

void doc_scene_description_current_on_enter(Doc* app) {
    doc_description_set_callback(app->description, doc_scene_callback, app);

    doc_scene_description_current_change_text(app);

    view_dispatcher_switch_to_view(app->view_dispatcher, DocDescriptionView);
}

bool doc_scene_description_current_on_event(Doc* app, SceneManagerEvent event) {
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom && event.event == (uint8_t) - 1) {
        doc_scene_description_current_change_text(app);
        doc_description_force_redraw(app->description);
        consumed = true;
    }

    return consumed;
}

void doc_scene_description_current_on_exit(Doc* app) {
    UNUSED(app);
}