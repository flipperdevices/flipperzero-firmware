#pragma once

#include "slix.h"

#include <toolbox/bit_buffer.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SLIX_NXP_MANUFACTURER_CODE (0x04U)

#define SLIX_CMD_EAS_ALARM (0xA5U)
#define SLIX_CMD_GET_NXP_SYSINFO (0xABU)
#define SLIX_CMD_READ_SIGNATURE (0xBDU)

#define SLIX_TYPE_FEATURES_SLIX (SLIX_TYPE_FEATURE_EAS)
#define SLIX_TYPE_FEATURES_SLIX_S                                                   \
    (SLIX_TYPE_FEATURE_READ | SLIX_TYPE_FEATURE_WRITE | SLIX_TYPE_FEATURE_PRIVACY | \
     SLIX_TYPE_FEATURE_DESTROY | SLIX_TYPE_FEATURE_EAS)
#define SLIX_TYPE_FEATURES_SLIX_L \
    (SLIX_TYPE_FEATURE_PRIVACY | SLIX_TYPE_FEATURE_DESTROY | SLIX_TYPE_FEATURE_EAS)
#define SLIX_TYPE_FEATURES_SLIX2                                                       \
    (SLIX_TYPE_FEATURE_READ | SLIX_TYPE_FEATURE_WRITE | SLIX_TYPE_FEATURE_PRIVACY |    \
     SLIX_TYPE_FEATURE_DESTROY | SLIX_TYPE_FEATURE_EAS | SLIX_TYPE_FEATURE_SIGNATURE | \
     SLIX_TYPE_FEATURE_PROTECTION | SLIX_TYPE_FEATURE_LOCK_BITS)

#define SLIX2_FEATURE_FLAGS                                                                       \
    (SLIX_FEATURE_FLAG_UM_PP | SLIX_FEATURE_FLAG_COUNTER | SLIX_FEATURE_FLAG_EAS_ID |             \
     SLIX_FEATURE_FLAG_EAS_PP | SLIX_FEATURE_FLAG_AFI_PP | SLIX_FEATURE_FLAG_INVENTORY_READ_EXT | \
     SLIX_FEATURE_FLAG_EAS_IR | SLIX_FEATURE_FLAG_ORIGINALITY_SIG |                               \
     SLIX_FEATURE_FLAG_PERSISTENT_QUIET | SLIX_FEATURE_FLAG_PRIVACY | SLIX_FEATURE_FLAG_DESTROY)

// Same behaviour as iso15693_3_error_response_parse
bool slix_error_response_parse(SlixError* error, const BitBuffer* buf);

SlixError slix_process_iso15693_3_error(Iso15693_3Error iso15693_3_error);

SlixError slix_get_nxp_system_info_response_parse(SlixSystemInfo* data, const BitBuffer* buf);

SlixError slix_read_signature_response_parse(SlixSignature* data, const BitBuffer* buf);

#ifdef __cplusplus
}
#endif
