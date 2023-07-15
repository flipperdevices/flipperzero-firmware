#include "cfw_app.h"

static bool cfw_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    CfwApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

void callback_reboot(void* context) {
    UNUSED(context);
    power_reboot(PowerBootModeNormal);
}

bool cfw_app_apply(CfwApp* app) {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    if(app->save_mainmenu_apps) {
        Stream* stream = file_stream_alloc(storage);
        if(file_stream_open(stream, CFW_APPS_PATH, FSAM_READ_WRITE, FSOM_CREATE_ALWAYS)) {
            CharList_it_t it;
            CharList_it(it, app->mainmenu_app_paths);
            for(uint i = 0; i < CharList_size(app->mainmenu_app_paths); i++) {
                stream_write_format(stream, "%s\n", *CharList_get(app->mainmenu_app_paths, i));
            }
        }
        file_stream_close(stream);
        stream_free(stream);
    }

    if(app->save_subghz_frequencies) {
        FlipperFormat* file = flipper_format_file_alloc(storage);
        do {
            FrequencyList_it_t it;
            if(!flipper_format_file_open_always(file, EXT_PATH("subghz/assets/setting_user.txt")))
                break;

            if(!flipper_format_write_header_cstr(
                   file, SUBGHZ_SETTING_FILE_TYPE, SUBGHZ_SETTING_FILE_VERSION))
                break;

            while(flipper_format_delete_key(file, "Add_standard_frequencies"))
                ;
            flipper_format_write_bool(
                file, "Add_standard_frequencies", &app->subghz_use_defaults, 1);

            if(!flipper_format_rewind(file)) break;
            while(flipper_format_delete_key(file, "Frequency"))
                ;
            FrequencyList_it(it, app->subghz_static_freqs);
            for(uint i = 0; i < FrequencyList_size(app->subghz_static_freqs); i++) {
                flipper_format_write_uint32(
                    file, "Frequency", FrequencyList_get(app->subghz_static_freqs, i), 1);
            }

            if(!flipper_format_rewind(file)) break;
            while(flipper_format_delete_key(file, "Hopper_frequency"))
                ;
            for(uint i = 0; i < FrequencyList_size(app->subghz_hopper_freqs); i++) {
                flipper_format_write_uint32(
                    file, "Hopper_frequency", FrequencyList_get(app->subghz_hopper_freqs, i), 1);
            }
        } while(false);
        flipper_format_free(file);
    }

    if(app->save_subghz) {
        furi_hal_subghz_set_extend_settings(app->subghz_extend, app->subghz_bypass);
    }

    if(app->save_name) {
        if(strcmp(app->device_name, "") == 0) {
            storage_simply_remove(storage, NAMESPOOF_PATH);
        } else {
            FlipperFormat* file = flipper_format_file_alloc(storage);

            do {
                if(!flipper_format_file_open_always(file, NAMESPOOF_PATH)) break;

                if(!flipper_format_write_header_cstr(file, NAMESPOOF_HEADER, NAMESPOOF_VERSION))
                    break;

                if(!flipper_format_write_comment_cstr(
                       file, "Changing the value below will change your FlipperZero device name."))
                    break;
                if(!flipper_format_write_comment_cstr(
                       file,
                       "Note: This is limited to 8 characters using the following: a-z, A-Z, 0-9, and _"))
                    break;
                if(!flipper_format_write_comment_cstr(
                       file, "It cannot contain any other characters."))
                    break;

                if(!flipper_format_write_string_cstr(file, "Name", app->device_name)) break;

            } while(0);

            flipper_format_free(file);
        }
    }

    if(app->save_backlight) {
        rgb_backlight_save_settings();
    }

    if(app->save_settings) {
        CFW_SETTINGS_SAVE();
    }

    if(app->require_reboot) {
        popup_set_header(app->popup, "Rebooting...", 64, 26, AlignCenter, AlignCenter);
        popup_set_text(app->popup, "Applying changes...", 64, 40, AlignCenter, AlignCenter);
        popup_set_callback(app->popup, callback_reboot);
        popup_set_context(app->popup, app);
        popup_set_timeout(app->popup, 1000);
        popup_enable_timeout(app->popup);
        view_dispatcher_switch_to_view(app->view_dispatcher, CfwAppViewPopup);
        return true;
    }

    furi_record_close(RECORD_STORAGE);
    return false;
}

static bool cfw_app_back_event_callback(void* context) {
    furi_assert(context);
    CfwApp* app = context;

    if(!scene_manager_has_previous_scene(app->scene_manager, CfwAppSceneStart)) {
        if(cfw_app_apply(app)) {
            return true;
        }
    }

    return scene_manager_handle_back_event(app->scene_manager);
}

CfwApp* cfw_app_alloc() {
    CfwApp* app = malloc(sizeof(CfwApp));
    app->gui = furi_record_open(RECORD_GUI);
    app->dialogs = furi_record_open(RECORD_DIALOGS);
    app->notification = furi_record_open(RECORD_NOTIFICATION);

    // View Dispatcher and Scene Manager
    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&cfw_app_scene_handlers, app);
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    view_dispatcher_set_custom_event_callback(app->view_dispatcher, cfw_app_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, cfw_app_back_event_callback);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    // Gui Modules
    app->var_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        CfwAppViewVarItemList,
        variable_item_list_get_view(app->var_item_list));

    app->text_input = text_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, CfwAppViewTextInput, text_input_get_view(app->text_input));

    app->popup = popup_alloc();
    view_dispatcher_add_view(app->view_dispatcher, CfwAppViewPopup, popup_get_view(app->popup));

    CfwSettings* cfw_settings = CFW_SETTINGS();

    //Main Menu Add/Remove list

    Storage* storage = furi_record_open(RECORD_STORAGE);

    CharList_init(app->mainmenu_app_names);
    CharList_init(app->mainmenu_app_paths);
    Stream* stream = file_stream_alloc(storage);
    FuriString* line = furi_string_alloc();
    FuriString* filename = furi_string_alloc();
    if(file_stream_open(stream, CFW_APPS_PATH, FSAM_READ, FSOM_OPEN_EXISTING)) {
        while(stream_read_line(stream, line)) {
            furi_string_replace_all(line, "\r", "");
            furi_string_replace_all(line, "\n", "");
            uint8_t* icon_buf = malloc(MENU_ICON_MAX_SIZE);

            path_extract_filename(line, filename, true);
            if(!flipper_application_load_name_and_icon(line, storage, &icon_buf, filename)) {
                free(icon_buf);
                icon_buf = NULL;
            } else {
                free(icon_buf);
                icon_buf = NULL;
                CharList_push_back(app->mainmenu_app_paths, strdup(furi_string_get_cstr(line)));
                CharList_push_back(
                    app->mainmenu_app_names, strdup(furi_string_get_cstr(filename)));
            }
        }
    }
    furi_string_free(filename);
    furi_string_free(line);
    file_stream_close(stream);
    stream_free(stream);

    // Start Point list
    CharList_init(app->start_point_names);

    CharList_push_back(app->start_point_names, "Applications");

    for(size_t i = 0; i < FLIPPER_EXTERNAL_APPS_COUNT; i++) {
        CharList_push_back(app->start_point_names, strdup(FLIPPER_EXTERNAL_APPS[i].name));
    }

    for(size_t i = 0; i < FLIPPER_APPS_COUNT; i++) {
        CharList_push_back(app->start_point_names, strdup(FLIPPER_APPS[i].name));
    }

    for(size_t i = 0; i < CharList_size(app->mainmenu_app_names); i++) {
        CharList_push_back(
            app->start_point_names, strdup(*CharList_get(app->mainmenu_app_names, i)));
    }

    CharList_push_back(app->start_point_names, "Settings");

    FlipperFormat* file = flipper_format_file_alloc(storage);
    FrequencyList_init(app->subghz_static_freqs);
    FrequencyList_init(app->subghz_hopper_freqs);
    app->subghz_use_defaults = true;
    do {
        uint32_t temp;
        if(!flipper_format_file_open_existing(file, EXT_PATH("subghz/assets/setting_user.txt")))
            break;

        flipper_format_read_bool(file, "Add_standard_frequencies", &app->subghz_use_defaults, 1);

        if(!flipper_format_rewind(file)) break;
        while(flipper_format_read_uint32(file, "Frequency", &temp, 1)) {
            if(furi_hal_subghz_is_frequency_valid(temp)) {
                FrequencyList_push_back(app->subghz_static_freqs, temp);
            }
        }

        if(!flipper_format_rewind(file)) break;
        while(flipper_format_read_uint32(file, "Hopper_frequency", &temp, 1)) {
            if(furi_hal_subghz_is_frequency_valid(temp)) {
                FrequencyList_push_back(app->subghz_hopper_freqs, temp);
            }
        }
    } while(false);
    flipper_format_free(file);
    furi_record_close(RECORD_STORAGE);

    furi_hal_subghz_get_extend_settings(&app->subghz_extend, &app->subghz_bypass);

    strlcpy(app->device_name, furi_hal_version_get_name_ptr(), FURI_HAL_VERSION_ARRAY_NAME_LENGTH);

    app->version_tag =
        furi_string_alloc_printf("%s  %s", version_get_version(NULL), version_get_builddate(NULL));

    return app;
}

void cfw_app_free(CfwApp* app) {
    furi_assert(app);

    // Gui modules
    view_dispatcher_remove_view(app->view_dispatcher, CfwAppViewVarItemList);
    variable_item_list_free(app->var_item_list);
    view_dispatcher_remove_view(app->view_dispatcher, CfwAppViewTextInput);
    text_input_free(app->text_input);
    view_dispatcher_remove_view(app->view_dispatcher, CfwAppViewPopup);
    popup_free(app->popup);

    // View Dispatcher and Scene Manager
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    // Settings deinit

    CharList_it_t it;

    for(CharList_it(it, app->mainmenu_app_names); !CharList_end_p(it); CharList_next(it)) {
        free(*CharList_cref(it));
    }
    CharList_clear(app->mainmenu_app_names);
    for(CharList_it(it, app->mainmenu_app_paths); !CharList_end_p(it); CharList_next(it)) {
        free(*CharList_cref(it));
    }
    CharList_clear(app->mainmenu_app_paths);

    FrequencyList_clear(app->subghz_static_freqs);
    FrequencyList_clear(app->subghz_hopper_freqs);

    furi_string_free(app->version_tag);

    // Records
    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_DIALOGS);
    furi_record_close(RECORD_GUI);
    free(app);
}

extern int32_t cfw_app(void* p) {
    UNUSED(p);
    CfwApp* app = cfw_app_alloc();
    DESKTOP_SETTINGS_LOAD(&app->desktop);
    passport_settings_load(&app->passport);
    scene_manager_next_scene(app->scene_manager, CfwAppSceneStart);
    view_dispatcher_run(app->view_dispatcher);
    cfw_app_free(app);
    return 0;
}
