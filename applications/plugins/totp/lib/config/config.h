#ifndef _TOTP_CONFIG_FILE_H_
#define _TOTP_CONFIG_FILE_H_

#include <flipper_format/flipper_format.h>
#include <furi.h>
#include "../../types/plugin_state.h"

#define TOTP_CONFIG_KEY_TIMEZONE "Timezone"
#define TOTP_CONFIG_KEY_TOKEN_NAME "TokenName"
#define TOTP_CONFIG_KEY_TOKEN_SECRET "TokenSecret"
#define TOTP_CONFIG_KEY_CRYPTO_VERIFY "Crypto"
#define TOTP_CONFIG_KEY_BASE_IV "BaseIV"

Storage* totp_open_storage();
void totp_close_storage();
FlipperFormat* totp_open_config_file(Storage* storage);
void totp_close_config_file(FlipperFormat* file);
void totp_full_save_config_file(PluginState* const plugin_state);
void totp_config_file_load_base(PluginState* const plugin_state);
void totp_config_file_load_tokens(PluginState* const plugin_state);

#endif
