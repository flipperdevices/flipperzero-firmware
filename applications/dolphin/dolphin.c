#include "dolphin_i.h"

void dolphin_draw_callback(Canvas* canvas, void* context) {
    Dolphin* dolphin = context;

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_icon(canvas, 128 - 80, 0, dolphin->icon);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 10, TARGET " " BUILD_DATE);
    canvas_draw_str(canvas, 2, 22, GIT_BRANCH);
    canvas_draw_str(canvas, 2, 34, GIT_BRANCH_NUM);
    canvas_draw_str(canvas, 2, 46, GIT_COMMIT);
}

void dolphin_input_callback(InputEvent* event, void* context) {
    Dolphin* dolphin = context;

    if(!event->state || event->input != InputOk) return;

    with_value_mutex(
        dolphin->menu_vm, (Menu * menu) { menu_ok(menu); });
}

Dolphin* dolphin_alloc() {
    Dolphin* dolphin = furi_alloc(sizeof(Dolphin));

    dolphin->icon = assets_icons_get(I_Flipper_young_80x60);
    icon_start_animation(dolphin->icon);

    dolphin->widget = widget_alloc();
    widget_draw_callback_set(dolphin->widget, dolphin_draw_callback, dolphin);
    widget_input_callback_set(dolphin->widget, dolphin_input_callback, dolphin);

    dolphin->menu_vm = furi_open("menu");
    furi_check(dolphin->menu_vm);

    return dolphin;
}

void dolphin_deed(Dolphin* dolphin, DolphinDeed deed) {
    
}

void dolphin_task() {
    Dolphin* dolphin = dolphin_alloc();

    Gui* gui = furi_open("gui");
    gui_add_widget(gui, dolphin->widget, GuiLayerNone);

    if(!furi_create("dolphin", dolphin)) {
        printf("[dolphin_task] cannot create the dolphin record\n");
        furiac_exit(NULL);
    }

    furiac_ready();

    while(1) {
        osDelay(osWaitForever);
    }
}
