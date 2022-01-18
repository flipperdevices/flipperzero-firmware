#include "gui/view.h"
#include "furi/memmgr.h"
#include "view_composite.h"
#include "view_i.h"

typedef struct {
    View* bottom;
    View* top;
    bool top_enabled;
} ViewCompositeModel;

struct ViewComposite {
    View* view;
};

static void view_composite_draw(Canvas* canvas, void* model);
static bool view_composite_input(InputEvent* event, void* context);

static void view_composite_update_callback(View* view_top_or_bottom, void* context) {
    furi_assert(view_top_or_bottom);
    furi_assert(context);

    View* view_composite_view = context;
    view_composite_view->update_callback(
        view_composite_view, view_composite_view->update_callback_context);
}

static void view_composite_enter(void* context) {
    furi_assert(context);

    ViewComposite* view_composite = context;
    ViewCompositeModel* model = view_get_model(view_composite->view);

    /* if more than 1 composite views hold same view it has to reassign update_callback_context */
    if(model->bottom) {
        view_set_update_callback_context(model->bottom, view_composite->view);
        if(model->bottom->enter_callback) {
            model->bottom->enter_callback(model->bottom->context);
        }
    }
    if(model->top) {
        view_set_update_callback_context(model->top, view_composite->view);
        if(model->top->enter_callback) {
            model->top->enter_callback(model->top->context);
        }
    }

    view_commit_model(view_composite->view, false);
}

static void view_composite_exit(void* context) {
    furi_assert(context);

    ViewComposite* view_composite = context;
    ViewCompositeModel* model = view_get_model(view_composite->view);

    if(model->bottom) {
        if(model->bottom->exit_callback) {
            model->bottom->exit_callback(model->bottom->context);
        }
    }
    if(model->top) {
        if(model->top->exit_callback) {
            model->top->exit_callback(model->top->context);
        }
    }

    view_commit_model(view_composite->view, false);
}

ViewComposite* view_composite_alloc(void) {
    ViewComposite* view_composite = furi_alloc(sizeof(ViewComposite));
    view_composite->view = view_alloc();

    view_allocate_model(view_composite->view, ViewModelTypeLocking, sizeof(ViewCompositeModel));
    view_set_draw_callback(view_composite->view, view_composite_draw);
    view_set_input_callback(view_composite->view, view_composite_input);
    view_set_context(view_composite->view, view_composite);
    view_set_enter_callback(view_composite->view, view_composite_enter);
    view_set_exit_callback(view_composite->view, view_composite_exit);
    return view_composite;
}

void view_composite_free(ViewComposite* view_composite) {
    furi_assert(view_composite);

    ViewCompositeModel* view_composite_model = view_get_model(view_composite->view);
    view_set_update_callback(view_composite_model->bottom, NULL);
    view_set_update_callback_context(view_composite_model->bottom, NULL);
    view_set_update_callback(view_composite_model->top, NULL);
    view_set_update_callback_context(view_composite_model->top, NULL);
    view_commit_model(view_composite->view, true);

    view_free(view_composite->view);
    free(view_composite);
}

static void view_composite_draw(Canvas* canvas, void* model) {
    furi_assert(model);

    ViewCompositeModel* view_composite_model = model;

    view_draw(view_composite_model->bottom, canvas);
    if(view_composite_model->top_enabled && view_composite_model->top) {
        view_draw(view_composite_model->top, canvas);
    }
}

static bool view_composite_input(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);

    ViewComposite* view_composite = context;
    ViewCompositeModel* view_composite_model = view_get_model(view_composite->view);
    bool consumed = false;

    if(view_composite_model->top_enabled && view_composite_model->top) {
        consumed = view_input(view_composite_model->top, event);
    }
    if(!consumed) {
        consumed = view_input(view_composite_model->bottom, event);
    }

    view_commit_model(view_composite->view, false);

    return consumed;
}

void view_composite_top_enable(ViewComposite* view_composite, bool enable) {
    furi_assert(view_composite);

    ViewCompositeModel* view_composite_model = view_get_model(view_composite->view);
    bool update = (view_composite_model->top_enabled != enable);
    view_composite_model->top_enabled = enable;
    view_commit_model(view_composite->view, update);
}

void view_composite_tie_views(ViewComposite* view_composite, View* view_bottom, View* view_top) {
    furi_assert(view_composite);
    furi_assert(view_bottom);

    ViewCompositeModel* view_composite_model = view_get_model(view_composite->view);

    if(view_composite_model->bottom) {
        view_set_update_callback(view_composite_model->bottom, NULL);
        view_set_update_callback_context(view_composite_model->bottom, NULL);
    }
    if(view_composite_model->top) {
        view_set_update_callback(view_composite_model->top, NULL);
        view_set_update_callback_context(view_composite_model->top, NULL);
    }

    view_composite_model->bottom = view_bottom;
    view_set_update_callback(view_bottom, view_composite_update_callback);
    view_set_update_callback_context(view_bottom, view_composite->view);
    view_composite_model->top = view_top;
    view_set_update_callback(view_top, view_composite_update_callback);
    view_set_update_callback_context(view_top, view_composite->view);

    view_commit_model(view_composite->view, true);
}

View* view_composite_get_view(ViewComposite* view_composite) {
    furi_assert(view_composite);
    return view_composite->view;
}
