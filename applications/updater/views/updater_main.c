#include <gui/gui_i.h>
#include <gui/view.h>
#include <gui/elements.h>
#include <gui/canvas.h>
#include <furi.h>
#include <input/input.h>
#include <dolphin/dolphin.h>

#include "../updater_i.h"
#include "updater_main.h"

struct UpdaterMainView {
    View* view;
    //UpdaterMainViewCallback callback;
    FuriPubSubSubscription* subscription;
    void* context;
};

static const uint8_t PROGRESS_RENDER_STEP = 3; /* percent, to limit rendering rate */

typedef struct {
    string_t status;
    uint8_t progress;
    uint8_t rendered_progress;
} UpdaterProgressModel;

void updater_main_model_set_state(
    UpdaterMainView* main_view,
    const char* message,
    uint8_t progress) {
    with_view_model(
        main_view->view, (UpdaterProgressModel * model) {
            model->progress = progress;
            if(string_cmp_str(model->status, message)) {
                model->rendered_progress = 101; /* to force view update */
                string_set(model->status, message);
            }
            if((model->rendered_progress > progress) ||
               ((progress - model->rendered_progress) > PROGRESS_RENDER_STEP)) {
                model->rendered_progress = progress;
                return true;
            }
            return false;
        });
}

void updater_main_set_callback(
    UpdaterMainView* main_view,
    //UpdaterMainViewCallback callback,
    void* context) {
    furi_assert(main_view);
    //furi_assert(callback);
    //main_view->callback = callback;
    main_view->context = context;
}

View* updater_main_get_view(UpdaterMainView* main_view) {
    furi_assert(main_view);
    return main_view->view;
}

bool updater_main_input(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);

    //UpdaterMainView* main_view = context;

    //if(event->type == InputTypeShort) {
    //    if(event->key == InputKeyOk) {
    //        main_view->callback(UpdaterMainEventOpenMenu, main_view->context);
    //    } else if(event->key == InputKeyUp) {
    //        main_view->callback(UpdaterMainEventOpenLockMenu, main_view->context);
    //    } else if(event->key == InputKeyDown) {
    //        main_view->callback(UpdaterMainEventOpenArchive, main_view->context);
    //    } else if(event->key == InputKeyLeft) {
    //        main_view->callback(UpdaterMainEventOpenFavorite, main_view->context);
    //    } else if(event->key == InputKeyRight) {
    //        main_view->callback(UpdaterMainEventRightShort, main_view->context);
    //    }
    //} else if(event->type == InputTypeLong) {
    //    if(event->key == InputKeyDown) {
    //        main_view->callback(UpdaterMainEventOpenDebug, main_view->context);
    //    }
    //}
    return true;
}

static void updater_main_draw_callback(Canvas* canvas, void* _model) {
    UpdaterProgressModel* model = _model;

    //canvas_draw_icon(canvas, 8, 14, &I_Drive_112x35);
    canvas_set_font(canvas, FontPrimary);

    canvas_draw_str_aligned(
        canvas, 128 / 2, 13, AlignCenter, AlignTop, string_get_cstr(model->status));
    elements_progress_bar(canvas, 14, 35, 100, model->progress);
}

UpdaterMainView* updater_main_alloc() {
    UpdaterMainView* main_view = malloc(sizeof(UpdaterMainView));

    main_view->view = view_alloc();
    view_allocate_model(main_view->view, ViewModelTypeLocking, sizeof(UpdaterProgressModel));

    with_view_model(
        main_view->view, (UpdaterProgressModel * model) {
            string_init_set(model->status, "Waiting for storage");
            return true;
        });

    view_set_context(main_view->view, main_view);
    view_set_input_callback(main_view->view, updater_main_input);
    view_set_draw_callback(main_view->view, updater_main_draw_callback);

    return main_view;
}

void updater_main_free(UpdaterMainView* main_view) {
    furi_assert(main_view);
    with_view_model(
        main_view->view, (UpdaterProgressModel * model) {
            string_clear(model->status);
            return false;
        });
    view_free(main_view->view);
    free(main_view);
}

void updater_main_set_storage_pubsub(UpdaterMainView* main_view, FuriPubSubSubscription* sub) {
    main_view->subscription = sub;
}

FuriPubSubSubscription* updater_main_get_storage_pubsub(UpdaterMainView* main_view) {
    return main_view->subscription;
}