#include <flipper_application/api_hashtable/api_hashtable.h>
#include <flipper_application/api_hashtable/compilesort.hpp>

#include "js_gui_api_table_i.h"

static_assert(!has_hash_collisions(js_gui_api_table), "Detected API method hash collision!");

extern "C" constexpr HashtableApiInterface js_gui_hashtable_api_interface{
    {
        .api_version_major = 0,
        .api_version_minor = 0,
        .resolver_callback = &elf_resolve_from_hashtable,
    },
    js_gui_api_table.cbegin(),
    js_gui_api_table.cend(),
};
