#include <flipper_application/api_hashtable/api_hashtable.h>
#include <flipper_application/api_hashtable/compilesort.hpp>
#include "app_api_interface.h"
#include "app_api_table_i.h"

static_assert(!has_hash_collisions(app_api_table), "Detected API method hash collision!");

constexpr HashtableApiInterface applicaton_hashtable_api_interface{
    {
        .api_version_major = 0,
        .api_version_minor = 0,
        .resolver_callback = &elf_resolve_from_hashtable,
    },
    .table_cbegin = app_api_table.cbegin(),
    .table_cend = app_api_table.cend(),
};

extern "C" const ElfApiInterface* const application_api_interface =
    &applicaton_hashtable_api_interface;
