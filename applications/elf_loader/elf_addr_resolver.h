#pragma once
#include <stdlib.h>
#include <stdbool.h>
#include "elf_lib/elf.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef bool (*ELFResolver)(const char* name, Elf32_Addr* address);

bool elf_resolve_from_table(const char* name, Elf32_Addr* address);

bool elf_resolve_printf_only(const char* name, Elf32_Addr* address);

#ifdef __cplusplus
}
#endif