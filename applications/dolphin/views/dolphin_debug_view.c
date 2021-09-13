#include <furi.h>
#include "../dolphin_i.h"
#include "dolphin_debug_view.h"

void dolphin_debug_set_callback(
    DolphinDebugView* debug_view,
    DolphinDebugViewCallback callback,
    void* context) {
    furi_assert(debug_view);
    furi_assert(callback);
    debug_view->callback = callback;
    debug_view->context = context;
}

void dolphin_debug_view_render(Canvas* canvas, void* model) {
    canvas_clear(canvas);
    DolphinDebugViewModel* m = model;
    const Version* ver;
    char buffer[64];

    static const char* headers[] = {"FW Version info:", "Boot Version info:", "Dolphin info:"};

    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 13, headers[m->screen]);
    canvas_set_font(canvas, FontSecondary);

    if(m->screen != DolphinViewStatsMeta) {
        // Hardware version
        const char* my_name = furi_hal_version_get_name_ptr();
        snprintf(
            buffer,
            sizeof(buffer),
            "HW: %d.F%dB%dC%d %s",
            furi_hal_version_get_hw_version(),
            furi_hal_version_get_hw_target(),
            furi_hal_version_get_hw_body(),
            furi_hal_version_get_hw_connect(),
            my_name ? my_name : "Unknown");
        canvas_draw_str(canvas, 5, 23, buffer);

        ver = m->screen == DolphinViewStatsBoot ? furi_hal_version_get_boot_version() :
                                                  furi_hal_version_get_firmware_version();

        if(!ver) {
            canvas_draw_str(canvas, 5, 33, "No info");
            return;
        }

        snprintf(
            buffer,
            sizeof(buffer),
            "%s [%s]",
            version_get_version(ver),
            version_get_builddate(ver));
        canvas_draw_str(canvas, 5, 33, buffer);

        snprintf(
            buffer,
            sizeof(buffer),
            "%s [%s]",
            version_get_githash(ver),
            version_get_gitbranchnum(ver));
        canvas_draw_str(canvas, 5, 43, buffer);

        snprintf(
            buffer, sizeof(buffer), "[%s] %s", version_get_target(ver), version_get_gitbranch(ver));
        canvas_draw_str(canvas, 5, 53, buffer);

    } else {
        char buffer[64];
        canvas_set_font(canvas, FontSecondary);
        snprintf(buffer, 64, "Icounter: %ld", m->icounter);
        canvas_draw_str(canvas, 5, 30, buffer);
        snprintf(buffer, 64, "Butthurt: %ld", m->butthurt);
        canvas_draw_str(canvas, 5, 40, buffer);
        canvas_draw_str(canvas, 0, 53, "[< >] icounter value   [ok] save");
    }
}

View* dolphin_debug_get_view(DolphinDebugView* debug_view) {
    furi_assert(debug_view);
    return debug_view->view;
}

bool dolphin_debug_view_input(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);

    DolphinDebugView* debug_view = context;

    if(event->type != InputTypeShort) return false;

    with_view_model(
        debug_view->view, (DolphinDebugViewModel * model) {
            if(event->key == InputKeyDown) {
                model->screen = (model->screen + 1) % DolphinViewStatsTotalCount;
            } else if(event->key == InputKeyUp) {
                model->screen = ((model->screen - 1) + DolphinViewStatsTotalCount) %
                                DolphinViewStatsTotalCount;
            }
            return true;
        });

    // if(current == DolphinViewStatsMeta) {
    //     if(event->key == InputKeyLeft) {
    //         dolphin_deed(dolphin, DolphinDeedWrong);
    //     } else if(event->key == InputKeyRight) {
    //         dolphin_deed(dolphin, DolphinDeedIButtonRead);
    //     } else if(event->key == InputKeyOk) {
    //         dolphin_save(dolphin);
    //     } else {
    //         return false;
    //     }
    // }

    if(event->key == InputKeyBack) {
        debug_view->callback(DolphinDebugEventExit, debug_view->context);
    }

    return true;
}

DolphinDebugView* dolphin_debug_view_alloc() {
    DolphinDebugView* debug_view = furi_alloc(sizeof(DolphinDebugView));
    debug_view->view = view_alloc();
    view_allocate_model(debug_view->view, ViewModelTypeLocking, sizeof(DolphinDebugViewModel));
    view_set_context(debug_view->view, debug_view);
    view_set_draw_callback(debug_view->view, (ViewDrawCallback)dolphin_debug_view_render);
    view_set_input_callback(debug_view->view, dolphin_debug_view_input);

    return debug_view;
}

void dolphin_debug_view_free(DolphinDebugView* debug_view) {
    furi_assert(debug_view);

    view_free(debug_view->view);
    free(debug_view);
}

void dolphin_debug_get_dolphin_data(DolphinDebugView* debug_view, DolphinState* state) {
    with_view_model(
        debug_view->view, (DolphinDebugViewModel * model) {
            model->icounter = dolphin_state_get_icounter(state);
            model->butthurt = dolphin_state_get_butthurt(state);
            return true;
        });
}
