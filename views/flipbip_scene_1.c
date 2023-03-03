#include "../flipbip.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include <dolphin/dolphin.h>
#include "../helpers/flipbip_haptic.h"
//#include "../helpers/flipbip_speaker.h"
#include "../helpers/flipbip_led.h"
#include "../helpers/flipbip_string.h"

#include <string.h>
// #include "../helpers/printf.h"
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

    // for crypto
    CONFIDENTIAL const char* mnemonic;
    CONFIDENTIAL uint8_t seed[64];
    CONFIDENTIAL const HDNode* node;
    CONFIDENTIAL const char* xprv_root;
    CONFIDENTIAL const char* xprv_account;
    CONFIDENTIAL const char* xpub_account;
    CONFIDENTIAL const char* xprv_extended;
    CONFIDENTIAL const char* xpub_extended;

    // for display
    CONFIDENTIAL const char* seed1;
    CONFIDENTIAL const char* seed2;
    CONFIDENTIAL const char* seed3;
    CONFIDENTIAL const char* seed4;
    CONFIDENTIAL const char* seed5;
    CONFIDENTIAL const char* seed6;
    CONFIDENTIAL const char* mnemonic1;
    CONFIDENTIAL const char* mnemonic2;
    CONFIDENTIAL const char* mnemonic3;
    CONFIDENTIAL const char* mnemonic4;
    CONFIDENTIAL const char* mnemonic5;
    CONFIDENTIAL const char* mnemonic6;
} FlipBipScene1Model;

void flipbip_scene_1_set_callback(
    FlipBipScene1* instance,
    FlipBipScene1Callback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void flipbip_scene_1_draw(Canvas* canvas, FlipBipScene1Model* model) {
    //UNUSED(model);
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    //canvas_set_font(canvas, FontPrimary);
    //canvas_draw_str_aligned(canvas, 0, 10, AlignLeft, AlignTop, "This is Scene 1"); 
    
    canvas_set_font(canvas, FontSecondary);
    //canvas_draw_str_aligned(canvas, 1, 2, AlignLeft, AlignTop, model->strength == 128 ? "128-bit" : "256-bit");
    //canvas_draw_str_aligned(canvas, 1, 2, AlignLeft, AlignTop, model->seed);
    
    // Mnenomic
    if (model->page == 0) {
        canvas_draw_str_aligned(canvas, 1, 2, AlignLeft, AlignTop, model->mnemonic1);
        canvas_draw_str_aligned(canvas, 1, 12, AlignLeft, AlignTop, model->mnemonic2);
        canvas_draw_str_aligned(canvas, 1, 22, AlignLeft, AlignTop, model->mnemonic3);
        canvas_draw_str_aligned(canvas, 1, 32, AlignLeft, AlignTop, model->mnemonic4);
        canvas_draw_str_aligned(canvas, 1, 42, AlignLeft, AlignTop, model->mnemonic5);
        canvas_draw_str_aligned(canvas, 1, 52, AlignLeft, AlignTop, model->mnemonic6);
    }
    // Seed
    else if (model->page == 1) {
        canvas_draw_str_aligned(canvas, 1, 2, AlignLeft, AlignTop, model->seed1);
        canvas_draw_str_aligned(canvas, 1, 12, AlignLeft, AlignTop, model->seed2);
        canvas_draw_str_aligned(canvas, 1, 22, AlignLeft, AlignTop, model->seed3);
        canvas_draw_str_aligned(canvas, 1, 32, AlignLeft, AlignTop, model->seed4);
        canvas_draw_str_aligned(canvas, 1, 42, AlignLeft, AlignTop, model->seed5);
        canvas_draw_str_aligned(canvas, 1, 52, AlignLeft, AlignTop, model->seed6);
    }

    
}

static void flipbip_scene_1_model_init(FlipBipScene1Model* const model, const int strength) {
    
    model->page = 0;

    // Generate a random mnemonic using trezor-crypto
    model->strength = strength;
    //const char *mnemonic = mnemonic_generate(model->strength);
    model->mnemonic = "wealth budget salt video delay obey neutral tail sure soda hold rubber joy movie boat raccoon tornado noise off inmate payment patch group topple";

    // START DRAW MNEMONIC

    // Delineate sections of the mnemonic every 4 words
    char *mnemonic_working = malloc(strlen(model->mnemonic) + 1);
    strcpy(mnemonic_working, model->mnemonic);
    int word = 0;
    for (size_t i = 0; i < strlen(mnemonic_working); i++) {
        if (mnemonic_working[i] == ' ') {
            word++;
            if (word % 4 == 0) {
                mnemonic_working[i] = ',';
            }
        } 
    }

    // Split the mnemonic into parts
    char *mnemonic_part = flipbip_strtok(mnemonic_working, ",");
    int mi = 0;
    while(mnemonic_part != NULL)
    {
        char *ptr = malloc(strlen(mnemonic_part) + 1);
        strcpy(ptr, mnemonic_part);
        mi++;
        
        if      (mi == 1) model->mnemonic1 = ptr;
        else if (mi == 2) model->mnemonic2 = ptr;
        else if (mi == 3) model->mnemonic3 = ptr;
        else if (mi == 4) model->mnemonic4 = ptr;
        else if (mi == 5) model->mnemonic5 = ptr;
        else if (mi == 6) model->mnemonic6 = ptr;

        mnemonic_part = flipbip_strtok(NULL, ",");
    }

    // Free the working mnemonic memory
    memzero(mnemonic_working, strlen(mnemonic_working));
    free(mnemonic_working);

    // END DRAW MNEMONIC

    // Generate a BIP39 seed from the mnemonic
    //uint8_t seedbytes[64];
    mnemonic_to_seed(model->mnemonic, "", model->seed, 0);
    
    // BEGIN DRAW SEED

    char *seed_working = malloc(64 * 2 + 1);
    // Convert the seed to a hex string
    for (size_t i = 0; i < 64; i++) {
        flipbip_itoa(model->seed[i], seed_working + (i * 2), 64 * 2 + 1, 16);
        //sprintf(seed + (i * 2), "%.2x", seedbytes[i]);
    }
    
    // Split the seed into parts
    for (size_t si = 1; si <= 6; si++) {
        char *ptr = malloc(22 + 1);
        strncpy(ptr, seed_working + ((si - 1) * 22), 22);
        
        if      (si == 1) model->seed1 = ptr;
        else if (si == 2) model->seed2 = ptr;
        else if (si == 3) model->seed3 = ptr;
        else if (si == 4) model->seed4 = ptr;
        else if (si == 5) model->seed5 = ptr;
        else if (si == 6) model->seed6 = ptr;
    }

    // Free the working seed memory
    memzero(seed_working, sizeof(seed_working));
    free(seed_working);

    // END DRAW SEED

    // WIP / TODO: Generate a BIP32 root key from the mnemonic

    // //bool root_set = false;
    HDNode *root = malloc(sizeof(HDNode));
    hdnode_from_seed(model->seed, 64, SECP256K1_NAME, root);
    // //root_set = true;

    // m/44'/0'/0'/0
    uint32_t purpose = 44;
    uint32_t coin = 0; // Bitcoin
    uint32_t account = 0;
    uint32_t change = 0;
    
    // constants for Bitcoin
    const uint32_t version_public = 0x0488b21e;
    const uint32_t version_private = 0x0488ade4;
    const char addr_version = 0x00, wif_version = 0x80;
    
    // buffer for key serialization
    const size_t buflen = 128;
    char buf[128 + 1];
    
    // root
    uint32_t fingerprint = 0;
    hdnode_serialize_private(root, fingerprint, version_private, buf, buflen); 
    char *xprv_root = malloc(buflen + 1);
    strncpy(xprv_root, buf, buflen);
    model->xprv_root = xprv_root;
    
    HDNode *node = root;
    
    // purpose m/44'
    fingerprint = hdnode_fingerprint(node);
    hdnode_private_ckd_prime(node, purpose); // purpose
    
    // coin m/44'/0'
    fingerprint = hdnode_fingerprint(node);
    hdnode_private_ckd_prime(node, coin); // coin
    
    // account m/44'/0'/0'
    fingerprint = hdnode_fingerprint(node);
    hdnode_private_ckd_prime(node, account); // account
    
    hdnode_serialize_private(node, fingerprint, version_private, buf, buflen); 
    char *xprv_acc = malloc(buflen + 1);
    strncpy(xprv_acc, buf, buflen);
    model->xprv_account = xprv_acc;
    
    hdnode_serialize_public(node, fingerprint, version_public, buf, buflen); 
    char *xpub_acc = malloc(buflen + 1);
    strncpy(xpub_acc, buf, buflen);
    model->xpub_account = xpub_acc;
    
    // external/internal (change) m/44'/0'/0'/0
    fingerprint = hdnode_fingerprint(node);
    hdnode_private_ckd(node, change); // external/internal (change)
    
    hdnode_serialize_private(node, fingerprint, version_private, buf, buflen); 
    char *xprv_ext = malloc(buflen + 1);
    strncpy(xprv_ext, buf, buflen);
    model->xprv_extended = xprv_ext;
    
    hdnode_serialize_public(node, fingerprint, version_public, buf, buflen); 
    char *xpub_ext = malloc(buflen + 1);
    strncpy(xpub_ext, buf, buflen);
    model->xpub_extended = xpub_ext;

    model->node = node;

    // BEGIN DRAW ADDRESS

    for (int i = 0; i < 3; i++) {
        HDNode *addr_node = malloc(sizeof(HDNode));
        memcpy(addr_node, node, sizeof(HDNode));

        hdnode_private_ckd(addr_node, i);
        hdnode_fill_public_key(addr_node);
        ecdsa_get_address(addr_node->public_key, addr_version, HASHER_SHA2_RIPEMD, HASHER_SHA2D, buf, buflen); 
        char *address = malloc(buflen + 1);
        strncpy(address, buf, buflen);
        model->seed5 = address;
        ecdsa_get_wif(addr_node->private_key, wif_version, HASHER_SHA2D, buf, buflen); 
        char *wif = malloc(buflen + 1);
        strncpy(wif, buf, buflen);
        model->seed6 = wif;
        memzero(addr_node, sizeof(HDNode));
        free(addr_node);
    }

    // END DRAW ADDRESS

    // Clear the root node
    // memzero(root, sizeof(HDNode));
    // free(root);

    // Clear the mnemonic
    //mnemonic_clear();

#if USE_BIP39_CACHE
    // Clear the BIP39 cache
    bip39_cache_clear();
#endif
}

bool flipbip_scene_1_input(InputEvent* event, void* context) {
    furi_assert(context); 
    FlipBipScene1* instance = context;
    if (event->type == InputTypeRelease) {
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
            case InputKeyLeft:
            case InputKeyRight:
            case InputKeyUp:
            case InputKeyDown:
            case InputKeyOk:
                with_view_model(
                    instance->view,
                    FlipBipScene1Model* model,
                    {
                        //UNUSED(model);
                        model->page = (model->page + 1) % 2;
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
            // Clear the mnemonic from memory
            model->page = 0;
            model->strength = 0;
            memzero((void*)model->seed1, strlen(model->seed1));
            memzero((void*)model->seed2, strlen(model->seed2));
            memzero((void*)model->seed3, strlen(model->seed3));
            memzero((void*)model->seed4, strlen(model->seed4));
            memzero((void*)model->seed5, strlen(model->seed5));
            memzero((void*)model->seed6, strlen(model->seed6));
            memzero((void*)model->mnemonic1, strlen(model->mnemonic1));
            memzero((void*)model->mnemonic2, strlen(model->mnemonic2));
            memzero((void*)model->mnemonic3, strlen(model->mnemonic3));
            memzero((void*)model->mnemonic4, strlen(model->mnemonic4));
            memzero((void*)model->mnemonic5, strlen(model->mnemonic5));
            memzero((void*)model->mnemonic6, strlen(model->mnemonic6));
        },
        true
    );
}

void flipbip_scene_1_enter(void* context) {
    furi_assert(context);
    FlipBipScene1* instance = (FlipBipScene1*)context;

    FlipBip* app = instance->context;
    int strength_setting = app->bip39_strength;
    int strength = 256;
    if (strength_setting == 0) strength = 128;
    else if (strength_setting == 1) strength = 192;

    flipbip_play_happy_bump(app);
    flipbip_led_set_rgb(app, 255, 0, 0);

    with_view_model(
        instance->view,
        FlipBipScene1Model * model,
        {
            flipbip_scene_1_model_init(model, strength);
        },
        true
    );
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

    // FlipBip* app = instance->context;
    // int strength_setting = app->bip39_strength;
    // int strength = 256;
    // if (strength_setting == 0) strength = 128;
    // else if (strength_setting == 1) strength = 192;

    // with_view_model(
    //     instance->view,
    //     FlipBipScene1Model * model,
    //     {
    //         flipbip_scene_1_model_init(model, strength);
    //     },
    //     true
    // );
    
    return instance;
}

void flipbip_scene_1_free(FlipBipScene1* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        FlipBipScene1Model * model,
        {
            //UNUSED(model);
            free((void*)model->seed1);
            free((void*)model->seed2);
            free((void*)model->seed3);
            free((void*)model->seed4);
            free((void*)model->seed5);
            free((void*)model->seed6);
            free((void*)model->mnemonic1);
            free((void*)model->mnemonic2);
            free((void*)model->mnemonic3);
            free((void*)model->mnemonic4);
            free((void*)model->mnemonic5);
            free((void*)model->mnemonic6);
        },
        true);
    view_free(instance->view);
    free(instance);
}

View* flipbip_scene_1_get_view(FlipBipScene1* instance) {
    furi_assert(instance);
    return instance->view;
}

