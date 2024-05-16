#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_subghz_i.h>
#include <subghz/subghz_last_settings.h>
#include <flipper_format/flipper_format_i.h>

void subghz_extended_freq(void) {
    bool is_extended_i = false;
    bool is_bypassed = false;
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* file = flipper_format_file_alloc(storage);

    if(flipper_format_file_open_existing(file, "/ext/subghz/assets/extend_range.txt")) {
        flipper_format_read_bool(file, "use_ext_range_at_own_risk", &is_extended_i, 1);
        flipper_format_read_bool(file, "ignore_default_tx_region", &is_bypassed, 1);
    }

    furi_hal_subghz_set_extended_range(is_extended_i);
    furi_hal_subghz_set_bypass_region(is_bypassed);

    flipper_format_free(file);
    furi_record_close(RECORD_STORAGE);

    // Load external module power amp setting (TODO: move to other place)
    // TODO: Disable this when external module is not CC1101 E07
    SubGhzLastSettings* last_settings = subghz_last_settings_alloc();
    subghz_last_settings_load(last_settings, 0);

    subghz_last_settings_free(last_settings);
}
