#include <cli/cli_i.h>

#include <storage/storage.h>
#include <flipper.pb.h>
#include <pb_decode.h>

#define SUBGHZ_REGION_FILENAME "/int/.region_data"

static void subghz_cli_command_wrapper(Cli* cli, FuriString* args, void* context) {
    cli_plugin_wrapper("subghz_cli", 1, cli, args, context);
}

static void subghz_cli_command_chat_wrapper(Cli* cli, FuriString* args, void* context) {
    furi_string_replace_at(args, 0, 0, "chat ");
    subghz_cli_command_wrapper(cli, args, context);
}

static bool
    subghz_on_system_start_istream_read(pb_istream_t* istream, pb_byte_t* buf, size_t count) {
    File* file = istream->state;
    size_t ret = storage_file_read(file, buf, count);
    return (count == ret);
}

static bool subghz_on_system_start_istream_decode_band(
    pb_istream_t* stream,
    const pb_field_t* field,
    void** arg) {
    (void)field;
    FuriHalRegion* region = *arg;

    PB_Region_Band band = {0};
    if(!pb_decode(stream, PB_Region_Band_fields, &band)) {
        FURI_LOG_E("SubGhzOnStart", "PB Region band decode error: %s", PB_GET_ERROR(stream));
        return false;
    }

    region->bands_count += 1;
    region = realloc( //-V701
        region,
        sizeof(FuriHalRegion) + sizeof(FuriHalRegionBand) * region->bands_count);
    size_t pos = region->bands_count - 1;
    region->bands[pos].start = band.start;
    region->bands[pos].end = band.end;
    region->bands[pos].power_limit = band.power_limit;
    region->bands[pos].duty_cycle = band.duty_cycle;
    *arg = region;

    FURI_LOG_I(
        "SubGhzOnStart",
        "Add allowed band: start %luHz, stop %luHz, power_limit %ddBm, duty_cycle %u%%",
        band.start,
        band.end,
        band.power_limit,
        band.duty_cycle);
    return true;
}

void subghz_on_system_start(void) {
    Cli* cli = furi_record_open(RECORD_CLI);
    cli_add_command(cli, "subghz", CliCommandFlagDefault, subghz_cli_command_wrapper, NULL);
    cli_add_command(cli, "chat", CliCommandFlagDefault, subghz_cli_command_chat_wrapper, NULL);
    furi_record_close(RECORD_CLI);

#ifdef SRV_STORAGE
    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);
    FileInfo fileinfo = {0};
    PB_Region pb_region = {0};
    pb_region.bands.funcs.decode = subghz_on_system_start_istream_decode_band;

    do {
        if(storage_common_stat(storage, SUBGHZ_REGION_FILENAME, &fileinfo) != FSE_OK ||
           fileinfo.size == 0) {
            FURI_LOG_W("SubGhzOnStart", "Region data is missing or empty");
            break;
        }

        if(!storage_file_open(file, SUBGHZ_REGION_FILENAME, FSAM_READ, FSOM_OPEN_EXISTING)) {
            FURI_LOG_E("SubGhzOnStart", "Unable to open region data");
            break;
        }

        pb_istream_t istream = {
            .callback = subghz_on_system_start_istream_read,
            .state = file,
            .errmsg = NULL,
            .bytes_left = fileinfo.size,
        };

        pb_region.bands.arg = malloc(sizeof(FuriHalRegion));
        if(!pb_decode(&istream, PB_Region_fields, &pb_region)) {
            FURI_LOG_E("SubGhzOnStart", "Invalid region data");
            free(pb_region.bands.arg);
            break;
        }

        FuriHalRegion* region = pb_region.bands.arg;
        memcpy(
            region->country_code,
            pb_region.country_code->bytes,
            pb_region.country_code->size < 4 ? pb_region.country_code->size : 3);
        furi_hal_region_set(region);
    } while(0);

    pb_release(PB_Region_fields, &pb_region);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
#else
    UNUSED(subghz_on_system_start_istream_decode_band);
    UNUSED(subghz_on_system_start_istream_read);
#endif
}
