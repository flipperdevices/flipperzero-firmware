#include "widget_menu.h"

#include <stdlib.h>
#include <assert.h>
#include <furi.h>
#include <u8g2.h>

#include "assets/main.xbm"

#define MENU_FONT_PRIMARY   u8g2_font_Born2bSportyV2_tr
#define MENU_FONT_SECONDARY u8g2_font_HelvetiPixel_tr

struct widget_menu_t {
    FuriRecordSubscriber* fb_record;
};

widget_menu_t * widget_menu_alloc()
{
    widget_menu_t * p = malloc(sizeof(widget_menu_t));
    assert(p != NULL);

    p->fb_record = furi_open("u8g2_fb", false, false, NULL, NULL, NULL);
    assert(p->fb_record != NULL);
    return p;
}

void widget_menu_free(widget_menu_t *p)
{
    free(p);
}

void widget_menu_draw_idle(widget_menu_t *p)
{
    u8g2_t* fb = furi_take(p->fb_record);
    assert(fb != NULL);

    u8g2_DrawXBM(fb, 0, 0, ASSET_main_screen_width, ASSET_main_screen_height, ASSET_main_screen_bits);

    furi_commit(p->fb_record);
}

void widget_menu_draw_main(widget_menu_t *p, widget_items_t items[], size_t items_count)
{
    u8g2_t* fb = furi_take(p->fb_record);
    assert(fb != NULL);

    u8g2_ClearBuffer(fb);
    u8g2_SetFont(fb, MENU_FONT_SECONDARY);
    u8g2_SetDrawColor(fb, 1);
    u8g2_SetFontMode(fb, 1);
    
    for (int i=0; i<items_count; i++) {
        u8g2_DrawStr(fb, 2, 12*(i+1), items[i].label);
    }

    furi_commit(p->fb_record);
}