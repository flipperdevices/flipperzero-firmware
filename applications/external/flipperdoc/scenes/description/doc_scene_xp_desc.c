#include <scenes/doc_scene.h>
#include <scenes/doc_scene_common.h>
#include <helpers/dolphin_state.h>

void doc_scene_xp_desc_on_enter(void* ctx) {
    doc_scene_description_on_enter(DescriptionSceneXP, ctx);
}

bool doc_scene_xp_desc_on_event(void* ctx, SceneManagerEvent event) {
    return doc_scene_description_on_event(DescriptionSceneXP, ctx, event);
}

void doc_scene_xp_desc_on_exit(void* ctx) {
    doc_scene_description_on_exit(DescriptionSceneXP, ctx);
}

//! ------------------ Main ------------------

const char* doc_desc_subghz_category[7][2] = {
    {"Capture and View:", "1xp"},
    {"Save Signal:", "3xp"},
    {"Record Raw:", "1xp"},
    {"Save Raw:", "3xp"},
    {"Add mnl.:", "2xp"},
    {"Emulate saved:", "2xp"},
    {"Use Analyzer:", "1xp"}
};

const char* doc_desc_rfid_category[5][2] = {
    {"Enter read screen:", "1xp"},
    {"Read success:", "3xp"},
    {"Add mnl.:", "2xp"},
    {"Save:", "3xp"},
    {"Emulate:", "2xp"}
};

const char* doc_desc_nfc_category[8][2] = {
    {"Enter read screen:", "1xp"},
    {"Use detect reader:", "1xp"},
    {"Read success:", "3xp"},
    {"Save:", "3xp"},
    {"Add card mnl.:", "1xp"},
    {"Add key mnl.:", "2xp"},
    {"Emulate saved:", "2xp"},
    {"Emulate mnl.:", "1xp"}
};

const char* doc_desc_ir_category[3][2] = {
    {"Read success:", "3xp"},
    {"Save:", "3xp"},
    {"Emulate:", "1xp"}
};

const char* doc_desc_ibutton_category[5][2] = {
    {"Enter read screen:", "1xp"},
    {"Read success:", "3xp"},
    {"Save:", "3xp"},
    {"Add mnl.:", "2xp"},
    {"Emulate:", "2xp"}
};

const char* doc_desc_badusb_category[1][2] = {
    {"Play script:", "3xp"}
};

const char* doc_desc_plugin_category[5][2] = {
    {"U2F Auth:", "3xp"},
    {"Enter USB-UART:", "1xp"},
    {"Plugin Start:", "2xp"},
    {"Game Start:", "1xp"},
    {"Game Win:", "10xp"}
};

const uint8_t doc_desc_size_category[7] = {7, 5, 8, 3, 5, 1, 5};

void doc_scene_description_xp_change_text(Doc* app, uint8_t size, const char* array[size][2]) {
    FuriString* text = doc_description_get_string(app->description);
    furi_string_reset(text);

    for(int i = 0; i < size; ++i) {
        furi_string_cat_printf(text, "%s\t%s\n", array[i][0], array[i][1]);
    }

    doc_description_set_size(app->description, size);
}

void doc_scene_xp_desc_select_category(Doc* app) {
    switch(doc_description_get_category(app->description)) {
    case XpCategorySubGhz:
        doc_scene_description_xp_change_text(app, doc_desc_size_category[0], doc_desc_subghz_category);
        break;
    case XpCategoryRFID:
        doc_scene_description_xp_change_text(app, doc_desc_size_category[1], doc_desc_rfid_category);
        break;
    case XpCategoryNFC:
        doc_scene_description_xp_change_text(app, doc_desc_size_category[2], doc_desc_nfc_category);
        break;
    case XpCategoryIR:
        doc_scene_description_xp_change_text(app, doc_desc_size_category[3], doc_desc_ir_category);
        break;
    case XpCategoryIButton:
        doc_scene_description_xp_change_text(app, doc_desc_size_category[4], doc_desc_ibutton_category);
        break;
    case XpCategoryBadUSB:
        doc_scene_description_xp_change_text(app, doc_desc_size_category[5], doc_desc_badusb_category);
        break;
    case XpCategoryPlugin:
        doc_scene_description_xp_change_text(app, doc_desc_size_category[6], doc_desc_plugin_category);
        break;
    default:
        break;
    }
}

void doc_scene_description_xp_on_enter(Doc* app) {
    doc_description_set_callback(app->description, doc_scene_callback, app);

    doc_scene_xp_desc_select_category(app);

    view_dispatcher_switch_to_view(app->view_dispatcher, DocDescriptionView);
}

bool doc_scene_description_xp_on_event(Doc* app, SceneManagerEvent event) {
    UNUSED(app);
    UNUSED(event);
    return false;
}

void doc_scene_description_xp_on_exit(Doc* app) {
    UNUSED(app);
}