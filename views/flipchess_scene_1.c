#include "../flipchess.h"
#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_random.h>
#include <input/input.h>
#include <gui/elements.h>
//#include <dolphin/dolphin.h>
#include <string.h>
//#include "flipchess_icons.h"
#include "../helpers/flipchess_haptic.h"

#define SCL_960_CASTLING 0 // setting to 1 compiles a 960 version of smolchess
#define XBOARD_DEBUG 0 // will create files with xboard communication
#define SCL_EVALUATION_FUNCTION SCL_boardEvaluateStatic
#define SCL_DEBUG_AI 0

#include "../chess/smallchesslib.h"

#define MAX_TEXT_LEN 30 // 30 = max length of text
#define MAX_TEXT_BUF (MAX_TEXT_LEN + 1) // max length of text + null terminator

struct FlipChessScene1 {
    View* view;
    FlipChessScene1Callback callback;
    void* context;
};
typedef struct {
    uint8_t paramPlayerW;
    uint8_t paramPlayerB;

    // uint8_t paramBoard = 1;
    uint8_t paramAnalyze; // depth of analysis
    uint8_t paramMoves;
    //uint8_t paramXboard = 0;
    uint8_t paramInfo;
    //uint8_t paramDraw = 1;
    uint8_t paramFlipBoard;
    //uint8_t paramHelp = 0;
    uint8_t paramExit;
    uint16_t paramStep;
    char* paramFEN;
    char* paramPGN;
    //uint16_t paramRandom = 0;
    //uint8_t paramBlind = 0;

    int clockSeconds;
    SCL_Game game;
    SCL_Board startState;
    int16_t random960PosNumber;

    //uint8_t picture[SCL_BOARD_PICTURE_WIDTH * SCL_BOARD_PICTURE_WIDTH];
    uint8_t selected;
    char* msg;

    SCL_SquareSet squareSet;
    char moveString[16];
    SCL_SquareSet moveHighlight;
    uint8_t squareFrom;
    uint8_t squareTo;

} FlipChessScene1Model;

uint8_t picture[SCL_BOARD_PICTURE_WIDTH * SCL_BOARD_PICTURE_WIDTH];

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

uint8_t flipchess_round(FlipChessScene1Model* model) {
    // 0: none, 1: player, 2: AI, 3: undo
    uint8_t moveType = 0;

    //for(int i = 0; i < 40; ++i) putchar('\n');
    //putchar('\n');

    if(model->game.ply > 0) {
        model->msg = (SCL_boardWhitesTurn(model->game.board) ? "black played" : "white  played");
        // printf(" played ");

        uint8_t s0, s1;
        char p;

        SCL_recordGetMove(model->game.record, model->game.ply - 1, &s0, &s1, &p);
        SCL_moveToString(model->game.board, s0, s1, p, model->moveString);
        model->msg = model->moveString;
        //printf("%s\n", moveString);
    }

    model->msg = (SCL_boardWhitesTurn(model->game.board) ? "white to move" : "black to move");
    //printf(" to move\n");

    // if(paramInfo) {
    //     //putchar('\n');

    //     if(random960PosNumber >= 0)
    //         printf("960 random position number: %d\n", random960PosNumber);

    //     printf("ply number: %d\n", game.ply);

    //     //SCL_boardToFEN(game.board, string);
    //     //printf("FEN: %s\n", string);

    //     int16_t eval = SCL_boardEvaluateStatic(game.board);
    //     printf(
    //         "board static evaluation: %lf (%d)\n",
    //         ((double)eval) / ((double)SCL_VALUE_PAWN),
    //         eval);
    //     printf("board hash: %u\n", SCL_boardHash32(game.board));
    //     printf("phase: ");

    //     switch(SCL_boardEstimatePhase(game.board)) {
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
    //         ((game.board[SCL_BOARD_ENPASSANT_CASTLE_BYTE] & 0x0f) + 1) % 16);
    //     printf(
    //         "50 move rule count: %d\n", game.board[SCL_BOARD_MOVE_COUNT_BYTE]);

    //     // if(paramFEN == NULL && paramPGN == NULL) {
    //     //     //printf("PGN: ");
    //     //     //SCL_printPGN(game.record, putCharacter, startState);
    //     //     //putchar('\n');
    //     // }
    // }

    if(model->game.state != SCL_GAME_STATE_PLAYING || model->paramExit)
        return FlipChessStatusSuccess;

    //uint8_t squareFrom = 0;
    //uint8_t squareTo = 0;
    char movePromote = 'q';

    if((SCL_boardWhitesTurn(model->game.board) && model->paramPlayerW == 0) ||
       (!SCL_boardWhitesTurn(model->game.board) && model->paramPlayerB == 0)) {
        // printf("\nmove: ");
        // scanf("%s", string);
        // char string[256];

        // if(stringsEqual(string, "undo", 5))
        //     moveType = 3;
        // else if(stringsEqual(string, "quit", 5))
        //     break;
        // else {
        //squareFrom = selected; //SCL_stringToSquare(string);
        //squareTo = selected; //SCL_stringToSquare(string + 2);

        //uint8_t r =
        //    SCL_stringToMove(string, &squareFrom, &squareTo, &movePromote);

        if(model->squareFrom != 255) {
            if((model->game.board[model->squareFrom] != '.') &&
               (SCL_pieceIsWhite(model->game.board[model->squareFrom]) ==
                SCL_boardWhitesTurn(model->game.board))) {
                SCL_boardGetMoves(model->game.board, model->squareFrom, model->squareSet);

                if(SCL_squareSetContains(model->squareSet, model->squareTo)) {
                    moveType = 1;
                }
            }
        }
        // }
    } else {
        flipchess_makeAIMove(
            model->game.board, &(model->squareFrom), &(model->squareTo), &movePromote, model);
        moveType = 2;
    }

    if(moveType == 1 || moveType == 2) {
        SCL_moveToString(
            model->game.board, model->squareFrom, model->squareTo, movePromote, model->moveString);

        SCL_gameMakeMove(&(model->game), model->squareFrom, model->squareTo, movePromote);

        SCL_squareSetClear(model->moveHighlight);
        SCL_squareSetAdd(model->moveHighlight, model->squareFrom);
        SCL_squareSetAdd(model->moveHighlight, model->squareTo);
    } else if(moveType == 3) {
        if(model->paramPlayerW != 0 || model->paramPlayerB != 0) SCL_gameUndoMove(&(model->game));

        SCL_gameUndoMove(&(model->game));
        SCL_squareSetClear(model->moveHighlight);
    }

    //putchar('\n');

    SCL_drawBoard(
        model->game.board,
        flipchess_putImagePixel,
        model->selected,
        model->squareSet,
        model->paramFlipBoard);

    switch(model->game.state) {
    case SCL_GAME_STATE_WHITE_WIN:
        model->msg = "white wins";
        break;

    case SCL_GAME_STATE_BLACK_WIN:
        model->msg = "black wins";
        break;

    case SCL_GAME_STATE_DRAW_STALEMATE:
        model->msg = "draw (stalemate)";
        break;

    case SCL_GAME_STATE_DRAW_REPETITION:
        model->msg = "draw (repeated position)";
        break;

    case SCL_GAME_STATE_DRAW_DEAD:
        model->msg = "draw (dead position)";
        break;

    case SCL_GAME_STATE_DRAW:
        model->msg = "draw";
        break;

    case SCL_GAME_STATE_DRAW_50:
        model->msg = "draw (50 move rule)";
        break;

    default:
        //model->msg = "game over";
        break;
    }

    return FlipChessStatusSuccess;
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

    // Frame
    canvas_draw_frame(canvas, 0, 0, 128, 64);

    // Message
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 66, 10, model->msg);

    // Board
    for(uint16_t y = 0; y < SCL_BOARD_PICTURE_WIDTH; y++) {
        for(uint16_t x = 0; x < SCL_BOARD_PICTURE_WIDTH; x++) {
            if(picture[x + (y * SCL_BOARD_PICTURE_WIDTH)]) {
                canvas_draw_dot(canvas, x, y);
            }
        }
    }
}

static int flipchess_scene_1_model_init(
    FlipChessScene1Model* const model,
    const int white_mode,
    const int black_mode) {
    model->paramPlayerW = white_mode;
    model->paramPlayerB = black_mode;

    model->paramAnalyze = 255; // depth of analysis
    model->paramMoves = 0;
    model->paramInfo = 1;
    model->paramFlipBoard = 0;
    model->paramExit = 0;
    model->paramStep = 0;
    model->paramFEN = NULL;
    model->paramPGN = NULL;

    model->clockSeconds = -1;
    SCL_Board emptyStartState = SCL_BOARD_START_STATE;
    memcpy(model->startState, &emptyStartState, sizeof(SCL_Board));
    model->random960PosNumber = -1;

    model->selected = 255;
    model->msg = "Flip Chess";

    SCL_SquareSet emptySquareSet = SCL_SQUARE_SET_EMPTY;
    memcpy(model->squareSet, &emptySquareSet, sizeof(SCL_SquareSet));
    model->moveString[0] = '\0';
    memcpy(model->moveHighlight, &emptySquareSet, sizeof(SCL_SquareSet));
    model->squareFrom = 255;
    model->squareTo = 255;

    furi_hal_random_init();
    SCL_randomBetterSeed(furi_hal_random_get());

#if SCL_960_CASTLING
    if(model->random960PosNumber < 0) model->random960PosNumber = SCL_randomBetter();
#endif

    if(model->random960PosNumber >= 0) model->random960PosNumber %= 960;

    if(model->paramFEN != NULL)
        SCL_boardFromFEN(model->startState, model->paramFEN);
    else if(model->paramPGN != NULL) {
        SCL_Record record;
        SCL_recordFromPGN(record, model->paramPGN);
        SCL_boardInit(model->startState);
        SCL_recordApply(record, model->startState, model->paramStep);
    }
#if SCL_960_CASTLING
    else
        SCL_boardInit960(model->startState, model->random960PosNumber);
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

    model->moveString[0] = 0;

    SCL_drawBoard(
        model->game.board,
        flipchess_putImagePixel,
        model->selected,
        model->squareSet,
        model->paramFlipBoard);

    // 0 = success
    return FlipChessStatusSuccess;
}

bool flipchess_scene_1_input(InputEvent* event, void* context) {
    furi_assert(context);
    FlipChessScene1* instance = context;

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
            with_view_model(
                instance->view,
                FlipChessScene1Model * model,
                {
                    model->selected = (model->selected + 1) % 64;
                    SCL_drawBoard(
                        model->game.board,
                        flipchess_putImagePixel,
                        model->selected,
                        model->squareSet,
                        model->paramFlipBoard);
                },
                true);
            break;
        case InputKeyDown:
            with_view_model(
                instance->view,
                FlipChessScene1Model * model,
                {
                    model->selected = (model->selected + 56) % 64;
                    SCL_drawBoard(
                        model->game.board,
                        flipchess_putImagePixel,
                        model->selected,
                        model->squareSet,
                        model->paramFlipBoard);
                },
                true);
            break;
        case InputKeyLeft:
            with_view_model(
                instance->view,
                FlipChessScene1Model * model,
                {
                    model->selected = (model->selected + 63) % 64;
                    SCL_drawBoard(
                        model->game.board,
                        flipchess_putImagePixel,
                        model->selected,
                        model->squareSet,
                        model->paramFlipBoard);
                },
                true);
            break;
        case InputKeyUp:
            with_view_model(
                instance->view,
                FlipChessScene1Model * model,
                {
                    model->selected = (model->selected + 8) % 64;
                    SCL_drawBoard(
                        model->game.board,
                        flipchess_putImagePixel,
                        model->selected,
                        model->squareSet,
                        model->paramFlipBoard);
                },
                true);
            break;
        case InputKeyOk:
            with_view_model(
                instance->view, FlipChessScene1Model * model, { flipchess_round(model); }, true);
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
        instance->view, FlipChessScene1Model * model, { model->selected = 255; }, true);
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
            const int status =
                flipchess_scene_1_model_init(model, app->white_mode, app->white_mode);

            // nonzero status
            if(status != FlipChessStatusSuccess) {
            }

            // if return status, return from scene immediately
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
