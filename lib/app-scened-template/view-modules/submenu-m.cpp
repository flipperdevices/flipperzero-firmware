#include "submenu-m.h"

SubmenuM::SubmenuM() {
    submenu = submenu_alloc();
}

SubmenuM::~SubmenuM() {
    submenu_free(submenu);
}

View* SubmenuM::get_view() {
    return submenu_get_view(submenu);
}

void SubmenuM::clean() {
    submenu_clean(submenu);
}

SubmenuItem* SubmenuM::add_item(
    const char* label,
    uint32_t index,
    SubmenuItemCallback callback,
    void* callback_context) {
    return submenu_add_item(submenu, label, index, callback, callback_context);
}

void SubmenuM::set_selected_item(uint32_t index) {
    submenu_set_selected_item(submenu, index);
}

void SubmenuM::set_header(const char* header) {
    submenu_set_header(submenu, header);
}
