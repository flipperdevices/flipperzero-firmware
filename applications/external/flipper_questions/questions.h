#ifndef QUSTIONS
#define QUESTIONS

#include <furi.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <notification/notification.h>

#include <gui/modules/submenu.h>
#include <gui/modules/dialog_ex.h>

#include "qdb.h"
#include "qview.h"

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    QView* q_main_view;
} QuestionApp;

typedef enum {
    QuestionViewSubmenu,
    QuestionViewMain,
} QuestionView;

#endif
