#include <furi.h>
#include <furi_hal.h>

#include <firmware/targets/f7/furi_hal/furi_hal_subghz_i.h>

#include <flipper_format/flipper_format_i.h>

void subghz_dangerous_freq() {
    bool is_extended_i = false;
    bool is_bypassed_i = false;

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* fff_data_file = flipper_format_file_alloc(storage);

    if(flipper_format_file_open_existing(fff_data_file, "/ext/subghz/assets/extend_range.txt")) {
        flipper_format_read_bool(fff_data_file, "use_ext_range_at_own_risk", &is_extended_i, 1);
        flipper_format_read_bool(fff_data_file, "ignore_default_tx_region", &is_bypassed_i, 1);
    }

    // Not set up to store and use a variable on tests yet (function below re-sets the settings on the file that it just read)
    // furi_hal_subghz_set_extend_settings(is_extended_i, is_bypassed_i);

    flipper_format_free(fff_data_file);
    furi_record_close(RECORD_STORAGE);
}
