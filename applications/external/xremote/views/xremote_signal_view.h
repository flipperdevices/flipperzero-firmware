/*!
 *  @file flipper-xremote/views/xremote_signal_view.h
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief Signal analyzer page view components and functionality.
 */

#pragma once

#include "xremote_common_view.h"

XRemoteView* xremote_signal_view_alloc(void* app_ctx, void* learn_ctx);
XRemoteView* xremote_signal_success_view_alloc(void* app_ctx, void* rx_ctx);
