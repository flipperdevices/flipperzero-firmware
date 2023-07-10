#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include <dolphin/dolphin.h>

#include <furi_hal_random.h>

#define SCL_960_CASTLING 0 // setting to 1 compiles a 960 version of smolchess
#define XBOARD_DEBUG 0 // will create files with xboard communication
#define SCL_EVALUATION_FUNCTION SCL_boardEvaluateStatic

#define SCL_DEBUG_AI 0

#include "smallchesslib.h"

typedef struct {
    uint8_t status;
    FuriMutex* mutex;
} ChessState;

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} Event;

uint8_t paramPlayerW = 0;
uint8_t paramPlayerB = 0;
// uint8_t paramBoard = 1;
uint8_t paramAnalyze = 255; // depth of analysis
uint8_t paramMoves = 0;
//uint8_t paramXboard = 0;
uint8_t paramInfo = 1;
//uint8_t paramDraw = 1;
uint8_t paramFlipBoard = 0;
uint8_t paramHelp = 0;
uint8_t paramExit = 0;
uint16_t paramStep = 0;
char* paramFEN = NULL;
char* paramPGN = NULL;
uint16_t paramRandom = 0;
uint8_t paramBlind = 0;
int clockSeconds = -1;
SCL_Game game;
SCL_Board startState = SCL_BOARD_START_STATE;

int16_t random960PosNumber = -1;

uint8_t picture[SCL_BOARD_PICTURE_WIDTH * SCL_BOARD_PICTURE_WIDTH];
uint8_t selected = 255;
char* msg = "Flip Chess";

void putImagePixel(uint8_t pixel, uint16_t index) {
    picture[index] = pixel;
}

uint8_t stringsEqual(const char* s1, const char* s2, int max) {
    for(int i = 0; i < max; ++i) {
        if(*s1 != *s2) return 0;

        if(*s1 == 0) return 1;

        s1++;
        s2++;
    }

    return 1;
}

int16_t makeAIMove(SCL_Board board, uint8_t* s0, uint8_t* s1, char* prom) {
    uint8_t level = SCL_boardWhitesTurn(board) ? paramPlayerW : paramPlayerB;
    uint8_t depth = (level > 0) ? level : 1;
    uint8_t extraDepth = 3;
    uint8_t endgameDepth = 1;
    uint8_t randomness = game.ply < 2 ? 1 : 0; /* in first moves increase randomness for different 
                             openings */
    uint8_t rs0, rs1;

    SCL_gameGetRepetiotionMove(&game, &rs0, &rs1);

    if(clockSeconds >= 0) // when using clock, choose AI params accordingly
    {
        if(clockSeconds <= 5) {
            depth = 1;
            extraDepth = 2;
            endgameDepth = 0;
        } else if(clockSeconds < 15) {
            depth = 2;
            extraDepth = 2;
        } else if(clockSeconds < 100) {
            depth = 2;
        } else if(clockSeconds < 5 * 60) {
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

void initGame() {
    SCL_randomBetterSeed(furi_hal_random_get());

#if SCL_960_CASTLING
    if(random960PosNumber < 0) random960PosNumber = SCL_randomBetter();
#endif

    if(random960PosNumber >= 0) random960PosNumber %= 960;

    if(paramFEN != NULL)
        SCL_boardFromFEN(startState, paramFEN);
    else if(paramPGN != NULL) {
        SCL_Record record;
        SCL_recordFromPGN(record, paramPGN);
        SCL_boardInit(startState);
        SCL_recordApply(record, startState, paramStep);
    }
#if SCL_960_CASTLING
    else
        SCL_boardInit960(startState, random960PosNumber);
#endif

    SCL_gameInit(&game, startState);

    if(paramAnalyze != 255) {
        char p;

        uint8_t move[] = {0, 0};

        paramPlayerW = paramAnalyze;
        paramPlayerB = paramAnalyze;

        int16_t evaluation = makeAIMove(game.board, &(move[0]), &(move[1]), &p);

        if(paramAnalyze == 0) evaluation = SCL_boardEvaluateStatic(game.board);

        char moveStr[5];
        moveStr[4] = 0;

        SCL_squareToString(move[0], moveStr);
        SCL_squareToString(move[1], moveStr + 2);

        //printf("%lf (%d)\n", ((double)evaluation) / ((double)SCL_VALUE_PAWN), evaluation);
        //puts(moveStr);

        return 0;
    }

    if(paramMoves) {
        char string[256];

        for(int i = 0; i < 64; ++i)
            if(game.board[i] != '.' &&
               SCL_pieceIsWhite(game.board[i]) == SCL_boardWhitesTurn(game.board)) {
                SCL_SquareSet possibleMoves = SCL_SQUARE_SET_EMPTY;

                SCL_boardGetMoves(game.board, i, possibleMoves);

                SCL_SQUARE_SET_ITERATE_BEGIN(possibleMoves)
                SCL_moveToString(game.board, i, iteratedSquare, 'q', string);
                //printf("%s ", string);
                SCL_SQUARE_SET_ITERATE_END
            }

        return 0;
    }
}

static void flip_chess_render_callback(Canvas* const canvas, void* ctx) {
    furi_assert(ctx);
    const ChessState* chess_state = ctx;
    furi_mutex_acquire(chess_state->mutex, FuriWaitForever);

    // Before the function is called, the state is set with the canvas_reset(canvas)

    // Frame
    canvas_draw_frame(canvas, 0, 0, 128, 64);

    // Message
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 66, 10, msg);

    // Board
    for(uint16_t y = 0; y < SCL_BOARD_PICTURE_WIDTH; y++) {
        for(uint16_t x = 0; x < SCL_BOARD_PICTURE_WIDTH; x++) {
            if(picture[x + (y * SCL_BOARD_PICTURE_WIDTH)]) {
                canvas_draw_dot(canvas, x, y);
            }
        }
    }

    furi_mutex_release(chess_state->mutex);
}

static void flip_chess_input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    Event event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

int32_t flip_chess_app(void* p) {
    UNUSED(p);

    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(Event));
    dolphin_deed(DolphinDeedPluginStart);

    ChessState* chess_state = malloc(sizeof(ChessState));

    chess_state->status = 0;

    chess_state->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!chess_state->mutex) {
        FURI_LOG_E("FlipChess", "cannot create mutex\r\n");
        furi_message_queue_free(event_queue);
        free(chess_state);
        return 255;
    }

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, flip_chess_render_callback, chess_state);
    view_port_input_callback_set(view_port, flip_chess_input_callback, event_queue);

    // Open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    Event event;

    furi_hal_random_init();
    initGame();

    //char string[256];
    SCL_SquareSet squareSet = SCL_SQUARE_SET_EMPTY;
    char moveString[16];
    moveString[0] = 0;
    SCL_SquareSet moveHighlight = SCL_SQUARE_SET_EMPTY;
    uint8_t squareFrom = 255;
    uint8_t squareTo = 255;

    SCL_drawBoard(game.board, putImagePixel, selected, squareSet, paramFlipBoard);

    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);

        furi_mutex_acquire(chess_state->mutex, FuriWaitForever);

        if(event_status == FuriStatusOk) {
            // press events
            if(event.type == EventTypeKey) {
                if(event.input.type == InputTypePress) {
                    switch(event.input.key) {
                    case InputKeyUp:
                        selected = (selected + 8) % 64;
                        SCL_drawBoard(
                            game.board, putImagePixel, selected, squareSet, paramFlipBoard);
                        break;
                    case InputKeyDown:
                        selected = (selected + 56) % 64;
                        SCL_drawBoard(
                            game.board, putImagePixel, selected, squareSet, paramFlipBoard);
                        break;
                    case InputKeyRight:
                        selected = (selected + 1) % 64;
                        SCL_drawBoard(
                            game.board, putImagePixel, selected, squareSet, paramFlipBoard);
                        break;
                    case InputKeyLeft:
                        selected = (selected + 63) % 64;
                        SCL_drawBoard(
                            game.board, putImagePixel, selected, squareSet, paramFlipBoard);
                        break;
                    case InputKeyOk: ;

                        if (chess_state->status == 1) {
                            squareFrom = selected;
                            chess_state->status = 2;
                        } else {
                            squareTo = selected;
                            chess_state->status = 1;
                        }


                        // // // // //

                        // 0: none, 1: player, 2: AI, 3: undo
                        uint8_t moveType = 0;

                        //for(int i = 0; i < 40; ++i) putchar('\n');
                        //putchar('\n');

                        if(game.ply > 0) {
                            msg =
                                (SCL_boardWhitesTurn(game.board) ? "black played" :
                                                                   "white  played");
                            // printf(" played ");

                            uint8_t s0, s1;
                            char p;

                            SCL_recordGetMove(game.record, game.ply - 1, &s0, &s1, &p);
                            SCL_moveToString(game.board, s0, s1, p, moveString);
                            msg = moveString;
                            //printf("%s\n", moveString);
                        }

                        msg =
                            (SCL_boardWhitesTurn(game.board) ? "white to move" : "black to move");
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

                        if(game.state != SCL_GAME_STATE_PLAYING || paramExit) break;

                        //uint8_t squareFrom = 0;
                        //uint8_t squareTo = 0;
                        char movePromote = 'q';

                        if((SCL_boardWhitesTurn(game.board) && paramPlayerW == 0) ||
                           (!SCL_boardWhitesTurn(game.board) && paramPlayerB == 0)) {
                            
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

                                if(squareFrom != 255) {
                                    if((game.board[squareFrom] != '.') &&
                                       (SCL_pieceIsWhite(game.board[squareFrom]) ==
                                        SCL_boardWhitesTurn(game.board))) {
                                        SCL_boardGetMoves(game.board, squareFrom, squareSet);

                                        if(SCL_squareSetContains(squareSet, squareTo)) {
                                            moveType = 1;
                                        }
                                    }
                                }
                            // }
                        } else {
                            makeAIMove(game.board, &squareFrom, &squareTo, &movePromote);
                            moveType = 2;
                        }

                        if(moveType == 1 || moveType == 2) {
                            SCL_moveToString(
                                game.board, squareFrom, squareTo, movePromote, moveString);

                            SCL_gameMakeMove(&game, squareFrom, squareTo, movePromote);

                            SCL_squareSetClear(moveHighlight);
                            SCL_squareSetAdd(moveHighlight, squareFrom);
                            SCL_squareSetAdd(moveHighlight, squareTo);
                        } else if(moveType == 3) {
                            if(paramPlayerW != 0 || paramPlayerB != 0) SCL_gameUndoMove(&game);

                            SCL_gameUndoMove(&game);
                            SCL_squareSetClear(moveHighlight);
                        }

                        //putchar('\n');

                        SCL_drawBoard(
                            game.board, putImagePixel, selected, squareSet, paramFlipBoard);

                        switch(game.state) {
                        case SCL_GAME_STATE_WHITE_WIN:
                            msg = "white wins";
                            break;

                        case SCL_GAME_STATE_BLACK_WIN:
                            msg = "black wins";
                            break;

                        case SCL_GAME_STATE_DRAW_STALEMATE:
                            msg = "draw (stalemate)";
                            break;

                        case SCL_GAME_STATE_DRAW_REPETITION:
                            msg = "draw (repeated position)";
                            break;

                        case SCL_GAME_STATE_DRAW_DEAD:
                            msg = "draw (dead position)";
                            break;

                        case SCL_GAME_STATE_DRAW:
                            msg = "draw";
                            break;

                        case SCL_GAME_STATE_DRAW_50:
                            msg = "draw (50 move rule)";
                            break;

                        default:
                            //msg = "game over";
                            break;

                            // // // // //
                        }

                        break;
                    case InputKeyBack:
                        processing = false;
                        break;
                    default:
                        break;
                    }
                }
            }
        }

        view_port_update(view_port);
        furi_mutex_release(chess_state->mutex);
    }

    // Reset GPIO pins to default state
    //furi_hal_gpio_init(&gpio_ext_pc1, GpioModeAnalog, GpioPullNo, GpioSpeedLow);

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    furi_mutex_free(chess_state->mutex);
    free(chess_state);

    return 0;
}