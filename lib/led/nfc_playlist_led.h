#pragma once
#include <../../nfc_playlist.h>
#include <notification/notification_messages.h>

void start_normal_blink(NfcPlaylist* nfc_playlist);
void start_error_blink(NfcPlaylist* nfc_playlist);
void stop_blink(NfcPlaylist* nfc_playlist);