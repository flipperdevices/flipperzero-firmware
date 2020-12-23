#include "view_i.h"

View* view_alloc() {
    View* view = furi_alloc(sizeof(View));
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

void view_set_previous_callback(View* view, ViewNavigationCallback callback) {
    furi_assert(view);
    view->previous_callback = callback;
}

void view_set_next_callback(View* view, ViewNavigationCallback callback) {
    furi_assert(view);
    view->next_callback = callback;
}

void view_set_context(View* view, void* context) {
    furi_assert(view);
    furi_assert(context);
    view->context = context;
}

void view_allocate_model(View* view, ViewModelType type, size_t size) {
    furi_assert(view);
    furi_assert(size > 0);
    furi_assert(view->model == NULL);
    if(type == ViewModelTypeLockFree) {
        view->model = furi_alloc(size);
    } else if(type == ViewModelTypeLocking) {
        ViewModelLocking* model = furi_alloc(sizeof(ViewModelLocking));
        model->mutex = osMutexNew(NULL);
        furi_check(model->mutex);
        model->data = furi_alloc(size);
        view->model = model;
    } else {
        furi_assert(false);
    }
    view->model_type = type;
}

void* view_get_model(View* view) {
    furi_assert(view);
    furi_assert(view->model);
    if(view->model_type == ViewModelTypeLocking) {
        ViewModelLocking* model = (ViewModelLocking*)(view->model);
        furi_check(osMutexAcquire(model->mutex, osWaitForever) == osOK);
        return model->data;
    }
    return view->model;
}

void view_commit_model(View* view) {
    furi_assert(view);
    furi_assert(view->model);
    if(view->model_type == ViewModelTypeLocking) {
        ViewModelLocking* model = (ViewModelLocking*)(view->model);
        furi_check(osMutexRelease(model->mutex) == osOK);
    }
    // Update
}

void view_draw(View* view, Canvas* canvas) {
    furi_assert(view);
    if(view->draw_callback) {
        void* data = view_get_model(view);
        view->draw_callback(canvas, data);
        view_commit_model(view);
    }
}

bool view_input(View* view, InputEvent* event) {
    furi_assert(view);
    if(view->input_callback) {
        return view->input_callback(event, view->context);
    } else {
        return false;
    }
}

uint32_t view_previous(View* view) {
    furi_assert(view);
    if(view->previous_callback) {
        return view->previous_callback(view->context);
    } else {
        return VIEW_NONE;
    }
}

uint32_t view_next(View* view) {
    furi_assert(view);
    if(view->next_callback) {
        return view->next_callback(view->context);
    } else {
        return VIEW_NONE;
    }
}
