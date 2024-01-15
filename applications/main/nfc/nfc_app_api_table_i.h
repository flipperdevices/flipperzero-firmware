#include "nfc_app_api.h"

/* 
 * A list of app's private functions and objects to expose for plugins.
 * It is used to generate a table of symbols for import resolver to use.
 * TBD: automatically generate this table from app's header files
 */
static constexpr auto nfc_app_api_table = sort(create_array_t<sym_entry>(
    API_METHOD(
        nfc_app_api_gallagher_deobfuscate_and_parse_credential,
        void,
        (GallagherCredential * credential, const uint8_t* cardholder_data_obfuscated)),
    API_VARIABLE(GALLAGHER_CREDENTIAL_SECTOR, const uint8_t),
    API_VARIABLE(
        GALLAGHER_CARDAX_ASCII,
        const uint8_t GALLAGHER_CARDAX_ASCII[MF_CLASSIC_BLOCK_SIZE])));