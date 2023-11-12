#include "qview.h"

void q_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    QModel* model = context;
    canvas_set_font(canvas, FontSecondary);
    elements_multiline_text_aligned(canvas, 3, 3, AlignLeft, AlignTop, Q_GROUPS[model->gi].name);
    char buff[12];
    snprintf(buff, sizeof(buff), "%03d/%03d", model->qi + 1, Q_GROUPS[model->gi].size);
    elements_multiline_text_aligned(canvas, 125, 3, AlignRight, AlignTop, buff);
    if(model->questions != NULL)
        elements_multiline_text_aligned(
            canvas, 66, 36, AlignCenter, AlignCenter, model->questions[model->qi]);
}

bool q_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    QView* view = context;

    if((event->type == InputTypeLong || event->type == InputTypePress) &&
       event->key == InputKeyBack) {
        return false;
    } else {
        with_view_model(
            view->view,
            QModel * model,
            {
                if(event->type == InputTypePress) {
                    if(event->key == InputKeyLeft && model->qi) {
                        model->qi--;
                    } else if(event->key == InputKeyRight && model->qi < Q_GROUPS[model->gi].size - 1) {
                        model->qi++;
                    }
                }
            },
            true);
        return true;
    }
}

void q_reset_view(QView* view, const uint8_t gi) {
    furi_assert(view);
    with_view_model(
        view->view,
        QModel * model,
        {
            model->gi = gi;
            model->qi = 0;
            if(model->questions != NULL) free(model->questions);
            model->questions = q_shuffle(gi);
        },
        true);
}

QView* q_main_view_alloc() {
    QView* qview = malloc(sizeof(QView));
    qview->view = view_alloc();
    view_set_context(qview->view, qview);
    view_allocate_model(qview->view, ViewModelTypeLocking, sizeof(QModel));
    view_set_draw_callback(qview->view, q_draw_callback);
    view_set_input_callback(qview->view, q_input_callback);

    return qview;
}

void q_main_view_free(QView* view) {
    furi_assert(view);
    QModel* model = view_get_model(view->view);
    if(model->questions != NULL) free(model->questions);
    view_free(view->view);
    free(view);
}
