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

// Same behaviour as iso15693_3_error_response_parse
bool slix_error_response_parse(SlixError* error, const BitBuffer* buf);

SlixError slix_process_iso15693_3_error(Iso15693_3Error iso15693_3_error);

SlixError slix_get_nxp_system_info_response_parse(SlixSystemInfo* data, const BitBuffer* buf);

SlixError slix_read_signature_response_parse(SlixSignature* data, const BitBuffer* buf);

#ifdef __cplusplus
}
#endif
