#include "app.h"
#include "structures.h"

/**
 * @brief      Callback for exiting the application.
 * @details    This function is called when user press back button.  We return VIEW_NONE to
 *            indicate that we want to exit the application.
 * @param      context  The context - unused
 * @return     next view id
*/
uint32_t uhf_reader_navigation_exit_callback(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

/**
 * @brief      Handle submenu item selection.
 * @details    This function is called when user selects an item from the submenu.
 * @param      context  The context - UHFReaderApp object.
 * @param      index     The UHFReaderSubmenuIndex item that was clicked.
*/
void uhf_reader_submenu_callback(void* context, uint32_t index) {
    UHFReaderApp* App = (UHFReaderApp*)context;
    switch(index) {
    case UHFReaderSubmenuIndexRead:
        view_dispatcher_switch_to_view(App->ViewDispatcher, UHFReaderViewRead);
        break;
    case UHFReaderSubmenuIndexSaved:
        view_dispatcher_switch_to_view(App->ViewDispatcher, UHFReaderViewSaved);
        break;
    case UHFReaderSubmenuIndexConfig:
        view_dispatcher_switch_to_view(App->ViewDispatcher, UHFReaderViewConfigure);
        break;
    case UHFReaderSubmenuIndexAbout:
        view_dispatcher_switch_to_view(App->ViewDispatcher, UHFReaderViewAbout);
        break;
    default:
        break;
    }
}

/**
 * @brief      Allocates Main Menu
 * @details    Allocates the main submenu with the read, saved, config, and about submenu items
 * @param      app  The UHFReaderApp object.
*/
void main_menu_alloc(UHFReaderApp* App) {
    App->Submenu = submenu_alloc();
    submenu_set_header(App->Submenu, "UHF RFID Reader");
    submenu_add_item(
        App->Submenu, "Read", UHFReaderSubmenuIndexRead, uhf_reader_submenu_callback, App);
    submenu_add_item(
        App->Submenu, "Saved", UHFReaderSubmenuIndexSaved, uhf_reader_submenu_callback, App);
    submenu_add_item(
        App->Submenu, "Configure", UHFReaderSubmenuIndexConfig, uhf_reader_submenu_callback, App);
    submenu_add_item(
        App->Submenu, "About", UHFReaderSubmenuIndexAbout, uhf_reader_submenu_callback, App);
    view_set_previous_callback(
        submenu_get_view(App->Submenu), uhf_reader_navigation_exit_callback);
    view_dispatcher_add_view(
        App->ViewDispatcher, UHFReaderViewSubmenu, submenu_get_view(App->Submenu));
    view_dispatcher_switch_to_view(App->ViewDispatcher, UHFReaderViewSubmenu);
}

/**
 * @brief      Allocate the UHF RFID Reader Application.
 * @details    This function allocates all resources for the UHF RFID Reader Application.
 * @return     UHFReaderApp object.
*/
static UHFReaderApp* uhf_reader_app_alloc() {
    //Allocating storage for the saved_epcs and index file
    UHFReaderApp* App = (UHFReaderApp*)malloc(sizeof(UHFReaderApp));
    Storage* Storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* File = flipper_format_file_alloc(Storage);
    FlipperFormat* IndexFile = flipper_format_file_alloc(Storage);
    App->TagStorage = Storage;
    App->EpcFile = File;
    App->EpcIndexFile = IndexFile;

    //Initializing the arrays for storing all tag information from the read screen
    App->EpcValues = (char**)malloc(150 * 26);
    App->TidValues = (char**)malloc(150 * 41);
    App->ResValues = (char**)malloc(150 * 17);
    App->MemValues = (char**)malloc(150 * 33);
    App->EpcToSave = (char*)malloc(25);
    App->NumberOfEpcsToRead = 0;

    //Initializing the indices for each array and the file name
    App->NameSize = 36;
    App->NameSizeParse = 27;
    App->CurEpcIndex = 26;
    App->CurTidIndex = 1;
    App->CurResIndex = 1;
    App->CurMemIndex = 1;
    App->FileName = (char*)malloc(App->NameSize);

    //Creating the initial GUI
    Gui* Gui = furi_record_open(RECORD_GUI);
    App->ViewDispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(App->ViewDispatcher);
    view_dispatcher_attach_to_gui(App->ViewDispatcher, Gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_set_event_callback_context(App->ViewDispatcher, App);

    //Allocating the different views, menus, and widgets for the app
    main_menu_alloc(App);
    view_config_alloc(App);
    view_read_alloc(App);
    view_saved_menu_alloc(App);
    view_tag_actions_alloc(App);
    view_write_alloc(App);
    view_epc_alloc(App);
    view_epc_info_alloc(App);
    view_delete_alloc(App);
    view_delete_success_alloc(App);
    view_about_alloc(App);
    App->Notifications = furi_record_open(RECORD_NOTIFICATION);

#ifdef BACKLIGHT_ON
    notification_message(App->Notifications, &sequence_display_backlight_enforce_on);
#endif
    //Create the UART helper object used to communicate with the RPi Zero via UART
    App->UartHelper = uart_helper_alloc();
    uart_helper_set_baud_rate(App->UartHelper, DEVICE_BAUDRATE);
    uart_helper_set_delimiter(App->UartHelper, LINE_DELIMITER, INCLUDE_LINE_DELIMITER);
    uart_helper_set_callback(App->UartHelper, uart_demo_process_line, App);
    return App;
}

/**
 * @brief      Free the UHFReaderApp application.
 * @details    This function frees the UHF RFID Reader application resources.
 * @param      app  The UHFReaderApp application object.
*/
static void uhf_reader_app_free(UHFReaderApp* App) {
#ifdef BACKLIGHT_ON
    notification_message(App->Notifications, &sequence_display_backlight_enforce_auto);
#endif
    //Freeing the notification and storage records
    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_GUI);

    //Freeing the UART helper
    uart_helper_free(App->UartHelper);

    //Freeing all views, widgets, and menus
    view_delete_free(App);
    view_delete_success_free(App);
    view_about_free(App);
    view_epc_free(App);
    view_epc_info_free(App);
    view_read_free(App);
    view_write_free(App);
    view_config_free(App);
    view_saved_free(App);
    view_tag_actions_free(App);

    //Freeing the main menu view
    view_dispatcher_remove_view(App->ViewDispatcher, UHFReaderViewSubmenu);
    submenu_free(App->Submenu);
    view_dispatcher_free(App->ViewDispatcher);

    //Freeing variables used in app
    furi_string_free(App->EpcName);
    furi_string_free(App->EpcDelete);
    furi_string_free(App->EpcNameDelete);
    furi_string_free(App->EpcToWrite);
    free(App->EpcValues);
    free(App->TidValues);
    free(App->ResValues);
    free(App->MemValues);
    free(App->EpcToSave);
    free(App);
}

/**
 * @brief      Main function for UHF RFID Reader application.
 * @details    This function is the entry point for the UHF RFID Reader application.
 * @param      _p  Input parameter - unused
 * @return     0 - Success
*/
int32_t main_uhf_reader_app(void* _p) {
    UNUSED(_p);
    UHFReaderApp* App = uhf_reader_app_alloc();
    view_dispatcher_run(App->ViewDispatcher);
    uhf_reader_app_free(App);
    return 0;
}
