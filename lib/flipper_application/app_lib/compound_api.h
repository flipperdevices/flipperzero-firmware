#pragma once

#include <flipper_application/elf/elf_api_interface.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CompundApiResolver CompundApiResolver;

CompundApiResolver* compound_api_resolver_alloc();

void compound_api_resolver_free(CompundApiResolver* resolver);

void compound_api_resolver_add(CompundApiResolver* resolver, const ElfApiInterface* interface);

const ElfApiInterface* compound_api_resolver_get(CompundApiResolver* resolver);

#ifdef __cplusplus
}
#endif