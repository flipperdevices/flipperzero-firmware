#include "nfc_supported_cards.h"
#include "../plugins/nfc_supported_card_plugin.h"

#include <flipper_application/flipper_application.h>
#include <flipper_application/plugins/plugin_manager.h>
#include <loader/firmware_api/firmware_api.h>

#include <furi.h>

#define TAG "NfcSupportedCards"

#define NFC_SUPPORTED_CARDS_PLUGINS_PATH EXT_PATH("nfc/plugins")

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

bool nfc_supported_cards_read(
    NfcSupportedCards* instance,
    NfcDevProtocol protocol,
    void* poller,
    void* data) {
    furi_assert(instance);
    furi_assert(poller);
    furi_assert(data);

    bool card_read = false;
    bool verified = false;

    do {
        if(instance->status != NfcSupportCardsStatusLoadSuccess) break;

        for(size_t i = 0; i < instance->plugin_cnt; i++) {
            const NfcSupportedCardsPlugin* plugin = plugin_manager_get_ep(instance->manager, i);
            if(plugin->protocol != protocol) continue;
            if(plugin->verify) {
                verified = plugin->verify(poller);
            }
            if(verified && (plugin->read)) {
                card_read = plugin->read(poller, data);
            }
            if(card_read) break;
        }
    } while(false);

    return card_read;
}

bool nfc_supported_cards_parse(
    NfcSupportedCards* instance,
    NfcDevProtocol protocol,
    void* data,
    FuriString* parsed_data) {
    furi_assert(instance);
    furi_assert(data);
    furi_assert(parsed_data);

    bool parsed = false;

    do {
        if(instance->status != NfcSupportCardsStatusLoadSuccess) break;

        for(size_t i = 0; i < instance->plugin_cnt; i++) {
            const NfcSupportedCardsPlugin* plugin = plugin_manager_get_ep(instance->manager, i);
            if(plugin->protocol != protocol) continue;
            if(plugin->parse) {
                parsed = plugin->parse(data, parsed_data);
            }
            if(parsed) break;
        }
    } while(false);

    return parsed;
}
