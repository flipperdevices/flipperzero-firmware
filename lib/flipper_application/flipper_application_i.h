#pragma once

#include "elf.h"
#include "flipper_application.h"
#include <m-dict.h>

#ifdef __cplusplus
extern "C" {
#endif

DICT_DEF2(AddressCache, int, M_DEFAULT_OPLIST, Elf32_Addr, M_DEFAULT_OPLIST)

/**
 * Callable elf entry type
 */
typedef int32_t(entry_t)(void*);

typedef struct {
    void* data;
    uint16_t sec_idx;
    uint16_t rel_sec_idx;
} ELFSection;

DICT_DEF2(ELFSectionDict, string_t, STRING_OPLIST, ELFSection, M_POD_OPLIST)

typedef struct {
    size_t sections_count;
    off_t section_table;
    off_t section_table_strings;

    size_t symbol_count;
    off_t symbol_table;
    off_t symbol_table_strings;
    off_t entry;
    ELFSectionDict_t sections;

    AddressCache_t relocation_cache;
    AddressCache_t trampoline_cache;
} ELFFile;

struct FlipperApplication {
    const ElfApiInterface* api_interface;
    File* fd;
    FlipperApplicationState state;
    FlipperApplicationManifest manifest;
    ELFFile elf;
    FuriThread* thread;
};

/**
 * @brief Load and validate basic ELF file headers
 * @param e Application instance
 * @param path FS path to application file
 * @return true if ELF file is valid 
 */
bool flipper_application_load_elf_headers(FlipperApplication* e, const char* path);

/**
 * @brief Iterate over all sections and save related indexes
 * @param e Application instance
 * @return true if all required sections are found
 */
bool flipper_application_load_section_table(FlipperApplication* e);

/**
 * @brief Load section data to memory and process relocations
 * @param e Application instance 
 * @return Status code
 */
FlipperApplicationLoadStatus flipper_application_load_sections(FlipperApplication* e);

/**
 * @brief Release elf file data
 * @param elf elf file pointer
 */
void flipper_application_free_elf_data(ELFFile* elf);

bool flipper_application_load_manifest(FlipperApplication* e);

#ifdef __cplusplus
}
#endif