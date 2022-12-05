#include "UnitempViews.h"
#include <gui/modules/variable_item_list.h>
#include <stdio.h>
#include <assets_icons.h>

uint32_t _prev_view_id;

#define VIEW_ID UnitempViewPopup

static void _popup_callback(void* context) {
    UNUSED(context);
    view_dispatcher_switch_to_view(app->view_dispatcher, _prev_view_id);
}

void unitemp_popup(const Icon* icon, char* header, char* message, uint32_t prev_view_id) {
    _prev_view_id = prev_view_id;
    popup_reset(app->popup);
    popup_set_icon(app->popup, 0, 64 - icon_get_height(icon), icon);
    popup_set_header(app->popup, header, 64, 6, AlignCenter, AlignCenter);
    popup_set_text(
        app->popup,
        message,
        (128 - icon_get_width(icon)) / 2 + icon_get_width(icon),
        32,
        AlignCenter,
        AlignCenter);

    popup_set_timeout(app->popup, 5000);
    popup_set_callback(app->popup, _popup_callback);
    popup_enable_timeout(app->popup);

    view_dispatcher_switch_to_view(app->view_dispatcher, VIEW_ID);
}