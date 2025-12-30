#include "mf_plus.h"
#include "mf_plus_render.h"

#include <nfc/protocols/mf_plus/mf_plus_poller.h>
#include <nfc/protocols/mf_classic/mf_classic.h>

#include <string.h>
#include <lib/bit_lib/bit_lib.h>

#include "nfc/nfc_app_i.h"
#include "../../mf_classic_key_cache.h"

#include "../nfc_protocol_support_common.h"
#include "../nfc_protocol_support_gui_common.h"
#include "../iso14443_4a/iso14443_4a_i.h"

static void nfc_scene_info_on_enter_mf_plus(NfcApp* instance) {
    const NfcDevice* device = instance->nfc_device;
    const MfPlusData* data = nfc_device_get_data(device, NfcProtocolMfPlus);

    FuriString* temp_str = furi_string_alloc();
    nfc_append_filename_string_when_present(instance, temp_str);
    furi_string_cat_printf(
        temp_str, "\e#%s\n", nfc_device_get_name(device, NfcDeviceNameTypeFull));
    furi_string_replace(temp_str, "Mifare", "MIFARE");
    nfc_render_mf_plus_info(data, NfcProtocolFormatTypeFull, temp_str);

    widget_add_text_scroll_element(
        instance->widget, 0, 0, 128, 64, furi_string_get_cstr(temp_str));

    furi_string_free(temp_str);
}
static NfcCommand nfc_scene_read_poller_callback_mf_plus(NfcGenericEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.protocol == NfcProtocolMfPlus);
    furi_assert(event.event_data);

    NfcApp* instance = context;
    const MfPlusPollerEvent* mf_plus_event = event.event_data;

    NfcCommand command = NfcCommandContinue;

    if(mf_plus_event->type == MfPlusPollerEventTypeReadSuccess) {
        nfc_device_set_data(
            instance->nfc_device, NfcProtocolMfPlus, nfc_poller_get_data(instance->poller));
        view_dispatcher_send_custom_event(instance->view_dispatcher, NfcCustomEventPollerSuccess);
        command = NfcCommandStop;
    } else if(mf_plus_event->type == MfPlusPollerEventTypeReadFailed) {
        command = NfcCommandReset;
    }

    return command;
}

static void nfc_scene_read_on_enter_mf_plus(NfcApp* instance) {
    nfc_poller_start(instance->poller, nfc_scene_read_poller_callback_mf_plus, instance);
}

static void nfc_scene_read_success_on_enter_mf_plus(NfcApp* instance) {
    const NfcDevice* device = instance->nfc_device;
    const MfPlusData* data = nfc_device_get_data(device, NfcProtocolMfPlus);

    FuriString* temp_str = furi_string_alloc();
    furi_string_cat_printf(
        temp_str, "\e#%s\n", nfc_device_get_name(device, NfcDeviceNameTypeFull));
    furi_string_replace(temp_str, "Mifare", "MIFARE");
    nfc_render_mf_plus_info(data, NfcProtocolFormatTypeShort, temp_str);

    widget_add_text_scroll_element(
        instance->widget, 0, 0, 128, 52, furi_string_get_cstr(temp_str));

    furi_string_free(temp_str);
}

static void nfc_scene_emulate_on_enter_mf_plus(NfcApp* instance) {
    const MfPlusData* mf_plus_data =
        nfc_device_get_data(instance->nfc_device, NfcProtocolMfPlus);

    // For SL1 2K cards, use Classic emulation (compatible with Classic readers)
    // MIFARE Plus 2K SL1 emulates as Classic with 32 sectors (128 blocks total)
    // This allows UID-only readers (like printers) to work, and exposes all 32 sectors
    if(mf_plus_data->security_level == MfPlusSecurityLevel1 &&
       mf_plus_data->size == MfPlusSize2K) {
        MfClassicData* classic_data = NULL;
        
        // Try to get Classic data if the card was read as Classic
        // This ensures we emulate the actual data that was scanned (all sectors, keys, blocks)
        const MfClassicData* existing_classic_data = NULL;
        // Check if device protocol is Classic (card was read as Classic, not just Plus)
        if(nfc_device_get_protocol(instance->nfc_device) == NfcProtocolMfClassic) {
            existing_classic_data = nfc_device_get_data(instance->nfc_device, NfcProtocolMfClassic);
        }
        
        if(existing_classic_data && existing_classic_data->type == MfClassicTypePlus2k) {
            // Use the actual Classic data that was read from the card
            // This contains all the real sector data, keys, and blocks from the scan
            classic_data = mf_classic_alloc();
            mf_classic_copy(classic_data, existing_classic_data);
            
            // Ensure sectors 18-31 are treated as uninitialized to match real card behavior
            // On real MIFARE Plus 2K SL1 cards, sectors 18-31 are typically empty/uninitialized
            // Clear key masks for sectors 18-31 if keys are zero (uninitialized)
            for(uint8_t sector = 18; sector < 32; sector++) {
                uint8_t sector_trailer_block = sector * 4 + 3;
                MfClassicSectorTrailer* sec_tr =
                    (MfClassicSectorTrailer*)&classic_data->block[sector_trailer_block];
                
                // Check if both keys are zero (uninitialized)
                bool key_a_zero = true;
                bool key_b_zero = true;
                for(int i = 0; i < 6; i++) {
                    if(sec_tr->key_a.data[i] != 0) key_a_zero = false;
                    if(sec_tr->key_b.data[i] != 0) key_b_zero = false;
                }
                
                // Check if keys were found in original read
                bool key_a_found_orig = mf_classic_is_key_found(existing_classic_data, sector, MfClassicKeyTypeA);
                bool key_b_found_orig = mf_classic_is_key_found(existing_classic_data, sector, MfClassicKeyTypeB);
                
                // Clear key masks if keys are zero (uninitialized) OR if they weren't found in original
                // This ensures empty sectors appear as uninitialized, matching real card
                // The listener will reject authentication attempts to sectors without keys found
                if(key_a_zero || !key_a_found_orig) {
                    mf_classic_set_key_not_found(classic_data, sector, MfClassicKeyTypeA);
                }
                if(key_b_zero || !key_b_found_orig) {
                    mf_classic_set_key_not_found(classic_data, sector, MfClassicKeyTypeB);
                }
            }
        } else {
            // No Classic data available - create minimal Classic data from MF Plus
            // This is a fallback when card was only read as Plus (without sector data)
            classic_data = mf_classic_alloc();
            classic_data->type = MfClassicTypePlus2k;
            
            // Initialize key masks to zero (no keys found) - sectors are uninitialized
            classic_data->key_a_mask = 0ULL;
            classic_data->key_b_mask = 0ULL;
            
            // Copy ISO14443-3A data from MF Plus (UID, ATQA, SAK)
            const Iso14443_3aData* iso3_data =
                iso14443_4a_get_base_data(mf_plus_data->iso14443_4a_data);
            if(iso3_data) {
                iso14443_3a_copy(classic_data->iso14443_3a_data, iso3_data);
                // Force SL1 Classic view: ATQA 0x0004, SAK 0x08 (no cascade bit)
                classic_data->iso14443_3a_data->atqa[0] = 0x04;
                classic_data->iso14443_3a_data->atqa[1] = 0x00;
                classic_data->iso14443_3a_data->sak = 0x08;
                // Ensure 4-byte UID form (the real card uses 4B UID)
                if(classic_data->iso14443_3a_data->uid_len > 4) {
                    classic_data->iso14443_3a_data->uid_len = 4;
                }
                
                // Try to load keys from key cache to speed up emulation
                // This allows emulation to work faster if keys were previously cached
                if(instance->mfc_key_cache) {
                    size_t uid_len = 0;
                    const uint8_t* uid = iso14443_3a_get_uid(iso3_data, &uid_len);
                    if(mf_classic_key_cache_load(instance->mfc_key_cache, uid, uid_len)) {
                        // Keys loaded from cache - copy them to classic_data
                        MfClassicDeviceKeys* cached_keys = &instance->mfc_key_cache->keys;
                        classic_data->key_a_mask = cached_keys->key_a_mask;
                        classic_data->key_b_mask = cached_keys->key_b_mask;
                        
                        // Copy cached keys to sector trailers
                        for(uint8_t sector = 0; sector < 32; sector++) {
                            if(FURI_BIT(cached_keys->key_a_mask, sector)) {
                                MfClassicSectorTrailer* sec_tr =
                                    mf_classic_get_sector_trailer_by_sector(classic_data, sector);
                                sec_tr->key_a = cached_keys->key_a[sector];
                                mf_classic_set_key_found(
                                    classic_data, sector, MfClassicKeyTypeA,
                                    bit_lib_bytes_to_num_be(cached_keys->key_a[sector].data, 6));
                            }
                            if(FURI_BIT(cached_keys->key_b_mask, sector)) {
                                MfClassicSectorTrailer* sec_tr =
                                    mf_classic_get_sector_trailer_by_sector(classic_data, sector);
                                sec_tr->key_b = cached_keys->key_b[sector];
                                mf_classic_set_key_found(
                                    classic_data, sector, MfClassicKeyTypeB,
                                    bit_lib_bytes_to_num_be(cached_keys->key_b[sector].data, 6));
                            }
                        }
                    }
                }
            }
            // Note: Without Classic data, sectors without cached keys are uninitialized (no keys found)
            // This matches real card behavior for empty sectors
        }
        
        instance->listener =
            nfc_listener_alloc(instance->nfc, NfcProtocolMfClassic, classic_data);
        nfc_listener_start(instance->listener, NULL, NULL);
    } else {
        // For SL2/SL3, use ISO14443-4A emulation
        const Iso14443_4aData* iso14443_4a_data =
            nfc_device_get_data(instance->nfc_device, NfcProtocolIso14443_4a);

        instance->listener =
            nfc_listener_alloc(instance->nfc, NfcProtocolIso14443_4a, iso14443_4a_data);
        nfc_listener_start(
            instance->listener, nfc_scene_emulate_listener_callback_iso14443_4a, instance);
    }
}

const NfcProtocolSupportBase nfc_protocol_support_mf_plus = {
    .features = NfcProtocolFeatureEmulateUid | NfcProtocolFeatureEmulateFull,

    .scene_info =
        {
            .on_enter = nfc_scene_info_on_enter_mf_plus,
            .on_event = nfc_protocol_support_common_on_event_empty,
        },
    .scene_more_info =
        {
            .on_enter = nfc_protocol_support_common_on_enter_empty,
            .on_event = nfc_protocol_support_common_on_event_empty,
        },
    .scene_read =
        {
            .on_enter = nfc_scene_read_on_enter_mf_plus,
            .on_event = nfc_protocol_support_common_on_event_empty,
        },
    .scene_read_menu =
        {
            .on_enter = nfc_protocol_support_common_on_enter_empty,
            .on_event = nfc_protocol_support_common_on_event_empty,
        },
    .scene_read_success =
        {
            .on_enter = nfc_scene_read_success_on_enter_mf_plus,
            .on_event = nfc_protocol_support_common_on_event_empty,
        },
    .scene_saved_menu =
        {
            .on_enter = nfc_protocol_support_common_on_enter_empty,
            .on_event = nfc_protocol_support_common_on_event_empty,
        },
    .scene_save_name =
        {
            .on_enter = nfc_protocol_support_common_on_enter_empty,
            .on_event = nfc_protocol_support_common_on_event_empty,
        },
    .scene_emulate =
        {
            .on_enter = nfc_scene_emulate_on_enter_mf_plus,
            .on_event = nfc_protocol_support_common_on_event_empty,
        },
};
