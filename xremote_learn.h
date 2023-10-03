/*!
 *  @file flipper-xremote/xremote_learn.h
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief Functionality to read and store input from new remote.
 */

#pragma once

#include "xremote_app.h"

XRemoteApp* xremote_learn_alloc(XRemoteAppContext* app_ctx);
