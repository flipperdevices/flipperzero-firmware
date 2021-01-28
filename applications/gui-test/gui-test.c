#include <furi.h>
#include <gui/gui.h>
#include <gui/widget.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/dialog.h>

typedef enum { GuiTesterViewDialog } GuiTesterView;

typedef struct {
    ViewDispatcher* view_dispatcher;
    Dialog* dialog;
} GuiTester;

GuiTester* gui_test_alloc(void) {
    GuiTester* gui_tester = furi_alloc(sizeof(GuiTester));
    gui_tester->view_dispatcher = view_dispatcher_alloc();

    gui_tester->dialog = dialog_alloc();
    dialog_set_context(gui_tester->dialog, gui_tester);
    view_dispatcher_add_view(
        gui_tester->view_dispatcher, GuiTesterViewDialog, dialog_get_view(gui_tester->dialog));

    return gui_tester;
}

void gui_test(void* param) {
    (void)param;
    GuiTester* gui_tester = gui_test_alloc();

    Gui* gui = furi_record_open("gui");
    view_dispatcher_attach_to_gui(gui_tester->view_dispatcher, gui, ViewDispatcherTypeFullscreen);

    //dialog_set_result_callback(gui_tester->dialog, power_menu_reset_dialog_result);
    dialog_set_header_text(gui_tester->dialog, "Reset type");
    dialog_set_text(gui_tester->dialog, "Reboot where?");
    dialog_set_left_button_text(gui_tester->dialog, "DFU");
    dialog_set_right_button_text(gui_tester->dialog, "OS");
    view_dispatcher_switch_to_view(gui_tester->view_dispatcher, GuiTesterViewDialog);

    while(1) {
        osDelay(1000);
    }
}