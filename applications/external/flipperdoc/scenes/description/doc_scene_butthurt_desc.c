#include <scenes/doc_scene.h>
#include <scenes/doc_scene_common.h>
#include <helpers/dolphin_state.h>

void doc_scene_butthurt_desc_on_enter(void* ctx) {
    doc_scene_description_on_enter(DescriptionSceneButthurt, ctx);
}

bool doc_scene_butthurt_desc_on_event(void* ctx, SceneManagerEvent event) {
    return doc_scene_description_on_event(DescriptionSceneButthurt, ctx, event);
}

void doc_scene_butthurt_desc_on_exit(void* ctx) {
    doc_scene_description_on_exit(DescriptionSceneButthurt, ctx);
}

//! ------------------ Main ------------------

const char* doc_butthurt_deeds_category[3][2] = {
    {"A Deed is basically", ""},
    {"just 1xp.", ""},
    {"It's maximum is 46.", ""},
};

const char* doc_butthurt_next_category[5][2] = {
    {"0 deeds:", "0"},
    {"+1....+15 deeds:", "-1"},
    {"+16....+30 deeds:", "-1"},
    {"+31....+45 deeds:", "-1"},
    {"+46 deeds:", "-1"}
};

const uint8_t doc_description_size_category[2] = {3, 5};

void doc_scene_description_butthurt_change_text(Doc* app, uint8_t size, const char* array[size][2]) {
    FuriString* text = doc_description_get_string(app->description);
    furi_string_reset(text);

    for(int i = 0; i < size; ++i) {
        furi_string_cat_printf(text, "%s\t%s\n", array[i][0], array[i][1]);
    }

    doc_description_set_size(app->description, size);
}

void doc_scene_butthurt_desc_select_category(Doc* app) {
    switch(doc_description_get_category(app->description)) {
    case ButthurtCategoryDeeds:
        doc_scene_description_butthurt_change_text(app, doc_description_size_category[0], doc_butthurt_deeds_category);
        break;
    case ButthurtCategoryNext:
        doc_scene_description_butthurt_change_text(app, doc_description_size_category[1], doc_butthurt_next_category);
        break;
    default:
        break;
    }
}

void doc_scene_description_butthurt_on_enter(Doc* app) {
    doc_description_set_callback(app->description, doc_scene_callback, app);

    doc_scene_butthurt_desc_select_category(app);

    view_dispatcher_switch_to_view(app->view_dispatcher, DocDescriptionView);
}

bool doc_scene_description_butthurt_on_event(Doc* app, SceneManagerEvent event) {
    UNUSED(app);
    UNUSED(event);
    return false;
}

void doc_scene_description_butthurt_on_exit(Doc* app) {
    UNUSED(app);
}