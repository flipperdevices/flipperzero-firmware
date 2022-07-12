#include <furi.h>
#include <string.h>
#include <gui/gui.h>
#include "elf_addr_resolver.h"

typedef struct {
    const char* name;
    const Elf32_Addr addr;
} ELFAddrTable;

#define ELF_FN(x) \
    { .name = #x, .addr = (Elf32_Addr)x }

ELFAddrTable elf_addr_table[] = {
    ELF_FN(furi_crash),
};

const size_t elf_addr_table_size = COUNT_OF(elf_addr_table);

bool elf_resolve_from_table(const char* name, Elf32_Addr* address) {
    bool result = false;

    for(size_t i = 0; i < elf_addr_table_size; i++) {
        if(strcmp(name, elf_addr_table[i].name) == 0) {
            result = true;
            *address = elf_addr_table[i].addr;
            break;
        }
    }

    return result;
}

bool elf_resolve_printf_only(const char* name, Elf32_Addr* address) {
    if(strcmp(name, "printf") == 0) {
        *address = (Elf32_Addr)printf;
        return true;
    } else {
        return false;
    }
}