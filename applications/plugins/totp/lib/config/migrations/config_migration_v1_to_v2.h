#ifndef _TOTP_CONFIG_FILE_MIGRATE_V1_TO_V2_H_
#define _TOTP_CONFIG_FILE_MIGRATE_V1_TO_V2_H_

#include <flipper_format/flipper_format.h>

bool totp_config_migrate_v1_to_v2(FlipperFormat* fff_data_file, FlipperFormat* fff_backup_data_file);

#endif
