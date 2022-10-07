#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include "views/spi_mem_read.h"
#include "views/spi_mem_chipinfo.h"
#include "spi_mem_app.h"

#define TAG "SPIMem"

uint32_t spi_mem_exit(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

void spi_mem_submenu_callback(void* context, uint32_t index) {
    SPIMemApp* instance = context;
    if(index == SPIMemSubmenuIndexRead) {
        instance->view_id = SPIMemViewRead;
    }
    if(index == SPIMemSubmenuIndexChipinfo) {
        instance->view_id = SPIMemViewChipinfo;
    }
    view_dispatcher_switch_to_view(instance->view_dispatcher, instance->view_id);
}

void spi_mem_add_submenu_items(SPIMemApp* instance) {
    submenu_add_item(
        instance->submenu, "Read", SPIMemSubmenuIndexRead, spi_mem_submenu_callback, instance);
    submenu_add_item(
        instance->submenu, "Saved", SPIMemSubmenuIndexSaved, spi_mem_submenu_callback, instance);
    submenu_add_item(
        instance->submenu,
        "Chip info",
        SPIMemSubmenuIndexChipinfo,
        spi_mem_submenu_callback,
        instance);
}

uint32_t spi_mem_show_submenu(void* context) {
    UNUSED(context);
    return SPIMemViewSubmenu;
}

void spi_mem_add_views(SPIMemApp* instance) {
    instance->view_read = spi_mem_read_alloc();
    instance->view_chipinfo = spi_mem_chipinfo_alloc();
    view_set_previous_callback(spi_mem_read_get_view(instance->view_read), spi_mem_show_submenu);
    view_set_previous_callback(
        spi_mem_chipinfo_get_view(instance->view_chipinfo), spi_mem_show_submenu);
    view_dispatcher_add_view(
        instance->view_dispatcher, SPIMemViewRead, spi_mem_read_get_view(instance->view_read));
    view_dispatcher_add_view(
        instance->view_dispatcher,
        SPIMemViewChipinfo,
        spi_mem_chipinfo_get_view(instance->view_chipinfo));
}

SPIMemApp* spi_mem_alloc(void) {
    SPIMemApp* instance = malloc(sizeof(SPIMemApp));
    instance->gui = furi_record_open(RECORD_GUI);
    instance->view_dispatcher = view_dispatcher_alloc();
    instance->submenu = submenu_alloc();
    view_dispatcher_enable_queue(instance->view_dispatcher);
    view_dispatcher_attach_to_gui(
        instance->view_dispatcher, instance->gui, ViewDispatcherTypeFullscreen);
    spi_mem_add_submenu_items(instance);
    spi_mem_add_views(instance);
    view_set_previous_callback(submenu_get_view(instance->submenu), spi_mem_exit);
    view_dispatcher_add_view(
        instance->view_dispatcher, SPIMemViewSubmenu, submenu_get_view(instance->submenu));
    view_set_previous_callback(submenu_get_view(instance->submenu), spi_mem_exit);
    instance->view_id = SPIMemViewSubmenu;
    view_dispatcher_switch_to_view(instance->view_dispatcher, instance->view_id);
    return instance;
}

void spi_mem_free(SPIMemApp* instance) {
    furi_record_close(RECORD_GUI);
    view_dispatcher_remove_view(instance->view_dispatcher, SPIMemViewSubmenu);
    view_dispatcher_remove_view(instance->view_dispatcher, SPIMemViewRead);
    view_dispatcher_remove_view(instance->view_dispatcher, SPIMemViewChipinfo);
    view_dispatcher_free(instance->view_dispatcher);
    submenu_free(instance->submenu);
    spi_mem_read_free(instance->view_read);
    spi_mem_chipinfo_free(instance->view_chipinfo);
    free(instance);
}

int32_t spi_mem_app(void* p) {
    UNUSED(p);
    SPIMemApp* instance = spi_mem_alloc();
    view_dispatcher_run(instance->view_dispatcher);
    spi_mem_free(instance);
    return 0;
}
