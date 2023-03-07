#include "../flipbip.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
//#include <dolphin/dolphin.h>
#include <storage/storage.h>
#include "FlipBIP_icons.h"
#include "../helpers/flipbip_haptic.h"
#include "../helpers/flipbip_led.h"
#include "../helpers/flipbip_string.h"
#include "../helpers/flipbip_file.h"

#include <string.h>
#include "../crypto/rand.h"
#include "../crypto/bip32.h"
#include "../crypto/bip39.h"
#include "../crypto/curves.h"
#include "../crypto/memzero.h"

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
} FlipBipScene1Model;

static CONFIDENTIAL char s_disp_text1[30 + 1];
static CONFIDENTIAL char s_disp_text2[30 + 1];
static CONFIDENTIAL char s_disp_text3[30 + 1];
static CONFIDENTIAL char s_disp_text4[30 + 1];
static CONFIDENTIAL char s_disp_text5[30 + 1];
static CONFIDENTIAL char s_disp_text6[30 + 1];
static bool s_busy = false;

void flipbip_scene_1_set_callback(
    FlipBipScene1* instance,
    FlipBipScene1Callback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

static void flipbip_scene_1_draw_generic(const char* text, size_t line_len) {
    // Split the text into parts
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

        memzero(ptr, 30 + 1);
        if(line_len > 30) {
            strncpy(ptr, text + ((si - 1) * 30), 30);
        } else {
            strncpy(ptr, text + ((si - 1) * line_len), line_len);
        }
    }
}

static void flipbip_scene_1_draw_mnemonic(const char* mnemonic) {
    // Delineate sections of the mnemonic every 4 words
    char* mnemonic_working = malloc(strlen(mnemonic) + 1);
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

        memzero(ptr, 30 + 1);
        if(strlen(mnemonic_part) > 30) {
            strncpy(ptr, mnemonic_part, 30);
        } else {
            strncpy(ptr, mnemonic_part, strlen(mnemonic_part));
        }

        mnemonic_part = flipbip_strtok(NULL, ",");
    }

    // Free the working mnemonic memory
    memzero(mnemonic_working, strlen(mnemonic_working));
    free(mnemonic_working);
}

static void flipbip_scene_1_draw_seed(FlipBipScene1Model* const model) {
    char* seed_working = malloc(64 * 2 + 1);
    // Convert the seed to a hex string
    for(size_t i = 0; i < 64; i++) {
        flipbip_btox(model->seed[i], seed_working + (i * 2));
    }

    flipbip_scene_1_draw_generic(seed_working, 22);

    // Free the working seed memory
    memzero(seed_working, sizeof(seed_working));
    free(seed_working);
}

static void
    flipbip_scene_1_draw_address(const HDNode* node, uint32_t addr_type, uint32_t addr_index) {
    s_busy = true;

    // buffer for key serialization
    const size_t buflen = 128;
    char buf[128 + 1];

    HDNode* addr_node = malloc(sizeof(HDNode));
    memcpy(addr_node, node, sizeof(HDNode));

    hdnode_private_ckd(addr_node, addr_index);
    hdnode_fill_public_key(addr_node);

    if(addr_type == 0) { // BTC
        // BTC style address
        const char addr_version = 0x00;
        //const char wif_version = 0x80;
        ecdsa_get_address(
            addr_node->public_key, addr_version, HASHER_SHA2_RIPEMD, HASHER_SHA2D, buf, buflen);

        char* address = malloc(buflen + 1);
        strncpy(address, buf, buflen);
        flipbip_scene_1_draw_generic(address, 12);
        memzero(address, buflen + 1);
        free(address);

        //ecdsa_get_wif(addr_node->private_key, wif_version, HASHER_SHA2D, buf, buflen);
        //char *wif = malloc(buflen + 1);
        //strncpy(wif, buf, buflen);
    } else if(addr_type == 60) { // ETH
        // ETH style address
        hdnode_get_ethereum_pubkeyhash(addr_node, (uint8_t*)buf);
        char* address = malloc(42 + 1);
        memcpy(address, "0x", 2);
        // Convert the hash to a hex string
        for(size_t i = 0; i < 20; i++) {
            flipbip_btox(buf[i], address + 2 + (i * 2));
        }
        flipbip_scene_1_draw_generic(address, 12);
        memzero(address, 42 + 1);
        free(address);
    }

    memzero(addr_node, sizeof(HDNode));
    free(addr_node);

    s_busy = false;
}

static void flipbip_scene_1_clear_text() {
    memzero((void*)s_disp_text1, 30 + 1);
    memzero((void*)s_disp_text2, 30 + 1);
    memzero((void*)s_disp_text3, 30 + 1);
    memzero((void*)s_disp_text4, 30 + 1);
    memzero((void*)s_disp_text5, 30 + 1);
    memzero((void*)s_disp_text6, 30 + 1);
}

void flipbip_scene_1_draw(Canvas* canvas, FlipBipScene1Model* model) {
    //UNUSED(model);
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    flipbip_scene_1_clear_text();
    if(model->page == 1) {
        const char* info = "-Scroll pages with up/down-"
                           "p1,2)    Mnemonic/Seed     "
                           "p3)       xprv Root Key    "
                           "p4,5)  xprv/xpub Accnt Keys"
                           "p6,7)  xprv/xpub Extnd Keys"
                           "p8+)    Receive Addresses  ";
        flipbip_scene_1_draw_generic(info, 27);
    } else if(model->page == 2) {
        flipbip_scene_1_draw_mnemonic(model->mnemonic);
    } else if(model->page == 3) {
        flipbip_scene_1_draw_seed(model);
    } else if(model->page == 4) {
        flipbip_scene_1_draw_generic(model->xprv_root, 20);
    } else if(model->page == 5) {
        flipbip_scene_1_draw_generic(model->xprv_account, 20);
    } else if(model->page == 6) {
        flipbip_scene_1_draw_generic(model->xpub_account, 20);
    } else if(model->page == 7) {
        flipbip_scene_1_draw_generic(model->xprv_extended, 20);
    } else if(model->page == 8) {
        flipbip_scene_1_draw_generic(model->xpub_extended, 20);
    } else if(model->page >= 9 && model->page <= 13) {
        flipbip_scene_1_draw_address(model->node, model->coin, model->page - 9);
    }

    if(model->page == 0) {
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 1, 10, "Loading...");
        canvas_draw_str(canvas, 6, 30, "m/44'/x'/0'/0");
        canvas_draw_icon(canvas, 86, 25, &I_Keychain_39x36);
    } else if(model->page >= 9 && model->page <= 13) {
        canvas_set_font(canvas, FontSecondary);
        const char* receive_text;
        if(model->coin == 0) { // BTC
            receive_text = "BTC receive address:";
        } else if(model->coin == 60) { // ETH
            receive_text = "ETH receive address:";
        } else {
            receive_text = "Receive address:";
        }
        canvas_draw_str_aligned(canvas, 1, 2, AlignLeft, AlignTop, receive_text);
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 6, 22, s_disp_text1);
        canvas_draw_str(canvas, 6, 34, s_disp_text2);
        canvas_draw_str(canvas, 6, 46, s_disp_text3);
        canvas_draw_str(canvas, 6, 58, s_disp_text4);
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
    const bool overwrite) {
    model->page = 0;
    model->mnemonic_only = false;
    model->strength = strength;
    model->coin = coin;
    model->overwrite = overwrite;

    // Allocate memory for mnemonic
    char* mnemonic = malloc(256);
    memzero(mnemonic, 256);

    // Check if the mnemonic key & data is already saved in persistent storage, or overwrite is true
    if(overwrite || (!flipbip_has_settings(true) && !flipbip_has_settings(false))) {
        // Generate a random mnemonic using trezor-crypto
        const char* mnemonic_gen = mnemonic_generate(strength);
        // Save the mnemonic to persistent storage
        if(!flipbip_save_settings_secure(mnemonic_gen)) return 1; // 1 = save error
        // Clear the generated mnemonic from memory
        mnemonic_clear();
        model->mnemonic_only = true;
    }

    // Load the mnemonic from persistent storage
    if(!flipbip_load_settings_secure(mnemonic)) return 2; // 2 = load error
    model->mnemonic = mnemonic;
    // Check if the mnemonic is valid
    if(mnemonic_check(model->mnemonic) == 0) return 3; // 3 = mnemonic check error

    // if we are only generating the mnemonic, return
    if(model->mnemonic_only) {
        return -1; // -1 = mnemonic only, return from parent
    }

    // test mnemonic
    //model->mnemonic = "wealth budget salt video delay obey neutral tail sure soda hold rubber joy movie boat raccoon tornado noise off inmate payment patch group topple";

    // Generate a BIP39 seed from the mnemonic
    mnemonic_to_seed(model->mnemonic, "", model->seed, 0);

    // Generate a BIP32 root HD node from the mnemonic
    HDNode* root = malloc(sizeof(HDNode));
    hdnode_from_seed(model->seed, 64, SECP256K1_NAME, root);

    // m/44'/0'/0'/0 or m/44'/60'/0'/0
    const uint32_t purpose = 44;
    //const uint32_t coin = 0;  // BTC
    //const uint32_t coin = 60; // ETH
    const uint32_t account = 0;
    const uint32_t change = 0;

    // constants for BTC / ETH
    const uint32_t version_public = 0x0488b21e;
    const uint32_t version_private = 0x0488ade4;
    // "xprv_magic": 76066276,
    // "xpub_magic": 76067358,
    // "xpub_magic_segwit_p2sh": 77429938,
    // "xpub_magic_segwit_native": 78792518,
    // "xpub_magic_multisig_segwit_p2sh": 43365439,
    // "xpub_magic_multisig_segwit_native": 44728019,

    // buffer for key serialization
    const size_t buflen = 128;
    char buf[128 + 1];

    // root
    uint32_t fingerprint = 0;
    hdnode_serialize_private(root, fingerprint, version_private, buf, buflen);
    char* xprv_root = malloc(buflen + 1);
    strncpy(xprv_root, buf, buflen);
    model->xprv_root = xprv_root;

    HDNode* node = root;

    // purpose m/44'
    fingerprint = hdnode_fingerprint(node);
    hdnode_private_ckd_prime(node, purpose); // purpose

    // coin m/44'/0' or m/44'/60'
    fingerprint = hdnode_fingerprint(node);
    hdnode_private_ckd_prime(node, model->coin); // coin

    // account m/44'/0'/0' or m/44'/60'/0'
    fingerprint = hdnode_fingerprint(node);
    hdnode_private_ckd_prime(node, account); // account

    hdnode_serialize_private(node, fingerprint, version_private, buf, buflen);
    char* xprv_acc = malloc(buflen + 1);
    strncpy(xprv_acc, buf, buflen);
    model->xprv_account = xprv_acc;

    hdnode_serialize_public(node, fingerprint, version_public, buf, buflen);
    char* xpub_acc = malloc(buflen + 1);
    strncpy(xpub_acc, buf, buflen);
    model->xpub_account = xpub_acc;

    // external/internal (change) m/44'/0'/0'/0 or m/44'/60'/0'/0
    fingerprint = hdnode_fingerprint(node);
    hdnode_private_ckd(node, change); // external/internal (change)

    hdnode_serialize_private(node, fingerprint, version_private, buf, buflen);
    char* xprv_ext = malloc(buflen + 1);
    strncpy(xprv_ext, buf, buflen);
    model->xprv_extended = xprv_ext;

    hdnode_serialize_public(node, fingerprint, version_public, buf, buflen);
    char* xpub_ext = malloc(buflen + 1);
    strncpy(xpub_ext, buf, buflen);
    model->xpub_extended = xpub_ext;

    model->node = node;

    model->page = 1;

#if USE_BIP39_CACHE
    // Clear the BIP39 cache
    bip39_cache_clear();
#endif

    // 0 = success
    return 0;
}

bool flipbip_scene_1_input(InputEvent* event, void* context) {
    furi_assert(context);
    FlipBipScene1* instance = context;

    // Ignore input if busy
    if(s_busy) {
        return false;
    }

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
        case InputKeyOk:
            with_view_model(
                instance->view,
                FlipBipScene1Model * model,
                {
                    //UNUSED(model);
                    model->page = (model->page + 1) % 14;
                    if(model->page == 0) {
                        model->page = 1;
                    }
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
                    model->page = (model->page - 1) % 14;
                    if(model->page == 0) {
                        model->page = 13;
                    }
                },
                true);
            break;
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
            model->page = 0;
            model->strength = 0;
            model->coin = 0;
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
    int strength_setting = app->bip39_strength;
    int strength = 256; // FlipBipStrength256                        // 24 words (256 bit)
    if(strength_setting == FlipBipStrength128)
        strength = 128; // 12 words (128 bit)
    else if(strength_setting == FlipBipStrength192)
        strength = 192; // 18 words (192 bit)

    // BIP44 Coin setting
    int coin_setting = app->bip44_coin;
    uint32_t coin = 0; //FlipBipCoinBTC0             // BTC (0)
    if(coin_setting == FlipBipCoinETH60) coin = 60; // ETH (60)

    // Overwrite the saved seed with a new one setting
    bool overwrite = app->overwrite_saved_seed != 0;

    flipbip_play_happy_bump(app);
    flipbip_led_set_rgb(app, 255, 0, 0);

    with_view_model(
        instance->view,
        FlipBipScene1Model * model,
        {
            s_busy = true;

            const int status = flipbip_scene_1_model_init(model, strength, coin, overwrite);

            // nonzero status, free the mnemonic
            if(status != 0) {
                memzero((void*)model->mnemonic, strlen(model->mnemonic));
                free((void*)model->mnemonic);
            }

            // if error, set the error message
            if(status == 1) {
                model->mnemonic = "ERROR:,Save error";
                model->page = 2;
            } else if(status == 2) {
                model->mnemonic = "ERROR:,Load error";
                model->page = 2;
            } else if(status == 3) {
                model->mnemonic = "ERROR:,Mnemonic check failed";
                model->page = 2;
            }

            s_busy = false;

            // if overwrite is set and mnemonic generated, return from scene immediately
            if(status == -1) {
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

    return instance;
}

void flipbip_scene_1_free(FlipBipScene1* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view, FlipBipScene1Model * model, { UNUSED(model); }, true);

    flipbip_scene_1_clear_text();

    view_free(instance->view);
    free(instance);
}

View* flipbip_scene_1_get_view(FlipBipScene1* instance) {
    furi_assert(instance);
    return instance->view;
}
