#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MfClassicDictAttack MfClassicDictAttack;

MfClassicDictAttack* mf_classic_dict_attack_alloc();

void mf_classic_dict_attack_free(MfClassicDictAttack* instance);

#ifdef __cplusplus
}
#endif
