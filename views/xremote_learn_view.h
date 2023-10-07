/*!
 *  @file flipper-xremote/views/xremote_learn_view.h
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief Learn new remote page view components and functionality.
 */

#pragma once

#include "xremote_common_view.h"

XRemoteView* xremote_learn_view_alloc(void* app_ctx, void* learn_ctx);
XRemoteView* xremote_learn_success_view_alloc(void* app_ctx, void* rx_ctx);
