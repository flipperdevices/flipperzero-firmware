// List of compatible firmwares
#define TOTP_FIRMWARE_OFFICIAL_STABLE (1)
#define TOTP_FIRMWARE_OFFICIAL_DEV (2)
#define TOTP_FIRMWARE_XTREME_UL (3)
// End of list

#if __has_include("ufbt_def.h")
#include "ufbt_def.h"
#endif

#ifndef TOTP_TARGET_FIRMWARE
#if defined(TARGET_FIRMWARE_OFFICIAL) || defined(FW_ORIGIN_Official)
#define TOTP_TARGET_FIRMWARE TOTP_FIRMWARE_OFFICIAL_STABLE
#elif defined(TARGET_FIRMWARE_UNLEASHED) || defined(FW_ORIGIN_Unleashed)
#define TOTP_TARGET_FIRMWARE TOTP_FIRMWARE_XTREME_UL
#elif defined(TARGET_FIRMWARE_XTREME) || defined(FW_ORIGIN_Xtreme)
#define TOTP_TARGET_FIRMWARE TOTP_FIRMWARE_XTREME_UL
#else
#define TOTP_TARGET_FIRMWARE TOTP_FIRMWARE_OFFICIAL_STABLE
#endif
#endif

// Application automatic lock timeout if user IDLE. (ticks)
#ifndef TOTP_AUTO_LOCK_IDLE_TIMEOUT_SEC
#define TOTP_AUTO_LOCK_IDLE_TIMEOUT_SEC (60)
#endif

// Enables\disables Bluetooth token input automation
#ifndef TOTP_NO_BADBT_AUTOMATION
#define TOTP_BADBT_AUTOMATION_ENABLED
#endif

// Enables\disables backward compatibility with crypto algorithms v1
// #define TOTP_OBSOLETE_CRYPTO_V1_COMPATIBILITY_ENABLED

// Enables\disables backward compatibility with crypto algorithms v2
#ifndef TOTP_NO_OBSOLETE_CRYPTO_V2_COMPATIBILITY
#define TOTP_OBSOLETE_CRYPTO_V2_COMPATIBILITY_ENABLED
#endif

// Enables\disables "Add new token" UI
// If disabled it will print a link to wiki page
#ifndef TOTP_UI_NO_ADD_NEW_TOKEN
#define TOTP_UI_ADD_NEW_TOKEN_ENABLED
#endif
