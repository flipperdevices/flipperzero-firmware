#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include "spi_mem_manager_app.h"

#define TAG "SPIMemManager"

uint32_t spi_mem_manager_exit(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

void spi_mem_manager_submenu_callback(void* context, uint32_t index) {
    UNUSED(context);
    UNUSED(index);
}

void spi_mem_manager_add_submenu_items(SPIMemManager* instance) {
    submenu_add_item(instance->submenu, "Read", SPIMemManagerSubmenuIndexRead, spi_mem_manager_submenu_callback, instance);
    submenu_add_item(instance->submenu, "Saved", SPIMemManagerSubmenuIndexSaved, spi_mem_manager_submenu_callback, instance);
    submenu_add_item(instance->submenu, "Chip info", SPIMemManagerSubmenuIndexChipInfo, spi_mem_manager_submenu_callback, instance);
}

SPIMemManager* spi_mem_manager_alloc(void) {
    SPIMemManager* instance = malloc(sizeof(SPIMemManager));
    instance->gui = furi_record_open(RECORD_GUI);
    instance->view_dispatcher = view_dispatcher_alloc();
    instance->submenu = submenu_alloc();
    view_dispatcher_enable_queue(instance->view_dispatcher);
    view_dispatcher_attach_to_gui(instance->view_dispatcher, instance->gui, ViewDispatcherTypeFullscreen);
    spi_mem_manager_add_submenu_items(instance);
    view_set_previous_callback(submenu_get_view(instance->submenu), spi_mem_manager_exit);
    instance->view_id = SPIMemManagerViewSubmenu;
    view_dispatcher_add_view(instance->view_dispatcher, SPIMemManagerViewSubmenu, submenu_get_view(instance->submenu));
    view_dispatcher_switch_to_view(instance->view_dispatcher, instance->view_id);
    return instance;
}

void spi_mem_manager_free(SPIMemManager* instance) {
    furi_record_close(RECORD_GUI);
    view_dispatcher_remove_view(instance->view_dispatcher, SPIMemManagerViewSubmenu);
    view_dispatcher_free(instance->view_dispatcher);
    submenu_free(instance->submenu);
    free(instance);
}

int32_t spi_mem_manager_app(void* p) {
    UNUSED(p);
    SPIMemManager* SPIMemManagerApp = spi_mem_manager_alloc();
    view_dispatcher_run(SPIMemManagerApp->view_dispatcher);
    spi_mem_manager_free(SPIMemManagerApp);
    return 0;
}
