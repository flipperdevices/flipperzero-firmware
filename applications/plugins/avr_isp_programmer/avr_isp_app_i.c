#include "avr_isp_app_i.h"

#define TAG "AvrIsp"
#include <flipper_format/flipper_format_i.h>


bool avr_isp_load_from_file(AvrIspApp* app) {
    furi_assert(app);

    FuriString* file_path;
    file_path = furi_string_alloc();

    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, AVR_ISP_APP_EXTENSION, &I_avr_isp);
    browser_options.base_path = STORAGE_APP_DATA_PATH_PREFIX;

    // Input events and views are managed by file_select
    bool res = dialog_file_browser_show(
        app->dialogs, app->file_path, app->file_path, &browser_options);

    // if(res) {
    //     res = subghz_key_load(app, furi_string_get_cstr(app->file_path), true);
    // }

    furi_string_free(file_path);

    return res;
}



