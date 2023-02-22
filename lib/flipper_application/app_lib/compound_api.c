#include "compound_api.h"

#include <m-list.h>
#include <m-algo.h>

LIST_DEF(ElfApiInterfaceList, const ElfApiInterface*, M_POD_OPLIST)
#define M_OPL_ElfApiInterfaceList_t() LIST_OPLIST(ElfApiInterfaceList, M_POD_OPLIST)

struct CompundApiResolver {
    ElfApiInterface api_interface;
    ElfApiInterfaceList_t interfaces;
};

static bool compound_api_resolver_callback(
    const ElfApiInterface* interface,
    const char* name,
    Elf32_Addr* address) {
    CompundApiResolver* resolver = (CompundApiResolver*)interface;
    for
        M_EACH(interface, resolver->interfaces, ElfApiInterfaceList_t) {
            if((*interface)->resolver_callback(*interface, name, address)) {
                return true;
            }
        }
    return false;
}

CompundApiResolver* compound_api_resolver_alloc() {
    CompundApiResolver* resolver = malloc(sizeof(CompundApiResolver));
    resolver->api_interface.api_version_major = 0;
    resolver->api_interface.api_version_minor = 0;
    resolver->api_interface.resolver_callback = &compound_api_resolver_callback;
    ElfApiInterfaceList_init(resolver->interfaces);
    return resolver;
}

void compound_api_resolver_free(CompundApiResolver* resolver) {
    ElfApiInterfaceList_clear(resolver->interfaces);
    free(resolver);
}

void compound_api_resolver_add(CompundApiResolver* resolver, const ElfApiInterface* interface) {
    if(ElfApiInterfaceList_empty_p(resolver->interfaces)) {
        resolver->api_interface.api_version_major = interface->api_version_major;
        resolver->api_interface.api_version_minor = interface->api_version_minor;
    }
    ElfApiInterfaceList_push_back(resolver->interfaces, interface);
}

const ElfApiInterface* compound_api_resolver_get(CompundApiResolver* resolver) {
    return &resolver->api_interface;
}
