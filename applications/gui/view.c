#include "view_i.h"

View* view_alloc() {
    View* view = furi_alloc(sizeof(View));
    view->model_type = ViewModelTypeNone;
    return view;
}

void view_free(View* view) {
    furi_assert(view);
    free(view);
}

void view_set_dispatcher(View* view, ViewDispatcher* view_dispatcher) {
    furi_assert(view);
    furi_assert(view_dispatcher);
    furi_assert(view->dispatcher == NULL);
    view->dispatcher = view_dispatcher;
}

void view_set_draw_callback(View* view, ViewDrawCallback callback) {
    furi_assert(view);
    furi_assert(view->draw_callback == NULL);
    view->draw_callback = callback;
}

void view_set_input_callback(View* view, ViewInputCallback callback) {
    furi_assert(view);
    furi_assert(view->input_callback == NULL);
    view->input_callback = callback;
}

void view_set_context(View* view, void* context) {
    furi_assert(view);
    furi_assert(context);
    view->context = context;
}

void view_set_model(View* view, ViewModelType model_type, void* data) {
    furi_assert(view);
    furi_assert(data);
    furi_assert(view->model == NULL);
    if (model_type == ViewModelTypeLockFree) {
        view_set_lock_free_model(view, data);
    } else if (model_type == ViewModelTypeLocking) {
        view_set_locking_model(view, data);
    } else {
        furi_assert(false);
    }
    view->model_type = model_type;
}

void* view_get_model(View* view) {
    furi_assert(view);
    furi_assert(view->model);
    if (view->model_type == ViewModelTypeLocking) {
        return view->model;
    } else if (view->model_type == ViewModelTypeLocking) {
        ViewModelLocking* model = (ViewModelLocking*)(view->model);
        furi_check(osMutexAcquire(model->mutex, osWaitForever) == osOK);
        return model->data;
    }
}

void view_commit_model(View* view) {
    furi_assert(view);
    furi_assert(view->model);
    if (view->model_type == ViewModelTypeLocking) {
        ViewModelLocking* model = (ViewModelLocking*)(view->model);
        furi_check(osMutexRelease(model->mutex) == osOK);
    }
}

void view_set_lock_free_model(View* view, void* data) {
    view->model = data;
}

void view_set_locking_model(View* view, void* data) {
    ViewModelLocking* model = furi_alloc(sizeof(ViewModelLocking));
    model->mutex = osMutexNew(NULL);
    furi_check(model->mutex);
    model->data = data;
    view->model = model;
}

void view_draw(View* view, Canvas* canvas) {
    furi_assert(view);
    if (view->draw_callback) {
        void* data = view_get_model(view);
        view->draw_callback(canvas, data);
        view_commit_model(view);
    }
}

bool view_input(View* view, InputEvent* event) {
    furi_assert(view);
    if (view->input_callback) {
        return view->input_callback(event, view->context);
    } else {
        return false;
    }
}
