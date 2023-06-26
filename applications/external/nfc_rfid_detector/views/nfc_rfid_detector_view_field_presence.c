#include "nfc_rfid_detector_view_field_presence.h"
#include "../nfc_rfid_detector_app_i.h"
#include <nfc_rfid_detector_icons.h>

#include <input/input.h>
#include <gui/elements.h>

typedef enum {
    NfcRfidDetectorTypeFieldPresenceNfc,
    NfcRfidDetectorTypeFieldPresenceRfid,
} NfcRfidDetectorTypeFieldPresence;

static const Icon* NfcRfidDetectorFieldPresenceIcons[] = {
    [NfcRfidDetectorTypeFieldPresenceNfc] = &I_NFC_detect_45x30,
    [NfcRfidDetectorTypeFieldPresenceRfid] = &I_Rfid_detect_45x30,
};

struct NfcRfidDetectorFieldPresence {
    View* view;
};

typedef struct {
    bool nfc_field;
    bool rfid_field;
    uint32_t rfid_frequncy;
} NfcRfidDetectorFieldPresenceModel;

void nfc_rfid_detector_view_field_presence_update(
    NfcRfidDetectorFieldPresence* instance,
    bool nfc_field,
    bool rfid_field,
    uint32_t rfid_frequncy) {
    furi_assert(instance);
    with_view_model(
        instance->view,
        NfcRfidDetectorFieldPresenceModel * model,
        {
            model->nfc_field = nfc_field;
            model->rfid_field = rfid_field;
            model->rfid_frequncy = rfid_frequncy;
        },
        true);
}

void nfc_rfid_detector_view_field_presence_draw(
    Canvas* canvas,
    NfcRfidDetectorFieldPresenceModel* model) {
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);

    if(model->nfc_field) {
        canvas_draw_str(canvas, 10, 8, "NFC");
        canvas_draw_icon(
            canvas, 4, 16, NfcRfidDetectorFieldPresenceIcons[NfcRfidDetectorTypeFieldPresenceNfc]);
        canvas_draw_str(canvas, 10, 64, "13,56 MHz");
    }

    if(model->rfid_field) {
        char str[16];
        snprintf(str, sizeof(str), "%.02f kHz", (double)model->rfid_frequncy / 1000);
        canvas_draw_str(canvas, 70, 8, "RFID");
        canvas_draw_icon(
            canvas,
            60,
            16,
            NfcRfidDetectorFieldPresenceIcons[NfcRfidDetectorTypeFieldPresenceRfid]);
        canvas_draw_str(canvas, 70, 64, str);
    }
}

bool nfc_rfid_detector_view_field_presence_input(InputEvent* event, void* context) {
    furi_assert(context);
    NfcRfidDetectorFieldPresence* instance = context;
    UNUSED(instance);

    if(event->key == InputKeyBack) {
        return false;
    }

    return true;
}

void nfc_rfid_detector_view_field_presence_enter(void* context) {
    furi_assert(context);
    NfcRfidDetectorFieldPresence* instance = context;
    with_view_model(
        instance->view,
        NfcRfidDetectorFieldPresenceModel * model,
        {
            model->nfc_field = false;
            model->rfid_field = false;
            model->rfid_frequncy = 0;
        },
        true);
}

void nfc_rfid_detector_view_field_presence_exit(void* context) {
    furi_assert(context);
    NfcRfidDetectorFieldPresence* instance = context;
    UNUSED(instance);
}

NfcRfidDetectorFieldPresence* nfc_rfid_detector_view_field_presence_alloc() {
    NfcRfidDetectorFieldPresence* instance = malloc(sizeof(NfcRfidDetectorFieldPresence));

    // View allocation and configuration
    instance->view = view_alloc();

    view_allocate_model(
        instance->view, ViewModelTypeLocking, sizeof(NfcRfidDetectorFieldPresenceModel));
    view_set_context(instance->view, instance);
    view_set_draw_callback(
        instance->view, (ViewDrawCallback)nfc_rfid_detector_view_field_presence_draw);
    view_set_input_callback(instance->view, nfc_rfid_detector_view_field_presence_input);
    view_set_enter_callback(instance->view, nfc_rfid_detector_view_field_presence_enter);
    view_set_exit_callback(instance->view, nfc_rfid_detector_view_field_presence_exit);

    with_view_model(
        instance->view,
        NfcRfidDetectorFieldPresenceModel * model,
        {
            model->nfc_field = false;
            model->rfid_field = false;
            model->rfid_frequncy = 0;
        },
        true);
    return instance;
}

void nfc_rfid_detector_view_field_presence_free(NfcRfidDetectorFieldPresence* instance) {
    furi_assert(instance);

    view_free(instance->view);
    free(instance);
}

View* nfc_rfid_detector_view_field_presence_get_view(NfcRfidDetectorFieldPresence* instance) {
    furi_assert(instance);
    return instance->view;
}
