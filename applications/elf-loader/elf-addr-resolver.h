#pragma once
#include <stdlib.h>
#include <stdbool.h>
#include "elf-lib\elf.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef bool (*ELFResolver)(const char* name, Elf32_Addr* address);

bool elf_resolve(const char* name, Elf32_Addr* address);

#ifdef __cplusplus
}
#endif