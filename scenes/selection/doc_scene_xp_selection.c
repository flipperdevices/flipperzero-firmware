#include <scenes/doc_scene.h>
#include <scenes/doc_scene_common.h>

void doc_scene_xp_on_enter(void* ctx) {
    doc_scene_selection_on_enter(SelectionSceneXP, ctx);
}

bool doc_scene_xp_on_event(void* ctx, SceneManagerEvent event) {
    return doc_scene_selection_on_event(SelectionSceneXP, ctx, event);
}

void doc_scene_xp_on_exit(void* ctx) {
    doc_scene_selection_on_exit(SelectionSceneXP, ctx);
}

//! ------------------ Main ------------------

#define MAX_CATEGORY 7
const char* selection_xp_category[MAX_CATEGORY] = {
    "SubGhz:",
    "RFID:",
    "NFC:",
    "IR:",
    "IButton:",
    "BadUSB:",
    "Plugin:"
};

void doc_scene_selection_xp_change_text(Doc* app) {
    FuriString* text = doc_selection_get_string(app->selection);
    furi_string_reset(text);

    for(int i = 0; i < MAX_CATEGORY; ++i) {
        furi_string_cat_printf(text, "%s\t%hhu | 20xp\n", selection_xp_category[i], app->dolphin->data.icounter_daily_limit[i]);
    }

    doc_selection_set_size(app->selection, MAX_CATEGORY);
}

void doc_scene_selection_xp_on_enter(Doc* app) {
    doc_selection_set_callback(app->selection, doc_scene_callback, app);

    doc_selection_set_title(app->selection, "Daily XP Limit");
    doc_scene_selection_xp_change_text(app);
    doc_selection_set_index(app->selection, scene_manager_get_scene_state(app->scene_manager, DocSceneXP));

    view_dispatcher_switch_to_view(app->view_dispatcher, DocSelectionView);
}

bool doc_scene_selection_xp_on_event(Doc* app, SceneManagerEvent event) {
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom && event.event < XpCategoryMax) {
        scene_manager_set_scene_state(app->scene_manager, DocSceneXP, event.event);
        doc_description_set_category(app->description, event.event);
        scene_manager_next_scene(app->scene_manager, DocSceneXPDesc);
        consumed = true;
    } else if(event.type == SceneManagerEventTypeCustom && event.event == (uint8_t) - 1) {
        doc_scene_selection_xp_change_text(app);
        doc_selection_force_redraw(app->selection);
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_set_scene_state(app->scene_manager, DocSceneXP, 0);
    }

    return consumed;
}

void doc_scene_selection_xp_on_exit(Doc* app) {
    UNUSED(app);
}