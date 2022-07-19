#pragma once
#include <stdint.h>
#include <gui/view.h>
#include <gui/modules/widget.h>

#include <lib/nfc/protocols/mifare_classic.h>

typedef struct MfClassicDictAttack MfClassicDictAttack;

typedef void (*MfClassicDictAttackCallback)(void* context);

MfClassicDictAttack* mf_classic_dict_attack_alloc();

void mf_classic_dict_attack_free(MfClassicDictAttack* dict_attack);

void mf_classic_dict_attack_reset(MfClassicDictAttack* dict_attack);

View* mf_classic_dict_attack_get_view(MfClassicDictAttack* dict_attack);

void mf_classic_dict_attack_set_callback(
    MfClassicDictAttack* dict_attack,
    MfClassicDictAttackCallback callback,
    void* context);

void mf_classic_dict_attack_set_header(MfClassicDictAttack* dict_attack, const char* header);

void mf_classic_dict_attack_set_card_detected(MfClassicDictAttack* dict_attack, MfClassicType type);

void mf_classic_dict_attack_set_card_removed(MfClassicDictAttack* dict_attack);

void mf_classic_dict_attack_set_sector_read(MfClassicDictAttack* dict_attack, uint8_t sec_read);

void mf_classic_dict_attack_set_keys_found(MfClassicDictAttack* dict_attack, uint8_t keys_found);

void mf_classic_dict_attack_set_current_sector(MfClassicDictAttack* dict_attack, uint8_t curr_sec);

void mf_classic_dict_attack_inc_current_sector(MfClassicDictAttack* dict_attack);

void mf_classic_dict_attack_inc_keys_found(MfClassicDictAttack* dict_attack);
