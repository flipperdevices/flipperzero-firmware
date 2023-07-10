#include "../flipchess.h"
#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_random.h>
#include <input/input.h>
#include <gui/elements.h>
//#include <dolphin/dolphin.h>
#include <string.h>
#include "flipchess_icons.h"
#include "../helpers/flipchess_haptic.h"

#define SCL_960_CASTLING 0 // setting to 1 compiles a 960 version of smolchess
#define XBOARD_DEBUG 0 // will create files with xboard communication
#define SCL_EVALUATION_FUNCTION SCL_boardEvaluateStatic
#define SCL_DEBUG_AI 0

#include "chess/smallchesslib.h"

#define MAX_TEXT_LEN 30 // 30 = max length of text
#define MAX_TEXT_BUF (MAX_TEXT_LEN + 1) // max length of text + null terminator

struct FlipChessScene1 {
    View* view;
    FlipChessScene1Callback callback;
    void* context;
};
typedef struct {
    uint8_t paramPlayerW = 0;
    uint8_t paramPlayerB = 0;

    // uint8_t paramBoard = 1;
    uint8_t paramAnalyze = 255; // depth of analysis
    uint8_t paramMoves = 0;
    //uint8_t paramXboard = 0;
    uint8_t paramInfo = 1;
    //uint8_t paramDraw = 1;
    uint8_t paramFlipBoard = 0;
    //uint8_t paramHelp = 0;
    uint8_t paramExit = 0;
    uint16_t paramStep = 0;
    char* paramFEN = NULL;
    char* paramPGN = NULL;
    //uint16_t paramRandom = 0;
    //uint8_t paramBlind = 0;

    int clockSeconds = -1;
    SCL_Game game;
    SCL_Board startState = SCL_BOARD_START_STATE;
    int16_t random960PosNumber = -1;
} FlipChessScene1Model;

void flipchess_scene_1_set_callback(
    FlipChessScene1* instance,
    FlipChessScene1Callback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void flipchess_scene_1_draw(Canvas* canvas, FlipChessScene1Model* model) {
    //UNUSED(model);
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    
}

static int flipchess_scene_1_model_init(
    FlipChessScene1Model* const model,
    const int white_mode,
    const int black_mode) {
    
    model->paramPlayerW = white_mode;
    model->paramPlayerB = black_mode;

    // 0 = success
    return FlipChessStatusSuccess;
}

bool flipchess_scene_1_input(InputEvent* event, void* context) {
    furi_assert(context);
    FlipChessScene1* instance = context;

    // Ignore input if busy
    // if(s_busy) {
    //     return false;
    // }

    if(event->type == InputTypeRelease) {
        switch(event->key) {
        case InputKeyBack:
            with_view_model(
                instance->view,
                FlipChessScene1Model * model,
                {
                    UNUSED(model);
                    instance->callback(FlipChessCustomEventScene1Back, instance->context);
                },
                true);
            break;
        case InputKeyRight:
        case InputKeyDown:
            with_view_model(
                instance->view,
                FlipChessScene1Model * model,
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
                FlipChessScene1Model * model,
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
            //     FlipChessScene1Model * model,
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

void flipchess_scene_1_exit(void* context) {
    furi_assert(context);
    FlipChessScene1* instance = (FlipChessScene1*)context;

    with_view_model(
        instance->view,
        FlipChessScene1Model * model,
        {
            model->page = PAGE_LOADING;
            model->strength = FlipChessStrength256;
            model->coin = FlipChessCoinBTC0;
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

    flipchess_scene_1_clear_text();
}

void flipchess_scene_1_enter(void* context) {
    furi_assert(context);
    FlipChessScene1* instance = (FlipChessScene1*)context;

    FlipChess* app = instance->context;

    // BIP39 Strength setting
    int strength = 256; // FlipChessStrength256 // 24 words (256 bit)
    if(app->bip39_strength == FlipChessStrength128) {
        strength = 128; // 12 words (128 bit)
    } else if(app->bip39_strength == FlipChessStrength192) {
        strength = 192; // 18 words (192 bit)
    }

    // BIP39 Passphrase setting
    const char* passphrase_text = "";
    if(app->passphrase == FlipChessPassphraseOn && strlen(app->passphrase_text) > 0) {
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

    flipchess_play_happy_bump(app);
    //notification_message(app->notification, &sequence_blink_cyan_100);
    flipchess_led_set_rgb(app, 255, 0, 0);

    with_view_model(
        instance->view,
        FlipChessScene1Model * model,
        {
            // s_busy = true;

            const int status =
                flipchess_scene_1_model_init(model, strength, coin, overwrite, passphrase_text);

            // nonzero status, free the mnemonic
            if(status != FlipChessStatusSuccess) {
                memzero((void*)model->mnemonic, strlen(model->mnemonic));
                free((void*)model->mnemonic);
            }

            // if error, set the error message
            if(status == FlipChessStatusSaveError) {
                model->mnemonic = "ERROR:,Save error";
                model->page = PAGE_MNEMONIC;
                flipchess_play_long_bump(app);
            } else if(status == FlipChessStatusLoadError) {
                model->mnemonic = "ERROR:,Load error";
                model->page = PAGE_MNEMONIC;
                flipchess_play_long_bump(app);
            } else if(status == FlipChessStatusMnemonicCheckError) {
                model->mnemonic = "ERROR:,Mnemonic check error";
                model->page = PAGE_MNEMONIC;
                flipchess_play_long_bump(app);
            }

            // s_busy = false;

            // if overwrite is set and mnemonic generated, return from scene immediately
            if(status == FlipChessStatusReturn) {
                instance->callback(FlipChessCustomEventScene1Back, instance->context);
            }
        },
        true);
}

FlipChessScene1* flipchess_scene_1_alloc() {
    FlipChessScene1* instance = malloc(sizeof(FlipChessScene1));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(FlipChessScene1Model));
    view_set_context(instance->view, instance); // furi_assert crashes in events without this
    view_set_draw_callback(instance->view, (ViewDrawCallback)flipchess_scene_1_draw);
    view_set_input_callback(instance->view, flipchess_scene_1_input);
    view_set_enter_callback(instance->view, flipchess_scene_1_enter);
    view_set_exit_callback(instance->view, flipchess_scene_1_exit);

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

void flipchess_scene_1_free(FlipChessScene1* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view, FlipChessScene1Model * model, { UNUSED(model); }, true);

    // free the address node
    memzero(s_addr_node, sizeof(HDNode));
    free(s_addr_node);

    // free the display text
    flipchess_scene_1_clear_text();
    free(s_disp_text1);
    free(s_disp_text2);
    free(s_disp_text3);
    free(s_disp_text4);
    free(s_disp_text5);
    free(s_disp_text6);

    view_free(instance->view);
    free(instance);
}

View* flipchess_scene_1_get_view(FlipChessScene1* instance) {
    furi_assert(instance);
    return instance->view;
}
