#pragma once
#include <../../nfc_playlist.h>
#include <notification/notification_messages.h>

typedef enum NfcPlaylistLedState {
   NfcPlaylistLedState_Normal,
   NfcPlaylistLedState_Error
} NfcPlaylistLedState;

void start_blink(NfcPlaylist* nfc_playlist, int state);
void stop_blink(NfcPlaylist* nfc_playlist);