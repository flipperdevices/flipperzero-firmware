#include <furi.h>
#include <gui/gui.h>
#include <gui/view_port.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/dialog.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>
#include <gui/modules/popup.h>

#include <gui/unimodule/gui_widget.h>
#include <gui/unimodule/element_string.h>
#include <gui/unimodule/element_button.h>
#include <math.h>

typedef enum {
    GuiTesterViewTextInput = 0,
    GuiTesterViewSubmenu,
    GuiTesterViewDialog,
    GuiTesterViewDialogEx,
    GuiTesterViewPopup,
    GuiTesterViewUnimodule,
    GuiTesterViewLast
} GuiTesterView;

typedef struct {
    ViewDispatcher* view_dispatcher;
    Dialog* dialog;
    DialogEx* dialog_ex;
    Submenu* submenu;
    TextInput* text_input;
    Popup* popup;
    GuiWidget* gui_widget;
    GuiTesterView view_index;
} GuiTester;

GuiTester* gui_test_alloc(void) {
    GuiTester* gui_tester = furi_alloc(sizeof(GuiTester));
    gui_tester->view_dispatcher = view_dispatcher_alloc();
    gui_tester->view_index = GuiTesterViewUnimodule;

    gui_tester->dialog = dialog_alloc();
    view_dispatcher_add_view(
        gui_tester->view_dispatcher, GuiTesterViewDialog, dialog_get_view(gui_tester->dialog));

    gui_tester->dialog_ex = dialog_ex_alloc();
    view_dispatcher_add_view(
        gui_tester->view_dispatcher,
        GuiTesterViewDialogEx,
        dialog_ex_get_view(gui_tester->dialog_ex));

    gui_tester->submenu = submenu_alloc();
    view_dispatcher_add_view(
        gui_tester->view_dispatcher, GuiTesterViewSubmenu, submenu_get_view(gui_tester->submenu));

    gui_tester->text_input = text_input_alloc();
    view_dispatcher_add_view(
        gui_tester->view_dispatcher,
        GuiTesterViewTextInput,
        text_input_get_view(gui_tester->text_input));

    gui_tester->popup = popup_alloc();
    view_dispatcher_add_view(
        gui_tester->view_dispatcher, GuiTesterViewPopup, popup_get_view(gui_tester->popup));

    gui_tester->gui_widget = gui_widget_alloc();
    view_dispatcher_add_view(
        gui_tester->view_dispatcher,
        GuiTesterViewUnimodule,
        gui_widget_get_view(gui_tester->gui_widget));

    return gui_tester;
}

void next_view(void* context) {
    furi_assert(context);
    GuiTester* gui_tester = context;

    gui_tester->view_index++;
    if(gui_tester->view_index >= GuiTesterViewLast) {
        gui_tester->view_index = 0;
    }

    view_dispatcher_switch_to_view(gui_tester->view_dispatcher, gui_tester->view_index);
}

void popup_callback(void* context) {
    next_view(context);
}

void submenu_callback(void* context) {
    next_view(context);
}

void dialog_callback(DialogResult result, void* context) {
    next_view(context);
}

void dialog_ex_callback(DialogExResult result, void* context) {
    next_view(context);
}

void text_input_callback(void* context, char* text) {
    next_view(context);
}

void gui_widget_button_callback(void* context) {
    next_view(context);
}

void gui_test(void* param) {
    (void)param;
    GuiTester* gui_tester = gui_test_alloc();

    Gui* gui = furi_record_open("gui");
    view_dispatcher_attach_to_gui(gui_tester->view_dispatcher, gui, ViewDispatcherTypeFullscreen);

    // Submenu
    submenu_add_item(gui_tester->submenu, "Read", submenu_callback, gui_tester);
    submenu_add_item(gui_tester->submenu, "Saved", submenu_callback, gui_tester);
    submenu_add_item(gui_tester->submenu, "Emulate", submenu_callback, gui_tester);
    submenu_add_item(gui_tester->submenu, "Enter manually", submenu_callback, gui_tester);
    submenu_add_item(gui_tester->submenu, "Blah blah", submenu_callback, gui_tester);
    submenu_add_item(gui_tester->submenu, "Set time", submenu_callback, gui_tester);
    submenu_add_item(gui_tester->submenu, "Gender-bender", submenu_callback, gui_tester);
    submenu_add_item(gui_tester->submenu, "Hack American Elections", submenu_callback, gui_tester);
    submenu_add_item(gui_tester->submenu, "Hack the White House", submenu_callback, gui_tester);

    // Dialog
    dialog_set_result_callback(gui_tester->dialog, dialog_callback);
    dialog_set_context(gui_tester->dialog, gui_tester);
    dialog_set_header_text(gui_tester->dialog, "Delete Abc123?");
    dialog_set_text(gui_tester->dialog, "ID: F0 00 01 02 03 04\nAre you shure?");
    dialog_set_left_button_text(gui_tester->dialog, "Yes");
    dialog_set_right_button_text(gui_tester->dialog, "No");

    // Dialog extended
    dialog_ex_set_result_callback(gui_tester->dialog_ex, dialog_ex_callback);
    dialog_ex_set_context(gui_tester->dialog_ex, gui_tester);
    dialog_ex_set_header(gui_tester->dialog_ex, "Dallas", 95, 12, AlignCenter, AlignCenter);
    dialog_ex_set_text(
        gui_tester->dialog_ex, "F6 E5 D4\nC3 B2 A1", 95, 32, AlignCenter, AlignCenter);
    dialog_ex_set_icon(gui_tester->dialog_ex, 0, 1, I_DolphinExcited_64x63);
    dialog_ex_set_left_button_text(gui_tester->dialog_ex, "More");
    dialog_ex_set_right_button_text(gui_tester->dialog_ex, "Save");

    // Popup
    popup_set_callback(gui_tester->popup, popup_callback);
    popup_set_context(gui_tester->popup, gui_tester);
    popup_set_icon(gui_tester->popup, 0, 2, I_DolphinMafia_115x62);
    popup_set_text(gui_tester->popup, "Deleted", 83, 19, AlignLeft, AlignBottom);
    popup_set_timeout(gui_tester->popup, 5000);
    popup_enable_timeout(gui_tester->popup);

    // Text input
    const uint8_t text_input_text_len = 64;
    char* text_input_text = calloc(text_input_text_len + 1, 1);
    memcpy(text_input_text, "New_ke", strlen("New_ke"));

    text_input_set_result_callback(
        gui_tester->text_input,
        text_input_callback,
        gui_tester,
        text_input_text,
        text_input_text_len);
    text_input_set_header_text(gui_tester->text_input, "Name the key");

    // Custom widget
    GuiString* gui_string_1 = gui_string_alloc(0, 10, "Custom string 1");
    GuiString* gui_string_2 = gui_string_alloc(0, 20, "Custom string 2");
    GuiString* gui_string_3 = gui_string_alloc(0, 30, "Custom string 3");
    GuiString* gui_string_4 = gui_string_alloc(0, 40, "Custom string 4");
    GuiString* gui_string_5 = gui_string_alloc(0, 50, "Custom string 5");
    GuiString* gui_string_6 = gui_string_alloc(0, 60, "Custom string 6");
    GuiButton* gui_button =
        gui_button_alloc(ButtonCenter, "Press OK", gui_widget_button_callback, gui_tester);

    gui_widget_add_element(gui_tester->gui_widget, gui_string_get_element(gui_string_1));
    gui_widget_add_element(gui_tester->gui_widget, gui_string_get_element(gui_string_2));
    gui_widget_add_element(gui_tester->gui_widget, gui_string_get_element(gui_string_3));
    gui_widget_add_element(gui_tester->gui_widget, gui_string_get_element(gui_string_4));
    gui_widget_add_element(gui_tester->gui_widget, gui_string_get_element(gui_string_5));
    gui_widget_add_element(gui_tester->gui_widget, gui_string_get_element(gui_string_6));
    gui_widget_add_element(gui_tester->gui_widget, gui_button_get_element(gui_button));

    view_dispatcher_switch_to_view(gui_tester->view_dispatcher, gui_tester->view_index);

    uint16_t ang = 0;
    const float rad = 3.14159265 / 180;

    while(1) {
        gui_string_set_x(gui_string_1, sin(ang * rad) * 32 + 32);
        gui_string_set_x(gui_string_2, sin((ang + 30) * rad) * 32 + 32);
        gui_string_set_x(gui_string_3, sin((ang + 60) * rad) * 32 + 32);
        gui_string_set_x(gui_string_4, sin((ang + 90) * rad) * 32 + 32);
        gui_string_set_x(gui_string_5, sin((ang + 120) * rad) * 32 + 32);
        gui_string_set_x(gui_string_6, sin((ang + 150) * rad) * 32 + 32);
        gui_string_set_y(gui_string_6, cos((ang + 150) * rad) * 16 + 32);
        ang += 1;
        if(ang > 360) ang = 0;
        
        if(ang % 40 < 20){
            gui_button_set_text(gui_button, "Press OK");
        } else {
            gui_button_set_text(gui_button, "Press --");
        }

        osDelay(10);
    }
}