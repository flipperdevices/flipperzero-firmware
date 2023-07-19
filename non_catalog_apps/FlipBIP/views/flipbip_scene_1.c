#include "../flipbip.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
//#include <dolphin/dolphin.h>
#include <storage/storage.h>
#include <string.h>
#include "flipbip_icons.h"
#include "../helpers/flipbip_haptic.h"
#include "../helpers/flipbip_led.h"
#include "../helpers/flipbip_string.h"
#include "../helpers/flipbip_file.h"
// From: /lib/crypto
#include <memzero.h>
#include <rand.h>
#include <curves.h>
#include <bip32.h>
#include <bip39.h>

#define DERIV_PURPOSE 44
#define DERIV_ACCOUNT 0
#define DERIV_CHANGE 0

#define MAX_TEXT_LEN 30 // 30 = max length of text
#define MAX_TEXT_BUF (MAX_TEXT_LEN + 1) // max length of text + null terminator
#define MAX_ADDR_BUF (42 + 1) // 42 = max length of address + null terminator
#define NUM_ADDRS 6

#define PAGE_LOADING 0
#define PAGE_INFO 1
#define PAGE_MNEMONIC 2
#define PAGE_SEED 3
#define PAGE_XPRV_ROOT 4
#define PAGE_XPRV_ACCT 5
#define PAGE_XPUB_ACCT 6
#define PAGE_XPRV_EXTD 7
#define PAGE_XPUB_EXTD 8
#define PAGE_ADDR_BEGIN 9
#define PAGE_ADDR_END (PAGE_ADDR_BEGIN + NUM_ADDRS - 1)

#define TEXT_LOADING "Loading..."
#define TEXT_NEW_WALLET "New wallet"
#define TEXT_DEFAULT_COIN "Coin"
#define TEXT_RECEIVE_ADDRESS "receive address:"
#define TEXT_DEFAULT_DERIV "m/44'/X'/0'/0"
const char* TEXT_INFO = "-Scroll pages with up/down-"
                        "p1,2)   BIP39 Mnemonic/Seed"
                        "p3)       BIP32 Root Key   "
                        "p4,5)  Prv/Pub Account Keys"
                        "p6,7)  Prv/Pub BIP32 Keys  "
                        "p8+)    Receive Addresses  ";

// #define TEXT_SAVE_QR "Save QR"
#define TEXT_QRFILE_EXT ".qrcode" // 7 chars + 1 null

// bip44_coin, xprv_version, xpub_version, addr_version, wif_version, addr_format
const uint32_t COIN_INFO_ARRAY[3][6] = {
    {COIN_BTC, 0x0488ade4, 0x0488b21e, 0x00, 0x80, FlipBipCoinBTC0},
    {COIN_ETH, 0x0488ade4, 0x0488b21e, 0x00, 0x80, FlipBipCoinETH60},
    {COIN_DOGE, 0x02fac398, 0x02facafd, 0x1e, 0x9e, FlipBipCoinBTC0}};

// coin_name, derivation_path
const char* COIN_TEXT_ARRAY[3][3] = {
    {"BTC", "m/44'/0'/0'/0", "bitcoin:"},
    {"ETH", "m/44'/60'/0'/0", "ethereum:"},
    {"DOGE", "m/44'/3'/0'/0", "dogecoin:"}};

struct FlipBipScene1 {
    View* view;
    FlipBipScene1Callback callback;
    void* context;
};
typedef struct {
    int page;
    int strength;
    uint32_t coin;
    bool overwrite;
    bool mnemonic_only;
    CONFIDENTIAL const char* mnemonic;
    CONFIDENTIAL uint8_t seed[64];
    CONFIDENTIAL const HDNode* node;
    CONFIDENTIAL const char* xprv_root;
    CONFIDENTIAL const char* xprv_account;
    CONFIDENTIAL const char* xpub_account;
    CONFIDENTIAL const char* xprv_extended;
    CONFIDENTIAL const char* xpub_extended;
    char* recv_addresses[NUM_ADDRS];
} FlipBipScene1Model;

// Node for the receive address
static CONFIDENTIAL HDNode* s_addr_node = NULL;
// Generic display text
static CONFIDENTIAL char* s_disp_text1 = NULL;
static CONFIDENTIAL char* s_disp_text2 = NULL;
static CONFIDENTIAL char* s_disp_text3 = NULL;
static CONFIDENTIAL char* s_disp_text4 = NULL;
static CONFIDENTIAL char* s_disp_text5 = NULL;
static CONFIDENTIAL char* s_disp_text6 = NULL;
// Derivation path text
static const char* s_derivation_text = TEXT_DEFAULT_DERIV;
// Warning text
static bool s_warn_insecure = false;
#define WARN_INSECURE_TEXT_1 "Recommendation:"
#define WARN_INSECURE_TEXT_2 "Set BIP39 Passphrase"
//static bool s_busy = false;

void flipbip_scene_1_set_callback(
    FlipBipScene1* instance,
    FlipBipScene1Callback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

static void flipbip_scene_1_init_address(
    char* addr_text,
    const HDNode* node,
    uint32_t coin_type,
    uint32_t addr_index) {
    //s_busy = true;

    // buffer for address serialization
    // subtract 2 for "0x", 1 for null terminator
    const size_t buflen = MAX_ADDR_BUF - (2 + 1);
    // subtract 2 for "0x"
    char buf[MAX_ADDR_BUF - 2] = {0};

    // Use static node for address generation
    memcpy(s_addr_node, node, sizeof(HDNode));
    memzero(addr_text, MAX_ADDR_BUF);

    hdnode_private_ckd(s_addr_node, addr_index);
    hdnode_fill_public_key(s_addr_node);

    // coin info
    // bip44_coin, xprv_version, xpub_version, addr_version, wif_version, addr_format
    uint32_t coin_info[6] = {0};
    for(size_t i = 0; i < 6; i++) {
        coin_info[i] = COIN_INFO_ARRAY[coin_type][i];
    }

    if(coin_info[5] == FlipBipCoinBTC0) { // BTC / DOGE style address
        // BTC / DOGE style address
        ecdsa_get_address(
            s_addr_node->public_key, coin_info[3], HASHER_SHA2_RIPEMD, HASHER_SHA2D, buf, buflen);
        strcpy(addr_text, buf);

        //ecdsa_get_wif(addr_node->private_key, WIF_VERSION, HASHER_SHA2D, buf, buflen);

    } else if(coin_info[5] == FlipBipCoinETH60) { // ETH
        // ETH style address
        hdnode_get_ethereum_pubkeyhash(s_addr_node, (uint8_t*)buf);
        addr_text[0] = '0';
        addr_text[1] = 'x';
        // Convert the hash to a hex string
        flipbip_btox((uint8_t*)buf, 20, addr_text + 2);
    }

    // Clear the address node
    memzero(s_addr_node, sizeof(HDNode));

    //s_busy = false;
}

static void
    flipbip_scene_1_draw_generic(const char* text, const size_t line_len, const bool chunk) {
    // Split the text into parts
    size_t len = line_len;
    if(len > MAX_TEXT_LEN) {
        len = MAX_TEXT_LEN;
    }
    for(size_t si = 1; si <= 6; si++) {
        char* ptr = NULL;

        if(si == 1)
            ptr = s_disp_text1;
        else if(si == 2)
            ptr = s_disp_text2;
        else if(si == 3)
            ptr = s_disp_text3;
        else if(si == 4)
            ptr = s_disp_text4;
        else if(si == 5)
            ptr = s_disp_text5;
        else if(si == 6)
            ptr = s_disp_text6;

        memzero(ptr, MAX_TEXT_BUF);
        strncpy(ptr, text + ((si - 1) * len), len);
        // add a space every 4 characters and shift the text
        if(len < 23 && chunk) {
            for(size_t i = 0; i < strlen(ptr); i++) {
                if(i % 5 == 0) {
                    for(size_t j = strlen(ptr); j > i; j--) {
                        ptr[j] = ptr[j - 1];
                    }
                    ptr[i] = ' ';
                }
            }
        }
    }
}

static void flipbip_scene_1_draw_mnemonic(const char* mnemonic) {
    // Delineate sections of the mnemonic every 4 words
    const size_t mnemonic_working_len = strlen(mnemonic) + 1;
    char* mnemonic_working = malloc(mnemonic_working_len);
    strcpy(mnemonic_working, mnemonic);
    int word = 0;
    for(size_t i = 0; i < strlen(mnemonic_working); i++) {
        if(mnemonic_working[i] == ' ') {
            word++;
            if(word % 4 == 0) {
                mnemonic_working[i] = ',';
            }
        }
    }

    // Split the mnemonic into parts
    char* mnemonic_part = flipbip_strtok(mnemonic_working, ",");
    int mi = 0;
    while(mnemonic_part != NULL) {
        char* ptr = NULL;
        mi++;

        if(mi == 1)
            ptr = s_disp_text1;
        else if(mi == 2)
            ptr = s_disp_text2;
        else if(mi == 3)
            ptr = s_disp_text3;
        else if(mi == 4)
            ptr = s_disp_text4;
        else if(mi == 5)
            ptr = s_disp_text5;
        else if(mi == 6)
            ptr = s_disp_text6;

        memzero(ptr, MAX_TEXT_BUF);
        if(strlen(mnemonic_part) > MAX_TEXT_LEN) {
            strncpy(ptr, mnemonic_part, MAX_TEXT_LEN);
        } else {
            strncpy(ptr, mnemonic_part, strlen(mnemonic_part));
        }

        mnemonic_part = flipbip_strtok(NULL, ",");
    }

    // Free the working mnemonic memory
    memzero(mnemonic_working, mnemonic_working_len);
    free(mnemonic_working);
}

static void flipbip_scene_1_draw_seed(FlipBipScene1Model* const model) {
    const size_t seed_working_len = 64 * 2 + 1;
    char* seed_working = malloc(seed_working_len);
    // Convert the seed to a hex string
    flipbip_btox(model->seed, 64, seed_working);

    flipbip_scene_1_draw_generic(seed_working, 22, false);

    // Free the working seed memory
    memzero(seed_working, seed_working_len);
    free(seed_working);
}

static void flipbip_scene_1_clear_text() {
    memzero((void*)s_disp_text1, MAX_TEXT_BUF);
    memzero((void*)s_disp_text2, MAX_TEXT_BUF);
    memzero((void*)s_disp_text3, MAX_TEXT_BUF);
    memzero((void*)s_disp_text4, MAX_TEXT_BUF);
    memzero((void*)s_disp_text5, MAX_TEXT_BUF);
    memzero((void*)s_disp_text6, MAX_TEXT_BUF);
}

void flipbip_scene_1_draw(Canvas* canvas, FlipBipScene1Model* model) {
    //UNUSED(model);
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    flipbip_scene_1_clear_text();
    if(model->page == PAGE_INFO) {
        flipbip_scene_1_draw_generic(TEXT_INFO, 27, false);
    } else if(model->page == PAGE_MNEMONIC) {
        flipbip_scene_1_draw_mnemonic(model->mnemonic);
    } else if(model->page == PAGE_SEED) {
        flipbip_scene_1_draw_seed(model);
    } else if(model->page == PAGE_XPRV_ROOT) {
        flipbip_scene_1_draw_generic(model->xprv_root, 20, false);
    } else if(model->page == PAGE_XPRV_ACCT) {
        flipbip_scene_1_draw_generic(model->xprv_account, 20, false);
    } else if(model->page == PAGE_XPUB_ACCT) {
        flipbip_scene_1_draw_generic(model->xpub_account, 20, false);
    } else if(model->page == PAGE_XPRV_EXTD) {
        flipbip_scene_1_draw_generic(model->xprv_extended, 20, false);
    } else if(model->page == PAGE_XPUB_EXTD) {
        flipbip_scene_1_draw_generic(model->xpub_extended, 20, false);
    } else if(model->page >= PAGE_ADDR_BEGIN && model->page <= PAGE_ADDR_END) {
        size_t line_len = 12;
        if(model->coin == FlipBipCoinETH60) {
            line_len = 14;
        }
        flipbip_scene_1_draw_generic(
            model->recv_addresses[model->page - PAGE_ADDR_BEGIN], line_len, true);
    }

    if(model->page == PAGE_LOADING) {
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 2, 10, TEXT_LOADING);
        canvas_draw_str(canvas, 7, 30, s_derivation_text);
        canvas_draw_icon(canvas, 86, 22, &I_Keychain_39x36);
        if(s_warn_insecure) {
            canvas_set_font(canvas, FontSecondary);
            canvas_draw_str(canvas, 2, 50, WARN_INSECURE_TEXT_1);
            canvas_draw_str(canvas, 2, 60, WARN_INSECURE_TEXT_2);
        }
    } else if(model->page >= PAGE_ADDR_BEGIN && model->page <= PAGE_ADDR_END) {
        // draw address header
        canvas_set_font(canvas, FontSecondary);
        // coin_name, derivation_path
        const char* receive_text = COIN_TEXT_ARRAY[model->coin][0];
        if(receive_text == NULL) {
            receive_text = TEXT_DEFAULT_COIN;
        }
        const size_t receive_len = strlen(receive_text) * 7;
        canvas_draw_str_aligned(canvas, 2, 2, AlignLeft, AlignTop, receive_text);
        canvas_draw_str_aligned(
            canvas, receive_len + 1, 2, AlignLeft, AlignTop, TEXT_RECEIVE_ADDRESS);

        // draw address number
        const unsigned char addr_num[1] = {(unsigned char)(model->page - PAGE_ADDR_BEGIN)};
        char addr_num_text[3] = {0};
        flipbip_btox(addr_num, 1, addr_num_text);
        addr_num_text[0] = '/';
        canvas_draw_str_aligned(canvas, 125, 2, AlignRight, AlignTop, addr_num_text);

        // draw QR code file path
        char addr_name_text[14] = {0};
        strcpy(addr_name_text, COIN_TEXT_ARRAY[model->coin][0]);
        flipbip_btox(addr_num, 1, addr_name_text + strlen(addr_name_text));
        strcpy(addr_name_text + strlen(addr_name_text), TEXT_QRFILE_EXT);
        //elements_button_right(canvas, addr_name_text);
        canvas_draw_str_aligned(canvas, 125, 53, AlignRight, AlignTop, addr_name_text);

        // draw address
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 7, 22, s_disp_text1);
        canvas_draw_str(canvas, 7, 34, s_disp_text2);
        canvas_draw_str(canvas, 7, 46, s_disp_text3);
        canvas_draw_str(canvas, 7, 58, s_disp_text4);
    } else {
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 1, 2, AlignLeft, AlignTop, s_disp_text1);
        canvas_draw_str_aligned(canvas, 1, 12, AlignLeft, AlignTop, s_disp_text2);
        canvas_draw_str_aligned(canvas, 1, 22, AlignLeft, AlignTop, s_disp_text3);
        canvas_draw_str_aligned(canvas, 1, 32, AlignLeft, AlignTop, s_disp_text4);
        canvas_draw_str_aligned(canvas, 1, 42, AlignLeft, AlignTop, s_disp_text5);
        canvas_draw_str_aligned(canvas, 1, 52, AlignLeft, AlignTop, s_disp_text6);
    }
}

static int flipbip_scene_1_model_init(
    FlipBipScene1Model* const model,
    const int strength,
    const uint32_t coin,
    const bool overwrite,
    const char* passphrase_text) {
    model->page = PAGE_LOADING;
    model->mnemonic_only = false;
    model->strength = strength;
    model->coin = coin;
    model->overwrite = overwrite;

    // Allocate memory for mnemonic
    char* mnemonic = malloc(TEXT_BUFFER_SIZE);
    memzero(mnemonic, TEXT_BUFFER_SIZE);

    // Check if the mnemonic key & data is already saved in persistent storage, or overwrite is true
    if(overwrite || (!flipbip_has_file(FlipBipFileKey, NULL, false) &&
                     !flipbip_has_file(FlipBipFileDat, NULL, false))) {
        // Set mnemonic only mode
        model->mnemonic_only = true;
        // Generate a random mnemonic using trezor-crypto
        const char* mnemonic_gen = mnemonic_generate(strength);
        // Check if the mnemonic is valid
        if(mnemonic_check(mnemonic_gen) == 0)
            return FlipBipStatusMnemonicCheckError; // 13 = mnemonic check error
        // Save the mnemonic to persistent storage
        else if(!flipbip_save_file_secure(mnemonic_gen))
            return FlipBipStatusSaveError; // 12 = save error
        // Clear the generated mnemonic from memory
        mnemonic_clear();
    }

    // Load the mnemonic from persistent storage
    if(!flipbip_load_file_secure(mnemonic)) {
        // Set mnemonic only mode for this error for memory cleanup purposes
        model->mnemonic_only = true;
        return FlipBipStatusLoadError; // 11 = load error
    }
    model->mnemonic = mnemonic;
    // Check if the mnemonic is valid
    if(mnemonic_check(model->mnemonic) == 0) {
        // Set mnemonic only mode for this error for memory cleanup purposes
        model->mnemonic_only = true;
        return FlipBipStatusMnemonicCheckError; // 13 = mnemonic check error
    }

    // test return values
    //model->mnemonic_only = true;
    //return FlipBipStatusMnemonicCheckError; // 13 = mnemonic check error

    // if we are only generating the mnemonic, return
    if(model->mnemonic_only) {
        return FlipBipStatusReturn; // 10 = mnemonic only, return from parent
    }

    // Generate a BIP39 seed from the mnemonic
    mnemonic_to_seed(model->mnemonic, passphrase_text, model->seed, 0);

    // Generate a BIP32 root HD node from the mnemonic
    HDNode* root = malloc(sizeof(HDNode));
    hdnode_from_seed(model->seed, 64, SECP256K1_NAME, root);

    // buffer for key serialization
    const size_t buflen = 128;
    char buf[128 + 1] = {0};

    // coin info
    // bip44_coin, xprv_version, xpub_version, addr_version, wif_version, addr_format
    uint32_t coin_info[6] = {0};
    for(size_t i = 0; i < 6; i++) {
        coin_info[i] = COIN_INFO_ARRAY[coin][i];
    }

    // root
    uint32_t fingerprint = 0;
    hdnode_serialize_private(root, fingerprint, coin_info[1], buf, buflen);
    char* xprv_root = malloc(buflen + 1);
    strncpy(xprv_root, buf, buflen);
    model->xprv_root = xprv_root;

    HDNode* node = root;

    // purpose m/44'
    fingerprint = hdnode_fingerprint(node);
    hdnode_private_ckd_prime(node, DERIV_PURPOSE); // purpose

    // coin m/44'/0' or m/44'/60'
    fingerprint = hdnode_fingerprint(node);
    hdnode_private_ckd_prime(node, coin_info[0]); // coin

    // account m/44'/0'/0' or m/44'/60'/0'
    fingerprint = hdnode_fingerprint(node);
    hdnode_private_ckd_prime(node, DERIV_ACCOUNT); // account

    hdnode_serialize_private(node, fingerprint, coin_info[1], buf, buflen);
    char* xprv_acc = malloc(buflen + 1);
    strncpy(xprv_acc, buf, buflen);
    model->xprv_account = xprv_acc;

    hdnode_serialize_public(node, fingerprint, coin_info[2], buf, buflen);
    char* xpub_acc = malloc(buflen + 1);
    strncpy(xpub_acc, buf, buflen);
    model->xpub_account = xpub_acc;

    // external/internal (change) m/44'/0'/0'/0 or m/44'/60'/0'/0
    fingerprint = hdnode_fingerprint(node);
    hdnode_private_ckd(node, DERIV_CHANGE); // external/internal (change)

    hdnode_serialize_private(node, fingerprint, coin_info[1], buf, buflen);
    char* xprv_ext = malloc(buflen + 1);
    strncpy(xprv_ext, buf, buflen);
    model->xprv_extended = xprv_ext;

    hdnode_serialize_public(node, fingerprint, coin_info[2], buf, buflen);
    char* xpub_ext = malloc(buflen + 1);
    strncpy(xpub_ext, buf, buflen);
    model->xpub_extended = xpub_ext;

    model->node = node;

    // Initialize addresses
    for(uint8_t a = 0; a < NUM_ADDRS; a++) {
        model->recv_addresses[a] = malloc(MAX_ADDR_BUF);
        memzero(model->recv_addresses[a], MAX_ADDR_BUF);
        flipbip_scene_1_init_address(model->recv_addresses[a], node, coin, a);

        // Save QR code file
        memzero(buf, buflen);
        strcpy(buf, COIN_TEXT_ARRAY[coin][0]);
        const unsigned char addr_num[1] = {a};
        flipbip_btox(addr_num, 1, buf + strlen(buf));
        strcpy(buf + strlen(buf), TEXT_QRFILE_EXT);
        flipbip_save_qrfile(COIN_TEXT_ARRAY[coin][2], model->recv_addresses[a], buf);
        memzero(buf, buflen);
    }

    model->page = PAGE_INFO;

#if USE_BIP39_CACHE
    // Clear the BIP39 cache
    bip39_cache_clear();
#endif

    // 0 = success
    return FlipBipStatusSuccess;
}

bool flipbip_scene_1_input(InputEvent* event, void* context) {
    furi_assert(context);
    FlipBipScene1* instance = context;

    // Ignore input if busy
    // if(s_busy) {
    //     return false;
    // }

    if(event->type == InputTypeRelease) {
        switch(event->key) {
        case InputKeyBack:
            with_view_model(
                instance->view,
                FlipBipScene1Model * model,
                {
                    UNUSED(model);
                    instance->callback(FlipBipCustomEventScene1Back, instance->context);
                },
                true);
            break;
        case InputKeyRight:
        case InputKeyDown:
            with_view_model(
                instance->view,
                FlipBipScene1Model * model,
                {
                    //UNUSED(model);
                    int page = (model->page + 1) % (PAGE_ADDR_END + 1);
                    if(page == 0) {
                        page = PAGE_INFO;
                    }
                    model->page = page;
                },
                true);
            break;
        case InputKeyLeft:
        case InputKeyUp:
            with_view_model(
                instance->view,
                FlipBipScene1Model * model,
                {
                    //UNUSED(model);
                    int page = (model->page - 1) % (PAGE_ADDR_END + 1);
                    if(page == 0) {
                        page = PAGE_ADDR_END;
                    }
                    model->page = page;
                },
                true);
            break;
        // case InputKeyRight:
        case InputKeyOk:
            // with_view_model(
            //     instance->view,
            //     FlipBipScene1Model * model,
            //     {
            //         if(model->page >= PAGE_ADDR_BEGIN && model->page <= PAGE_ADDR_END) {

            //         }
            //     },
            //     true);
            // break;
        // case InputKeyLeft:
        case InputKeyMAX:
            break;
        }
    }
    return true;
}

void flipbip_scene_1_exit(void* context) {
    furi_assert(context);
    FlipBipScene1* instance = (FlipBipScene1*)context;

    with_view_model(
        instance->view,
        FlipBipScene1Model * model,
        {
            model->page = PAGE_LOADING;
            model->strength = FlipBipStrength256;
            model->coin = FlipBipCoinBTC0;
            memzero(model->seed, 64);
            // if mnemonic_only is true, then we don't need to free the data here
            if(!model->mnemonic_only) {
                memzero((void*)model->mnemonic, strlen(model->mnemonic));
                free((void*)model->mnemonic);
                memzero((void*)model->node, sizeof(HDNode));
                free((void*)model->node);
                memzero((void*)model->xprv_root, strlen(model->xprv_root));
                memzero((void*)model->xprv_account, strlen(model->xprv_account));
                memzero((void*)model->xpub_account, strlen(model->xpub_account));
                memzero((void*)model->xprv_extended, strlen(model->xprv_extended));
                memzero((void*)model->xpub_extended, strlen(model->xpub_extended));
                free((void*)model->xprv_root);
                free((void*)model->xprv_account);
                free((void*)model->xpub_account);
                free((void*)model->xprv_extended);
                free((void*)model->xpub_extended);
                for(int a = 0; a < NUM_ADDRS; a++) {
                    memzero((void*)model->recv_addresses[a], MAX_ADDR_BUF);
                    free((void*)model->recv_addresses[a]);
                }
            }
        },
        true);

    flipbip_scene_1_clear_text();
}

void flipbip_scene_1_enter(void* context) {
    furi_assert(context);
    FlipBipScene1* instance = (FlipBipScene1*)context;

    FlipBip* app = instance->context;

    // BIP39 Strength setting
    int strength = 256; // FlipBipStrength256 // 24 words (256 bit)
    if(app->bip39_strength == FlipBipStrength128) {
        strength = 128; // 12 words (128 bit)
    } else if(app->bip39_strength == FlipBipStrength192) {
        strength = 192; // 18 words (192 bit)
    }

    // BIP39 Passphrase setting
    const char* passphrase_text = "";
    if(app->passphrase == FlipBipPassphraseOn && strlen(app->passphrase_text) > 0) {
        passphrase_text = app->passphrase_text;
        s_warn_insecure = false;
    } else {
        s_warn_insecure = true;
    }

    // BIP44 Coin setting
    const uint32_t coin = app->bip44_coin;
    // coin_name, derivation_path
    s_derivation_text = COIN_TEXT_ARRAY[coin][1];

    // Overwrite the saved seed with a new one setting
    bool overwrite = app->overwrite_saved_seed != 0;
    if(overwrite) {
        s_derivation_text = TEXT_NEW_WALLET;
    }

    flipbip_play_happy_bump(app);
    //notification_message(app->notification, &sequence_blink_cyan_100);
    flipbip_led_set_rgb(app, 255, 0, 0);

    with_view_model(
        instance->view,
        FlipBipScene1Model * model,
        {
            // s_busy = true;

            const int status =
                flipbip_scene_1_model_init(model, strength, coin, overwrite, passphrase_text);

            // nonzero status, free the mnemonic
            if(status != FlipBipStatusSuccess) {
                memzero((void*)model->mnemonic, strlen(model->mnemonic));
                free((void*)model->mnemonic);
            }

            // if error, set the error message
            if(status == FlipBipStatusSaveError) {
                model->mnemonic = "ERROR:,Save error";
                model->page = PAGE_MNEMONIC;
                flipbip_play_long_bump(app);
            } else if(status == FlipBipStatusLoadError) {
                model->mnemonic = "ERROR:,Load error";
                model->page = PAGE_MNEMONIC;
                flipbip_play_long_bump(app);
            } else if(status == FlipBipStatusMnemonicCheckError) {
                model->mnemonic = "ERROR:,Mnemonic check error";
                model->page = PAGE_MNEMONIC;
                flipbip_play_long_bump(app);
            }

            // s_busy = false;

            // if overwrite is set and mnemonic generated, return from scene immediately
            if(status == FlipBipStatusReturn) {
                instance->callback(FlipBipCustomEventScene1Back, instance->context);
            }
        },
        true);
}

FlipBipScene1* flipbip_scene_1_alloc() {
    FlipBipScene1* instance = malloc(sizeof(FlipBipScene1));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(FlipBipScene1Model));
    view_set_context(instance->view, instance); // furi_assert crashes in events without this
    view_set_draw_callback(instance->view, (ViewDrawCallback)flipbip_scene_1_draw);
    view_set_input_callback(instance->view, flipbip_scene_1_input);
    view_set_enter_callback(instance->view, flipbip_scene_1_enter);
    view_set_exit_callback(instance->view, flipbip_scene_1_exit);

    // allocate the address node
    s_addr_node = (HDNode*)malloc(sizeof(HDNode));

    // allocate the display text
    s_disp_text1 = (char*)malloc(MAX_TEXT_BUF);
    s_disp_text2 = (char*)malloc(MAX_TEXT_BUF);
    s_disp_text3 = (char*)malloc(MAX_TEXT_BUF);
    s_disp_text4 = (char*)malloc(MAX_TEXT_BUF);
    s_disp_text5 = (char*)malloc(MAX_TEXT_BUF);
    s_disp_text6 = (char*)malloc(MAX_TEXT_BUF);

    return instance;
}

void flipbip_scene_1_free(FlipBipScene1* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view, FlipBipScene1Model * model, { UNUSED(model); }, true);

    // free the address node
    memzero(s_addr_node, sizeof(HDNode));
    free(s_addr_node);

    // free the display text
    flipbip_scene_1_clear_text();
    free(s_disp_text1);
    free(s_disp_text2);
    free(s_disp_text3);
    free(s_disp_text4);
    free(s_disp_text5);
    free(s_disp_text6);

    view_free(instance->view);
    free(instance);
}

View* flipbip_scene_1_get_view(FlipBipScene1* instance) {
    furi_assert(instance);
    return instance->view;
}
