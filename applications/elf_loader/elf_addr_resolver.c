#include <furi.h>
#include <string.h>
#include <gui/gui.h>
#include "elf_addr_resolver.h"

typedef struct {
    const char* name;
    const Elf32_Addr addr;
} ELFAddrTable;

#define ELF_FN(x) \
    { .name = #x, .addr = (Elf32_Addr)x }

ELFAddrTable elf_addr_table[] = {
    ELF_FN(acquire_mutex),
    ELF_FN(canvas_draw_frame),
    ELF_FN(canvas_draw_rframe),
    ELF_FN(canvas_draw_box),
    ELF_FN(canvas_set_color),
    ELF_FN(canvas_set_font),
    ELF_FN(canvas_draw_str),
    ELF_FN(snprintf),
    ELF_FN(canvas_draw_str_aligned),
    ELF_FN(release_mutex),
    ELF_FN(osMessageQueuePut),
    ELF_FN(memset),
    ELF_FN(rand),
    ELF_FN(memmove),
    ELF_FN(srand),
    ELF_FN(osMessageQueueNew),
    // ELF_FN(furi_alloc),
    ELF_FN(init_mutex),
    ELF_FN(furi_log_print),
    ELF_FN(free),
    ELF_FN(view_port_alloc),
    ELF_FN(view_port_draw_callback_set),
    ELF_FN(view_port_input_callback_set),
    ELF_FN(osTimerNew),
    ELF_FN(osKernelGetTickFreq),
    ELF_FN(osTimerStart),
    ELF_FN(furi_record_open),
    ELF_FN(gui_add_view_port),
    ELF_FN(osMessageQueueGet),
    ELF_FN(view_port_update),
    ELF_FN(release_mutex),
    ELF_FN(osTimerDelete),
    ELF_FN(view_port_enabled_set),
    ELF_FN(gui_remove_view_port),
    ELF_FN(furi_record_close),
    ELF_FN(view_port_free),
    ELF_FN(osMessageQueueDelete),
    ELF_FN(delete_mutex),
    ELF_FN(furi_crash),
};

const size_t elf_addr_table_size = COUNT_OF(elf_addr_table);

bool elf_resolve_from_table(const char* name, Elf32_Addr* address) {
    bool result = false;

    for(size_t i = 0; i < elf_addr_table_size; i++) {
        if(strcmp(name, elf_addr_table[i].name) == 0) {
            result = true;
            *address = elf_addr_table[i].addr;
            break;
        }
    }

    return result;
}

bool elf_resolve_printf_only(const char* name, Elf32_Addr* address) {
    if(strcmp(name, "printf") == 0) {
        *address = (Elf32_Addr)printf;
        return true;
    } else {
        return false;
    }
}