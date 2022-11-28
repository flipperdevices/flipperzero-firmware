#pragma once

#include <gui/view.h>

typedef struct Hid Hid;
typedef struct BtHidTikTok BtHidTikTok;

BtHidTikTok* hid_tiktok_alloc(Hid* bt_hid);

void hid_tiktok_free(BtHidTikTok* hid_tiktok);

View* hid_tiktok_get_view(BtHidTikTok* hid_tiktok);

void hid_tiktok_set_connected_status(BtHidTikTok* hid_tiktok, bool connected);
