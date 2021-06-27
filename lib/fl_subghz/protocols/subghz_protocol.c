#include "subghz_protocol.h"

#include "subghz_protocol_came.h"
#include "subghz_protocol_cfm.h"
#include "subghz_protocol_keeloq.h"
#include "subghz_protocol_nice_flo.h"
#include "subghz_protocol_nice_flor_s.h"
#include "subghz_protocol_princeton.h"

#include <furi.h>

struct SubGhzProtocol {
    SubGhzProtocolCame* came;
    SubGhzProtocolKeeloq* keeloq;
    SubGhzProtocolPrinceton* princeton;
    SubGhzProtocolNiceFlo* nice_flo;
    SubGhzProtocolNiceFlorS* nice_flor_s;
};

SubGhzProtocol* subghz_protocol_alloc() {
    SubGhzProtocol* instance = furi_alloc(sizeof(SubGhzProtocol));
    instance->came = subghz_protocol_came_alloc();
    instance->keeloq = subghz_protocol_keeloq_alloc();
    instance->princeton = subghz_protocol_princeton_alloc();
    instance->nice_flo = subghz_protocol_nice_flo_alloc();
    instance->nice_flor_s = subghz_protocol_nice_flor_s_alloc();
    return instance;
}

void subghz_protocol_free(SubGhzProtocol* instance) {
    furi_assert(instance);

    subghz_protocol_came_free(instance->came);
    subghz_protocol_keeloq_free(instance->keeloq);
    subghz_protocol_princeton_free(instance->princeton);
    subghz_protocol_nice_flo_free(instance->nice_flo);
    subghz_protocol_nice_flor_s_free(instance->nice_flor_s);

    free(instance);
}

void subghz_protocol_reset(SubGhzProtocol* instance) {
}

void subghz_protocol_parse(SubGhzProtocol* instance, LevelPair data) {
    subghz_protocol_came_parse(instance->came, data);
    subghz_protocol_keeloq_parse(instance->keeloq, data);
    subghz_protocol_princeton_parse(instance->princeton, data);
    subghz_protocol_nice_flo_parse(instance->nice_flo, data);
    subghz_protocol_nice_flor_s_parse(instance->nice_flor_s, data);
}
