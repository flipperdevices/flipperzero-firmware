#pragma once

#include <stdint.h>

// #define RFID_125_PROTOCOL

typedef struct FuzzerPayload FuzzerPayload;

typedef uint8_t FuzzerProtocolsID;

typedef enum {
    FuzzerAttackIdDefaultValues = 0,
    FuzzerAttackIdLoadFile,
    FuzzerAttackIdLoadFileCustomUids,
    FuzzerAttackIdBFCustomerID,
} FuzzerAttackId;

struct FuzzerPayload {
    uint8_t* data;
    uint8_t data_size;
};

/**
 * Allocate FuzzerPayload
 * 
 * @return FuzzerPayload* pointer to FuzzerPayload
 */
FuzzerPayload* fuzzer_payload_alloc();

/**
 * Free FuzzerPayload
 * 
 * @param instance Pointer to a FuzzerPayload
 */
void fuzzer_payload_free(FuzzerPayload*);

/**
 * Get maximum length of UID among all supported protocols
 * @return Maximum length of UID
 */
uint8_t fuzzer_proto_get_max_data_size();

/**
 * Get recomended/default emulation time
 * @return Default emulation time
 */
uint8_t fuzzer_proto_get_def_emu_time();

/**
 * Get recomended/default idle time
 * @return Default idle time
 */
uint8_t fuzzer_proto_get_def_idle_time();

/**
 * Get protocol name based on its index
 * @param index protocol index
 * @return pointer to a string containing the name
 */
const char* fuzzer_proto_get_name(FuzzerProtocolsID index);

/**
 * Get number of protocols
 * @return number of protocols
 */
uint8_t fuzzer_proto_get_count_of_protocols();

/**
 * Get menu label based on its index
 * @param index menu index
 * @return pointer to a string containing the menu label
 */
const char* fuzzer_proto_get_menu_label(uint8_t index);

/**
 * Get FuzzerAttackId based on its index
 * @param index menu index
 * @return FuzzerAttackId
 */
FuzzerAttackId fuzzer_proto_get_attack_id_by_index(uint8_t index);

/**
 * Get number of menu items
 * @return number of menu items
 */
uint8_t fuzzer_proto_get_count_of_menu_items();