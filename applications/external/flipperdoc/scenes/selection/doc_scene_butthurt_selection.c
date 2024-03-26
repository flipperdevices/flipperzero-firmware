#include <scenes/doc_scene.h>
#include <scenes/doc_scene_common.h>

void doc_scene_butthurt_on_enter(void* ctx) {
    doc_scene_selection_on_enter(SelectionSceneButthurt, ctx);
}

bool doc_scene_butthurt_on_event(void* ctx, SceneManagerEvent event) {
    return doc_scene_selection_on_event(SelectionSceneButthurt, ctx, event);
}

void doc_scene_butthurt_on_exit(void* ctx) {
    doc_scene_selection_on_exit(SelectionSceneButthurt, ctx);
}

//! ------------------ Main ------------------

#define MAX_CATEGORY 3
const char* selection_butthurt_category[MAX_CATEGORY] = {
    "Deeds:",
    "Next:",
    "Total:"
};

void doc_scene_selection_butthurt_change_text(Doc* app) {
    const uint8_t butthurt = app->dolphin->data.butthurt_daily_limit / 15 +
        (app->dolphin->data.butthurt_daily_limit % 15 != 0);

    char* next;
    switch(butthurt) {
    case 0:
        next = "1+";
        break;
    case 1:
        next = "16+";
        break;
    case 2:
        next = "31+";
        break;
    case 3:
        next = "46+";
        break;
    default:
        next = "/";
        break;
    }

    FuriString* text = doc_selection_get_string(app->selection);
    furi_string_reset(text);

    furi_string_cat_printf(text, "%s\t%hhu\n", selection_butthurt_category[0], app->dolphin->data.butthurt_daily_limit);
    furi_string_cat_printf(text, "%s\t%s\n", selection_butthurt_category[1], next);
    furi_string_cat_printf(text, "%s\t-%hhu | -4\n", selection_butthurt_category[2], butthurt);

    doc_selection_set_size(app->selection, MAX_CATEGORY);
}

void doc_scene_selection_butthurt_on_enter(Doc* app) {
    doc_selection_set_callback(app->selection, doc_scene_callback, app);

    doc_selection_set_title(app->selection, "Daily Butthurt");
    doc_scene_selection_butthurt_change_text(app);
    doc_selection_set_index(app->selection, scene_manager_get_scene_state(app->scene_manager, DocSceneButthurt));

    view_dispatcher_switch_to_view(app->view_dispatcher, DocSelectionView);
}

bool doc_scene_selection_butthurt_on_event(Doc* app, SceneManagerEvent event) {
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom && event.event < ButthurtCategoryMax) {
        scene_manager_set_scene_state(app->scene_manager, DocSceneButthurt, event.event);
        doc_description_set_category(app->description, event.event);
        scene_manager_next_scene(app->scene_manager, DocSceneButthurtDesc);
        consumed = true;
    } else if(event.type == SceneManagerEventTypeCustom && event.event == (uint8_t) - 1) {
        doc_scene_selection_butthurt_change_text(app);
        doc_selection_force_redraw(app->selection);
    }  else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_set_scene_state(app->scene_manager, DocSceneButthurt, 0);
    }

    return consumed;
}

void doc_scene_selection_butthurt_on_exit(Doc* app) {
    UNUSED(app);
}