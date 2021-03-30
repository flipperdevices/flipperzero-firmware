#include "subghz_i.h"

/* Private API header */

void subghz_menu_callback(void* context, uint32_t index) {
    furi_assert(context);

    SubGhz* subghz = context;

    if(index == 0) {
        view_dispatcher_switch_to_view(subghz->view_dispatcher, SubGhzViewTestBasic);
    } else if(index == 1) {
        view_dispatcher_switch_to_view(subghz->view_dispatcher, SubGhzViewTestPacket);
    }
}

SubGhz* subghz_alloc() {
    SubGhz* subghz = furi_alloc(sizeof(SubGhz));

    // Thread id
    subghz->thread_id = osThreadGetId();

    // GUI
    subghz->gui = furi_record_open("gui");

    // View Dispatcher
    subghz->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_attach_to_gui(
        subghz->view_dispatcher, subghz->gui, ViewDispatcherTypeFullscreen);

    // Menu
    subghz->submenu = submenu_alloc();
    submenu_add_item(subghz->submenu, "Basic Test", 0, subghz_menu_callback, subghz);
    submenu_add_item(subghz->submenu, "Packet Test", 1, subghz_menu_callback, subghz);
    view_dispatcher_add_view(
        subghz->view_dispatcher, SubGhzViewMenu, submenu_get_view(subghz->submenu));

    // Basic Test Module
    subghz->subghz_test_basic = subghz_test_basic_alloc();
    view_dispatcher_add_view(
        subghz->view_dispatcher,
        SubGhzViewTestBasic,
        subghz_test_basic_get_view(subghz->subghz_test_basic));

    // Packet Test

    // Switch to menu
    view_dispatcher_switch_to_view(subghz->view_dispatcher, SubGhzViewMenu);

    return subghz;
}

void subghz_exit(SubGhz* subghz) {
    furi_assert(subghz);

    osThreadResume(subghz->thread_id);
}

void subghz_free(SubGhz* subghz) {
    furi_assert(subghz);

    // Packet Test

    // Basic Test
    view_dispatcher_remove_view(subghz->view_dispatcher, SubGhzViewTestBasic);

    // Submenu
    view_dispatcher_remove_view(subghz->view_dispatcher, SubGhzViewMenu);
    submenu_free(subghz->submenu);

    // View Dispatcher
    view_dispatcher_free(subghz->view_dispatcher);

    // GUI
    furi_record_close("gui");
    subghz->gui = NULL;

    // The rest
    free(subghz);
}

int32_t subghz_app(void* context) {
    SubGhz* subghz = subghz_alloc();

    // view_dispatcher_loop(subghz->view_dispatcher);
    osThreadSuspend(subghz->thread_id);

    subghz_free(subghz);

    return 0;
}