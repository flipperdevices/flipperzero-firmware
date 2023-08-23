#pragma once

#include <stdint.h>
#include <gui/view.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NewDictAttack NewDictAttack;

typedef enum {
    NewDictAttackEventSkipPressed,
} NewDictAttackEvent;

typedef void (*NewDictAttackCallback)(NewDictAttackEvent event, void* context);

NewDictAttack* new_dict_attack_alloc();

void new_dict_attack_free(NewDictAttack* instance);

void new_dict_attack_reset(NewDictAttack* instance);

View* new_dict_attack_get_view(NewDictAttack* instance);

void new_dict_attack_set_callback(
    NewDictAttack* instance,
    NewDictAttackCallback callback,
    void* context);

void new_dict_attack_set_header(NewDictAttack* instance, const char* header);

void new_dict_attack_set_card_state(NewDictAttack* instance, bool detected);

void new_dict_attack_set_sectors_read(NewDictAttack* instance, size_t sectors_read);

void new_dict_attack_set_keys_found(NewDictAttack* instance, size_t keys_found);

void new_dict_attack_set_current_sector(NewDictAttack* instance, size_t curr_sec);

void new_dict_attack_set_total_dict_keys(NewDictAttack* instance, size_t dict_keys_total);

void new_dict_attack_set_current_dict_key(NewDictAttack* instance, size_t cur_key_num);

void new_dict_attack_set_key_attack(NewDictAttack* instance, size_t sector);

void new_dict_attack_reset_key_attack(NewDictAttack* instance);

#ifdef __cplusplus
}
#endif
