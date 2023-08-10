# ARM Keil MDK 5 wolfSSL Support

wolfSSL has a Keil CMSIS pack available that is updated for each release.

This CMSIS pack contains the wolfCrypt and wolfSSL (TLS) libraries including test, benchmark and example applications.

## To install the wolfSSL pack

1) Open the Keil "Pack Installer"
2) Under Generic locate "wolfSSL::wolfSSL"
3) Click "Install"

## To add the pack to your project

1) Project -> Manage -> "Runtime Environment"
2) Expand "wolfSSL" and check the boxes for wolfCrypt CORE and wolfSSL CORE.
3) If running the wolfCrypt test or any of the TLS examples check those as well.

Note: By default the pack's user_settings.h assumes the CMSIS RTOS v2 and Keil TCP packs are also installed. See below for how to change these settings (`MDK_CONF_THREAD` and `MDK_CONF_NETWORK`).

If the wolfSSL::wolfSSL pack isn't showing:
1) Project -> Manage -> "Select Software Packs"
2) Make sure wolfSSL:wolfSSL is selected to "latest"
3) Hit "OK"

## To configure wolfSSL

1) Add a pre-processor macro `WOLFSSL_USER_SETTINGS`
  - Project -> Options for Target... -> C/C++ -> Preprocessor Symbols -> Define.
  - Add `WOLFSSL_USER_SETTINGS`
2) Open the `user_settings.h` file. In wolfSSL -> user_settings.h
3) Configure math library (`MDK_CONF_MATH`). Default 0=SP Math all (sp_int.c)
4) Configure MPU (`MDK_CONF_MPU`): If not STM32, use 0 for none.
5) Configure the RTOS (`MDK_CONF_THREAD`): By default 15 = "CMSIS RTOSv2". For bare-metal use 0. For FreeRTOS use 1.
6) Configure the TCP stack (`MDK_CONF_NETWORK`). By default uses Keil TCP `WOLFSSL_KEIL_TCP_NET`. Use 0 for none or 2 for user io callbacks.
6) For wolfCrypt only (no TLS) add `#define WOLFCRYPT_ONLY` (resolves GetCA errors)
7) Increase stack/heap (if needed). This is typically in the startup.s, but for RTX is in the `RTX_Config.h`. For CMSIS RTOSv2 stack is set in `osThreadAttr_t` on call to `osThreadNew`.

## Building

If getting an error for an unknown type `inline`, change your project C standard to c99 in the C/C++ options. Alternatively,
add `#define WC_INLINE __inline` to `user_settings.h` to stick to c89 or the default c90.

If getting error for missing GetCA or GetCAByName then include `Src/ssl-dummy.c` or define `WOLFCRYPT_ONLY`.

If getting an error with missing `current_time` then please implement a function to return elapsed seconds (used by benchmark) like this:

```c
extern uint32_t os_time;
double current_time(int reset)
{
      if (reset) os_time = 0 ;
      return (double)os_time /1000.0;
}
```


## For Support

For question email support@wolfssl.com
