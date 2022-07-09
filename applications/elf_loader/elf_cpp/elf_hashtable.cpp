#include "compilesort.hpp"
#include "elf_hashtable.h"
#include "elf_hashtable_entry.h"
#include "elf_hashtable_checks.hpp"

#include <gui/gui.h>
#include <furi.h>

#include <array>
#include <algorithm>

#define TAG "elf_hashtable"

/**
 * 
 */
static const constexpr auto elf_api_table = sort(create_array_t<sym_entry>(
    API_METHOD(init_mutex, bool, (ValueMutex*, void*, size_t)),
    API_METHOD(acquire_mutex, void*, (ValueMutex*, uint32_t)),
    API_METHOD(release_mutex, bool, (ValueMutex*, const void*)),
    API_METHOD(delete_mutex, bool, (ValueMutex*)),

    API_METHOD(canvas_draw_frame, void, (Canvas*, uint8_t, uint8_t, uint8_t, uint8_t)),
    API_METHOD(canvas_draw_rframe, void, (Canvas*, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t)),
    API_METHOD(canvas_draw_box, void, (Canvas*, uint8_t, uint8_t, uint8_t, uint8_t)),
    API_METHOD(canvas_set_color, void, (Canvas*, Color)),
    API_METHOD(canvas_set_font, void, (Canvas*, Font)),
    API_METHOD(canvas_draw_str, void, (Canvas*, uint8_t, uint8_t, const char*)),
    API_METHOD(
        canvas_draw_str_aligned,
        void,
        (Canvas*, uint8_t, uint8_t, Align, Align, const char*)),

    API_METHOD(view_port_alloc, ViewPort*, (void)),
    API_METHOD(view_port_draw_callback_set, void, (ViewPort*, ViewPortDrawCallback, void*)),
    API_METHOD(view_port_input_callback_set, void, (ViewPort*, ViewPortInputCallback, void*)),

    API_METHOD(rand, int, (void)),
    API_METHOD(srand, void, (unsigned)),
    API_METHOD(snprintf, int, (char*, size_t, const char*, ...)),
    API_METHOD(memset, void*, (void*, int, size_t)),
    API_METHOD(memmove, void*, (void*, const void*, size_t)),
    API_METHOD(free, void, (void*)),

    API_METHOD(osTimerNew, osTimerId_t, (osTimerFunc_t, osTimerType_t, void*, const osTimerAttr_t*)),
    API_METHOD(osKernelGetTickFreq, uint32_t, (void)),
    API_METHOD(osTimerDelete, osStatus_t, (osTimerId_t)),
    API_METHOD(osTimerStart, osStatus_t, (osTimerId_t, uint32_t)),
    API_METHOD(
        osMessageQueueNew,
        osMessageQueueId_t,
        (uint32_t, uint32_t, const osMessageQueueAttr_t*)),
    API_METHOD(
        osMessageQueueGet,
        osStatus_t,
        (osMessageQueueId_t, void*, uint8_t*, uint32_t timout)),
    API_METHOD(osMessageQueuePut, osStatus_t, (osMessageQueueId_t, const void*, uint8_t, uint32_t)),
    API_METHOD(osMessageQueueDelete, osStatus_t, (osMessageQueueId_t)),

    API_METHOD(malloc, void*, (size_t)),
    API_METHOD(furi_crash, void, (const char*)),
    API_METHOD(furi_log_print, void, (FuriLogLevel, const char*, ...)),
    API_METHOD(furi_record_open, void*, (const char*)),
    API_METHOD(furi_record_close, void, (const char* name)),

    API_METHOD(gui_add_view_port, void, (Gui*, ViewPort*, GuiLayer)),
    API_METHOD(gui_remove_view_port, void, (Gui*, ViewPort*)),
    API_METHOD(view_port_update, void, (ViewPort*)),
    API_METHOD(view_port_enabled_set, void, (ViewPort*, bool)),
    API_METHOD(view_port_free, void, (ViewPort*))));

static_assert(!has_hash_collisions(elf_api_table), "Detected API method hash collision!");

extern "C" bool elf_resolve_from_hashtable(const char* name, Elf32_Addr* address) {
    bool result = false;
    uint32_t gnu_sym_hash = elf_gnu_hash(name);

    sym_entry key = {
        .hash = gnu_sym_hash,
        .address = 0,
    };

    auto find_res = std::lower_bound(elf_api_table.cbegin(), elf_api_table.cend(), key);
    if((find_res == elf_api_table.cend() || (find_res->hash != gnu_sym_hash))) {
        FURI_LOG_W(TAG, "Cant find symbol '%s' (hash %x)!", name, gnu_sym_hash);
        result = false;
    } else {
        result = true;
        *address = find_res->address;
    }

    return result;
}