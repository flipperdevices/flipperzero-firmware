#include "../flipbip39.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include <dolphin/dolphin.h>
#include "../helpers/flipbip39_haptic.h"
#include "../helpers/flipbip39_speaker.h"
#include "../helpers/flipbip39_led.h"

#include <string.h>
// #include "../crypto/bip32.h"
#include "../crypto/bip39.h"
// #include "../crypto/ecdsa.h"
// #include "../crypto/curves.h"
#include "../crypto/memzero.h"

struct FlipBip39Scene1 {
    View* view;
    FlipBip39Scene1Callback callback;
    void* context;
};


typedef struct {
    int strength;
    const char* mnemonic1;
    const char* mnemonic2;
    const char* mnemonic3;
    const char* mnemonic4;
    const char* mnemonic5;
    const char* mnemonic6;
} FlipBip39Scene1Model;

void flipbip39_scene_1_set_callback(
    FlipBip39Scene1* instance,
    FlipBip39Scene1Callback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void flipbip39_scene_1_draw(Canvas* canvas, FlipBip39Scene1Model* model) {
    //UNUSED(model);
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    //canvas_set_font(canvas, FontPrimary);
    //canvas_draw_str_aligned(canvas, 0, 10, AlignLeft, AlignTop, "This is Scene 1"); 
    
    canvas_set_font(canvas, FontSecondary);
    //canvas_draw_str_aligned(canvas, 1, 2, AlignLeft, AlignTop, model->strength == 128 ? "128-bit" : "256-bit");
    canvas_draw_str_aligned(canvas, 1, 2, AlignLeft, AlignTop, model->mnemonic1);
    canvas_draw_str_aligned(canvas, 1, 12, AlignLeft, AlignTop, model->mnemonic2);
    canvas_draw_str_aligned(canvas, 1, 22, AlignLeft, AlignTop, model->mnemonic3);
    canvas_draw_str_aligned(canvas, 1, 32, AlignLeft, AlignTop, model->mnemonic4);
    canvas_draw_str_aligned(canvas, 1, 42, AlignLeft, AlignTop, model->mnemonic5);
    canvas_draw_str_aligned(canvas, 1, 52, AlignLeft, AlignTop, model->mnemonic6);

}

static void flipbip39_scene_1_model_init(FlipBip39Scene1Model* const model, const int strength) {
    // Generate a random mnemonic using trezor-crypto
    model->strength = strength;
    const char* mnemonic = mnemonic_generate(strength);

    // Delineate 6 sections of the mnemonic
    char *str = malloc(strlen(mnemonic) + 1);
    strcpy(str, mnemonic);
    int word = 0;
    for (size_t i = 0; i < strlen(str); i++) {
        if (str[i] == ' ') {
            word++;
            if (word % 4 == 0) {
                str[i] = ',';
            }
        } 
    }

    // Split the mnemonic into 6 parts
    char *ptr = strtok (str, ",");
    int partnum = 0;
    while(ptr != NULL)
    {
        char *part = malloc(strlen(ptr) + 1);
        strcpy(part, ptr);
        partnum++;
        
        if (partnum == 1) model->mnemonic1 = part;
        if (partnum == 2) model->mnemonic2 = part;
        if (partnum == 3) model->mnemonic3 = part;
        if (partnum == 4) model->mnemonic4 = part;
        if (partnum == 5) model->mnemonic5 = part;
        if (partnum == 6) model->mnemonic6 = part;

        ptr = strtok(NULL, ",");
    }

    // WIP / TODO: Generate a BIP32 root key from the mnemonic

    // //bool root_set = false;
    // HDNode root;
    // uint8_t seed[64];
    // mnemonic_to_seed(mnemonic, "", seed, 0);
    // hdnode_from_seed(seed, 64, SECP256K1_NAME, &root);
    // //root_set = true;

    // int arg1 = 1;
    // // constants for Bitcoin
    // const uint32_t version_public = 0x0488b21e;
    // const uint32_t version_private = 0x0488ade4;
    // const char addr_version = 0x00, wif_version = 0x80;
    // const size_t buflen = 128;
    // char buf[buflen + 1];
    // HDNode node;
    // uint32_t fingerprint;
    // // external chain
    // for (int chain = 0; chain < 2; chain++) {
    //     //QTableWidget *list = chain == 0 ? ui->listAddress : ui->listChange;
    //     node = root;
    //     hdnode_private_ckd(&node, 44 | 0x80000000);
    //     hdnode_private_ckd(&node, 0 | 0x80000000); // bitcoin
    //     hdnode_private_ckd(&node, (arg1 - 1) | 0x80000000);
    //     fingerprint = hdnode_fingerprint(&node);
    //     hdnode_serialize_private(&node, fingerprint, version_private, buf, buflen); 
    //     //QString xprv = QString(buf); ui->lineXprv->setText(xprv);
    //     hdnode_serialize_public(&node, fingerprint, version_public, buf, buflen); 
    //     //QString xpub = QString(buf); ui->lineXpub->setText(xpub);
    //     hdnode_private_ckd(&node, chain); // external / internal
    //     for (int i = 0; i < 10; i++) {
    //         HDNode node2 = node;
    //         hdnode_private_ckd(&node2, i);
    //         hdnode_fill_public_key(&node2);
    //         ecdsa_get_address(node2.public_key, addr_version, HASHER_SHA2_RIPEMD, HASHER_SHA2D, buf, buflen); 
    //         //QString address = QString(buf);
    //         ecdsa_get_wif(node2.private_key, wif_version, HASHER_SHA2D, buf, buflen); 
    //         //QString wif = QString(buf);
    //         // list->setItem(i, 0, new QTableWidgetItem(address));
    //         // list->setItem(i, 1, new QTableWidgetItem(wif));
    //         // list->setItem(i, 2, new QTableWidgetItem("0.0"));
    //     }
    // }

    // Clear the mnemonic
    mnemonic_clear();
    bip39_cache_clear();
}

bool flipbip39_scene_1_input(InputEvent* event, void* context) {
    furi_assert(context); 
    FlipBip39Scene1* instance = context;
    if (event->type == InputTypeRelease) {
        switch(event->key) {
            case InputKeyBack:
                with_view_model(
                    instance->view,
                    FlipBip39Scene1Model * model,
                    {
                        UNUSED(model);
                        instance->callback(FlipBip39CustomEventScene1Back, instance->context);
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
                    FlipBip39Scene1Model* model,
                    {
                        UNUSED(model);
                    },
                    true);
                break;
            case InputKeyMAX:
                break;
        }
    }
    return true;
}

void flipbip39_scene_1_exit(void* context) {
    furi_assert(context);
    FlipBip39Scene1* instance = (FlipBip39Scene1*)context;

    with_view_model(
        instance->view,
        FlipBip39Scene1Model * model,
        {
            // Clear the mnemonic from memory
            model->strength = 0;
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

void flipbip39_scene_1_enter(void* context) {
    furi_assert(context);
    FlipBip39Scene1* instance = (FlipBip39Scene1*)context;

    FlipBip39* app = instance->context;
    int strength_setting = app->bip39_strength;
    int strength = 256;
    if (strength_setting == 0) strength = 128;
    else if (strength_setting == 1) strength = 192;

    flipbip39_play_happy_bump(app);
    flipbip39_led_set_rgb(app, 255, 0, 0);

    with_view_model(
        instance->view,
        FlipBip39Scene1Model * model,
        {
            flipbip39_scene_1_model_init(model, strength);
        },
        true
    );
}

FlipBip39Scene1* flipbip39_scene_1_alloc() {
    FlipBip39Scene1* instance = malloc(sizeof(FlipBip39Scene1));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(FlipBip39Scene1Model));
    view_set_context(instance->view, instance); // furi_assert crashes in events without this
    view_set_draw_callback(instance->view, (ViewDrawCallback)flipbip39_scene_1_draw);
    view_set_input_callback(instance->view, flipbip39_scene_1_input);
    view_set_enter_callback(instance->view, flipbip39_scene_1_enter);
    view_set_exit_callback(instance->view, flipbip39_scene_1_exit);

    // FlipBip39* app = instance->context;
    // int strength_setting = app->bip39_strength;
    // int strength = 256;
    // if (strength_setting == 0) strength = 128;
    // else if (strength_setting == 1) strength = 192;

    // with_view_model(
    //     instance->view,
    //     FlipBip39Scene1Model * model,
    //     {
    //         flipbip39_scene_1_model_init(model, strength);
    //     },
    //     true
    // );
    
    return instance;
}

void flipbip39_scene_1_free(FlipBip39Scene1* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        FlipBip39Scene1Model * model,
        {
            //UNUSED(model);
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

View* flipbip39_scene_1_get_view(FlipBip39Scene1* instance) {
    furi_assert(instance);
    return instance->view;
}

