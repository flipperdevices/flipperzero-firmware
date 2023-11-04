/*!
 *  @file flipper-xremote/xremote.h
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief Entrypoint and factory of the XRemote main app.
 */

#include "xremote_app.h"

#define XREMOTE_BUILD_NUMBER 2

/* Returns FAP_VERSION + XREMOTE_BUILD_NUMBER */
void xremote_get_version(char* version, size_t length);
