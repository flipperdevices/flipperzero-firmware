#include "hid_error.h"

struct HidError {
    View* view;
    Popup* popup;
    ViewDispatcher* view_dispatcher;
};

void hid_error(void* context) {
    HidError* hiderror = context;
    Popup* popup = hiderror->popup;

    popup_set_header(popup, "Bonkers", 83, 19, AlignLeft, AlignBottom);

    popup_set_context(popup, hiderror);
    popup_set_timeout(popup, 1500);
    popup_enable_timeout(popup);

    view_dispatcher_switch_to_view(hiderror->view_dispatcher, HidViewConnTypeSubMenu);
}

HidError* hid_error_alloc() {
    HidError* hid_error = malloc(sizeof(HidError));
    hid_error->view = view_alloc();
    view_set_context(hid_error->view, hid_error);

    return hid_error;
}

void hid_error_free(HidError* hid_error) {
    furi_assert(hid_error);
    view_free(hid_error->view);
    free(hid_error);
}

View* hid_error_get_view(HidError* hid_error) {
    furi_assert(hid_error);
    return hid_error->view;
}