#include "list.h"
#include <cli/cli.h>
#include <stdlib.h>
#include "../../../list/list.h"
#include "../../../../types/token_info.h"
#include "../../../config/constants.h"

static char* get_algo_as_cstr(TokenHashAlgo algo) {
    switch(algo) {
    case SHA1:
        return TOTP_CONFIG_TOKEN_ALGO_SHA1_NAME;
    case SHA256:
        return TOTP_CONFIG_TOKEN_ALGO_SHA256_NAME;
    case SHA512:
        return TOTP_CONFIG_TOKEN_ALGO_SHA512_NAME;
    }

    return "UNKNOWN";
}

static uint8_t get_digits_as_int(TokenDigitsCount digits) {
    switch(digits) {
    case TOTP_6_DIGITS:
        return 6;
    case TOTP_8_DIGITS:
        return 8;
    }

    return 6;
}

void totp_cli_handle_list_command(PluginState* plugin_state) {
    if (plugin_state->tokens_list == NULL) {
        printf("There are no tokens");
        return;
    }

    ListNode* node = plugin_state->tokens_list;

    printf("+-----+-----------------------------+--------+--------+\r\n");
    printf("| %-*s | %-*s | %-*s | %-s |\r\n", 3, "#", 27, "Name", 6, "Algo", "Digits");
    printf("+-----+-----------------------------+--------+--------+\r\n");
    uint16_t index = 1;
    while(node != NULL) {
        TokenInfo* token_info = (TokenInfo* )node->data;
        token_info_get_digits_count(token_info);
        printf("| %-3" PRIu16 " | %-27.27s | %-6s | %-6" PRIu8 " |\r\n", index, token_info->name, get_algo_as_cstr(token_info->algo), get_digits_as_int(token_info->digits));
        node = node->next;
        index++;
    }
    printf("+-----+-----------------------------+--------+--------+\r\n");
}