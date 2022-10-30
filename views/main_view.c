#include "main_view.h"

void draw_main_view(Canvas* canvas, lightmeterMainView* main_view, lightmeterI2CSender* sender) {
    
    canvas_clear(canvas);

    if(sender->must_send) {
        i2c_send(sender);
    }

    // top row
    // draw line
    canvas_draw_line(canvas, 0, 10, 128, 10);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 0, 0, AlignLeft, AlignTop, "F");

    canvas_draw_str_aligned(canvas, 20, 0, AlignLeft, AlignTop, "ISO: 400");

    char str[12];

    if(sender->sended) {
        // for(uint8_t i = 0; i < sizeof(lightmeter->sender->recv); i++) {
        //     snprintf(str, sizeof(str), "0x%02x", (int)lightmeter->sender->recv[i]);
        //     canvas_draw_str_aligned(canvas, 90, 25 + (i * 10), AlignLeft, AlignTop, str);
        // }
        
        int lx = ((int)sender->recv[0] << 8) | ((int)sender->recv[1]);
        snprintf(str, sizeof(str), "lx: %d", lx);
        canvas_draw_str_aligned(canvas, 80, 0, AlignLeft, AlignTop, str);
    }

    // add f, T values
    canvas_set_font(canvas, FontBigNumbers);
    canvas_draw_icon(canvas, 15, 17, &I_f_10x14);
    canvas_draw_str_aligned(canvas, 27, 15, AlignLeft, AlignTop, "/3.2");
    canvas_draw_icon(canvas, 15, 34, &I_T_10x14);
    canvas_draw_str_aligned(canvas, 27, 34, AlignLeft, AlignTop, ":1/50");
    
    // create buttons (for the future)
    canvas_set_font(canvas, FontSecondary);
    elements_button_left(canvas, "Config");
    // elements_button_right(canvas, "Config");

    // draw EV number
    canvas_draw_line(canvas, 95, 15, 95, 33);
    canvas_draw_line(canvas, 96, 15, 96, 33);
    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, 100, 15, AlignLeft, AlignTop, "EV:\n+7");

    switch(main_view->current_mode) {
    case FIXED_TIME:
        canvas_set_font(canvas, FontBigNumbers);
        canvas_draw_str_aligned(canvas, 3, 36, AlignLeft, AlignTop, "*");
        break;

    case FIXED_APERTURE:
        canvas_set_font(canvas, FontBigNumbers);
        canvas_draw_str_aligned(canvas, 3, 17, AlignLeft, AlignTop, "*");
        break;

    default:
        break;
    }
}

lightmeterMainView* lightmeter_main_view_alloc() {
    lightmeterMainView* main_view = malloc(sizeof(lightmeterMainView));
    main_view->current_mode = FIXED_TIME;
    return main_view;
}

void lightmeter_main_view_free(lightmeterMainView* main_view) {
    furi_assert(main_view);
    free(main_view);
}
