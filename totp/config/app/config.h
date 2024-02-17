#if __has_include("ufbt_def.h")
#include "ufbt_def.h"
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
