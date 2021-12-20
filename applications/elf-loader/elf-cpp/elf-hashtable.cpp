#include "compilesort.hpp"
#include "elf-hashtable.h"

#include <gui/gui.h>
#include <furi.h>

#include <array>
#include <algorithm>

#define TAG "elf-hashtable"

#define API_METHOD(x)                                    \
    sym_entry {                                          \
        .hash = elf_gnu_hash(#x), .address = (uint32_t)x \
    }

struct sym_entry {
    uint32_t hash;
    uint32_t address;
};

constexpr bool operator<(const sym_entry& k1, const sym_entry& k2) {
    return k1.hash < k2.hash;
}

static constexpr uint32_t elf_gnu_hash(const char* s) {
    uint32_t h = 0x1505;
    for(unsigned char c = *s; c != '\0'; c = *++s) h = (h << 5) + h + c;
    return h;
}

static constexpr auto elf_addr_table = sort(create_array_t<sym_entry>(
    API_METHOD(acquire_mutex),
    API_METHOD(canvas_draw_frame),
    API_METHOD(canvas_draw_rframe),
    API_METHOD(canvas_draw_box),
    API_METHOD(canvas_set_color),
    API_METHOD(canvas_set_font),
    API_METHOD(canvas_draw_str),
    API_METHOD(snprintf),
    API_METHOD(canvas_draw_str_aligned),
    API_METHOD(release_mutex),
    API_METHOD(osMessageQueuePut),
    API_METHOD(memset),
    API_METHOD(rand),
    API_METHOD(memmove),
    API_METHOD(srand),
    API_METHOD(osMessageQueueNew),
    API_METHOD(furi_alloc),
    API_METHOD(init_mutex),
    API_METHOD(furi_log_print),
    API_METHOD(free),
    API_METHOD(view_port_alloc),
    API_METHOD(view_port_draw_callback_set),
    API_METHOD(view_port_input_callback_set),
    API_METHOD(osTimerNew),
    API_METHOD(osKernelGetTickFreq),
    API_METHOD(osTimerStart),
    API_METHOD(furi_record_open),
    API_METHOD(gui_add_view_port),
    API_METHOD(osMessageQueueGet),
    API_METHOD(view_port_update),
    API_METHOD(osTimerDelete),
    API_METHOD(view_port_enabled_set),
    API_METHOD(gui_remove_view_port),
    API_METHOD(furi_record_close),
    API_METHOD(view_port_free),
    API_METHOD(osMessageQueueDelete),
    API_METHOD(delete_mutex)));

extern "C" bool elf_resolve_from_hashtable(const char* name, Elf32_Addr* address) {
    bool result = false;
    uint32_t gnu_sym_hash = elf_gnu_hash(name);

    sym_entry key = {.hash = gnu_sym_hash};

    auto find_res = std::lower_bound(elf_addr_table.cbegin(), elf_addr_table.cend(), key);
    if((find_res == elf_addr_table.cend() || (find_res->hash != gnu_sym_hash))) {
        FURI_LOG_W(TAG, "Cant find symbol '%s' (hash %x)!", name, gnu_sym_hash);
        result = false;
    } else {
        result = true;
        *address = find_res->address;
    }

    return result;
}