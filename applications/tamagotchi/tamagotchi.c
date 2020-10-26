#include "tamagotchi.h"

#include <assert.h>
#include <flipper_v2.h>

#include <gui/gui.h>
#include <gui/widget.h>
#include <gui/canvas.h>
#include <menu/menu.h>

#include <assets_icons.h>

struct Tamagotchi {
    Icon *icon;
    Widget* widget;
    ValueMutex *menu_vm;
};

void tamagotchi_draw_callback(CanvasApi* canvas, void* context) {
    Tamagotchi* tamagotchi = context;

    canvas->clear(canvas);
    canvas->set_color(canvas, ColorBlack);
    canvas->set_font(canvas, FontPrimary);
    canvas->draw_icon(canvas, 10, 20, tamagotchi->icon);
    canvas->draw_str(canvas, 30, 32, "Tamagotchi");
}

void tamagotchi_input_callback(InputEvent* event, void* context) {
    Tamagotchi* tamagotchi = context;

    if(!event->state || event->input != InputOk) return;

    with_value_mutex(tamagotchi->menu_vm, (Menu *menu) {
        menu_ok(menu);
    });
}

Tamagotchi* tamagotchi_alloc() {
    Tamagotchi* tamagotchi = furi_alloc(sizeof(Tamagotchi));

    tamagotchi->icon = assets_icons_get(A_Tamagotchi_14);
    icon_start_animation(tamagotchi->icon);

    tamagotchi->widget = widget_alloc();
    widget_draw_callback_set(tamagotchi->widget, tamagotchi_draw_callback, tamagotchi);
    widget_input_callback_set(tamagotchi->widget, tamagotchi_input_callback, tamagotchi);

    tamagotchi->menu_vm = furi_open("menu");
    assert(tamagotchi->menu_vm);

    return tamagotchi;
}

void tamagotchi_task() {
    Tamagotchi *tamagotchi = tamagotchi_alloc();

    FuriRecordSubscriber* gui_record = furi_open_deprecated("gui", false, false, NULL, NULL, NULL);
    assert(gui_record);
    GuiApi* gui = furi_take(gui_record);
    assert(gui);
    gui->add_widget(gui, tamagotchi->widget, GuiLayerTamagotchi);
    furi_commit(gui_record);


    furiac_ready();

    while (1) {
        osDelay(osWaitForever);
    }
}