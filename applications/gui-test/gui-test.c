#include <furi.h>
#include <gui/gui.h>
#include <gui/view_port.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/dialog.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>

typedef enum {
    GuiTesterViewTextInput = 0,
    GuiTesterViewSubmenu,
    GuiTesterViewDialog,
    GuiTesterViewLast
} GuiTesterView;

typedef struct {
    ViewDispatcher* view_dispatcher;
    Dialog* dialog;
    Submenu* submenu;
    TextInput* text_input;
    GuiTesterView view_index;
} GuiTester;

GuiTester* gui_test_alloc(void) {
    GuiTester* gui_tester = furi_alloc(sizeof(GuiTester));
    gui_tester->view_dispatcher = view_dispatcher_alloc();
    gui_tester->view_index = GuiTesterViewTextInput;

    gui_tester->dialog = dialog_alloc();
    view_dispatcher_add_view(
        gui_tester->view_dispatcher, GuiTesterViewDialog, dialog_get_view(gui_tester->dialog));

    gui_tester->submenu = submenu_alloc();
    view_dispatcher_add_view(
        gui_tester->view_dispatcher, GuiTesterViewSubmenu, submenu_get_view(gui_tester->submenu));

    gui_tester->text_input = text_input_alloc();
    view_dispatcher_add_view(
        gui_tester->view_dispatcher,
        GuiTesterViewTextInput,
        text_input_get_view(gui_tester->text_input));

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

void submenu_callback(void* context) {
    next_view(context);
}

void dialog_callback(DialogResult result, void* context) {
    next_view(context);
}

void text_input_callback(void* context, char* text) {
    next_view(context);
}

void gui_test(void* param) {
    (void)param;
    GuiTester* gui_tester = gui_test_alloc();

    Gui* gui = furi_record_open("gui");
    view_dispatcher_attach_to_gui(gui_tester->view_dispatcher, gui, ViewDispatcherTypeFullscreen);

    submenu_add_item(gui_tester->submenu, "Read", submenu_callback, gui_tester);
    submenu_add_item(gui_tester->submenu, "Saved", submenu_callback, gui_tester);
    submenu_add_item(gui_tester->submenu, "Emulate", submenu_callback, gui_tester);
    submenu_add_item(gui_tester->submenu, "Enter manually", submenu_callback, gui_tester);
    submenu_add_item(gui_tester->submenu, "Blah blah", submenu_callback, gui_tester);
    submenu_add_item(gui_tester->submenu, "Set time", submenu_callback, gui_tester);
    submenu_add_item(gui_tester->submenu, "Gender-bender", submenu_callback, gui_tester);
    submenu_add_item(gui_tester->submenu, "Hack American Elections", submenu_callback, gui_tester);
    submenu_add_item(gui_tester->submenu, "Hack the White House", submenu_callback, gui_tester);

    dialog_set_result_callback(gui_tester->dialog, dialog_callback);
    dialog_set_context(gui_tester->dialog, gui_tester);
    dialog_set_header_text(gui_tester->dialog, "Delete Abc123?");
    dialog_set_text(gui_tester->dialog, "ID: F0 00 01 02 03 04");
    dialog_set_left_button_text(gui_tester->dialog, "< More");
    dialog_set_right_button_text(gui_tester->dialog, "Save >");

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

    view_dispatcher_switch_to_view(gui_tester->view_dispatcher, gui_tester->view_index);

    while(1) {
        osDelay(1000);
    }
}