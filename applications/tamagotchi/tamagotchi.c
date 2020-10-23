#include "tamagotchi.h"

#include <assert.h>
#include <flipper_v2.h>

#include <gui/gui.h>
#include <gui/widget.h>
#include <gui/canvas.h>
#include <menu/menu.h>

#include <assets_icons.h>

struct Tamagotchi {
    Widget* widget;
};

void tamagotchi_draw_callback(CanvasApi* canvas, void* context) {
    canvas->clear(canvas);
    canvas->set_color(canvas, ColorBlack);
    canvas->set_font(canvas, FontPrimary);
    canvas->draw_icon(canvas, 10, 20, assets_icons_get(A_Tamagotchi_14));
    canvas->draw_str(canvas, 30, 32, "Tamagotchi");
}

void tamagotchi_input_callback(InputEvent* event, void* context) {
    if(!event->state) return;

    ValueMutex* menu_mutex = furi_open("menu");
    assert(menu_mutex);
    Menu* menu = acquire_mutex_block(menu_mutex);
    menu_ok(menu);
    release_mutex(menu_mutex, menu);
}

Tamagotchi* tamagotchi_alloc() {
    Tamagotchi* tamagotchi = furi_alloc(sizeof(Tamagotchi));

    tamagotchi->widget = widget_alloc();
    widget_draw_callback_set(tamagotchi->widget, tamagotchi_draw_callback, tamagotchi);
    widget_input_callback_set(tamagotchi->widget, tamagotchi_input_callback, tamagotchi);

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