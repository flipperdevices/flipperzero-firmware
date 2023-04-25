#pragma once

#include "../../types/token_info.h"
#include <flipper_format/flipper_format.h>
#include "constants.h"

typedef int TotpIteratorUpdateTokenResult;

enum TotpIteratorUpdateTokenResults {
    TotpIteratorUpdateTokenResultSuccess = 0,
    TotpIteratorUpdateTokenResultFileUpdateFailed = -1
};

typedef TotpIteratorUpdateTokenResult (*TOTP_ITERATOR_UPDATE_TOKEN_ACTION)(
    TokenInfo* const token_info,
    const void* context);

typedef struct TokenInfoIteratorContext TokenInfoIteratorContext;

TokenInfoIteratorContext* totp_token_info_iterator_alloc(Storage* storage, FlipperFormat* config_file, uint8_t* iv);

bool totp_token_info_iterator_go_to(TokenInfoIteratorContext* context, size_t token_index);

bool totp_token_info_iterator_move_current_token_info(
    TokenInfoIteratorContext* context,
    size_t new_index);

TotpIteratorUpdateTokenResult totp_token_info_iterator_update_current_token(TokenInfoIteratorContext* context, TOTP_ITERATOR_UPDATE_TOKEN_ACTION update, const void* update_context);

TotpIteratorUpdateTokenResult totp_token_info_iterator_add_new_token(TokenInfoIteratorContext* context, TOTP_ITERATOR_UPDATE_TOKEN_ACTION update, const void* update_context);

bool totp_token_info_iterator_remove_current_token_info(TokenInfoIteratorContext* context);

void totp_token_info_iterator_free(TokenInfoIteratorContext* context);

const TokenInfo* totp_token_info_iterator_get_current_token(const TokenInfoIteratorContext* context);

size_t totp_token_info_iterator_get_current_token_index(const TokenInfoIteratorContext* context);

size_t totp_token_info_iterator_get_total_count(const TokenInfoIteratorContext* context);

void totp_token_info_iterator_attach_to_config_file(TokenInfoIteratorContext* context, FlipperFormat* config_file);