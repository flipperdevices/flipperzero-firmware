#pragma once
#include <storage/storage.h>
#include "../application_manifest.h"
#include "elf_api_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ELFFile ELFFile;

typedef struct {
    const char* name;
    uint32_t address;
} ELFMemoryMapEntry;

typedef struct {
    uint32_t debug_link_size;
    uint8_t* debug_link;
} ELFDebugLinkInfo;

typedef struct {
    uint32_t mmap_entry_count;
    ELFMemoryMapEntry* mmap_entries;
    ELFDebugLinkInfo debug_link_info;
} ELFDebugInfo;

typedef enum {
    ELFFileLoadStatusSuccess = 0,
    ELFFileLoadStatusUnspecifiedError,
    ELFFileLoadStatusNoFreeMemory,
    ELFFileLoadStatusMissingImports,
} ELFFileLoadStatus;

ELFFile* elf_file_alloc(Storage* storage, const ElfApiInterface* api_interface);

void elf_file_free(ELFFile* elf_file);

bool elf_file_open(ELFFile* elf_file, const char* path);

bool elf_file_load_manifest(ELFFile* elf, FlipperApplicationManifest* manifest);

bool elf_file_load_section_table(ELFFile* elf_file, FlipperApplicationManifest* manifest);

ELFFileLoadStatus elf_file_load_sections(ELFFile* elf_file);

void elf_file_pre_run(ELFFile* elf);

int32_t elf_file_run(ELFFile* elf_file, void* args);

void elf_file_post_run(ELFFile* elf);

off_t elf_file_get_entry(ELFFile* elf_file);

const ElfApiInterface* elf_file_get_api_interface(ELFFile* elf_file);

void elf_file_init_debug_info(ELFFile* elf_file, ELFDebugInfo* debug_info);

void elf_file_clear_debug_info(ELFDebugInfo* debug_info);

#ifdef __cplusplus
}
#endif