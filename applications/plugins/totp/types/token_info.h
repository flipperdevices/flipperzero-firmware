#ifndef _TOTP_TOKEN_INFO_H_
#define _TOTP_TOKEN_INFO_H_

#include <inttypes.h>

typedef struct {
    uint8_t* token;
    uint8_t token_length;
    char* name;
} TokenInfo;

void token_info_free(TokenInfo* token_info);
void token_info_set_secret(TokenInfo* token_info, const char* base32_token_secret, uint8_t token_secret_length, uint8_t* iv);

#endif
