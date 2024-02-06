#include <furi.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/text_box.h>

#include "about_scene.h"
#include "scenes.h"
#include "../../constants.h"
#include "../../flipper_structs.h"

void scene_about_on_enter(void* ctx) {
    struct ApplicationContext* context = (struct ApplicationContext*)ctx;
    TextBox* tbm = context->text_box_module;
    FURI_LOG_T(LOG_TAG, "scene_about_on_enter");

    text_box_reset(tbm);
    text_box_set_font(tbm, TextBoxFontText);
    text_box_set_text(tbm, ABOUT_TEXT);

    // Start the view
    view_dispatcher_switch_to_view(context->view_dispatcher, scene_about);
}

bool scene_about_on_event(void* ctx, SceneManagerEvent event) {
    UNUSED(ctx);
    UNUSED(event);
    FURI_LOG_T(LOG_TAG, "scene_about_on_event");
    return false;
}

void scene_about_on_exit(void* ctx) {
    struct ApplicationContext* context = (struct ApplicationContext*)ctx;
    FURI_LOG_T(LOG_TAG, "scene_about_on_exit");
    text_box_reset(context->text_box_module);
}
