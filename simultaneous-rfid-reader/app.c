#include "app.h"
#include "structures.h"

/**
 * @brief      Callback for exiting the application.
 * @details    This function is called when user press back button.  We return VIEW_NONE to
 *            indicate that we want to exit the application.
 * @param      _context  The context - unused
 * @return     next view id
*/
uint32_t uhf_reader_navigation_exit_callback(void* _context) {
    UNUSED(_context);
    return VIEW_NONE;
}


/**
 * @brief      Handle submenu item selection.
 * @details    This function is called when user selects an item from the submenu.
 * @param      context  The context - UHFReaderApp object.
 * @param      index     The UHFReaderSubmenuIndex item that was clicked.
*/
void uhf_reader_submenu_callback(void* context, uint32_t index) {
    UHFReaderApp* app = (UHFReaderApp*)context;
    switch(index) {
    case UHFReaderSubmenuIndexRead:
        view_dispatcher_switch_to_view(app->view_dispatcher, UHFReaderViewRead);
        break;
    case UHFReaderSubmenuIndexSaved:
        view_dispatcher_switch_to_view(app->view_dispatcher, UHFReaderViewSaved);
        break;
    case UHFReaderSubmenuIndexConfig:
        view_dispatcher_switch_to_view(app->view_dispatcher, UHFReaderViewConfigure);
        break;
    case UHFReaderSubmenuIndexAbout:
        view_dispatcher_switch_to_view(app->view_dispatcher, UHFReaderViewAbout);
        break;
    default:
        break;
    }
}

/**
 * @brief      Allocates Main Menu
 * @details    Allocates the main submenu with the read, saved, config, and about submenu items
 * @param      UHFReaderApp app - UHFReaderApp object.
*/
void main_menu_alloc(UHFReaderApp* app){

    app->submenu = submenu_alloc();
    submenu_set_header(app->submenu, "UHF RFID Reader");
    submenu_add_item(
        app->submenu, "Read", UHFReaderSubmenuIndexRead, uhf_reader_submenu_callback, app);
    submenu_add_item(
        app->submenu, "Saved", UHFReaderSubmenuIndexSaved, uhf_reader_submenu_callback, app);
    submenu_add_item(
        app->submenu, "Configure", UHFReaderSubmenuIndexConfig, uhf_reader_submenu_callback, app);
    submenu_add_item(
        app->submenu, "About", UHFReaderSubmenuIndexAbout, uhf_reader_submenu_callback, app);
    view_set_previous_callback(
        submenu_get_view(app->submenu), uhf_reader_navigation_exit_callback);
    view_dispatcher_add_view(
        app->view_dispatcher, UHFReaderViewSubmenu, submenu_get_view(app->submenu));
    view_dispatcher_switch_to_view(app->view_dispatcher, UHFReaderViewSubmenu);
}

/**
 * @brief      Allocate the UHF RFID Reader Application.
 * @details    This function allocates all resources for the UHF RFID Reader Application.
 * @return     UHFReaderApp object.
*/
static UHFReaderApp* uhf_reader_app_alloc() {
    
    //Allocating storage for the saved_epcs and index file
    UHFReaderApp* app = (UHFReaderApp*)malloc(sizeof(UHFReaderApp));
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* file = flipper_format_file_alloc(storage);
    FlipperFormat* index_file = flipper_format_file_alloc(storage);
    app->tag_storage = storage;
    app->epc_file = file;
    app->epc_index_file = index_file;
    
    //Initializing the arrays for storing all tag information from the read screen
    app->epc_values = (char**)malloc(150 * 26);
    app->tid_values = (char**)malloc(150 * 41);
    app->res_values = (char**)malloc(150 * 17);
    app->mem_values = (char**)malloc(150 * 33);
    app->epc_to_save = (char*)malloc(25);
    app->number_of_epcs_to_read = 0;
    
    //Initializing the indicies for each array and the file name 
    app->name_size = 36;
    app->name_size_parse = 27;
    app->cur_epc_index = 26;
    app->cur_tid_index = 1;
    app->cur_res_index = 1;
    app->cur_mem_index = 1;
    app->file_name = (char*)malloc(app->name_size);

    //Creating the initial GUI
    Gui* gui = furi_record_open(RECORD_GUI);
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    //Allocating the different views, menus, and widgets for the app
    main_menu_alloc(app);
    view_config_alloc(app);
    view_read_alloc(app);
    view_saved_menu_alloc(app);
    view_tag_actions_alloc(app);
    view_write_alloc(app);
    view_epc_alloc(app);
    view_epc_info_alloc(app);
    view_delete_alloc(app);
    view_delete_success_alloc(app);
    view_about_alloc(app);
    app->notifications = furi_record_open(RECORD_NOTIFICATION);

#ifdef BACKLIGHT_ON
    notification_message(app->notifications, &sequence_display_backlight_enforce_on);
#endif
    //Create the UART helper object used to communicate with the RPi Zero via UART
    app->uart_helper = uart_helper_alloc();
    uart_helper_set_baud_rate(app->uart_helper, DEVICE_BAUDRATE);
    uart_helper_set_delimiter(app->uart_helper, LINE_DELIMITER, INCLUDE_LINE_DELIMITER);
    uart_helper_set_callback(app->uart_helper, uart_demo_process_line, app);
    return app;
}

/**
 * @brief      Free the UHFReaderApp application.
 * @details    This function frees the UHF RFID Reader application resources.
 * @param      app  The UHFReaderApp application object.
*/
static void uhf_reader_app_free(UHFReaderApp* app) {
#ifdef BACKLIGHT_ON
    notification_message(app->notifications, &sequence_display_backlight_enforce_auto);
#endif
    //Freeing the notification and storage records
    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_GUI);
    
    //Freeing the uart helper
    uart_helper_free(app->uart_helper);
    
    //Freeing all views, widgets, and menus
    view_delete_free(app);
    view_delete_success_free(app);
    view_about_free(app);
    view_epc_free(app);
    view_epc_info_free(app);
    view_read_free(app);
    view_write_free(app);
    view_config_free(app);
    view_saved_free(app);
    view_tag_actions_free(app);
    
    //Freeing the main menu view
    view_dispatcher_remove_view(app->view_dispatcher, UHFReaderViewSubmenu);
    submenu_free(app->submenu);
    view_dispatcher_free(app->view_dispatcher);

    //Freeing variables used in app
    furi_string_free(app->epc_name);
    furi_string_free(app->epc_delete);
    furi_string_free(app->epc_name_delete);
    furi_string_free(app->epc_to_write);
    free(app->epc_values);
    free(app->tid_values);
    free(app->res_values);
    free(app->mem_values);
    free(app->epc_to_save);
    free(app);
}

/**
 * @brief      Main function for UHF RFID Reader application.
 * @details    This function is the entry point for the UHF RFID Reader application.
 * @param      _p  Input parameter - unused
 * @return     0 - Success
*/
int32_t main_uhf_reader_app(void* _p) {
    UNUSED(_p);
    UHFReaderApp* app = uhf_reader_app_alloc();
    view_dispatcher_run(app->view_dispatcher);
    uhf_reader_app_free(app);
    return 0;
}