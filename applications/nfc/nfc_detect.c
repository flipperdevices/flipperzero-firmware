#include "nfc_detect.h"

#include "nfc_i.h"
#include "nfc_types.h"
#include <furi.h>
#include <api-hal.h>
#include <input/input.h>

struct NfcDetect {
    View* view;
};

typedef struct {
    bool found;
    uint8_t uid_len;
    uint8_t uid[10];
    uint8_t atqa[2];
    uint8_t sak;
    NfcDeviceType type;
    NfcProtocol protocol;
} NfcDetectModel;

void nfc_detect_draw(Canvas* canvas, NfcDetectModel* model) {
    char buffer[32];
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    if(model->found) {
        canvas_draw_str(canvas, 0, 12, "Found");
        canvas_draw_str(canvas, 20, 12, nfc_get_dev_type(model->type));
        canvas_set_font(canvas, FontSecondary);
        if(model->protocol != NfcDeviceProtocolUnknown) {
            canvas_draw_str(canvas, 0, 22, nfc_get_protocol(model->protocol));
        }
        // Display UID
        for(uint8_t i = 0; i < model->uid_len; i++) {
            snprintf(buffer + (i * 2), sizeof(buffer) - (i * 2), "%02X", model->uid[i]);
            buffer[model->uid_len * 2] = 0;
        }
        canvas_draw_str(canvas, 0, 32, "UID: ");
        canvas_draw_str(canvas, 12, 32, buffer);
        // Display ATQA and SAK
        snprintf(buffer, sizeof(buffer), "ATQA: %02X %02X   SAK: %02X", model->atqa[1], model->atqa[0], model->sak);
        canvas_draw_str(canvas, 0, 42, buffer);
    } else {
        canvas_draw_str(canvas, 0, 12, "Searching");
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 2, 22, "Place card to the back");
    }
}

bool nfc_detect_input(InputEvent* event, void* context) {
    if(event->key == InputKeyBack) {
        return false;
    }
}

void nfc_detect_enter(void* context) {
    furi_assert(context);
    SubghzStatic* subghz_static = context;
    
    with_view_model(
        nfc_detect->view, (NfcDetectModel * model) {
            model->found = false;
            return true;
        });
}

void nfc_detect_exit(void* context) {
    furi_assert(context);
}

uint32_t nfc_detect_back(void* context) {
    return NfcViewMenu;
}

NfcDetect* nfc_detect_alloc() {
    NfcDetect* nfc_detect = furi_alloc(sizeof(NfcDetect));

    // View allocation and configuration
    nfc_detect->view = view_alloc();
    view_allocate_model(nfc_detect->view, ViewModelTypeLockFree, sizeof(NfcDetectModel));
    view_set_context(nfc_detect->view, nfc_detect);
    view_set_draw_callback(nfc_detect->view, (ViewDrawCallback) nfc_detect_draw);
    view_set_input_callback(nfc_detect->view, nfc_detect_input);
    view_set_enter_callback(nfc_detect->view, nfc_detect_enter);
    view_set_exit_callback(nfc_detect->view, nfc_detect_exit);
    view_set_previous_callback(nfc_detect->view, nfc_detect_back);

    return nfc_detect;
}
