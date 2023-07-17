#include "nfc_supported_cards.h"
#include "../plugins/supported_cards/nfc_supported_card_plugin.h"

#include <flipper_application/flipper_application.h>
#include <flipper_application/plugins/plugin_manager.h>
#include <loader/firmware_api/firmware_api.h>

#include <furi.h>

#define TAG "NfcSupportedCards"

#define NFC_SUPPORTED_CARDS_PLUGINS_PATH APP_DATA_PATH("plugins")

typedef enum {
    NfcSupportCardsStatusIdle,
    NfcSupportCardsStatusLoadSuccess,
    NfcSupportCardsStatusLoadFailed,
} NfcSupportCardsStatus;

struct NfcSupportedCards {
    PluginManager* manager;
    uint32_t plugin_cnt;
    NfcSupportCardsStatus status;
};

NfcSupportedCards* nfc_supported_cards_alloc() {
    NfcSupportedCards* instance = malloc(sizeof(NfcSupportedCards));
    instance->manager = plugin_manager_alloc(
        NFC_SUPPORTED_CARD_PLUGIN_APP_ID,
        NFC_SUPPORTED_CARD_PLUGIN_API_VERSION,
        firmware_api_interface);
    if(plugin_manager_load_all(instance->manager, NFC_SUPPORTED_CARDS_PLUGINS_PATH) !=
       PluginManagerErrorNone) {
        FURI_LOG_W(TAG, "Failed to load all libs");
        instance->status = NfcSupportCardsStatusLoadFailed;
    } else {
        instance->plugin_cnt = plugin_manager_get_count(instance->manager);
        instance->status = NfcSupportCardsStatusLoadSuccess;
        FURI_LOG_D(TAG, "Loaded %ld parsers", instance->plugin_cnt);
    }

    return instance;
}

void nfc_supported_cards_free(NfcSupportedCards* instance) {
    furi_assert(instance);

    plugin_manager_free(instance->manager);
    free(instance);
}

bool nfc_supported_cards_read(NfcSupportedCards* instance, NfcDevice* device, Nfc* nfc) {
    furi_assert(instance);
    furi_assert(device);
    furi_assert(nfc);

    bool card_read = false;

    do {
        if(instance->status != NfcSupportCardsStatusLoadSuccess) break;

        for(uint32_t i = 0; i < instance->plugin_cnt; i++) {
            const NfcSupportedCardsPlugin* plugin = plugin_manager_get_ep(instance->manager, i);
            const NfcProtocol protocol = nfc_device_get_protocol(device);

            if(plugin->protocol != protocol) continue;

            if(plugin->verify) {
                if(!plugin->verify(nfc)) continue;
            }

            if(plugin->read) {
                card_read = plugin->read(nfc, device);
            }

            if(card_read) break;
        }
    } while(false);

    return card_read;
}

bool nfc_supported_cards_parse(
    NfcSupportedCards* instance,
    const NfcDevice* device,
    FuriString* parsed_data) {
    furi_assert(instance);
    furi_assert(device);
    furi_assert(parsed_data);

    bool parsed = false;

    do {
        if(instance->status != NfcSupportCardsStatusLoadSuccess) break;

        for(uint32_t i = 0; i < instance->plugin_cnt; i++) {
            const NfcSupportedCardsPlugin* plugin = plugin_manager_get_ep(instance->manager, i);
            const NfcProtocol protocol = nfc_device_get_protocol(device);
            if(plugin->protocol != protocol) continue;
            if(plugin->parse) {
                parsed = plugin->parse(device, parsed_data);
            }
            if(parsed) break;
        }
    } while(false);

    return parsed;
}
