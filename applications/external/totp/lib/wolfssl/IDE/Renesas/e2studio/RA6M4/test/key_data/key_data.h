
#ifndef __KEY_DATA_H__

#include "r_sce.h"

/** Firmware update data and user key data */
typedef struct user_key_block_data
{
    /* Provisioning key wrapped by Renesas DLM */
    uint8_t encrypted_provisioning_key[HW_SCE_AES_CBC_IV_BYTE_SIZE * 2];
    /* Initial vector to be used when creating encrypted key */
    uint8_t iv[HW_SCE_AES_CBC_IV_BYTE_SIZE];
    /* RSA 2048 bit key, encrypted by AES128-ECB */
    uint8_t encrypted_user_rsa2048_ne_key[HW_SCE_RSA2048_NE_KEY_BYTE_SIZE + 16];
} st_user_key_block_data_t;

 extern const unsigned char     ca_cert_der_sign[];
 extern const unsigned char     ca_ecc_cert_der_sign[];

#endif /* __KEY_DATA_H__ */
