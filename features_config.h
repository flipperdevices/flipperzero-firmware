// Application automatic lock timeout if user IDLE. (ticks)
#ifndef TOTP_AUTO_LOCK_IDLE_TIMEOUT_SEC
#define TOTP_AUTO_LOCK_IDLE_TIMEOUT_SEC (60)
#endif

// Include Bluetooth token input automation
#ifndef TOTP_NO_BADBT_TYPE
#define TOTP_BADBT_TYPE_ENABLED
#endif

// Include token input automation icons on the main screen
#ifndef TOTP_NO_AUTOMATION_ICONS
#define TOTP_AUTOMATION_ICONS_ENABLED
#endif

// List of compatible firmwares
#define TOTP_FIRMWARE_OFFICIAL_STABLE (1)
#define TOTP_FIRMWARE_OFFICIAL_DEV (2)
#define TOTP_FIRMWARE_XTREME_UL (3)
// End of list

// Checking FlipC.org definitions (https://github.com/playmean/fap-list/issues/9)
#if defined(TARGET_FIRMWARE_OFFICIAL)
#define TOTP_TARGET_FIRMWARE TOTP_FIRMWARE_OFFICIAL_STABLE
#elif defined(TARGET_FIRMWARE_UNLEASHED)
#define TOTP_TARGET_FIRMWARE TOTP_FIRMWARE_XTREME_UL
#elif defined(TARGET_FIRMWARE_XTREME)
#define TOTP_TARGET_FIRMWARE TOTP_FIRMWARE_XTREME_UL
#endif
// End of FlipC.org definition checks

// If target firmware is not yet set, default it to OFW because there is no chance to force Flipper Devices to update their build pipelines :angry:. 
// I'm still using Xtreme firmware, it is still the best one and I highly recommend it to everybody.
#ifndef TOTP_TARGET_FIRMWARE
#define TOTP_TARGET_FIRMWARE TOTP_FIRMWARE_OFFICIAL_STABLE
#endif

// List of available font for TOTP code
#define TOTP_FONT_MODENINE (1)
#define TOTP_FONT_REDHATMONO (2)
#define TOTP_FONT_BEDSTEAD (3)
#define TOTP_FONT_ZECTOR (4)
#define TOTP_FONT_712SERIF (5)
#define TOTP_FONT_GRAPH35PIX (6)
#define TOTP_FONT_KARMAFUTURE (7)
#define TOTP_FONT_FUNCLIMBING (8)
#define TOTP_FONT_DPCOMIC (9)
#define TOTP_FONT_PIXELFLAG (10)

// End of list

// Active font for TOTP codes
#ifndef TOTP_FONT
#define TOTP_FONT TOTP_FONT_MODENINE
#endif
