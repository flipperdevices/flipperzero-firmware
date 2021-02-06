#include "ibutton-mode-start.h"
#include "../ibutton-app.h"
#include "../ibutton-view.h"
#include "../ibutton-event.h"

typedef enum {
    SubmenuIndexRead,
    SubmenuIndexSaved,
    SubmenuIndexAdd,
} SubmenuIndex;

void iButtonModeRead::on_enter(iButtonApp* app) {
    iButtonAppView* view = app->get_view();
    Popup* popup = view->get_popup();

    popup_set_header(popup, "iButton", 95, 26, AlignCenter, AlignBottom);
    popup_set_text(popup, "waiting\nfor key ...", 95, 32, AlignCenter, AlignTop);
    popup_set_icon(popup, 0, 5, I_DolphinWait_61x59);

    view->switch_to(iButtonAppView::Type::iButtonAppViewPopup);
}

bool iButtonModeRead::on_event(iButtonApp* app, iButtonEvent* event) {
    bool consumed = false;

    return consumed;
}

void iButtonModeRead::on_exit(iButtonApp* app) {
}