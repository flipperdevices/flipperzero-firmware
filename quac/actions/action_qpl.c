// Methods for Quac Playlist transmission

#include <toolbox/stream/stream.h>
#include <toolbox/stream/file_stream.h>
#include <toolbox/path.h>
#include <toolbox/args.h>

#include <notification/notification_messages.h>

#include "action_i.h"
#include "quac.h"

/** Open the Playlist file and then transmit each action
 * 
 * Each line of the playlist file is one of:
 *   <file_path>
 *      Full SD card path, or relative path to action to be transmitted. Must be
 *      one of the supported filetypes (.sub, .rfid, [.ir coming soon])
 * 
 *      If an .rfid file has a space followed by a number, that will be the
 *      duration for that RFID transmission. All other .rfid files will use
 *      the value specified in the Settings
 * 
 *   pause <ms>
 *      Pauses the playback for 'ms' milliseconds.
 * 
 * Blank lines, and comments (start with '#') are ignored. Whitespace is trimmed.
 * 
*/
void action_qpl_tx(void* context, const FuriString* action_path, FuriString* error) {
    App* app = context;

    // Save the current RFID and NFC Durations, in case the are changed during playback
    uint32_t orig_rfid_duration = app->settings.rfid_duration;
    uint32_t orig_nfc_duration = app->settings.nfc_duration;

    FuriString* buffer;
    buffer = furi_string_alloc();

    Stream* file = file_stream_alloc(app->storage);
    if(file_stream_open(file, furi_string_get_cstr(action_path), FSAM_READ, FSOM_OPEN_EXISTING)) {
        while(stream_read_line(file, buffer)) {
            furi_string_trim(buffer); // remove '\n\r' line endings, cleanup spaces
            // FURI_LOG_I(TAG, "line: %s", furi_string_get_cstr(buffer));

            // Skip blank lines
            if(furi_string_size(buffer) == 0) {
                continue;
            }

            // Skip comments
            char first_char = furi_string_get_char(buffer, 0);
            if(first_char == '#') {
                continue;
            }

            // Check if buffer is a "command", and not just a filename
            // Commands will contain spaces
            bool processed_special_command = false;
            FuriString* args_tmp;
            args_tmp = furi_string_alloc();
            do {
                if(!args_read_string_and_trim(buffer, args_tmp)) {
                    // No spaces found, buffer and args_tmp are now have same contents
                    break;
                }

                // FURI_LOG_I(
                //     TAG,
                //     "args_temp: '%s', buffer: '%s'",
                //     furi_string_get_cstr(args_tmp),
                //     furi_string_get_cstr(buffer));

                // OK, there's a space, and args_tmp is the first token, buffer is the rest
                if(furi_string_cmpi_str(args_tmp, "pause") == 0) {
                    processed_special_command = true;
                    uint32_t pause_length = 0;
                    if(sscanf(furi_string_get_cstr(buffer), "%lu", &pause_length) == 1) {
                        FURI_LOG_I(TAG, "Pausing playlist for %lu ms", pause_length);
                        furi_delay_ms(pause_length);
                    } else {
                        ACTION_SET_ERROR("Playlist: Invalid or missing pause time");
                    }
                    break;
                }

                // First token wasn't "pause", so maybe args_tmp is a .rfid filename followed
                // by a transmit duration in ms in buffer
                // Note: Not using path_extract_extension since it expects to find slashes in the
                // path, and thus won't work if we have a relative path file
                char ext[MAX_EXT_LEN + 1] = "";
                size_t dot = furi_string_search_rchar(args_tmp, '.');
                if(dot != FURI_STRING_FAILURE && furi_string_size(args_tmp) - dot <= MAX_EXT_LEN) {
                    strlcpy(ext, &(furi_string_get_cstr(args_tmp))[dot], MAX_EXT_LEN);
                }

                // FURI_LOG_I(TAG, " - Found extension of %s", ext);

                if(!strcmp(ext, ".rfid")) {
                    uint32_t rfid_duration = 0;
                    // FURI_LOG_I(TAG, "RFID file with duration");
                    if(sscanf(furi_string_get_cstr(buffer), "%lu", &rfid_duration) == 1) {
                        FURI_LOG_I(TAG, "RFID duration = %lu", rfid_duration);
                        app->settings.rfid_duration = rfid_duration;
                    }
                } else if(!strcmp(ext, ".nfc")) {
                    uint32_t nfc_duration = 0;
                    if(sscanf(furi_string_get_cstr(buffer), "%lu", &nfc_duration) == 1) {
                        FURI_LOG_I(TAG, "NFC duration = %lu", nfc_duration);
                        app->settings.nfc_duration = nfc_duration;
                    }
                }

            } while(false);

            furi_string_swap(buffer, args_tmp);
            furi_string_free(args_tmp);

            if(processed_special_command) {
                continue;
            }

            first_char = furi_string_get_char(buffer, 0);
            // Using relative paths? Prepend path of our playlist file
            if(first_char != '/') {
                FuriString* dirname;
                dirname = furi_string_alloc();
                path_extract_dirname(furi_string_get_cstr(action_path), dirname);
                furi_string_cat_printf(dirname, "/%s", furi_string_get_cstr(buffer));
                furi_string_swap(dirname, buffer);
                furi_string_free(dirname);
            }

            char ext[MAX_EXT_LEN + 1] = "";
            path_extract_extension(buffer, ext, MAX_EXT_LEN);
            if(!strcmp(ext, ".sub")) {
                action_subghz_tx(context, buffer, error);
            } else if(!strcmp(ext, ".rfid")) {
                action_rfid_tx(context, buffer, error);
                // Reset our default duration back - in case it was changed during playback
                app->settings.rfid_duration = orig_rfid_duration;
            } else if(!strcmp(ext, ".ir")) {
                action_ir_tx(context, buffer, error);
            } else if(!strcmp(ext, ".nfc")) {
                action_nfc_tx(context, buffer, error);
                // Reset our default duration back - in case it was changed during playback
                app->settings.nfc_duration = orig_nfc_duration;
            } else if(!strcmp(ext, ".qpl")) {
                ACTION_SET_ERROR("Playlist: Can't call playlist from playlist");
            } else {
                ACTION_SET_ERROR(
                    "Playlist: Unknown file/command! %s", furi_string_get_cstr(buffer));
            }

            if(furi_string_size(error)) {
                // Abort playing the playlist - one of our actions failed
                break;
            }

            // Playlist action complete!
            // TODO: Do we need a small delay (say 25ms) between actions?
            // TODO: Should we blip the light a diff color to indicate that
            //       we're done with one command and moving to the next?
            // furi_delay_ms(25);
        }
    } else {
        ACTION_SET_ERROR("Could not open playlist");
    }

    furi_string_free(buffer);
    file_stream_close(file);
    stream_free(file);
}