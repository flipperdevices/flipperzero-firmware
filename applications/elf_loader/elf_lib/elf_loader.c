#include "elf_loader.h"
#include "elf.h"
#include "../elf_manifest/elf_manifest.h"
#include "../elf_cpp/elf_hashtable.h"

#include <furi.h>
#include <string.h>

#define IS_FLAGS_SET(v, m) ((v & m) == m)
#define SECTION_OFFSET(e, n) (e->section_table + n * sizeof(Elf32_Shdr))

#define TAG "elf-loader"

#ifndef DOX

/**
 * Callable elf entry type
 */
typedef int32_t(entry_t)(void*);

typedef struct {
    void* data;
    int sec_idx;
    off_t rel_sec_idx;
} ELFSection_t;

typedef struct {
    File* fd;

    size_t sections;
    off_t section_table;
    off_t section_table_strings;

    size_t symbol_count;
    off_t symbol_table;
    off_t symbol_table_strings;
    off_t entry;

    size_t app_stack_size;

    ELFSection_t text;
    ELFSection_t rodata;
    ELFSection_t data;
    ELFSection_t bss;

    void* stack;

    ELFResolver resolver;
} ELFExec_t;

#endif

typedef enum {
    FoundERROR = 0,
    FoundSymTab = (1 << 0),
    FoundStrTab = (1 << 2),
    FoundText = (1 << 3),
    FoundRodata = (1 << 4),
    FoundData = (1 << 5),
    FoundBss = (1 << 6),
    FoundRelText = (1 << 7),
    FoundRelRodata = (1 << 8),
    FoundRelData = (1 << 9),
    FoundRelBss = (1 << 10),
    FoundValid = FoundSymTab | FoundStrTab,
    FoundExec = FoundValid | FoundText,
    FoundAll = FoundSymTab | FoundStrTab | FoundText | FoundRodata | FoundData | FoundBss |
               FoundRelText | FoundRelRodata | FoundRelData | FoundRelBss
} FindFlags_t;

static int read_section_name(ELFExec_t* e, off_t off, char* buf, size_t max) {
    int ret = -1;
    off_t offset = e->section_table_strings + off;
    off_t old = storage_file_tell(e->fd);
    if(storage_file_seek(e->fd, offset, true))
        if(storage_file_read(e->fd, buf, max) == 0) ret = 0;
    (void)storage_file_seek(e->fd, old, true);
    return ret;
}

static int read_symbol_name(ELFExec_t* e, off_t off, char* buf, size_t max) {
    int ret = -1;
    off_t offset = e->symbol_table_strings + off;
    off_t old = storage_file_tell(e->fd);
    if(storage_file_seek(e->fd, offset, true))
        if(storage_file_read(e->fd, buf, max) == 0) ret = 0;
    (void)storage_file_seek(e->fd, old, true);
    return ret;
}

static void free_section(ELFSection_t* s) {
    if(s->data) aligned_free(s->data);
    s->data = NULL;
}

#if 0
static uint32_t swabo(uint32_t hl) {
    return (
        (((hl) >> 24)) | /* */
        (((hl) >> 8) & 0x0000ff00) | /* */
        (((hl) << 8) & 0x00ff0000) | /* */
        (((hl) << 24))); /* */
}
#endif

static void dump_data(uint8_t* data, size_t size) {
#if 0
	int i = 0;
	while (i < size) {
		if ((i & 0xf) == 0)
			FURI_LOG_D(TAG, "  %04X: ", i);
		FURI_LOG_D(TAG, "%08x ", swabo(*((uint32_t*)(data + i))));
		i += sizeof(uint32_t);
	}
	FURI_LOG_D(TAG, "");
#else
    UNUSED(data);
    UNUSED(size);
#endif
}

static int load_section_data(ELFExec_t* e, ELFSection_t* s, Elf32_Shdr* h) {
    if(!h->sh_size) {
        FURI_LOG_I(TAG, " No data for section");
        return 0;
    }

    s->data = aligned_malloc(h->sh_size, h->sh_addralign);

    if(!storage_file_seek(e->fd, h->sh_offset, true)) {
        FURI_LOG_E(TAG, "    seek fail");
        free_section(s);
        return -1;
    }

    uint16_t read = storage_file_read(e->fd, s->data, h->sh_size);
    if(read != h->sh_size) {
        FURI_LOG_E(TAG, "     read %d, expected %d", read, h->sh_size);
        FURI_LOG_E(TAG, "     read data fail '%s'", storage_file_get_error_desc(e->fd));
        return -1;
    }

    /* FURI_LOG_D(TAG, "DATA: "); */
    dump_data(s->data, h->sh_size);

    FURI_LOG_I(TAG, "0x%X", s->data);
    return 0;
}

static int read_section_header(ELFExec_t* e, int n, Elf32_Shdr* h) {
    off_t offset = SECTION_OFFSET(e, n);
    if(!storage_file_seek(e->fd, offset, true)) return -1;
    if(storage_file_read(e->fd, h, sizeof(Elf32_Shdr)) != sizeof(Elf32_Shdr)) return -1;
    return 0;
}

static int read_section(ELFExec_t* e, int n, Elf32_Shdr* h, char* name, size_t nlen) {
    if(read_section_header(e, n, h) != 0) return -1;
    if(h->sh_name) return read_section_name(e, h->sh_name, name, nlen);
    return 0;
}

static int read_symbol(ELFExec_t* e, int n, Elf32_Sym* sym, char* name, size_t nlen) {
    int ret = -1;
    off_t old = storage_file_tell(e->fd);
    off_t pos = e->symbol_table + n * sizeof(Elf32_Sym);
    if(storage_file_seek(e->fd, pos, true))
        if(storage_file_read(e->fd, sym, sizeof(Elf32_Sym)) == sizeof(Elf32_Sym)) {
            if(sym->st_name)
                ret = read_symbol_name(e, sym->st_name, name, nlen);
            else {
                Elf32_Shdr shdr;
                ret = read_section(e, sym->st_shndx, &shdr, name, nlen);
            }
        }
    (void)storage_file_seek(e->fd, old, true);
    return ret;
}

static const char* type_to_str(int symt) {
#define STRCASE(name) \
    case name:        \
        return #name;
    switch(symt) {
        STRCASE(R_ARM_NONE)
        STRCASE(R_ARM_ABS32)
        STRCASE(R_ARM_THM_PC22)
        STRCASE(R_ARM_THM_JUMP24)
    default:
        return "R_<unknow>";
    }
#undef STRCASE
}

static void relocate_jmp_call(Elf32_Addr relAddr, int type, Elf32_Addr symAddr) {
    UNUSED(type);
    uint16_t upper_insn = ((uint16_t*)relAddr)[0];
    uint16_t lower_insn = ((uint16_t*)relAddr)[1];
    uint32_t S = (upper_insn >> 10) & 1;
    uint32_t J1 = (lower_insn >> 13) & 1;
    uint32_t J2 = (lower_insn >> 11) & 1;

    int32_t offset = (S << 24) | /* S     -> offset[24] */
                     ((~(J1 ^ S) & 1) << 23) | /* J1    -> offset[23] */
                     ((~(J2 ^ S) & 1) << 22) | /* J2    -> offset[22] */
                     ((upper_insn & 0x03ff) << 12) | /* imm10 -> offset[12:21] */
                     ((lower_insn & 0x07ff) << 1); /* imm11 -> offset[1:11] */
    if(offset & 0x01000000) offset -= 0x02000000;

    offset += symAddr - relAddr;

    S = (offset >> 24) & 1;
    J1 = S ^ (~(offset >> 23) & 1);
    J2 = S ^ (~(offset >> 22) & 1);

    upper_insn = ((upper_insn & 0xf800) | (S << 10) | ((offset >> 12) & 0x03ff));
    ((uint16_t*)relAddr)[0] = upper_insn;

    lower_insn = ((lower_insn & 0xd000) | (J1 << 13) | (J2 << 11) | ((offset >> 1) & 0x07ff));
    ((uint16_t*)relAddr)[1] = lower_insn;
}

static int relocate_symbol(Elf32_Addr relAddr, int type, Elf32_Addr symAddr) {
    switch(type) {
    case R_ARM_ABS32:
        *((uint32_t*)relAddr) += symAddr;
        FURI_LOG_D(TAG, "  R_ARM_ABS32 relocated is 0x%08X", (unsigned int)*((uint32_t*)relAddr));
        break;
    case R_ARM_THM_PC22:
    case R_ARM_THM_JUMP24:
        relocate_jmp_call(relAddr, type, symAddr);
        FURI_LOG_D(
            TAG, "  R_ARM_THM_CALL/JMP relocated is 0x%08X", (unsigned int)*((uint32_t*)relAddr));
        break;
    default:
        FURI_LOG_D(TAG, "  Undefined relocation %d", type);
        return -1;
    }
    return 0;
}

static ELFSection_t* section_of(ELFExec_t* e, int index) {
#define IFSECTION(sec, i)                     \
    do {                                      \
        if((sec).sec_idx == i) return &(sec); \
    } while(0)
    IFSECTION(e->text, index);
    IFSECTION(e->rodata, index);
    IFSECTION(e->data, index);
    IFSECTION(e->bss, index);
#undef IFSECTION
    return NULL;
}

static Elf32_Addr address_of(ELFExec_t* e, Elf32_Sym* sym, const char* sName) {
    if(sym->st_shndx == SHN_UNDEF) {
        Elf32_Addr addr = 0;
        if(e->resolver(sName, &addr)) {
            return addr;
        }
    } else {
        ELFSection_t* symSec = section_of(e, sym->st_shndx);
        if(symSec) return ((Elf32_Addr)symSec->data) + sym->st_value;
    }
    FURI_LOG_D(TAG, "  Can not find address for symbol %s", sName);
    return 0xffffffff;
}

static int relocate(ELFExec_t* e, Elf32_Shdr* h, ELFSection_t* s, const char* name) {
    UNUSED(name);

    if(s->data) {
        Elf32_Rel rel;
        size_t relEntries = h->sh_size / sizeof(rel);
        size_t relCount;
        (void)storage_file_seek(e->fd, h->sh_offset, true);
        FURI_LOG_D(TAG, " Offset   Info     Type             Name");

        int relocate_result = 0;
        string_t name;
        const size_t max_name_size = 256;
        string_init(name);
        string_reserve(name, max_name_size + 1);

        for(relCount = 0; relCount < relEntries; relCount++) {
            if(storage_file_read(e->fd, &rel, sizeof(rel)) == sizeof(rel)) {
                Elf32_Sym sym;
                Elf32_Addr symAddr;

                int symEntry = ELF32_R_SYM(rel.r_info);
                int relType = ELF32_R_TYPE(rel.r_info);
                Elf32_Addr relAddr = ((Elf32_Addr)s->data) + rel.r_offset;

                read_symbol(e, symEntry, &sym, m_str1ng_get_cstr(name), max_name_size);
                FURI_LOG_D(
                    TAG,
                    " %08X %08X %-16s %s",
                    (unsigned int)rel.r_offset,
                    (unsigned int)rel.r_info,
                    type_to_str(relType),
                    string_get_cstr(name));

                symAddr = address_of(e, &sym, string_get_cstr(name));
                if(symAddr != 0xffffffff) {
                    FURI_LOG_D(
                        TAG,
                        "  symAddr=%08X relAddr=%08X",
                        (unsigned int)symAddr,
                        (unsigned int)relAddr);
                    if(relocate_symbol(relAddr, relType, symAddr) == -1) {
                        relocate_result = -1;
                    }
                } else {
                    FURI_LOG_D(TAG, "  No symbol address of %s", string_get_cstr(name));
                    relocate_result = -1;
                }
            }
        }

        string_clear(name);
        return relocate_result;
    } else
        FURI_LOG_I(TAG, "Section not loaded");

    return -1;
}

int place_info(ELFExec_t* e, Elf32_Shdr* sh, const char* name, int n) {
    FURI_LOG_I(TAG, "Name: %s", name);

    if(strcmp(name, ".symtab") == 0) {
        e->symbol_table = sh->sh_offset;
        e->symbol_count = sh->sh_size / sizeof(Elf32_Sym);
        return FoundSymTab;
    } else if(strcmp(name, ".strtab") == 0) {
        e->symbol_table_strings = sh->sh_offset;
        return FoundStrTab;
    } else if(strcmp(name, ".text") == 0) {
        if(load_section_data(e, &e->text, sh) == -1) return FoundERROR;
        e->text.sec_idx = n;
        return FoundText;
    } else if(strcmp(name, ".rodata") == 0) {
        if(load_section_data(e, &e->rodata, sh) == -1) return FoundERROR;
        e->rodata.sec_idx = n;
        return FoundRodata;
    } else if(strcmp(name, ".data") == 0) {
        if(load_section_data(e, &e->data, sh) == -1) return FoundERROR;
        e->data.sec_idx = n;
        return FoundData;
    } else if(strcmp(name, ".bss") == 0) {
        if(load_section_data(e, &e->bss, sh) == -1) return FoundERROR;
        e->bss.sec_idx = n;
        return FoundBss;
    } else if(strcmp(name, ".rel.text") == 0) {
        e->text.rel_sec_idx = n;
        return FoundRelText;
    } else if(strcmp(name, ".rel.rodata") == 0) {
        e->rodata.rel_sec_idx = n;
        return FoundRelText;
    } else if(strcmp(name, ".rel.data") == 0) {
        e->data.rel_sec_idx = n;
        return FoundRelText;
    }
    /* BSS not need relocation */
#if 0
	else if (strcmp(name, ".rel.bss") == 0) {
		e->bss.rel_sec_idx = n;
		return FoundRelText;
	}
#endif
    return 0;
}

static int load_symbols(ELFExec_t* e) {
    size_t n;
    int found = 0;
    FURI_LOG_I(TAG, "Scan ELF indexs...");
    for(n = 1; n < e->sections; n++) {
        Elf32_Shdr section_header;
        char name[33] = "<unamed>";
        if(read_section_header(e, n, &section_header) != 0) {
            FURI_LOG_E(TAG, "Error reading section");
            return -1;
        }
        if(section_header.sh_name)
            read_section_name(e, section_header.sh_name, name, sizeof(name));
        FURI_LOG_D(TAG, "Examining section %d %s", n, name);
        found |= place_info(e, &section_header, name, n);
        if(IS_FLAGS_SET(found, FoundAll)) return FoundAll;
    }
    FURI_LOG_I(TAG, "Load symbols done");
    return found;
}

bool load_metadata(ELFExec_t* e, ElfManifest* manifest) {
    size_t n;
    int found = false;
    FURI_LOG_I(TAG, "Scan ELF index for meta...");
    char name[33] = "";

    ELFSection_t meta_section = {0};

    for(n = 1; n < e->sections; n++) {
        Elf32_Shdr section_header;
        if(read_section_header(e, n, &section_header) != 0) {
            FURI_LOG_E(TAG, "Error reading section");
            return -1;
        }
        if(section_header.sh_name)
            read_section_name(e, section_header.sh_name, name, sizeof(name));

        FURI_LOG_D(TAG, "Examining section %d %s", n, name);
        if(strcmp(name, ".fapmeta") == 0) {
            // LOAD
            FURI_LOG_E(TAG, "FOUND META SECTION");
            if(section_header.sh_size < sizeof(ElfManifestV1)) {
                FURI_LOG_E(
                    TAG,
                    "Metadata section too small (%d < %d)",
                    section_header.sh_size,
                    sizeof(ElfManifestV1));
            }

            if(load_section_data(e, &meta_section, &section_header) != 0) {
                FURI_LOG_E(TAG, "Failed to load meta section data");
                break;
            }

            memcpy(manifest, meta_section.data, sizeof(ElfManifest));
            found = true;
            break;
        }
    }

    free_section(&meta_section);
    FURI_LOG_I(TAG, "Load meta done");
    return found;
}

static int init_elf(ELFExec_t* e, File* f) {
    Elf32_Ehdr h;
    Elf32_Shdr sH;

    memset(e, 0, sizeof(ELFExec_t));

    if(storage_file_read(f, &h, sizeof(h)) != sizeof(h)) return -1;

    e->fd = f;

    if(!storage_file_seek(e->fd, h.e_shoff + h.e_shstrndx * sizeof(sH), true)) return -1;
    if(storage_file_read(e->fd, &sH, sizeof(Elf32_Shdr)) != sizeof(Elf32_Shdr)) return -1;

    e->entry = h.e_entry;
    e->sections = h.e_shnum;
    e->section_table = h.e_shoff;
    e->section_table_strings = sH.sh_offset;

    /* TODO Check ELF validity */
    ElfManifest manifest = {0};
    if(!load_metadata(e, &manifest)) {
        FURI_LOG_E(TAG, "Failed to load metadata");
        return -1;
    }

    /* Validate metadata */
    if(manifest.base.manifest_magic != ELF_MANIFEST_MAGIC) {
        FURI_LOG_E(TAG, "Invalid manifest magic %x", manifest.base.manifest_magic);
        return -1;
    }

    if(manifest.base.manifest_version > ELF_MANIFEST_MAX_SUPPORTED_VERSION) {
        FURI_LOG_E(TAG, "Unsupported metadata version %d", manifest.base.manifest_version);
        return -1;
    }

    if((manifest.base.api_version.major != ELF_API_MAJOR) ||
       (manifest.base.api_version.minor < ELF_API_MINOR)) {
        FURI_LOG_E(
            TAG,
            "API version mismatch (fw %d.%d, elf %d.%d)",
            ELF_API_MAJOR,
            ELF_API_MINOR,
            manifest.base.api_version.major,
            manifest.base.api_version.minor);
        return -1;
    }

    e->app_stack_size = manifest.stack_size;

    return 0;
}

static void free_elf(ELFExec_t* e) {
    free_section(&e->text);
    free_section(&e->rodata);
    free_section(&e->data);
    free_section(&e->bss);
}

static int relocate_section(ELFExec_t* e, ELFSection_t* s, const char* name) {
    FURI_LOG_D(TAG, "Relocating section %s", name);
    if(s->rel_sec_idx) {
        Elf32_Shdr section_header;
        if(read_section_header(e, s->rel_sec_idx, &section_header) == 0)
            return relocate(e, &section_header, s, name);
        else {
            FURI_LOG_E(TAG, "Error reading section header");
            return -1;
        }
    } else
        FURI_LOG_D(TAG, "No relocation index"); /* Not an error */
    return 0;
}

static int relocate_sections(ELFExec_t* e) {
    int reloc_error = 0;

    if(relocate_section(e, &e->text, ".text") == -1) reloc_error = -1;
    if(relocate_section(e, &e->rodata, ".rodata") == -1) reloc_error = -1;
    if(relocate_section(e, &e->data, ".data") == -1) reloc_error = -1;

    return reloc_error;
}

static void arch_jump_to(entry_t entry) {
    // TODO: allocate thread and stack
    entry(NULL);
}

static int loader_jump_to(ELFExec_t* e) {
    if(e->entry) {
        entry_t* entry = (entry_t*)(e->text.data + e->entry);
        // Stack size is @ e->app_stack_size, use it!
        arch_jump_to(entry);
        return 0;
    } else {
        FURI_LOG_I(TAG, "No entry defined.");
        return -1;
    }
}

int loader_exec_elf(const char* path, ELFResolver resolver, Storage* storage) {
    ELFExec_t exec;
    File* file = storage_file_alloc(storage);
    int result = -1;

    do {
        if(!storage_file_open(file, path, FSAM_READ, FSOM_OPEN_EXISTING)) {
            FURI_LOG_D(TAG, "Cannot open %s", path);
            result = -1;
            break;
        }

        if(init_elf(&exec, file) != 0) {
            FURI_LOG_D(TAG, "Invalid elf %s", path);
            result = -1;
            break;
        }

        exec.resolver = resolver;
        if(IS_FLAGS_SET(load_symbols(&exec), FoundValid)) {
            int ret = -1;
            if(relocate_sections(&exec) == 0) {
                FURI_LOG_I(TAG, "Start");
                ret = loader_jump_to(&exec);
            }
            free_elf(&exec);
            result = ret;
            break;
        } else {
            FURI_LOG_I(TAG, "Invalid EXEC");
            result = -1;
            break;
        }
    } while(false);

    storage_file_close(file);
    storage_file_free(file);
    return result;
}
