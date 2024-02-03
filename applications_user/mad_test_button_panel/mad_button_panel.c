#include <furi.h>
#include <gui/gui.h>

void my_draw_start(Canvas* canvas, void* context) {
    UNUSED(context);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 5, 8, "MAD Testprogramm");
    canvas_draw_str(canvas, 5, 22, "zum testen für");
    canvas_draw_str(canvas, 5, 32, "Button Panel:");
    canvas_draw_str(canvas, 5, 42, "Geändert von:");
    canvas_draw_str(canvas, 5, 52, "Matthias");
    canvas_draw_str(canvas, 5, 62, "20230918");
}

void my_draw_ende(Canvas* canvas, void* context){
    UNUSED(context);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str (canvas, 5,32, "Wird in 5 sec beendet");
}


int mad_test_button_panel_app(void* p) {
    UNUSED(p);

    // Show directions to user.
    Gui* gui = furi_record_open(RECORD_GUI);
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, my_draw_start, NULL);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    furi_delay_ms (5000);

    view_port_draw_callback_set(view_port, my_draw_ende, NULL);

    furi_delay_ms(5000);

    view_port_draw_callback_set(view_port, my_draw_start, NULL);

    furi_delay_ms (5000);

    view_port_draw_callback_set(view_port,my_draw_ende, NULL);

    furi_delay_ms(5000);

    gui_remove_view_port(gui, view_port);
    return 0;
}