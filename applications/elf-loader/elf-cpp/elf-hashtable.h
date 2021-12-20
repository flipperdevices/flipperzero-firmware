#pragma once
#include <stdint.h>
#include "../elf-lib/elf.h"

#ifdef __cplusplus
extern "C" {
#endif

bool elf_resolve_from_hashtable(const char* name, Elf32_Addr* address);

#ifdef __cplusplus
}
#endif