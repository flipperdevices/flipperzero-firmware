#include <furi.h>
#include <dialogs/dialogs.h>
#include <gui/gui.h>

typedef struct {
    DialogsApp* dialogs;
    int taps;
    float interval;
    int bpm;
} BPMTapper;

int32_t bpm_tapper_app(void* p) {
  UNUSED(p);
  BPMTapper* bpm = malloc(sizeof(BPMTapper));
  bpm->dialogs = furi_record_open(RECORD_DIALOGS);
  DialogMessage* message = dialog_message_alloc();

  const char* header_text = "BPM Tapper";
  const char* message_text = "Tap center to start";

  dialog_message_set_header(message, header_text, 63, 3, AlignCenter, AlignTop);
  dialog_message_set_text(message, message_text, 0, 17, AlignLeft, AlignTop);
  dialog_message_set_buttons(message, NULL, "Tap", NULL);

  dialog_message_set_icon(message, &I_DolphinCommon_56x48, 72, 17);

  dialog_message_show(bpm->dialogs, message);

  dialog_message_free(message);
  furi_record_close(RECORD_DIALOGS);
  free(bpm);

  return 0;
}

