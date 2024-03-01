#include <stdbool.h>
#include <cli/cli.h>
#include <lib/print/wrappers.h>
#include <lib/toolbox/args.h>
#include <memset_s.h>
#include "services/crypto/crypto_facade.h"
#include "types/crypto_settings.h"
#include "ui/scene_director.h"
#include "services/config/config.h"
#include "services/kb_layouts/kb_layout_provider.h"
#include "cli/cli_helpers.h"
#include "workers/bt_type_code/bt_type_code.h"

static constexpr auto app_api_table = sort(create_array_t<sym_entry>(
    API_METHOD(memset_s, errno_t, (void*, rsize_t, int, rsize_t)),
    API_METHOD(totp_scene_director_activate_scene, void, (PluginState* const, Scene)),
    API_METHOD(totp_scene_director_force_redraw, void, (PluginState* const)),
    API_METHOD(totp_config_file_update_timezone_offset, bool, (const PluginState*)),
    API_METHOD(totp_config_file_reset, void, (PluginState* const)),
    API_METHOD(
        totp_config_get_token_iterator_context,
        TokenInfoIteratorContext*,
        (const PluginState*)),
    API_METHOD(totp_config_file_backup, char*, (const PluginState*)),
    API_METHOD(
        totp_config_file_update_encryption,
        bool,
        (PluginState*, uint8_t, const uint8_t*, uint8_t)),
    API_METHOD(totp_config_file_update_automation_method, bool, (const PluginState*)),
    API_METHOD(totp_config_file_update_notification_method, bool, (const PluginState*)),
    API_METHOD(totp_token_info_iterator_get_total_count, size_t, (const TokenInfoIteratorContext*)),
    API_METHOD(
        totp_token_info_iterator_get_current_token_index,
        size_t,
        (const TokenInfoIteratorContext*)),
    API_METHOD(totp_token_info_iterator_go_to, bool, (TokenInfoIteratorContext*, size_t)),
    API_METHOD(
        totp_token_info_iterator_get_current_token,
        const TokenInfo*,
        (const TokenInfoIteratorContext*)),
    API_METHOD(
        totp_token_info_iterator_add_new_token,
        TotpIteratorUpdateTokenResult,
        (TokenInfoIteratorContext*, TOTP_ITERATOR_UPDATE_TOKEN_ACTION, const void*)),
    API_METHOD(
        totp_token_info_iterator_update_current_token,
        TotpIteratorUpdateTokenResult,
        (TokenInfoIteratorContext*, TOTP_ITERATOR_UPDATE_TOKEN_ACTION, const void*)),
    API_METHOD(
        totp_token_info_iterator_move_current_token_info,
        bool,
        (TokenInfoIteratorContext*, size_t)),
    API_METHOD(
        totp_token_info_iterator_remove_current_token_info,
        bool,
        (TokenInfoIteratorContext*)),
    API_METHOD(token_info_get_algo_as_cstr, const char*, (const TokenInfo*)),
    API_METHOD(token_info_set_algo_from_str, bool, (TokenInfo*, const FuriString*)),
    API_METHOD(token_info_set_digits_from_int, bool, (TokenInfo*, uint8_t)),
    API_METHOD(token_info_set_duration_from_int, bool, (TokenInfo*, uint8_t)),
    API_METHOD(token_info_set_automation_feature_from_str, bool, (TokenInfo*, const FuriString*)),
    API_METHOD(
        token_info_set_secret,
        bool,
        (TokenInfo*, const char*, size_t, PlainTokenSecretEncoding, const CryptoSettings*)),
    API_METHOD(totp_crypto_check_key_slot, bool, (uint8_t)),
    API_METHOD(totp_bt_type_code_worker_free, void, (TotpBtTypeCodeWorkerContext*)),
    API_METHOD(token_info_set_token_type_from_str, bool, (TokenInfo*, const FuriString*)),
    API_METHOD(token_info_set_token_counter_from_str, bool, (TokenInfo*, const FuriString*)),
    API_METHOD(token_info_get_type_as_cstr, const char*, (const TokenInfo*)),
    API_METHOD(
        totp_kb_layout_provider_get_layout_by_name,
        bool,
        (const char*, AutomationKeyboardLayout*)),
    API_METHOD(
        totp_kb_layout_provider_get_layout_name,
        bool,
        (AutomationKeyboardLayout, char*, size_t)),
    API_METHOD(
        totp_crypto_decrypt,
        uint8_t*,
        (const uint8_t*, const size_t, const CryptoSettings*, size_t*))));
