#pragma once

#include <stdbool.h>
#include <storage/storage.h>
#include <flipper_format/flipper_format.h>
#include <toolbox/stream/file_stream.h>
#include <toolbox/stream/buffered_file_stream.h>
#include <nfc/protocols/mf_classic/mf_classic.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MfDictTypeUser,
    MfDictTypeSystem,
    MfDictTypeUnitTest,

    MfDictTypeNum,
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

bool mf_dict_is_key_present(MfDict* dict, const MfClassicKey* key);

bool mf_dict_get_next_key(MfDict* dict, MfClassicKey* key);

bool mf_dict_add_key(MfDict* dict, const MfClassicKey* key);

bool mf_dict_delete_key(MfDict* dict, const MfClassicKey* key);

#ifdef __cplusplus
}
#endif
