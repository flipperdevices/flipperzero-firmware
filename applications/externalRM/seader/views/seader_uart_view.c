#include "../seader_bridge.h"
#include "../seader_i.h"
#include <furi_hal.h>
#include <gui/elements.h>

struct SeaderUartView {
    View* view;
    SeaderUartViewCallback callback;
    void* context;
};

typedef struct {
    uint32_t tx_cnt;
    uint32_t rx_cnt;
    bool tx_active;
    bool rx_active;
    uint8_t protocol;
} SeaderUartViewModel;

static void seader_uart_view_draw_callback(Canvas* canvas, void* _model) {
    SeaderUartViewModel* model = _model;
    char temp_str[18];

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "PICC <=> SAM");
    canvas_draw_str(canvas, 3, 25, "TX:");
    canvas_draw_str(canvas, 3, 42, "RX:");

    if(model->protocol == FrameProtocol_iclass) {
        canvas_draw_str_aligned(canvas, 64, 62, AlignCenter, AlignBottom, "Detecting picopass");
    } else if(model->protocol == FrameProtocol_nfc) {
        canvas_draw_str_aligned(canvas, 64, 62, AlignCenter, AlignBottom, "Detecting 14a");
    }

    if(model->tx_cnt < 100000000) {
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 127, 24, AlignRight, AlignBottom, "B.");
        canvas_set_font(canvas, FontKeyboard);
        snprintf(temp_str, 18, "%lu", model->tx_cnt);
        canvas_draw_str_aligned(canvas, 116, 24, AlignRight, AlignBottom, temp_str);
    } else {
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 127, 24, AlignRight, AlignBottom, "KiB.");
        canvas_set_font(canvas, FontKeyboard);
        snprintf(temp_str, 18, "%lu", model->tx_cnt / 1024);
        canvas_draw_str_aligned(canvas, 111, 24, AlignRight, AlignBottom, temp_str);
    }

    if(model->rx_cnt < 100000000) {
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 127, 41, AlignRight, AlignBottom, "B.");
        canvas_set_font(canvas, FontKeyboard);
        snprintf(temp_str, 18, "%lu", model->rx_cnt);
        canvas_draw_str_aligned(canvas, 116, 41, AlignRight, AlignBottom, temp_str);
    } else {
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 127, 41, AlignRight, AlignBottom, "KiB.");
        canvas_set_font(canvas, FontKeyboard);
        snprintf(temp_str, 18, "%lu", model->rx_cnt / 1024);
        canvas_draw_str_aligned(canvas, 111, 41, AlignRight, AlignBottom, temp_str);
    }

    if(model->tx_active) {
        canvas_draw_icon(canvas, 48, 14, &I_ArrowUpFilled_14x15);
    } else {
        canvas_draw_icon(canvas, 48, 14, &I_ArrowUpEmpty_14x15);
    }

    if(model->rx_active) {
        canvas_draw_icon_ex(canvas, 48, 34, &I_ArrowUpFilled_14x15, IconRotation180);
    } else {
        canvas_draw_icon_ex(canvas, 48, 34, &I_ArrowUpEmpty_14x15, IconRotation180);
    }
}

static bool seader_uart_view_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    SeaderUartView* seader_uart_view = context;
    bool consumed = false;

    if(event->type == InputTypeShort) {
        if(event->key == InputKeyLeft) {
            consumed = true;
            furi_assert(seader_uart_view->callback);
            // seader_uart_view->callback(SeaderUartViewEventConfig, seader_uart_view->context);
        }
    }

    return consumed;
}

SeaderUartView* seader_uart_view_alloc() {
    SeaderUartView* seader_uart_view = malloc(sizeof(SeaderUartView));

    seader_uart_view->view = view_alloc();
    view_allocate_model(seader_uart_view->view, ViewModelTypeLocking, sizeof(SeaderUartViewModel));
    view_set_context(seader_uart_view->view, seader_uart_view);
    view_set_draw_callback(seader_uart_view->view, seader_uart_view_draw_callback);
    view_set_input_callback(seader_uart_view->view, seader_uart_view_input_callback);

    return seader_uart_view;
}

void seader_uart_view_free(SeaderUartView* seader_uart_view) {
    furi_assert(seader_uart_view);
    view_free(seader_uart_view->view);
    free(seader_uart_view);
}

View* seader_uart_view_get_view(SeaderUartView* seader_uart_view) {
    furi_assert(seader_uart_view);
    return seader_uart_view->view;
}

void seader_uart_view_set_callback(
    SeaderUartView* seader_uart_view,
    SeaderUartViewCallback callback,
    void* context) {
    furi_assert(seader_uart_view);
    furi_assert(callback);

    with_view_model(
        seader_uart_view->view,
        SeaderUartViewModel * model,
        {
            UNUSED(model);
            seader_uart_view->callback = callback;
            seader_uart_view->context = context;
        },
        false);
}

void seader_uart_view_update_state(
    SeaderUartView* instance,
    SeaderUartConfig* cfg,
    SeaderUartState* st) {
    furi_assert(instance);
    furi_assert(cfg);
    furi_assert(st);

    with_view_model(
        instance->view,
        SeaderUartViewModel * model,
        {
            model->tx_active = (model->tx_cnt != st->tx_cnt);
            model->rx_active = (model->rx_cnt != st->rx_cnt);
            model->tx_cnt = st->tx_cnt;
            model->rx_cnt = st->rx_cnt;
            model->protocol = st->protocol;
        },
        true);
}
