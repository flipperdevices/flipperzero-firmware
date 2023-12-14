#include "nfc_sup_cards.h"

#include "../plugins/supported_cards/nfc_supported_card_plugin.h"

#include <flipper_application/flipper_application.h>
#include <flipper_application/plugins/plugin_manager.h>
#include <loader/firmware_api/firmware_api.h>

#include <furi.h>
#include <path.h>
#include <m-array.h>

#define TAG "NfcSupportedCards"

#define NFC_SUPPORTED_CARDS_PLUGINS_PATH APP_DATA_PATH("plugins")
#define NFC_SUPPORTED_CARDS_PLUGIN_SUFFIX "_parser.fal"

typedef enum {
    NfcSupCardsPluginFeatureHasVerify = (1U << 0),
    NfcSupCardsPluginFeatureHasRead = (1U << 1),
    NfcSupCardsPluginFeatureHasParse = (1U << 2),
} NfcSupCardsPluginFeature;

typedef struct {
    FuriString* path;
    NfcProtocol protocol;
    NfcSupCardsPluginFeature feature;
} NfcSupCardsPluginCache;

ARRAY_DEF(NfcSupCardsPluginCache, NfcSupCardsPluginCache, M_POD_OPLIST);

typedef enum {
    NfcSupCardsLoadStateIdle,
    NfcSupCardsLoadStateInProgress,
    NfcSupCardsLoadStateSuccess,
    NfcSupCardsLoadStateFail,
} NfcSupCardsLoadState;

typedef struct {
    Storage* storage;
    File* directory;
    FuriString* file_path;
    char file_name[256];
    FlipperApplication* app;
} NfcSupCardsLoadContext;

struct NfcSupCards {
    NfcSupCardsPluginCache_t plugins_cache_arr;
    NfcSupCardsLoadState load_state;
    NfcSupCardsLoadContext* load_context;
};

NfcSupCards* nfc_sup_cards_alloc() {
    NfcSupCards* instance = malloc(sizeof(NfcSupCards));
    NfcSupCardsPluginCache_init(instance->plugins_cache_arr);

    return instance;
}

void nfc_sup_cards_free(NfcSupCards* instance) {
    furi_assert(instance);

    NfcSupCardsPluginCache_it_t iter;
    for(NfcSupCardsPluginCache_it(iter, instance->plugins_cache_arr);
        !NfcSupCardsPluginCache_end_p(iter);
        NfcSupCardsPluginCache_next(iter)) {
        NfcSupCardsPluginCache* plugin_cache = NfcSupCardsPluginCache_ref(iter);
        furi_string_free(plugin_cache->path);
    }

    NfcSupCardsPluginCache_clear(instance->plugins_cache_arr);
    free(instance);
}

static NfcSupCardsLoadContext* nfc_sup_cards_load_context_alloc() {
    NfcSupCardsLoadContext* instance = malloc(sizeof(NfcSupCardsLoadContext));

    instance->storage = furi_record_open(RECORD_STORAGE);
    instance->directory = storage_file_alloc(instance->storage);
    instance->file_path = furi_string_alloc();

    if(!storage_dir_open(instance->directory, NFC_SUPPORTED_CARDS_PLUGINS_PATH)) {
        FURI_LOG_D(TAG, "Failed to open directory: %s", NFC_SUPPORTED_CARDS_PLUGINS_PATH);
    }

    return instance;
}

static void nfc_sup_cards_load_context_free(NfcSupCardsLoadContext* instance) {
    if(instance->app) {
        flipper_application_free(instance->app);
    }

    furi_string_free(instance->file_path);

    storage_dir_close(instance->directory);
    storage_file_free(instance->directory);

    furi_record_close(RECORD_STORAGE);
    free(instance);
}

static const NfcSupportedCardsPlugin*
    nfc_sup_cards_get_plugin(NfcSupCardsLoadContext* instance, FuriString* path) {
    furi_assert(instance);
    furi_assert(path);

    const NfcSupportedCardsPlugin* plugin = NULL;
    do {
        if(instance->app) flipper_application_free(instance->app);
        instance->app = flipper_application_alloc(instance->storage, firmware_api_interface);
        if(flipper_application_preload(instance->app, furi_string_get_cstr(path)) !=
           FlipperApplicationPreloadStatusSuccess)
            break;
        if(!flipper_application_is_plugin(instance->app)) break;
        if(flipper_application_map_to_memory(instance->app) != FlipperApplicationLoadStatusSuccess)
            break;
        const FlipperAppPluginDescriptor* descriptor =
            flipper_application_plugin_get_descriptor(instance->app);

        if(descriptor == NULL) break;

        if(strcmp(descriptor->appid, NFC_SUPPORTED_CARD_PLUGIN_APP_ID) != 0) break;
        if(descriptor->ep_api_version != NFC_SUPPORTED_CARD_PLUGIN_API_VERSION) break;

        plugin = descriptor->entry_point;
    } while(false);

    return plugin;
}

static const NfcSupportedCardsPlugin*
    nfc_sup_cards_get_next_plugin(NfcSupCardsLoadContext* instance) {
    const NfcSupportedCardsPlugin* plugin = NULL;

    do {
        if(!storage_file_is_open(instance->directory)) break;
        if(!storage_dir_read(
               instance->directory, NULL, instance->file_name, sizeof(instance->file_name)))
            break;

        furi_string_set(instance->file_path, instance->file_name);
        if(!furi_string_end_with_str(instance->file_path, NFC_SUPPORTED_CARDS_PLUGIN_SUFFIX))
            continue;

        path_concat(NFC_SUPPORTED_CARDS_PLUGINS_PATH, instance->file_name, instance->file_path);

        if(instance->app) flipper_application_free(instance->app);
        instance->app = flipper_application_alloc(instance->storage, firmware_api_interface);

        if(flipper_application_preload(instance->app, furi_string_get_cstr(instance->file_path)) !=
           FlipperApplicationPreloadStatusSuccess)
            continue;
        if(!flipper_application_is_plugin(instance->app)) continue;

        if(flipper_application_map_to_memory(instance->app) != FlipperApplicationLoadStatusSuccess)
            continue;

        const FlipperAppPluginDescriptor* descriptor =
            flipper_application_plugin_get_descriptor(instance->app);

        if(descriptor == NULL) continue;

        if(strcmp(descriptor->appid, NFC_SUPPORTED_CARD_PLUGIN_APP_ID) != 0) continue;
        if(descriptor->ep_api_version != NFC_SUPPORTED_CARD_PLUGIN_API_VERSION) continue;

        plugin = descriptor->entry_point;
    } while(plugin == NULL); //-V654

    return plugin;
}

void nfc_sup_cards_load_cache(NfcSupCards* instance) {
    furi_assert(instance);

    do {
        if((instance->load_state == NfcSupCardsLoadStateSuccess) ||
           (instance->load_state == NfcSupCardsLoadStateFail))
            break;

        instance->load_context = nfc_sup_cards_load_context_alloc();

        while(true) {
            const NfcSupportedCardsPlugin* plugin =
                nfc_sup_cards_get_next_plugin(instance->load_context);
            if(plugin == NULL) break; //-V547

            FURI_LOG_D(
                TAG,
                "Load cache for plugin %s",
                furi_string_get_cstr(instance->load_context->file_path));
            NfcSupCardsPluginCache plugin_cache = {};
            plugin_cache.path = furi_string_alloc_set(instance->load_context->file_path);
            plugin_cache.protocol = plugin->protocol;
            if(plugin->verify) {
                plugin_cache.feature |= NfcSupCardsPluginFeatureHasVerify;
            }
            if(plugin->read) {
                plugin_cache.feature |= NfcSupCardsPluginFeatureHasRead;
            }
            if(plugin->parse) {
                plugin_cache.feature |= NfcSupCardsPluginFeatureHasParse;
            }
            NfcSupCardsPluginCache_push_back(instance->plugins_cache_arr, plugin_cache);
        }

        nfc_sup_cards_load_context_free(instance->load_context);

        size_t plugins_loaded = NfcSupCardsPluginCache_size(instance->plugins_cache_arr);
        if(plugins_loaded == 0) {
            FURI_LOG_D(TAG, "Plugins not found");
            instance->load_state = NfcSupCardsLoadStateFail;
        } else {
            FURI_LOG_D(TAG, "Loaded %d plugins", plugins_loaded);
            instance->load_state = NfcSupCardsLoadStateSuccess;
        }

    } while(false);
}

bool nfc_sup_cards_read(NfcSupCards* instance, NfcDevice* device, Nfc* nfc) {
    furi_assert(instance);
    furi_assert(device);
    furi_assert(nfc);

    bool card_read = false;
    NfcProtocol protocol = nfc_device_get_protocol(device);

    do {
        if(instance->load_state != NfcSupCardsLoadStateSuccess) break;

        instance->load_context = nfc_sup_cards_load_context_alloc();

        NfcSupCardsPluginCache_it_t iter;
        for(NfcSupCardsPluginCache_it(iter, instance->plugins_cache_arr);
            !NfcSupCardsPluginCache_end_p(iter);
            NfcSupCardsPluginCache_next(iter)) {
            NfcSupCardsPluginCache* plugin_cache = NfcSupCardsPluginCache_ref(iter);
            if(plugin_cache->protocol != protocol) continue;
            if((plugin_cache->feature & NfcSupCardsPluginFeatureHasRead) == 0) continue;

            const NfcSupportedCardsPlugin* plugin =
                nfc_sup_cards_get_plugin(instance->load_context, plugin_cache->path);
            if(plugin == NULL) continue;

            if(plugin->verify) {
                if(!plugin->verify(nfc)) continue;
            }

            if(plugin->read) {
                if(plugin->read(nfc, device)) {
                    card_read = true;
                    break;
                }
            }
        }

        nfc_sup_cards_load_context_free(instance->load_context);
    } while(false);

    return card_read;
}

bool nfc_sup_cards_parse(NfcSupCards* instance, NfcDevice* device, FuriString* parsed_data) {
    furi_assert(instance);
    furi_assert(device);
    furi_assert(parsed_data);

    bool card_parsed = false;
    NfcProtocol protocol = nfc_device_get_protocol(device);

    do {
        if(instance->load_state != NfcSupCardsLoadStateSuccess) break;

        instance->load_context = nfc_sup_cards_load_context_alloc();

        NfcSupCardsPluginCache_it_t iter;
        for(NfcSupCardsPluginCache_it(iter, instance->plugins_cache_arr);
            !NfcSupCardsPluginCache_end_p(iter);
            NfcSupCardsPluginCache_next(iter)) {
            NfcSupCardsPluginCache* plugin_cache = NfcSupCardsPluginCache_ref(iter);
            if(plugin_cache->protocol != protocol) continue;
            if((plugin_cache->feature & NfcSupCardsPluginFeatureHasParse) == 0) continue;

            const NfcSupportedCardsPlugin* plugin =
                nfc_sup_cards_get_plugin(instance->load_context, plugin_cache->path);
            if(plugin == NULL) continue;

            if(plugin->parse) {
                if(plugin->parse(device, parsed_data)) {
                    card_parsed = true;
                    break;
                }
            }
        }

        nfc_sup_cards_load_context_free(instance->load_context);
    } while(false);

    return card_parsed;
}
