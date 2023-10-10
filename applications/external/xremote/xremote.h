/*!
 *  @file flipper-xremote/xremote.h
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief Entrypoint and factory of the XRemote main app.
 */

#include "xremote_app.h"

#define XREMOTE_VERSION_MAJOR 1
#define XREMOTE_VERSION_MINOR 0
#define XREMOTE_BUILD_NUMBER 5

void xremote_get_version(char* version, size_t length);
