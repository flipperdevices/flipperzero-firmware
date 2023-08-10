/* user_settings.h
 *
 * Custom wolfSSL User Settings File
 *
 * This is a custom user settings file which controls the build of wolfSSL.
 * The defines here are the defaults for this library project, but should
 * be updated to match the end project.
 *
 * As this is a standalone library project, it has been configured to be
 * compiled as a generic ARM Cortex-M4 build.  No operating system or
 * TCP/IP stack has been configured yet, so some associated features
 * have been disabled below.
 */

#ifndef USER_SETTINGS_H_
#define USER_SETTINGS_H_

/* no dirent.h support */
#define NO_WOLFSSL_DIR

/* no sys/uio.h, disable writev() */
#define NO_WRITEV

/* no OS yet, so no mutexes */
#define SINGLE_THREADED

/* no TCP/IP stack connected yet */
#define WOLFSSL_USER_IO

/* enable fastmath library */
#define USE_FAST_MATH

/* enable hardened build options */
#define TFM_TIMING_RESISTANT
#define ECC_TIMING_RESISTANT
#define WC_RSA_BLINDING

#endif /* USER_SETTINGS_H_ */

