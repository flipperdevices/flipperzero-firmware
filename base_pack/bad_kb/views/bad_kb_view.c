#include "../bad_kb_app_i.h"
#include "bad_kb_view.h"
#include "../helpers/ducky_script.h"
#include <toolbox/path.h>
#include <gui/elements.h>
#include <assets_icons.h>
#include <bt/bt_service/bt_i.h>

#define MAX_NAME_LEN 64

struct BadKb {
    View* view;
    BadKbButtonCallback callback;
    void* context;
};

typedef struct {
    char file_name[MAX_NAME_LEN];
    char layout[MAX_NAME_LEN];
    BadKbState state;
    bool pause_wait;
    uint8_t anim_frame;
} BadKbModel;

static void bad_kb_draw_callback(Canvas* canvas, void* _model) {
    BadKbModel* model = _model;
    BadKbWorkerState state = model->state.state;

    FuriString* disp_str = furi_string_alloc_set(
        state == BadKbStateInit ? "( . . . )" :
        model->state.is_bt      ? "(BT) " :
                                  "(USB) ");
    furi_string_cat_str(disp_str, model->file_name);
    elements_string_fit_width(canvas, disp_str, 128 - 2);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 8, furi_string_get_cstr(disp_str));

    if(strlen(model->layout) == 0) {
        furi_string_set(disp_str, "(default)");
    } else {
        furi_string_printf(disp_str, "(%s)", model->layout);
    }
    if(model->state.pin) {
        furi_string_cat_printf(disp_str, "  PIN: %ld", model->state.pin);
    } else {
        uint32_t e = model->state.elapsed;
        furi_string_cat_printf(disp_str, "  %02lu:%02lu.%ld", e / 60 / 1000, e / 1000, e % 1000);
    }
    elements_string_fit_width(canvas, disp_str, 128 - 2);
    canvas_draw_str(
        canvas, 2, 8 + canvas_current_font_height(canvas), furi_string_get_cstr(disp_str));

    furi_string_reset(disp_str);

    canvas_draw_icon(canvas, 22, 24, &I_UsbTree_48x22);

    if((state == BadKbStateIdle) || (state == BadKbStateDone) ||
       (state == BadKbStateNotConnected)) {
        elements_button_center(canvas, "Run");
        elements_button_left(canvas, "Config");
    } else if((state == BadKbStateRunning) || (state == BadKbStateDelay)) {
        elements_button_center(canvas, "Stop");
        if(!model->pause_wait) {
            elements_button_right(canvas, "Pause");
        }
    } else if(state == BadKbStatePaused) {
        elements_button_center(canvas, "End");
        elements_button_right(canvas, "Resume");
    } else if(state == BadKbStateWaitForBtn) {
        elements_button_center(canvas, "Press to continue");
    } else if(state == BadKbStateWillRun) {
        elements_button_center(canvas, "Cancel");
    }

    if(state == BadKbStateNotConnected) {
        canvas_draw_icon(canvas, 4, 26, &I_Clock_18x18);
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 127, 31, AlignRight, AlignBottom, "Connect");
        canvas_draw_str_aligned(canvas, 127, 43, AlignRight, AlignBottom, "to device");
    } else if(state == BadKbStateWillRun) {
        canvas_draw_icon(canvas, 4, 26, &I_Clock_18x18);
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 127, 31, AlignRight, AlignBottom, "Will run");
        canvas_draw_str_aligned(canvas, 127, 43, AlignRight, AlignBottom, "on connect");
    } else if(state == BadKbStateFileError) {
        canvas_draw_icon(canvas, 4, 26, &I_Error_18x18);
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 127, 31, AlignRight, AlignBottom, "File");
        canvas_draw_str_aligned(canvas, 127, 43, AlignRight, AlignBottom, "ERROR");
    } else if(state == BadKbStateScriptError) {
        canvas_draw_icon(canvas, 4, 26, &I_Error_18x18);
        furi_string_printf(disp_str, "line %zu", model->state.error_line);
        canvas_draw_str_aligned(
            canvas, 127, 46, AlignRight, AlignBottom, furi_string_get_cstr(disp_str));
        furi_string_set_str(disp_str, model->state.error);
        elements_string_fit_width(canvas, disp_str, canvas_width(canvas));
        canvas_draw_str_aligned(
            canvas, 127, 56, AlignRight, AlignBottom, furi_string_get_cstr(disp_str));
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 127, 33, AlignRight, AlignBottom, "ERROR:");
    } else if(state == BadKbStateIdle) {
        canvas_draw_icon(canvas, 4, 26, &I_Smile_18x18);
        furi_string_printf(disp_str, "0/%zu", model->state.line_nb);
        canvas_draw_str_aligned(
            canvas, 124, 47, AlignRight, AlignBottom, furi_string_get_cstr(disp_str));
        canvas_set_font(canvas, FontBigNumbers);
        canvas_draw_str_aligned(canvas, 112, 37, AlignRight, AlignBottom, "0");
        canvas_draw_icon(canvas, 115, 23, &I_Percent_10x14);
    } else if(state == BadKbStateRunning) {
        if(model->anim_frame == 0) {
            canvas_draw_icon(canvas, 4, 23, &I_EviSmile1_18x21);
        } else {
            canvas_draw_icon(canvas, 4, 23, &I_EviSmile2_18x21);
        }
        furi_string_printf(disp_str, "%zu/%zu", model->state.line_cur, model->state.line_nb);
        canvas_draw_str_aligned(
            canvas, 124, 47, AlignRight, AlignBottom, furi_string_get_cstr(disp_str));
        canvas_set_font(canvas, FontBigNumbers);
        furi_string_printf(
            disp_str, "%zu", ((model->state.line_cur - 1) * 100) / model->state.line_nb);
        canvas_draw_str_aligned(
            canvas, 112, 37, AlignRight, AlignBottom, furi_string_get_cstr(disp_str));
        canvas_draw_icon(canvas, 115, 23, &I_Percent_10x14);
    } else if(state == BadKbStateDone) {
        canvas_draw_icon(canvas, 4, 23, &I_EviSmile1_18x21);
        furi_string_printf(disp_str, "%zu/%zu", model->state.line_nb, model->state.line_nb);
        canvas_draw_str_aligned(
            canvas, 124, 47, AlignRight, AlignBottom, furi_string_get_cstr(disp_str));
        canvas_set_font(canvas, FontBigNumbers);
        canvas_draw_str_aligned(canvas, 112, 37, AlignRight, AlignBottom, "100");
        canvas_draw_icon(canvas, 115, 23, &I_Percent_10x14);
    } else if(state == BadKbStateDelay) {
        if(model->anim_frame == 0) {
            canvas_draw_icon(canvas, 4, 23, &I_EviWaiting1_18x21);
        } else {
            canvas_draw_icon(canvas, 4, 23, &I_EviWaiting2_18x21);
        }
        uint32_t delay = model->state.delay_remain / 10;
        if(delay) {
            furi_string_printf(disp_str, "Delay %lus", delay);
            canvas_draw_str_aligned(
                canvas, 4, 61, AlignLeft, AlignBottom, furi_string_get_cstr(disp_str));
        }
        furi_string_printf(disp_str, "%zu/%zu", model->state.line_cur, model->state.line_nb);
        canvas_draw_str_aligned(
            canvas, 124, 47, AlignRight, AlignBottom, furi_string_get_cstr(disp_str));
        canvas_set_font(canvas, FontBigNumbers);
        furi_string_printf(
            disp_str, "%zu", ((model->state.line_cur - 1) * 100) / model->state.line_nb);
        canvas_draw_str_aligned(
            canvas, 112, 37, AlignRight, AlignBottom, furi_string_get_cstr(disp_str));
        canvas_draw_icon(canvas, 115, 23, &I_Percent_10x14);
    } else if((state == BadKbStatePaused) || (state == BadKbStateWaitForBtn)) {
        if(model->anim_frame == 0) {
            canvas_draw_icon(canvas, 4, 23, &I_EviWaiting1_18x21);
        } else {
            canvas_draw_icon(canvas, 4, 23, &I_EviWaiting2_18x21);
        }
        if(state != BadKbStateWaitForBtn) {
            canvas_draw_str_aligned(canvas, 4, 61, AlignLeft, AlignBottom, "Paused");
        }
        furi_string_printf(disp_str, "%zu/%zu", model->state.line_cur, model->state.line_nb);
        canvas_draw_str_aligned(
            canvas, 124, 47, AlignRight, AlignBottom, furi_string_get_cstr(disp_str));
        canvas_set_font(canvas, FontBigNumbers);
        furi_string_printf(
            disp_str, "%zu", ((model->state.line_cur - 1) * 100) / model->state.line_nb);
        canvas_draw_str_aligned(
            canvas, 112, 37, AlignRight, AlignBottom, furi_string_get_cstr(disp_str));
        canvas_draw_icon(canvas, 115, 23, &I_Percent_10x14);
    } else {
        canvas_draw_icon(canvas, 4, 26, &I_Clock_18x18);
    }

    furi_string_free(disp_str);
}

static bool bad_kb_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    BadKb* bad_kb = context;
    bool consumed = false;

    if(event->type == InputTypeShort) {
        if(event->key == InputKeyLeft) {
            consumed = true;
            furi_assert(bad_kb->callback);
            bad_kb->callback(event->key, bad_kb->context);
        } else if(event->key == InputKeyOk) {
            with_view_model(
                bad_kb->view, BadKbModel * model, { model->pause_wait = false; }, true);
            consumed = true;
            furi_assert(bad_kb->callback);
            bad_kb->callback(event->key, bad_kb->context);
        } else if(event->key == InputKeyRight) {
            with_view_model(
                bad_kb->view,
                BadKbModel * model,
                {
                    if((model->state.state == BadKbStateRunning) ||
                       (model->state.state == BadKbStateDelay)) {
                        model->pause_wait = true;
                    }
                },
                true);
            consumed = true;
            furi_assert(bad_kb->callback);
            bad_kb->callback(event->key, bad_kb->context);
        }
    }

    return consumed;
}

BadKb* bad_kb_view_alloc(void) {
    BadKb* bad_kb = malloc(sizeof(BadKb));

    bad_kb->view = view_alloc();
    view_allocate_model(bad_kb->view, ViewModelTypeLocking, sizeof(BadKbModel));
    view_set_context(bad_kb->view, bad_kb);
    view_set_draw_callback(bad_kb->view, bad_kb_draw_callback);
    view_set_input_callback(bad_kb->view, bad_kb_input_callback);

    return bad_kb;
}

void bad_kb_view_free(BadKb* bad_kb) {
    furi_assert(bad_kb);
    view_free(bad_kb->view);
    free(bad_kb);
}

View* bad_kb_view_get_view(BadKb* bad_kb) {
    furi_assert(bad_kb);
    return bad_kb->view;
}

void bad_kb_view_set_button_callback(BadKb* bad_kb, BadKbButtonCallback callback, void* context) {
    furi_assert(bad_kb);
    furi_assert(callback);
    with_view_model(
        bad_kb->view,
        BadKbModel * model,
        {
            UNUSED(model);
            bad_kb->callback = callback;
            bad_kb->context = context;
        },
        true);
}

void bad_kb_view_set_file_name(BadKb* bad_kb, const char* name) {
    furi_assert(name);
    with_view_model(
        bad_kb->view, BadKbModel * model, { strlcpy(model->file_name, name, MAX_NAME_LEN); }, true);
}

void bad_kb_view_set_layout(BadKb* bad_kb, const char* layout) {
    furi_assert(layout);
    with_view_model(
        bad_kb->view, BadKbModel * model, { strlcpy(model->layout, layout, MAX_NAME_LEN); }, true);
}

void bad_kb_view_set_state(BadKb* bad_kb, BadKbState* st) {
    furi_assert(st);
    uint32_t pin = 0;
    if(bad_kb->context != NULL) {
        BadKbApp* app = bad_kb->context;
        if(app->bt != NULL) {
            pin = app->bt->pin;
        }
    }
    st->pin = pin;
    with_view_model(
        bad_kb->view,
        BadKbModel * model,
        {
            memcpy(&(model->state), st, sizeof(BadKbState));
            model->anim_frame ^= 1;
            if(model->state.state == BadKbStatePaused) {
                model->pause_wait = false;
            }
        },
        true);
}

bool bad_kb_view_is_idle_state(BadKb* bad_kb) {
    bool is_idle = false;
    with_view_model(
        bad_kb->view,
        BadKbModel * model,
        {
            if((model->state.state == BadKbStateIdle) || (model->state.state == BadKbStateDone) ||
               (model->state.state == BadKbStateNotConnected)) {
                is_idle = true;
            }
        },
        false);
    return is_idle;
}
