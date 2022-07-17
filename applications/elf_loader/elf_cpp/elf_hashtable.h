#pragma once
#include <stdint.h>

#include <flipper_application/elf/elf.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get function address by function name
 * @param name function name
 * @param address output for function address
 * @return true if the table contains a function
 */
bool elf_resolve_from_hashtable(const char* name, Elf32_Addr* address);

extern const int elf_api_version;

#define ELF_API_MAJOR (elf_api_version >> 16)
#define ELF_API_MINOR (elf_api_version & 0xFFFF)

#ifdef __cplusplus
}
#endif
