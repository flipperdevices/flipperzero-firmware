#include "mf_classic_dict_attack.h"

struct MfClassicDictAttack {

};

MfClassicDictAttack* mf_classic_dict_attack_alloc() {
    MfClassicDictAttack* instance = malloc(sizeof(MfClassicDictAttack));

    return instance;
}

void mf_classic_dict_attack_free(MfClassicDictAttack* instance) {
    furi_assert(instance);

    free(instance);
}
