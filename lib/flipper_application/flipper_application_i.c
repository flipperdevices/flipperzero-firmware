#include "flipper_application_i.h"
#include <furi.h>

#define TAG "fapp-i"

#define ELF_NAME_BUFFER_LEN 32
#define SECTION_OFFSET(e, n) (e.section_table + n * sizeof(Elf32_Shdr))
#define IS_FLAGS_SET(v, m) ((v & m) == m)

/**************************************************************************************************/
/********************************************** ELF ***********************************************/
/**************************************************************************************************/

static bool elf_read_string_from_offset(FlipperApplication* fap, off_t offset, string_t name) {
    bool result = false;

    off_t old = storage_file_tell(fap->fd);

    do {
        if(!storage_file_seek(fap->fd, offset, true)) break;

        char buffer[ELF_NAME_BUFFER_LEN + 1];
        buffer[ELF_NAME_BUFFER_LEN] = 0;

        while(true) {
            uint16_t read = storage_file_read(fap->fd, buffer, ELF_NAME_BUFFER_LEN);
            string_cat_str(name, buffer);
            if(strlen(buffer) < ELF_NAME_BUFFER_LEN) {
                result = true;
                break;
            }

            if(storage_file_get_error(fap->fd) != FSE_OK || read == 0) break;
        }

    } while(false);
    storage_file_seek(fap->fd, old, true);

    return result;
}

static bool elf_read_section_name(FlipperApplication* fap, off_t offset, string_t name) {
    return elf_read_string_from_offset(fap, fap->elf.section_table_strings + offset, name);
}

static bool elf_read_section_header(
    FlipperApplication* fap,
    size_t section_idx,
    Elf32_Shdr* section_header) {
    off_t offset = SECTION_OFFSET(fap->elf, section_idx);
    return storage_file_seek(fap->fd, offset, true) &&
           storage_file_read(fap->fd, section_header, sizeof(Elf32_Shdr)) == sizeof(Elf32_Shdr);
}

static bool elf_read_section_data(
    FlipperApplication* fap,
    size_t section_idx,
    Elf32_Shdr* section_header,
    string_t name) {
    if(!elf_read_section_header(fap, section_idx, section_header)) {
        return false;
    }

    if(section_header->sh_name && !elf_read_section_name(fap, section_header->sh_name, name)) {
        return false;
    }

    return true;
}

/**************************************************************************************************/
/********************************************* MISC ***********************************************/
/**************************************************************************************************/

bool cstr_prefix(const char* prefix, const char* string) {
    return strncmp(prefix, string, strlen(prefix)) == 0;
}

/**************************************************************************************************/
/************************************ Internal FAP interfaces *************************************/
/**************************************************************************************************/
typedef enum {
    SectionTypeERROR = 0,
    SectionTypeSymTab = (1 << 0),
    SectionTypeStrTab = (1 << 2),
    SectionTypeText = (1 << 3),
    SectionTypeRodata = (1 << 4),
    SectionTypeData = (1 << 5),
    SectionTypeBss = (1 << 6),
    SectionTypeRelText = (1 << 7),
    SectionTypeRelRodata = (1 << 8),
    SectionTypeRelData = (1 << 9),
    SectionTypeRelBss = (1 << 10),
    SectionTypeFappManifest = (1 << 11),
    SectionTypeDebugLink = (1 << 12),
    SectionTypeUnused = (1 << 13),
    // TODO add more section types to validate
    SectionTypeValid = SectionTypeSymTab | SectionTypeStrTab | SectionTypeFappManifest,
    SectionTypeRelocate = SectionTypeRelText | SectionTypeRelRodata | SectionTypeRelData |
                          SectionTypeRelBss,
    SectionTypeGdbSection = SectionTypeText | SectionTypeRodata | SectionTypeData | SectionTypeBss,
} SectionType;

static bool
    flipper_application_load_metadata(FlipperApplication* fap, Elf32_Shdr* section_header) {
    if(section_header->sh_size < sizeof(fap->manifest)) {
        return false;
    }

    return storage_file_seek(fap->fd, section_header->sh_offset, true) &&
           storage_file_read(fap->fd, &fap->manifest, section_header->sh_size) ==
               section_header->sh_size;
}

static bool
    flipper_application_load_debug_link(FlipperApplication* fap, Elf32_Shdr* section_header) {
    fap->state.debug_link_size = section_header->sh_size;
    fap->state.debug_link = malloc(section_header->sh_size);

    return storage_file_seek(fap->fd, section_header->sh_offset, true) &&
           storage_file_read(fap->fd, fap->state.debug_link, section_header->sh_size) ==
               section_header->sh_size;
}

static SectionType flipper_application_preload_section(
    FlipperApplication* fap,
    size_t section_idx,
    Elf32_Shdr* section_header,
    string_t name_string) {
    const char* name = string_get_cstr(name_string);

    const struct {
        const char* prefix;
        SectionType type;
    } lookup_sections[] = {
        {".text", SectionTypeText},
        {".rodata", SectionTypeRodata},
        {".data", SectionTypeData},
        {".bss", SectionTypeBss},
        {".rel.text", SectionTypeRelText},
        {".rel.rodata", SectionTypeRelRodata},
        {".rel.data", SectionTypeRelData},
    };

    for(size_t i = 0; i < COUNT_OF(lookup_sections); i++) {
        if(cstr_prefix(lookup_sections[i].prefix, name)) {
            FURI_LOG_D(TAG, "Found section %s", lookup_sections[i].prefix);

            if((lookup_sections[i].type & SectionTypeRelocate) != 0) {
                name = name + strlen(".rel");
            }

            string_t key;
            string_init_set(key, name);
            ELFSection* section_p = ELFSectionDict_get(fap->elf.sections, key);
            if(!section_p) {
                ELFSection section = {
                    .data = NULL,
                    .sec_idx = 0,
                    .rel_sec_idx = 0,
                };

                ELFSectionDict_set_at(fap->elf.sections, key, section);
                section_p = ELFSectionDict_get(fap->elf.sections, key);
            }
            string_clear(key);

            if((lookup_sections[i].type & SectionTypeRelocate) != 0) {
                section_p->rel_sec_idx = section_idx;
            } else {
                section_p->sec_idx = section_idx;
            }

            return lookup_sections[i].type;
        }
    }

    if(strcmp(name, ".symtab") == 0) {
        FURI_LOG_D(TAG, "Found .symtab section");
        fap->elf.symbol_table = section_header->sh_offset;
        fap->elf.symbol_count = section_header->sh_size / sizeof(Elf32_Sym);
        return SectionTypeSymTab;
    } else if(strcmp(name, ".strtab") == 0) {
        FURI_LOG_D(TAG, "Found .strtab section");
        fap->elf.symbol_table_strings = section_header->sh_offset;
        return SectionTypeStrTab;
    } else if(strcmp(name, ".fapmeta") == 0) {
        FURI_LOG_D(TAG, "Found .fapmeta section");
        if(flipper_application_load_metadata(fap, section_header)) {
            return SectionTypeFappManifest;
        } else {
            return SectionTypeERROR;
        }
    } else if(strcmp(name, ".gnu_debuglink") == 0) {
        FURI_LOG_D(TAG, "Found .gnu_debuglink section");
        if(flipper_application_load_debug_link(fap, section_header)) {
            return SectionTypeDebugLink;
        } else {
            return SectionTypeERROR;
        }
    }

    return SectionTypeUnused;
}

/**************************************************************************************************/
/************************************ External FAP interfaces *************************************/
/**************************************************************************************************/

bool flipper_application_load_elf_headers(FlipperApplication* fap, const char* path) {
    Elf32_Ehdr h;
    Elf32_Shdr sH;
    ELFFile* elf = &fap->elf;

    if(!storage_file_open(fap->fd, path, FSAM_READ, FSOM_OPEN_EXISTING) ||
       !storage_file_seek(fap->fd, 0, true) ||
       storage_file_read(fap->fd, &h, sizeof(h)) != sizeof(h) ||
       !storage_file_seek(fap->fd, h.e_shoff + h.e_shstrndx * sizeof(sH), true) ||
       storage_file_read(fap->fd, &sH, sizeof(Elf32_Shdr)) != sizeof(Elf32_Shdr)) {
        return false;
    }

    elf->entry = h.e_entry;
    elf->sections_count = h.e_shnum;
    elf->section_table = h.e_shoff;
    elf->section_table_strings = sH.sh_offset;
    return true;
}

bool flipper_application_load_manifest(FlipperApplication* fap) {
    bool result = false;
    ELFFile* elf = &fap->elf;
    string_t name;
    string_init(name);
    ELFSectionDict_init(elf->sections);

    FURI_LOG_D(TAG, "Looking for manifest section");
    for(size_t section_idx = 1; section_idx < elf->sections_count; section_idx++) {
        Elf32_Shdr section_header;

        string_reset(name);
        if(!elf_read_section_data(fap, section_idx, &section_header, name)) {
            break;
        }

        if(string_cmp(name, ".fapmeta") == 0) {
            if(flipper_application_load_metadata(fap, &section_header)) {
                FURI_LOG_D(TAG, "Load manifest done");
                result = true;
                break;
            } else {
                break;
            }
        }
    }

    string_clear(name);
    return result;
}

bool flipper_application_load_section_table(FlipperApplication* fap) {
    SectionType loaded_sections = SectionTypeERROR;
    ELFFile* elf = &fap->elf;
    string_t name;
    string_init(name);
    ELFSectionDict_init(elf->sections);

    fap->state.mmap_entry_count = 0;

    FURI_LOG_D(TAG, "Scan ELF indexs...");
    for(size_t section_idx = 1; section_idx < elf->sections_count; section_idx++) {
        Elf32_Shdr section_header;

        string_reset(name);
        if(!elf_read_section_data(fap, section_idx, &section_header, name)) {
            return false;
        }

        FURI_LOG_D(TAG, "Preloading data for section #%d %s", section_idx, string_get_cstr(name));
        SectionType section_type =
            flipper_application_preload_section(fap, section_idx, &section_header, name);

        if((section_type & SectionTypeGdbSection) != 0) {
            fap->state.mmap_entry_count++;
        }

        if(section_type == SectionTypeERROR) {
            loaded_sections = SectionTypeERROR;
            break;
        }
    }

    string_clear(name);
    FURI_LOG_D(TAG, "Load symbols done");

    ELFSectionDict_it_t it;
    for(ELFSectionDict_it(it, elf->sections); !ELFSectionDict_end_p(it); ELFSectionDict_next(it)) {
        const ELFSectionDict_itref_t* itref = ELFSectionDict_cref(it);
        FURI_LOG_D(
            TAG,
            "%s: %d %d",
            string_get_cstr(itref->key),
            itref->value.sec_idx,
            itref->value.rel_sec_idx);
    }

    return IS_FLAGS_SET(loaded_sections, SectionTypeValid);
}

FlipperApplicationLoadStatus flipper_application_load_sections(FlipperApplication* fap) {
    FlipperApplicationLoadStatus status = FlipperApplicationLoadStatusUnspecifiedError;
    RelocationAddressCache_init(fap->elf.relocation_cache);
    size_t start = furi_get_tick();

    FURI_LOG_D(
        TAG, "Relocation cache size: %u", RelocationAddressCache_size(fap->elf.relocation_cache));
    RelocationAddressCache_clear(fap->elf.relocation_cache);
    FURI_LOG_I(TAG, "Loaded in %ums", (size_t)(furi_get_tick() - start));

    return status;
}

void flipper_application_free_elf_data(ELFFile* elf) {
    UNUSED(elf);
    ELFSectionDict_it_t it;
    for(ELFSectionDict_it(it, elf->sections); !ELFSectionDict_end_p(it); ELFSectionDict_next(it)) {
        const ELFSectionDict_itref_t* itref = ELFSectionDict_cref(it);
        if(itref->value.data) {
            aligned_free(itref->value.data);
        }
    }

    ELFSectionDict_clear(elf->sections);
}