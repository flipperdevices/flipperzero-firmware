#include <flipper_application/api_hashtable/api_hashtable.h>
#include <flipper_application/api_hashtable/compilesort.hpp>

#include "module_demo_api_entries.h"

static_assert(!has_hash_collisions(module_demo_api_table), "Detected API method hash collision!");

constexpr HashtableApiInterface module_demo_hashtable_api_interface{
    {
        .api_version_major = 0,
        .api_version_minor = 0,
        .resolver_callback = &elf_resolve_from_hashtable,
    },
    .table_cbegin = module_demo_api_table.cbegin(),
    .table_cend = module_demo_api_table.cend(),
};

const ElfApiInterface* module_demo_api_interface = &module_demo_hashtable_api_interface;
