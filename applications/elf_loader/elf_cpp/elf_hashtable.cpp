#include "compilesort.hpp"
#include "elf_hashtable.h"
#include "elf_hashtable_entry.h"
#include "elf_hashtable_checks.hpp"


#include <array>
#include <algorithm>

/* Generated table */
#include <symbols.h>

#define TAG "elf_hashtable"

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
