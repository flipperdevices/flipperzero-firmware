#pragma once

#include <stdbool.h>
#include <storage/storage.h>
#include <lib/flipper_format/flipper_format.h>
#include <lib/toolbox/stream/file_stream.h>
#include <lib/toolbox/stream/buffered_file_stream.h>
#include <lib/nfc/protocols/mf_classic/mf_classic.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MfDictTypeUser,
    MfDictTypeSystem,
    MfDictTypeUnitTest,
} MfDictType;

typedef struct MfDict MfDict;

bool mf_dict_check_presence(MfDictType dict_type);

/** Allocate MfDict instance
 *
 * @param[in]  dict_type  The dictionary type
 *
 * @return     MfDict instance
 */
MfDict* mf_dict_alloc(MfDictType dict_type);

/** Free MfDict instance
 *
 * @param      dict  MfDict instance
 */
void mf_dict_free(MfDict* dict);

/** Get total keys count
 *
 * @param      dict  MfDict instance
 *
 * @return     total keys count
 */
uint32_t mf_dict_get_total_keys(MfDict* dict);

/** Rewind to the beginning
 *
 * @param      dict  MfDict instance
 *
 * @return     true on success
 */
bool mf_dict_rewind(MfDict* dict);

bool mf_dict_is_key_present(MfDict* dict, uint8_t* key);

bool mf_dict_is_key_present_str(MfDict* dict, FuriString* key);

bool mf_dict_get_next_key(MfDict* dict, MfClassicKey* key);

bool mf_dict_get_next_key_str(MfDict* dict, FuriString* key);

/** Get key at target offset as uint64_t
 *
 * @param      dict    MfDict instance
 * @param[out] key     Pointer to the uint64_t key
 * @param[in]  target  Target offset from current position
 *
 * @return     true on success
 */
bool mf_dict_get_key_at_index(MfDict* dict, uint64_t* key, uint32_t target);

/** Get key at target offset as string_t
 *
 * @param      dict    MfDict instance
 * @param[out] key     Found key destination buffer
 * @param[in]  target  Target offset from current position
 *
 * @return     true on success
 */
bool mf_dict_get_key_at_index_str(MfDict* dict, FuriString* key, uint32_t target);

bool mf_dict_add_key(MfDict* dict, uint8_t* key);

/** Add string representation of the key
 *
 * @param      dict  MfDict instance
 * @param[in]  key   String representation of the key
 *
 * @return     true on success
 */
bool mf_dict_add_key_str(MfDict* dict, FuriString* key);

bool mf_dict_find_index(MfDict* dict, uint8_t* key, uint32_t* target);

bool mf_dict_find_index_str(MfDict* dict, FuriString* key, uint32_t* target);

/** Delete key at target offset
 *
 * @param      dict    MfDict instance
 * @param[in]  target  Target offset from current position
 *
 * @return     true on success
 */
bool mf_dict_delete_index(MfDict* dict, uint32_t target);

#ifdef __cplusplus
}
#endif
