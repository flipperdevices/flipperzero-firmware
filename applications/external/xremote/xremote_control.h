/*!
 *  @file flipper-xremote/xremote_control.h
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief Remote controller application menu and view factory.
 */

#pragma once

#include "xremote_app.h"

XRemoteApp* xremote_control_alloc(XRemoteAppContext* app_ctx);
