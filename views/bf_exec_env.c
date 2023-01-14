
#include "bf_exec_env.h"
#include <gui/elements.h>

typedef struct BFExecEnv {
    View* view;
    ExecEnvOkCallback callback;
    void* context;
    BFApp* appExec;
} BFExecEnv;

typedef struct {
    rByte row;
    rByte col;
} BFExecEnvModel;

static bool bf_exec_process_up(BFExecEnv* ExecEnv);
static bool bf_exec_process_down(BFExecEnv* ExecEnv);
static bool bf_exec_process_left(BFExecEnv* ExecEnv);
static bool bf_exec_process_right(BFExecEnv* ExecEnv);
static bool bf_exec_process_ok(BFExecEnv* ExecEnv, InputEvent* event);

BFApp* appExec;

static void bf_exec_draw_callback(Canvas* canvas, void* _model) {
    UNUSED(_model);
    canvas_draw_str(canvas, 2, 13, workerGetOutput());
}

static bool bf_exec_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    BFExecEnv* ExecEnv = context;
    bool consumed = false;

    if(event->type == InputTypeShort) {
        if(event->key == InputKeyRight) {
            consumed = bf_exec_process_right(ExecEnv);
        } else if(event->key == InputKeyLeft) {
            consumed = bf_exec_process_left(ExecEnv);
        } else if(event->key == InputKeyUp) {
            consumed = bf_exec_process_up(ExecEnv);
        } else if(event->key == InputKeyDown) {
            consumed = bf_exec_process_down(ExecEnv);
        }
    } else if(event->key == InputKeyOk) {
        consumed = bf_exec_process_ok(ExecEnv, event);
    }

    return consumed;
}

static bool bf_exec_process_up(BFExecEnv* ExecEnv) {
    UNUSED(ExecEnv);    
    return true;
}

static bool bf_exec_process_down(BFExecEnv* ExecEnv) {
    UNUSED(ExecEnv);
    return true;
}

static bool bf_exec_process_left(BFExecEnv* ExecEnv) {
    UNUSED(ExecEnv);
    return true;
}

static bool bf_exec_process_right(BFExecEnv* ExecEnv) {
    UNUSED(ExecEnv);
    return true;
}

static bool bf_exec_process_ok(BFExecEnv* ExecEnv, InputEvent* event) {
    UNUSED(ExecEnv);
    UNUSED(event);
    if(event->type != InputTypePress){ return false; }

    FuriThread* t = furi_thread_alloc_ex("BFWorker", 1024, (void*)beginWorker, NULL);
    furi_thread_start(t);

    bool consumed = false;
    return consumed;
}

static void bf_exec_enter_callback(void* context) {
    furi_assert(context);
    BFExecEnv* ExecEnv = context;
    appExec = ExecEnv->appExec;
    UNUSED(ExecEnv);
}

BFExecEnv* bf_exec_env_alloc(BFApp* appExec) {
    BFExecEnv* ExecEnv = malloc(sizeof(BFExecEnv));

    ExecEnv->view = view_alloc();
    ExecEnv->appExec = appExec;
    view_allocate_model(ExecEnv->view, ViewModelTypeLocking, sizeof(BFExecEnvModel));

    with_view_model(
        ExecEnv->view,
        BFExecEnvModel* model,
        {
            model->col = 0;
            model->row = 0;
        },
        true);

    view_set_context(ExecEnv->view, ExecEnv);
    view_set_draw_callback(ExecEnv->view, bf_exec_draw_callback);
    view_set_input_callback(ExecEnv->view, bf_exec_input_callback);
    view_set_enter_callback(ExecEnv->view, bf_exec_enter_callback);
    return ExecEnv;
}

void bf_exec_env_free(BFExecEnv* ExecEnv) {
    furi_assert(ExecEnv);
    view_free(ExecEnv->view);
    free(ExecEnv);
}

View* bf_exec_env_get_view(BFExecEnv* ExecEnv) {
    furi_assert(ExecEnv);
    return ExecEnv->view;
}
