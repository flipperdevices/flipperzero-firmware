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
    NfcDictTypeUser,
    NfcDictTypeSystem,
    NfcDictTypeUnitTest,

    NfcDictTypeNum,
} NfcDictType;


typedef enum {
    NfcDictModeOpenExisting,
    NfcDictModeOpenAlways,
} NfcDictMode;

typedef struct NfcDict NfcDict;

bool nfc_dict_check_presence(NfcDictType dict_type);

/** Allocate NfcDict instance
 *
 * @param[in]  dict_type  The dictionary type
 *
 * @return     NfcDict instance
 */
NfcDict* nfc_dict_alloc(NfcDictType dict_type);

/** Free NfcDict instance
 *
 * @param      dict  NfcDict instance
 */
void nfc_dict_free(NfcDict* dict);

/** Get total keys count
 *
 * @param      dict  NfcDict instance
 *
 * @return     total keys count
 */
uint32_t nfc_dict_get_total_keys(NfcDict* dict);

/** Rewind to the beginning
 *
 * @param      dict  NfcDict instance
 *
 * @return     true on success
 */
bool nfc_dict_rewind(NfcDict* dict);

bool nfc_dict_is_key_present(NfcDict* dict, const MfClassicKey* key);

bool nfc_dict_get_next_key(NfcDict* dict, MfClassicKey* key);

bool nfc_dict_add_key(NfcDict* dict, const MfClassicKey* key);

bool nfc_dict_delete_key(NfcDict* dict, const MfClassicKey* key);

#ifdef __cplusplus
}
#endif
