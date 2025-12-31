// https://github.com/metrodroid/metrodroid/wiki/Octopus

#include "nfc_supported_card_plugin.h"
#include <flipper_application.h>
#include <nfc/protocols/felica/felica.h>
#include <bit_lib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAG "Octopus"

#define SERVICE_CODE_OCTOPUS_IN_LE (0x0117U)

bool octopus_parse(const NfcDevice* device, FuriString* parsed_data) {
    furi_assert(device);
    furi_assert(parsed_data);
    FURI_LOG_I(TAG, "Parsing Octopus card");
    bool parsed = false;

    if(nfc_device_get_protocol(device) != NfcProtocolFelica) return false;

    const FelicaData* felica_data = nfc_device_get_data(device, NfcProtocolFelica);

    for(uint16_t i = 0; i < simple_array_get_count(felica_data->public_blocks); i++) {
        FelicaPublicBlock* public_block = simple_array_get(felica_data->public_blocks, i);
        if(public_block->service_code == SERVICE_CODE_OCTOPUS_IN_LE) {
            uint32_t unsigned_balance = ((uint32_t)public_block->block.data[0] << 24) +
                                        ((uint32_t)public_block->block.data[1] << 16) +
                                        ((uint32_t)public_block->block.data[2] << 8) +
                                        (uint32_t)public_block->block.data[3];

            int64_t older_balance_dimes = (int64_t)unsigned_balance - 350; // pre-2017, in dimes
            int64_t newer_balance_dimes = (int64_t)unsigned_balance - 500; // post-2017, in dimes

            bool older_is_negative = older_balance_dimes < 0;
            bool newer_is_negative = newer_balance_dimes < 0;

            uint64_t older_abs_dimes =
                (uint64_t)(older_is_negative ? -older_balance_dimes : older_balance_dimes);
            uint64_t newer_abs_dimes =
                (uint64_t)(newer_is_negative ? -newer_balance_dimes : newer_balance_dimes);

            uint32_t older_dollars = (uint32_t)(older_abs_dimes / 10);
            uint8_t older_dimes = (uint8_t)(older_abs_dimes % 10);

            uint32_t newer_dollars = (uint32_t)(newer_abs_dimes / 10);
            uint8_t newer_dimes = (uint8_t)(newer_abs_dimes % 10);

            furi_string_printf(parsed_data, "\e#Octopus Card\n");

            furi_string_cat_str(
                parsed_data, "::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");

            furi_string_cat_printf(
                parsed_data, "If this card was issued \nafter 2017 October 1st:\n");
            furi_string_cat_printf(
                parsed_data,
                "Balance: %sHK$ %ld.%01d0\n",
                newer_is_negative ? "-" : "",
                newer_dollars,
                newer_dimes);

            furi_string_cat_str(
                parsed_data, "::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");

            furi_string_cat_printf(
                parsed_data,
                "If this card was issued \nbefore 2017 October 1st\n or is a tourist card:\n");
            furi_string_cat_printf(
                parsed_data,
                "Balance: %sHK$ %ld.%01d0\n",
                older_is_negative ? "-" : "",
                older_dollars,
                older_dimes);

            furi_string_cat_str(
                parsed_data, "::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::");

            parsed = true;
            break; // Octopus only has one public block
        }
    }

    return parsed;
}

/* Actual implementation of app<>plugin interface */
static const NfcSupportedCardsPlugin octopus_plugin = {
    .protocol = NfcProtocolFelica,
    .verify = NULL,
    .read = NULL,
    .parse = octopus_parse,
};

/* Plugin descriptor to comply with basic plugin specification */
static const FlipperAppPluginDescriptor octopus_plugin_descriptor = {
    .appid = NFC_SUPPORTED_CARD_PLUGIN_APP_ID,
    .ep_api_version = NFC_SUPPORTED_CARD_PLUGIN_API_VERSION,
    .entry_point = &octopus_plugin,
};

/* Plugin entry point - must return a pointer to const descriptor  */
const FlipperAppPluginDescriptor* octopus_plugin_ep(void) {
    return &octopus_plugin_descriptor;
}
