#include "../flipchess.h"
#include <furi.h>
// #include <furi_hal.h>
// #include <furi_hal_random.h>
#include <input/input.h>
#include <gui/elements.h>
//#include <dolphin/dolphin.h>
#include <string.h>
//#include "flipchess_icons.h"
#include "../helpers/flipchess_voice.h"
#include "../helpers/flipchess_haptic.h"

#define SCL_960_CASTLING 0 // setting to 1 compiles a 960 version of smolchess
#define XBOARD_DEBUG 0 // will create files with xboard communication
#define SCL_EVALUATION_FUNCTION SCL_boardEvaluateStatic
#define SCL_DEBUG_AI 0

#include "../chess/smallchesslib.h"

#define ENABLE_960 0 // setting to 1 enables 960 chess
#define MAX_TEXT_LEN 15 // 15 = max length of text
#define MAX_TEXT_BUF (MAX_TEXT_LEN + 1) // max length of text + null terminator
#define THREAD_WAIT_TIME 20 // time to wait for draw thread to finish

struct FlipChessScene1 {
    View* view;
    FlipChessScene1Callback callback;
    void* context;
};
typedef struct {
    uint8_t paramPlayerW;
    uint8_t paramPlayerB;

    uint8_t paramAnalyze; // depth of analysis
    uint8_t paramMoves;
    uint8_t paramInfo;
    uint8_t paramFlipBoard;
    uint8_t paramExit;
    uint16_t paramStep;
    char* paramFEN;
    char* paramPGN;

    int clockSeconds;
    SCL_Game game;
    SCL_Board startState;

#if ENABLE_960
    int16_t random960PosNumber;
#endif

    //uint8_t picture[SCL_BOARD_PICTURE_WIDTH * SCL_BOARD_PICTURE_WIDTH];
    uint8_t squareSelected;
    uint8_t squareSelectedLast;

    char* msg;
    char* msg2;
    char* msg3;
    char moveString[MAX_TEXT_BUF];
    char moveString2[MAX_TEXT_BUF];
    char moveString3[MAX_TEXT_BUF];
    uint8_t thinking;

    SCL_SquareSet moveHighlight;
    uint8_t squareFrom;
    uint8_t squareTo;
    uint8_t turnState;

} FlipChessScene1Model;

static uint8_t picture[SCL_BOARD_PICTURE_WIDTH * SCL_BOARD_PICTURE_WIDTH];

void flipchess_putImagePixel(uint8_t pixel, uint16_t index) {
    picture[index] = pixel;
}

uint8_t flipchess_stringsEqual(const char* s1, const char* s2, int max) {
    for(int i = 0; i < max; ++i) {
        if(*s1 != *s2) return 0;

        if(*s1 == 0) return 1;

        s1++;
        s2++;
    }

    return 1;
}

int16_t flipchess_makeAIMove(
    SCL_Board board,
    uint8_t* s0,
    uint8_t* s1,
    char* prom,
    FlipChessScene1Model* model) {
    uint8_t level = SCL_boardWhitesTurn(board) ? model->paramPlayerW : model->paramPlayerB;
    uint8_t depth = (level > 0) ? level : 1;
    uint8_t extraDepth = 3;
    uint8_t endgameDepth = 1;
    uint8_t randomness =
        model->game.ply < 2 ? 1 : 0; /* in first moves increase randomness for different 
                             openings */
    uint8_t rs0, rs1;

    SCL_gameGetRepetiotionMove(&(model->game), &rs0, &rs1);

    if(model->clockSeconds >= 0) // when using clock, choose AI params accordingly
    {
        if(model->clockSeconds <= 5) {
            depth = 1;
            extraDepth = 2;
            endgameDepth = 0;
        } else if(model->clockSeconds < 15) {
            depth = 2;
            extraDepth = 2;
        } else if(model->clockSeconds < 100) {
            depth = 2;
        } else if(model->clockSeconds < 5 * 60) {
            depth = 3;
        } else {
            depth = 3;
            extraDepth = 4;
        }
    }

    return SCL_getAIMove(
        board,
        depth,
        extraDepth,
        endgameDepth,
        SCL_boardEvaluateStatic,
        SCL_randomBetter,
        randomness,
        rs0,
        rs1,
        s0,
        s1,
        prom);
}

bool flipchess_isPlayerTurn(FlipChessScene1Model* model) {
    return (SCL_boardWhitesTurn(model->game.board) && model->paramPlayerW == 0) ||
           (!SCL_boardWhitesTurn(model->game.board) && model->paramPlayerB == 0);
}

void flipchess_shiftMessages(FlipChessScene1Model* model) {
    // shift messages
    model->msg3 = model->msg2;
    model->msg2 = model->msg;
    strncpy(model->moveString3, model->moveString2, MAX_TEXT_LEN);
    strncpy(model->moveString2, model->moveString, MAX_TEXT_LEN);
}

void flipchess_drawBoard(FlipChessScene1Model* model) {
    // draw chess board
    SCL_drawBoard(
        model->game.board,
        flipchess_putImagePixel,
        model->squareSelected,
        model->moveHighlight,
        model->paramFlipBoard);
}

uint8_t flipchess_saveState(FlipChess* app, FlipChessScene1Model* model) {
    for(uint8_t i = 0; i < SCL_FEN_MAX_LENGTH; i++) {
        app->import_game_text[i] = '\0';
    }
    const uint8_t res = SCL_boardToFEN(model->game.board, app->import_game_text);
    if(res > 0) {
        app->import_game = 1;
    }
    return res;
}

uint8_t flipchess_turn(FlipChessScene1Model* model) {
    // 0: none, 1: player, 2: AI, 3: undo
    uint8_t moveType = FlipChessStatusNone;

    // if(model->paramInfo) {

    //     if(model->random960PosNumber >= 0)
    //         printf("960 random position number: %d\n", model->random960PosNumber);

    //     printf("ply number: %d\n", model->game.ply);

    //     int16_t eval = SCL_boardEvaluateStatic(model->game.board);
    //     printf(
    //         "board static evaluation: %lf (%d)\n",
    //         ((double)eval) / ((double)SCL_VALUE_PAWN),
    //         eval);
    //     printf("board hash: %u\n", SCL_boardHash32(model->game.board));
    //     printf("phase: ");

    //     switch(SCL_boardEstimatePhase(model->game.board)) {
    //     case SCL_PHASE_OPENING:
    //         puts("opening");
    //         break;
    //     case SCL_PHASE_ENDGAME:
    //         puts("endgame");
    //         break;
    //     default:
    //         puts("midgame");
    //         break;
    //     }

    //     printf(
    //         "en passant: %d\n",
    //         ((model->game.board[SCL_BOARD_ENPASSANT_CASTLE_BYTE] & 0x0f) + 1) % 16);
    //     printf(
    //         "50 move rule count: %d\n", model->game.board[SCL_BOARD_MOVE_COUNT_BYTE]);

    //     if(model->paramFEN == NULL && model->paramPGN == NULL) {
    //         printf("PGN: ");
    //         SCL_printPGN(model->game.record, putCharacter, startState);
    //         putchar('\n');
    //     }
    // }

    if(model->game.state != SCL_GAME_STATE_PLAYING) {
        model->paramExit = FlipChessStatusNone;
        return model->paramExit;
    }

    char movePromote = 'q';

    if(flipchess_isPlayerTurn(model)) {
        // if(stringsEqual(string, "undo", 5))
        //     moveType = FlipChessStatusMoveUndo;
        // else if(stringsEqual(string, "quit", 5))
        //     break;

        if(model->turnState == 0 && model->squareSelected != 255) {
            model->squareFrom = model->squareSelected;
            model->turnState = 1;
        } else if(model->turnState == 1 && model->squareSelected != 255) {
            model->squareTo = model->squareSelected;
            model->turnState = 2;
            model->squareSelectedLast = model->squareSelected;
            //model->squareSelected = 255;
        }

        if(model->turnState == 1 && model->squareFrom != 255) {
            if((model->game.board[model->squareFrom] != '.') &&
               (SCL_pieceIsWhite(model->game.board[model->squareFrom]) ==
                SCL_boardWhitesTurn(model->game.board))) {
                SCL_boardGetMoves(model->game.board, model->squareFrom, model->moveHighlight);
            }
        } else if(model->turnState == 2) {
            if(SCL_squareSetContains(model->moveHighlight, model->squareTo)) {
                moveType = FlipChessStatusMovePlayer;
            }
            model->turnState = 0;
            SCL_squareSetClear(model->moveHighlight);
        }

    } else {
        model->squareSelected = 255;
        flipchess_makeAIMove(
            model->game.board, &(model->squareFrom), &(model->squareTo), &movePromote, model);
        moveType = FlipChessStatusMoveAI;
        model->turnState = 0;
    }

    if(moveType == FlipChessStatusMovePlayer || moveType == FlipChessStatusMoveAI) {
        flipchess_shiftMessages(model);

        SCL_moveToString(
            model->game.board, model->squareFrom, model->squareTo, movePromote, model->moveString);

        SCL_gameMakeMove(&(model->game), model->squareFrom, model->squareTo, movePromote);

        SCL_squareSetClear(model->moveHighlight);
        SCL_squareSetAdd(model->moveHighlight, model->squareFrom);
        SCL_squareSetAdd(model->moveHighlight, model->squareTo);
    } else if(moveType == FlipChessStatusMoveUndo) {
        flipchess_shiftMessages(model);

        if(model->paramPlayerW != 0 || model->paramPlayerB != 0) SCL_gameUndoMove(&(model->game));

        SCL_gameUndoMove(&(model->game));
        SCL_squareSetClear(model->moveHighlight);
    }

    switch(model->game.state) {
    case SCL_GAME_STATE_WHITE_WIN:
        model->msg = "white wins";
        model->paramExit = FlipChessStatusReturn;
        break;

    case SCL_GAME_STATE_BLACK_WIN:
        model->msg = "black wins";
        model->paramExit = FlipChessStatusReturn;
        break;

    case SCL_GAME_STATE_DRAW_STALEMATE:
        model->msg = "draw (stalemate)";
        model->paramExit = FlipChessStatusReturn;
        break;

    case SCL_GAME_STATE_DRAW_REPETITION:
        model->msg = "draw (repetition)";
        model->paramExit = FlipChessStatusReturn;
        break;

    case SCL_GAME_STATE_DRAW_DEAD:
        model->msg = "draw (dead pos.)";
        model->paramExit = FlipChessStatusReturn;
        break;

    case SCL_GAME_STATE_DRAW:
        model->msg = "draw";
        model->paramExit = FlipChessStatusReturn;
        break;

    case SCL_GAME_STATE_DRAW_50:
        model->msg = "draw (50 moves)";
        model->paramExit = FlipChessStatusReturn;
        break;

    default:
        if(model->game.ply > 0) {
            model->msg =
                (SCL_boardWhitesTurn(model->game.board) ? "black played" : "white played");

            uint8_t s0, s1;
            char p;

            SCL_recordGetMove(model->game.record, model->game.ply - 1, &s0, &s1, &p);
            SCL_moveToString(model->game.board, s0, s1, p, model->moveString);
        }
        break;
        model->paramExit = moveType;
    }

    model->thinking = 0;
    return model->paramExit;
}

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

    //canvas_draw_icon(canvas, 0, 0, &I_FLIPR_128x64);

    // Frame
    canvas_draw_frame(canvas, 0, 0, 66, 64);

    // Message
    canvas_set_font(canvas, FontSecondary);
    if(model->thinking) {
        canvas_draw_str(canvas, 68, 10, "thinking...");
    } else {
        canvas_draw_str(canvas, 68, 10, model->msg);
    }
    canvas_draw_str(canvas, 68, 19, model->moveString);
    canvas_draw_str(canvas, 68, 31, model->msg2);
    canvas_draw_str(canvas, 68, 40, model->moveString2);
    canvas_draw_str(canvas, 68, 52, model->msg3);
    canvas_draw_str(canvas, 68, 61, model->moveString3);

    // Board
    for(uint16_t y = 0; y < SCL_BOARD_PICTURE_WIDTH; y++) {
        for(uint16_t x = 0; x < SCL_BOARD_PICTURE_WIDTH; x++) {
            if(!picture[x + (y * SCL_BOARD_PICTURE_WIDTH)]) {
                canvas_draw_dot(canvas, x + 1, y);
            }
        }
    }
}

static int flipchess_scene_1_model_init(
    FlipChessScene1Model* const model,
    const int white_mode,
    const int black_mode,
    char* import_game_text) {
    model->paramPlayerW = white_mode;
    model->paramPlayerB = black_mode;

    model->paramAnalyze = 255; // depth of analysis
    model->paramMoves = 0;
    model->paramInfo = 1;
    model->paramFlipBoard = 0;
    model->paramExit = FlipChessStatusNone;
    model->paramStep = 0;
    model->paramFEN = import_game_text;
    model->paramPGN = NULL;
    model->clockSeconds = -1;

    SCL_Board emptyStartState = SCL_BOARD_START_STATE;
    memcpy(model->startState, &emptyStartState, sizeof(SCL_Board));

#if ENABLE_960
    model->random960PosNumber = -1;
#endif

    model->squareSelected = 255;
    model->squareSelectedLast = 28; // start selector near middle

    model->msg = "init";
    model->moveString[0] = '\0';
    model->msg2 = "";
    model->moveString2[0] = '\0';
    model->msg3 = "";
    model->moveString3[0] = '\0';
    model->thinking = 0;

    SCL_SquareSet emptySquareSet = SCL_SQUARE_SET_EMPTY;
    memcpy(model->moveHighlight, &emptySquareSet, sizeof(SCL_SquareSet));
    model->squareFrom = 255;
    model->squareTo = 255;
    model->turnState = 0;

    SCL_randomBetterSeed(furi_hal_random_get());

#if ENABLE_960
#if SCL_960_CASTLING
    if(model->random960PosNumber < 0) model->random960PosNumber = SCL_randomBetter();
#endif
    if(model->random960PosNumber >= 0) model->random960PosNumber %= 960;
#endif

    if(model->paramFEN != NULL)
        SCL_boardFromFEN(model->startState, model->paramFEN);
    else if(model->paramPGN != NULL) {
        SCL_Record record;
        SCL_recordFromPGN(record, model->paramPGN);
        SCL_boardInit(model->startState);
        SCL_recordApply(record, model->startState, model->paramStep);
    }

#if ENABLE_960
#if SCL_960_CASTLING
    else
        SCL_boardInit960(model->startState, model->random960PosNumber);
#endif
#endif

    SCL_gameInit(&(model->game), model->startState);

    if(model->paramAnalyze != 255) {
        char p;
        uint8_t move[] = {0, 0};

        model->paramPlayerW = model->paramAnalyze;
        model->paramPlayerB = model->paramAnalyze;

        int16_t evaluation =
            flipchess_makeAIMove(model->game.board, &(move[0]), &(move[1]), &p, model);

        if(model->paramAnalyze == 0) evaluation = SCL_boardEvaluateStatic(model->game.board);

        char moveStr[5];
        moveStr[4] = 0;

        SCL_squareToString(move[0], moveStr);
        SCL_squareToString(move[1], moveStr + 2);

        //printf("%lf (%d)\n", ((double)evaluation) / ((double)SCL_VALUE_PAWN), evaluation);
        //puts(moveStr);

        return evaluation;
    }

    if(model->paramMoves) {
        char string[256];

        for(int i = 0; i < 64; ++i)
            if(model->game.board[i] != '.' &&
               SCL_pieceIsWhite(model->game.board[i]) == SCL_boardWhitesTurn(model->game.board)) {
                SCL_SquareSet possibleMoves = SCL_SQUARE_SET_EMPTY;

                SCL_boardGetMoves(model->game.board, i, possibleMoves);

                SCL_SQUARE_SET_ITERATE_BEGIN(possibleMoves)
                SCL_moveToString(model->game.board, i, iteratedSquare, 'q', string);
                //printf("%s ", string);
                SCL_SQUARE_SET_ITERATE_END
            }

        return FlipChessStatusReturn;
    }

    model->msg = (SCL_boardWhitesTurn(model->game.board) ? "white to move" : "black to move");

    // 0 = success
    return FlipChessStatusNone;
}

bool flipchess_scene_1_input(InputEvent* event, void* context) {
    furi_assert(context);
    FlipChessScene1* instance = context;
    FlipChess* app = instance->context;

    if(event->type == InputTypeRelease) {
        switch(event->key) {
        case InputKeyBack:
            with_view_model(
                instance->view,
                FlipChessScene1Model * model,
                {
                    if(model->turnState == 1) {
                        model->turnState = 0;
                        SCL_squareSetClear(model->moveHighlight);
                        flipchess_drawBoard(model);
                    } else {
                        instance->callback(FlipChessCustomEventScene1Back, instance->context);
                    }
                },
                true);
            break;
        case InputKeyRight:
            with_view_model(
                instance->view,
                FlipChessScene1Model * model,
                {
                    if(model->squareSelectedLast != 255 && model->squareSelected == 255) {
                        model->squareSelected = model->squareSelectedLast;
                    } else {
                        model->squareSelected = (model->squareSelected + 1) % 64;
                    }
                    flipchess_drawBoard(model);
                },
                true);
            break;
        case InputKeyDown:
            with_view_model(
                instance->view,
                FlipChessScene1Model * model,
                {
                    if(model->squareSelectedLast != 255 && model->squareSelected == 255) {
                        model->squareSelected = model->squareSelectedLast;
                    } else {
                        model->squareSelected = (model->squareSelected + 56) % 64;
                    }
                    flipchess_drawBoard(model);
                },
                true);
            break;
        case InputKeyLeft:
            with_view_model(
                instance->view,
                FlipChessScene1Model * model,
                {
                    if(model->squareSelectedLast != 255 && model->squareSelected == 255) {
                        model->squareSelected = model->squareSelectedLast;
                    } else {
                        model->squareSelected = (model->squareSelected + 63) % 64;
                    }
                    flipchess_drawBoard(model);
                },
                true);
            break;
        case InputKeyUp:
            with_view_model(
                instance->view,
                FlipChessScene1Model * model,
                {
                    if(model->squareSelectedLast != 255 && model->squareSelected == 255) {
                        model->squareSelected = model->squareSelectedLast;
                    } else {
                        model->squareSelected = (model->squareSelected + 8) % 64;
                    }
                    flipchess_drawBoard(model);
                },
                true);
            break;
        case InputKeyOk:
            with_view_model(
                instance->view,
                FlipChessScene1Model * model,
                {
                    // if(model->paramExit == FlipChessStatusReturn) {
                    //     instance->callback(FlipChessCustomEventScene1Back, instance->context);
                    //     break;
                    // }
                    if(!flipchess_isPlayerTurn(model)) {
                        model->thinking = 1;
                    }
                },
                true);
            furi_thread_flags_wait(0, FuriFlagWaitAny, THREAD_WAIT_TIME);

            with_view_model(
                instance->view,
                FlipChessScene1Model * model,
                {
                    // first turn of round, probably player but could be AI
                    if(flipchess_turn(model) == FlipChessStatusReturn) {
                        if(app->sound == 1) flipchess_voice_a_strange_game();
                        flipchess_play_long_bump(app);
                    }
                    flipchess_saveState(app, model);
                    flipchess_drawBoard(model);
                },
                true);

            with_view_model(
                instance->view,
                FlipChessScene1Model * model,
                {
                    if(!flipchess_isPlayerTurn(model)) {
                        model->thinking = 1;
                    }
                },
                true);
            furi_thread_flags_wait(0, FuriFlagWaitAny, THREAD_WAIT_TIME);

            with_view_model(
                instance->view,
                FlipChessScene1Model * model,
                {
                    // if player played, let AI play
                    if(!flipchess_isPlayerTurn(model)) {
                        if(flipchess_turn(model) == FlipChessStatusReturn) {
                            if(app->sound == 1) flipchess_voice_a_strange_game();
                            flipchess_play_long_bump(app);
                        }
                        flipchess_saveState(app, model);
                        flipchess_drawBoard(model);
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

void flipchess_scene_1_exit(void* context) {
    furi_assert(context);
    FlipChessScene1* instance = (FlipChessScene1*)context;

    with_view_model(
        instance->view, FlipChessScene1Model * model, { model->paramExit = 0; }, true);
}

void flipchess_scene_1_enter(void* context) {
    furi_assert(context);
    FlipChessScene1* instance = (FlipChessScene1*)context;
    FlipChess* app = instance->context;

    flipchess_play_happy_bump(app);

    with_view_model(
        instance->view,
        FlipChessScene1Model * model,
        {
            // load imported game if applicable
            char* import_game_text = NULL;
            if(app->import_game == 1 && strlen(app->import_game_text) > 0) {
                import_game_text = app->import_game_text;
            } else {
                if(app->sound == 1) flipchess_voice_how_about_chess();
            }

            int init = flipchess_scene_1_model_init(
                model, app->white_mode, app->black_mode, import_game_text);

            if(init == FlipChessStatusNone) {
                // perform initial turn, sets up and lets white
                // AI play if applicable
                const uint8_t turn = flipchess_turn(model);
                if(turn == FlipChessStatusReturn) {
                    init = turn;
                } else {
                    flipchess_saveState(app, model);
                    flipchess_drawBoard(model);
                }
            }

            // if return status, return from scene immediately
            if(init == FlipChessStatusReturn) {
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

    return instance;
}

void flipchess_scene_1_free(FlipChessScene1* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view, FlipChessScene1Model * model, { UNUSED(model); }, true);

    view_free(instance->view);
    free(instance);
}

View* flipchess_scene_1_get_view(FlipChessScene1* instance) {
    furi_assert(instance);
    return instance->view;
}
