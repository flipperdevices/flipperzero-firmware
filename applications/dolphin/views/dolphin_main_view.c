#include <furi.h>
#include "../dolphin_i.h"
#include "dolphin_main_view.h"

static const Icon* idle_scenes[] = {&A_Wink_128x64, &A_WatchingTV_128x64};

void dolphin_main_set_callback(
    DolphinMainView* main_view,
    DolphinMainViewCallback callback,
    void* context) {
    furi_assert(main_view);
    furi_assert(callback);
    main_view->callback = callback;
    main_view->context = context;
}

void dolphin_main_view_reset_hint(DolphinMainView* main_view) {
    with_view_model(
        main_view->view, (DolphinMainViewModel * model) {
            model->hint_timeout = 0;
            return true;
        });
}
// temporary main screen animation managment
void dolphin_scene_handler_set_scene(DolphinMainView* main_view, const Icon* icon_data) {
    with_view_model(
        main_view->view, (DolphinMainViewModel * model) {
            if(model->animation) icon_animation_free(model->animation);
            model->animation = icon_animation_alloc(icon_data);
            icon_animation_start(model->animation);
            return true;
        });
}

void dolphin_scene_handler_switch_scene(DolphinMainView* main_view) {
    with_view_model(
        main_view->view, (DolphinMainViewModel * model) {
            if(icon_animation_is_last_frame(model->animation)) {
                if(model->animation) icon_animation_free(model->animation);
                model->animation = icon_animation_alloc(idle_scenes[model->scene_num]);
                icon_animation_start(model->animation);
                model->scene_num = random() % COUNT_OF(idle_scenes);
            }
            return true;
        });
}

void dolphin_main_view_render(Canvas* canvas, void* model) {
    canvas_clear(canvas);
    DolphinMainViewModel* m = model;

    if(m->animation) {
        canvas_draw_icon_animation(canvas, 0, -3, m->animation);
    }

    if(m->unlocked && m->hint_timeout) {
        m->hint_timeout = CLAMP(m->hint_timeout - 1, 2, 0);
        canvas_set_font(canvas, FontPrimary);
        elements_multiline_text_framed(canvas, 42, 30, "Unlocked");
    }
}

View* dolphin_main_get_view(DolphinMainView* main_view) {
    furi_assert(main_view);
    return main_view->view;
}

bool dolphin_main_view_input(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);

    DolphinMainView* main_view = context;

    if(event->key == InputKeyOk && event->type == InputTypeShort) {
        main_view->callback(DolphinMainEventOpenMenu, main_view->context);
    } else if(event->key == InputKeyDown && event->type == InputTypeLong) {
        main_view->callback(DolphinMainEventOpenDebug, main_view->context);
    } else if(event->key == InputKeyUp && event->type == InputTypeShort) {
        main_view->callback(DolphinMainEventOpenLockMenu, main_view->context);
    } else if(event->key == InputKeyDown && event->type == InputTypeShort) {
        main_view->callback(DolphinMainEventOpenArchive, main_view->context);
    }
    dolphin_main_view_reset_hint(main_view);

    return true;
}

DolphinMainView* dolphin_main_view_alloc() {
    DolphinMainView* main_view = furi_alloc(sizeof(DolphinMainView));
    main_view->view = view_alloc();
    view_allocate_model(main_view->view, ViewModelTypeLocking, sizeof(DolphinMainViewModel));
    view_set_context(main_view->view, main_view);
    view_set_draw_callback(main_view->view, (ViewDrawCallback)dolphin_main_view_render);
    view_set_input_callback(main_view->view, dolphin_main_view_input);

    dolphin_scene_handler_set_scene(main_view, idle_scenes[random() % COUNT_OF(idle_scenes)]);

    return main_view;
}

void dolphin_main_view_free(DolphinMainView* main_view) {
    furi_assert(main_view);
    view_free(main_view->view);
    free(main_view);
}

void dolphin_main_view_unlocked(DolphinMainView* main_view) {
    with_view_model(
        main_view->view, (DolphinMainViewModel * model) {
            model->unlocked = true;
            model->hint_timeout = 2;
            return true;
        });
}
