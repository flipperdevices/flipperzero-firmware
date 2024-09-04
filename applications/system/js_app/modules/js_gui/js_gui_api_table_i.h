#include "js_gui.h"

static constexpr auto js_gui_api_table = sort(create_array_t<sym_entry>(
    API_METHOD(js_gui_defer_free, void, (JsGui*, void (*)(void*), void*))));
