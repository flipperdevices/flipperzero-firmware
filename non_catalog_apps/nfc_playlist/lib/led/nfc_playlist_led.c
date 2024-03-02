#include "nfc_playlist_led.h"

NotificationMessage blink_message_normal = {
   .type = NotificationMessageTypeLedBlinkStart,
   .data.led_blink.color = LightBlue | LightGreen,
   .data.led_blink.on_time = 10,
   .data.led_blink.period = 100
};
const NotificationSequence blink_sequence_normal = {
   &blink_message_normal,
   &message_do_not_reset,
   NULL
};

NotificationMessage blink_message_error = {
   .type = NotificationMessageTypeLedBlinkStart,
   .data.led_blink.color = LightRed,
   .data.led_blink.on_time = 10,
   .data.led_blink.period = 100
};

const NotificationSequence blink_sequence_error = {
   &blink_message_error,
   &message_do_not_reset,
   NULL
};

void start_blink(NfcPlaylist* nfc_playlist, int state) {
   if (nfc_playlist->settings.emulate_led_indicator) {
      if (state == NfcPlaylistLedState_Normal) {
         notification_message_block(nfc_playlist->notification, &blink_sequence_normal);
      } else if (state == NfcPlaylistLedState_Error) {
         notification_message_block(nfc_playlist->notification, &blink_sequence_error);
      }
   }
}

void stop_blink(NfcPlaylist* nfc_playlist) {
   if (nfc_playlist->settings.emulate_led_indicator) {
      notification_message_block(nfc_playlist->notification, &sequence_blink_stop);
   }
}