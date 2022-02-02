#include "registry.h"

#include "came.h"
#include "gate_tx.h"
#include "nice_flo.h"
#include "came_twee.h"
#include "came_atomo.h"
#include "faac_slh.h"

const SubGhzProtocol* subghz_protocol_registry[] = {
    &subghz_protocol_came,
    &subghz_protocol_gate_tx,
    &subghz_protocol_nice_flo,
    &subghz_protocol_came_twee,
    &subghz_protocol_came_atomo,
    &subghz_protocol_faac_slh,
};

const SubGhzProtocol* subghz_protocol_registry_get_by_name(const char* name) {
    for(size_t i = 0; i < subghz_protocol_registry_count(); i++) {
        if(strcmp(name, subghz_protocol_registry[i]->name) == 0) {
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
