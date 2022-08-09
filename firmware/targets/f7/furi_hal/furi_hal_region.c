#include <furi_hal_region.h>
#include <flipper.pb.h>

FuriHalRegion* furi_hal_region = NULL;

void furi_hal_region_init() {
    PB_Region_Band band = {
        .start = 300000000,
        .end = 310000000,
        .power_limit = 6,
        .duty_cycle = 50,
    };

    (void)band;
}

void furi_hal_region_set(FuriHalRegion* region) {
    furi_hal_region = region;
}

bool furi_hal_region_is_provisioned() {
    return furi_hal_region != NULL;
}

bool furi_hal_region_is_frequency_allowed(uint32_t frequency) {
    if(!furi_hal_region) {
        return false;
    }

    FuriHalRegionBand* band = furi_hal_region_get_band(frequency);
    if(!band) {
        return false;
    }

    return true;
}

FuriHalRegionBand* furi_hal_region_get_band(uint32_t frequency) {
    if(!furi_hal_region) {
        return NULL;
    }

    for(size_t i = 0; i < furi_hal_region->bands_count; i++) {
        if(furi_hal_region->bands[i].start <= frequency &&
           furi_hal_region->bands[i].end >= frequency) {
            return &furi_hal_region->bands[i];
        }
    }

    return NULL;
}