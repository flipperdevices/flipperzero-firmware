#include "registry.h"

#include "came.h"

const SubGhzProtocol* subghz_protocol_registry[] = {
    &subghz_protocol_came,
};

const SubGhzProtocol* subghz_protocol_registry_get_by_name(const char* name) {
    for(size_t i = 0; i < subghz_protocol_registry_count(); i++) {
        if(strcmp(name, subghz_protocol_registry[i]->specification.name) == 0) {
            return subghz_protocol_registry[i];
        }
    }
    return NULL;
}

const SubGhzProtocol* subghz_protocol_registry_get_by_index(size_t index) {
    if(index < subghz_protocol_registry_count()) {
        return subghz_protocol_registry[index];
    } else {
        return NULL;
    }
}

size_t subghz_protocol_registry_count() {
    return COUNT_OF(subghz_protocol_registry);
}
