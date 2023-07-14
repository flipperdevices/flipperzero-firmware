#ifndef SMALLCHESSLIB_H
#define SMALLCHESSLIB_H

/**
  @file smallchesslib.h

  Small and simple single header C99 public domain chess library and engine.

  author: Miloslav Ciz (drummyfish)
  license: CC0 1.0 (public domain)
           found at https://creativecommons.org/publicdomain/zero/1.0/
           + additional waiver of all IP
  version: 0.8d

  Default notation format for this library is a coordinate one, i.e.
  
    squarefrom squareto [promotedpiece]

  e.g.: e2e4 or A2A1q

  This work's goal is to never be encumbered by any exclusive intellectual
  property rights. The work is therefore provided under CC0 1.0 + additional
  WAIVER OF ALL INTELLECTUAL PROPERTY RIGHTS that waives the rest of
  intellectual property rights not already waived by CC0 1.0. The WAIVER OF ALL
  INTELLECTUAL PROPERTY RGHTS is as follows:

  Each contributor to this work agrees that they waive any exclusive rights,
  including but not limited to copyright, patents, trademark, trade dress,
  industrial design, plant varieties and trade secrets, to any and all ideas,
  concepts, processes, discoveries, improvements and inventions conceived,
  discovered, made, designed, researched or developed by the contributor either
  solely or jointly with others, which relate to this work or result from this
  work. Should any waiver of such right be judged legally invalid or
  ineffective under applicable law, the contributor hereby grants to each
  affected person a royalty-free, non transferable, non sublicensable, non
  exclusive, irrevocable and unconditional license to this right.
*/

#include <stdint.h>

#ifndef SCL_DEBUG_AI
/** AI will print out a Newick-like tree of searched moves. */
#define SCL_DEBUG_AI 0
#endif

/**
  Maximum number of moves a chess piece can have (a queen in the middle of the
  board).
*/
#define SCL_CHESS_PIECE_MAX_MOVES 25
#define SCL_BOARD_SQUARES 64

typedef uint8_t (*SCL_RandomFunction)(void);

#if SCL_COUNT_EVALUATED_POSITIONS
uint32_t SCL_positionsEvaluated = 0; /**< If enabled by 
                                            SCL_COUNT_EVALUATED_POSITIONS, this
                                            will increment with every
                                            dynamically evaluated position (e.g.
                                            when AI computes its move). */
#endif

#ifndef SCL_CALL_WDT_RESET
#define SCL_CALL_WDT_RESET \
    0 /**< Option that should be enabled on some
                                    Arduinos. If 1, call to watchdog timer
                                    reset will be performed during dynamic
                                    evaluation (without it if AI takes long the
                                    program will reset). */
#endif

/**
  Returns a pseudorandom byte. This function has a period 256 and returns each
  possible byte value exactly once in the period.
*/
uint8_t SCL_randomSimple(void);
void SCL_randomSimpleSeed(uint8_t seed);

/**
  Like SCL_randomSimple, but internally uses a 16 bit value, so the period is
  65536.
*/
uint8_t SCL_randomBetter(void);
void SCL_randomBetterSeed(uint16_t seed);

#ifndef SCL_EVALUATION_FUNCTION
/**
    If defined, AI will always use the static evaluation function with this
    name. This helps avoid pointers to functions and can be faster but the
    function can't be changed at runtime.
  */
#define SCL_EVALUATION_FUNCTION
#undef SCL_EVALUATION_FUNCTION
#endif

#ifndef SCL_960_CASTLING
/**
    If set, chess 960 (Fisher random) castling will be considered by the library
    rather than normal castling. 960 castling is slightly different (e.g.
    requires the inital rook positions to be stored in board state). The
    castling move is performed as "capturing own rook".
  */
#define SCL_960_CASTLING 0
#endif

#ifndef SCL_ALPHA_BETA
/**
    Turns alpha-beta pruning (AI optimization) on or off. This can gain
    performance and should normally be turned on. AI behavior should not
    change at all.
  */
#define SCL_ALPHA_BETA 1
#endif

/**
  A set of game squares as a bit array, each bit representing one game square.
  Useful for representing e.g. possible moves. To easily iterate over the set
  use provided macros (SCL_SQUARE_SET_ITERATE, ...).
*/
typedef uint8_t SCL_SquareSet[8];

#define SCL_SQUARE_SET_EMPTY \
    { 0, 0, 0, 0, 0, 0, 0, 0 }

void SCL_squareSetClear(SCL_SquareSet squareSet);
void SCL_squareSetAdd(SCL_SquareSet squareSet, uint8_t square);
uint8_t SCL_squareSetContains(const SCL_SquareSet squareSet, uint8_t square);
uint8_t SCL_squareSetSize(const SCL_SquareSet squareSet);
uint8_t SCL_squareSetEmpty(const SCL_SquareSet squareSet);

/**
  Returns a random square from a square set.
*/
uint8_t SCL_squareSetGetRandom(const SCL_SquareSet squareSet, SCL_RandomFunction randFunc);

#define SCL_SQUARE_SET_ITERATE_BEGIN(squareSet)                  \
    {                                                            \
        uint8_t iteratedSquare = 0;                              \
        uint8_t iterationEnd = 0;                                \
        for(int8_t _i = 0; _i < 8 && !iterationEnd; ++_i) {      \
            uint8_t _row = squareSet[_i];                        \
            if(_row == 0) {                                      \
                iteratedSquare += 8;                             \
                continue;                                        \
            }                                                    \
                                                                 \
            for(uint8_t _j = 0; _j < 8 && !iterationEnd; ++_j) { \
                if(_row & 0x01) {
/*
  Between SCL_SQUARE_SET_ITERATE_BEGIN and _END iteratedSquare variable
  represents the next square contained in the set. To break out of the
  iteration set iterationEnd to 1.
*/

#define SCL_SQUARE_SET_ITERATE_END \
    }                              \
    _row >>= 1;                    \
    iteratedSquare++;              \
    }                              \
    } /*for*/                      \
    }

#define SCL_SQUARE_SET_ITERATE(squareSet, command) \
    SCL_SQUARE_SET_ITERATE_BEGIN(squareSet){command} SCL_SQUARE_SET_ITERATE_END

#define SCL_BOARD_STATE_SIZE 69

/**
  Represents chess board state as a string in this format:
  - First 64 characters represent the chess board (A1, B1, ... H8), each field
    can be either a piece (PRNBKQprnbkq) or empty ('.'). I.e. the state looks
    like this:

      0 (A1) RNBQKBNR 
             PPPPPPPP
             ........
             ........
             ........
             ........
             pppppppp
             rnbqkbnr 63 (H8)

  - After this more bytes follow to represent global state, these are:
    - 64: bits holding en-passant and castling related information:
      - bits 0-3 (lsb): Column of the pawn that can, in current turn, be
        taken by en-passant (0xF means no pawn can be taken this way).
      - bit 4: Whether white is not prevented from short castling by previous
        king or rook movement.
      - bit 5: Same as 4, but for long castling.
      - bit 6: Same as 4, but for black.
      - bit 7: Same as 4, but for black and long castling.
    - 65: Number saying the number of ply (half-moves) that have already been
      played, also determining whose turn it currently is.
    - 66: Move counter used in the 50 move rule, says the number of ply since
      the last pawn move or capture.
    - 67: Extra byte, left for storing additional info in variants. For normal
      chess this byte should always be 0.
    - 68: The last byte is always 0 to properly terminate the string in case
      someone tries to print it.
  - The state is designed so as to be simple and also print-friendly, i.e. you
    can simply print it with line break after 8 characters to get a human
    readable representation of the board.

  NOTE: there is a much more compact representation which however trades some
  access speed which would affect the AI performance and isn't print friendly,
  so we don't use it. In it each square takes 4 bits, using 15 out of 16
  possible values (empty square and W and B pieces including 2 types of pawns,
  one "en-passant takeable"). Then only one extra byte needed is for castling
  info (4 bits) and ply count (4 bits).
*/
typedef char SCL_Board[SCL_BOARD_STATE_SIZE];

#define SCL_BOARD_ENPASSANT_CASTLE_BYTE 64
#define SCL_BOARD_PLY_BYTE 65
#define SCL_BOARD_MOVE_COUNT_BYTE 66
#define SCL_BOARD_EXTRA_BYTE 67

#if SCL_960_CASTLING
#define _SCL_EXTRA_BYTE_VALUE (0 | (7 << 3)) // rooks on classic positions
#else
#define _SCL_EXTRA_BYTE_VALUE 0
#endif

#define SCL_BOARD_START_STATE                                                                   \
    {                                                                                           \
        82, 78, 66, 81, 75, 66, 78, 82, 80, 80, 80, 80, 80, 80, 80, 80, 46, 46, 46, 46, 46, 46, \
            46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, \
            46, 46, 46, 46, 46, 112, 112, 112, 112, 112, 112, 112, 112, 114, 110, 98, 113, 107, \
            98, 110, 114, (char)0xff, 0, 0, _SCL_EXTRA_BYTE_VALUE, 0                            \
    }

#define SCL_FEN_START "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define SCL_FEN_HORDE "ppp2ppp/pppppppp/pppppppp/pppppppp/3pp3/8/PPPPPPPP/RNBQKBNR w KQ - 0 1"

#define SCL_FEN_UPSIDE_DOWN "RNBKQBNR/PPPPPPPP/8/8/8/8/pppppppp/rnbkqbnr w - - 0 1"

#define SCL_FEN_PEASANT_REVOLT "1nn1k1n1/4p3/8/8/8/8/PPPPPPPP/4K3 w - - 0 1"

#define SCL_FEN_ENDGAME "4k3/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 0 1"

#define SCL_FEN_KNIGHTS "N6n/1N4n1/2N2n2/3Nn3/k2nN2K/2n2N2/1n4N1/n6N w - - 0 1"

/**
  Holds an info required to undo a single move.
*/
typedef struct {
    uint8_t squareFrom; ///< start square
    uint8_t squareTo; ///< target square
    char enPassantCastle; ///< previous en passant/castle byte
    char moveCount; ///< previous values of the move counter byte
    uint8_t other; /**< lowest 7 bits: previous value of target square,
                                highest bit: if 1 then the move was promotion or
                                en passant */
} SCL_MoveUndo;

#define SCL_GAME_STATE_PLAYING 0x00
#define SCL_GAME_STATE_WHITE_WIN 0x01
#define SCL_GAME_STATE_BLACK_WIN 0x02
#define SCL_GAME_STATE_DRAW 0x10 ///< further unspecified draw
#define SCL_GAME_STATE_DRAW_STALEMATE 0x11 ///< draw by stalemate
#define SCL_GAME_STATE_DRAW_REPETITION 0x12 ///< draw by repetition
#define SCL_GAME_STATE_DRAW_50 0x13 ///< draw by 50 move rule
#define SCL_GAME_STATE_DRAW_DEAD 0x14 ///< draw by dead position
#define SCL_GAME_STATE_END 0xff ///< end without known result

/**
  Converts square in common notation (e.g. 'c' 8) to square number. Only accepts
  lowercase column.
*/
#define SCL_SQUARE(colChar, rowInt) (((rowInt)-1) * 8 + ((colChar) - 'a'))
#define SCL_S(c, r) SCL_SQUARE(c, r)

void SCL_boardInit(SCL_Board board);
void SCL_boardCopy(const SCL_Board boardFrom, SCL_Board boardTo);

/**
  Initializes given chess 960 (Fisher random) position. If SCL_960_CASTLING
  is not set, castling will be disabled by this function.
*/
void SCL_boardInit960(SCL_Board board, uint16_t positionNumber);

void SCL_boardDisableCastling(SCL_Board board);

uint32_t SCL_boardHash32(const SCL_Board board);

#define SCL_PHASE_OPENING 0
#define SCL_PHASE_MIDGAME 1
#define SCL_PHASE_ENDGAME 2

/**
  Estimates the game phase: opening, midgame or endgame.
*/
uint8_t SCL_boardEstimatePhase(SCL_Board board);

/**
  Sets the board position. The input string should be 64 characters long zero
  terminated C string representing the board as squares A1, A2, ..., H8 with
  each char being either a piece (RKBKQPrkbkqp) or an empty square ('.').
*/
void SCL_boardSetPosition(
    SCL_Board board,
    const char* pieces,
    uint8_t castlingEnPassant,
    uint8_t moveCount,
    uint8_t ply);

uint8_t SCL_boardsDiffer(SCL_Board b1, SCL_Board b2);

/**
  Gets a random move on given board for the player whose move it is.
*/
void SCL_boardRandomMove(
    SCL_Board board,
    SCL_RandomFunction randFunc,
    uint8_t* squareFrom,
    uint8_t* squareTo,
    char* resultProm);

#define SCL_FEN_MAX_LENGTH 90

/**
  Converts a position to FEN (Forsyth–Edwards Notation) string. The string has
  to have at least SCL_FEN_MAX_LENGTH bytes allocated to guarantee the
  function won't write to unallocated memory. The string will be terminated by
  0 (this is included in SCL_FEN_MAX_LENGTH). The number of bytes written
  (including the terminating 0) is returned.
*/
uint8_t SCL_boardToFEN(SCL_Board board, char* string);

/**
  Loads a board from FEN (Forsyth–Edwards Notation) string. Returns 1 on
  success, 0 otherwise. XFEN isn't supported fully but a start position in
  chess960 can be loaded with this function. 
*/
uint8_t SCL_boardFromFEN(SCL_Board board, const char* string);

/**
  Returns an approximate/heuristic board rating as a number, 0 meaning equal
  chances for both players, positive favoring white, negative favoring black.
*/
typedef int16_t (*SCL_StaticEvaluationFunction)(SCL_Board);

/* 
  NOTE: int8_t as a return value was tried for evaluation function, which would
  be simpler, but it fails to capture important non-material position
  differences counted in fractions of pawn values, hence we have to use int16_t.
*/

/**
  Basic static evaluation function. WARNING: this function supposes a standard
  chess game, for non-standard positions it may either not work well or even
  crash the program. You should use a different function for non-standard games.
*/
int16_t SCL_boardEvaluateStatic(SCL_Board board);

/**
  Dynamic evaluation function (search), i.e. unlike SCL_boardEvaluateStatic,
  this one performs a recursive search for deeper positions to get a more
  accurate score. Of course, this is much slower and hugely dependent on
  baseDepth (you mostly want to keep this under 5).
*/
int16_t SCL_boardEvaluateDynamic(
    SCL_Board board,
    uint8_t baseDepth,
    uint8_t extensionExtraDepth,
    SCL_StaticEvaluationFunction evalFunction);

#define SCL_EVALUATION_MAX_SCORE 32600 // don't increase this, we need a margin

/**
  Checks if the board position is dead, i.e. mate is impossible (e.g. due to
  insufficient material), which by the rules results in a draw. WARNING: This
  function may fail to detect some dead positions as this is a non-trivial task.
*/
uint8_t SCL_boardDead(SCL_Board board);

/**
  Tests whether given player is in check.
*/
uint8_t SCL_boardCheck(SCL_Board board, uint8_t white);

/**
  Checks whether given move resets the move counter (used in the 50 move rule).
*/
uint8_t SCL_boardMoveResetsCount(SCL_Board board, uint8_t squareFrom, uint8_t squareTo);

uint8_t SCL_boardMate(SCL_Board board);

/**
  Performs a move on a board WITHOUT checking if the move is legal. Returns an
  info with which the move can be undone.
*/
SCL_MoveUndo
    SCL_boardMakeMove(SCL_Board board, uint8_t squareFrom, uint8_t squareTo, char promotePiece);

void SCL_boardUndoMove(SCL_Board board, SCL_MoveUndo moveUndo);

/**
  Checks if the game is over, i.e. the current player to move has no legal
  moves, the game is in dead position etc.
*/
uint8_t SCL_boardGameOver(SCL_Board board);

/**
  Checks if given move is legal.
*/
uint8_t SCL_boardMoveIsLegal(SCL_Board board, uint8_t squareFrom, uint8_t squareTo);

/**
  Checks if the player to move has at least one legal move.
*/
uint8_t SCL_boardMovePossible(SCL_Board board);

#define SCL_POSITION_NORMAL 0x00
#define SCL_POSITION_CHECK 0x01
#define SCL_POSITION_MATE 0x02
#define SCL_POSITION_STALEMATE 0x03
#define SCL_POSITION_DEAD 0x04

uint8_t SCL_boardGetPosition(SCL_Board board);

/**
  Returns 1 if the square is attacked by player of given color. This is used to
  examine checks, so for performance reasons the functions only checks whether
  or not the square is attacked (not the number of attackers).
*/
uint8_t SCL_boardSquareAttacked(SCL_Board board, uint8_t square, uint8_t byWhite);

/**
  Gets pseudo moves of a piece: all possible moves WITHOUT eliminating moves
  that lead to own check. To get only legal moves use SCL_boardGetMoves.
*/
void SCL_boardGetPseudoMoves(
    SCL_Board board,
    uint8_t pieceSquare,
    uint8_t checkCastling,
    SCL_SquareSet result);

/**
  Gets all legal moves of given piece.
*/
void SCL_boardGetMoves(SCL_Board board, uint8_t pieceSquare, SCL_SquareSet result);

static inline uint8_t SCL_boardWhitesTurn(SCL_Board board);

static inline uint8_t SCL_pieceIsWhite(char piece);
static inline uint8_t SCL_squareIsWhite(uint8_t square);
char SCL_pieceToColor(uint8_t piece, uint8_t toWhite);

/**
  Converts square coordinates to square number. Each coordinate must be a number
  <1,8>. Validity of the coordinates is NOT checked.
*/
static inline uint8_t SCL_coordsToSquare(uint8_t row, uint8_t column);

#ifndef SCL_VALUE_PAWN
#define SCL_VALUE_PAWN 256
#endif

#ifndef SCL_VALUE_KNIGHT
#define SCL_VALUE_KNIGHT 768
#endif

#ifndef SCL_VALUE_BISHOP
#define SCL_VALUE_BISHOP 800
#endif

#ifndef SCL_VALUE_ROOK
#define SCL_VALUE_ROOK 1280
#endif

#ifndef SCL_VALUE_QUEEN
#define SCL_VALUE_QUEEN 2304
#endif

#ifndef SCL_VALUE_KING
#define SCL_VALUE_KING 0
#endif

#define SCL_ENDGAME_MATERIAL_LIMIT \
    (2 *                           \
     (SCL_VALUE_PAWN * 4 + SCL_VALUE_QUEEN + SCL_VALUE_KING + SCL_VALUE_ROOK + SCL_VALUE_KNIGHT))

#define SCL_START_MATERIAL                                                                    \
    (16 * SCL_VALUE_PAWN + 4 * SCL_VALUE_ROOK + 4 * SCL_VALUE_KNIGHT + 4 * SCL_VALUE_BISHOP + \
     2 * SCL_VALUE_QUEEN + 2 * SCL_VALUE_KING)

#ifndef SCL_RECORD_MAX_LENGTH
#define SCL_RECORD_MAX_LENGTH 256
#endif

#define SCL_RECORD_MAX_SIZE (SCL_RECORD_MAX_LENGTH * 2)

/**
  Records a single chess game. The format is following:

  Each record item consists of 2 bytes which record a single move (ply):

  abxxxxxx cdyyyyyy

    xxxxxx  Start square of the move, counted as A0, A1, ...
    yyyyyy  End square of the move in the same format as the start square.
    ab      00 means this move isn't the last move of the game, other possible
            values are 01: white wins, 10: black wins, 11: draw or end for
            other reasons.
    cd      In case of pawn promotion move this encodes the promoted piece as
            00: queen, 01: rook, 10: bishop, 11: knight (pawn isn't allowed by
            chess rules).

  Every record should be ended by an ending move (ab != 00), empty record should
  have one move where xxxxxx == yyyyyy == 0 and ab == 11.
*/
typedef uint8_t SCL_Record[SCL_RECORD_MAX_SIZE];

#define SCL_RECORD_CONT 0x00
#define SCL_RECORD_W_WIN 0x40
#define SCL_RECORD_B_WIN 0x80
#define SCL_RECORD_END 0xc0

#define SCL_RECORD_PROM_Q 0x00
#define SCL_RECORD_PROM_R 0x40
#define SCL_RECORD_PROM_B 0x80
#define SCL_RECORD_PROM_N 0xc0

#define SCL_RECORD_ITEM(s0, s1, p, e) ((e) | (s0)), ((p) | (s1))

void SCL_recordInit(SCL_Record r);

void SCL_recordCopy(SCL_Record recordFrom, SCL_Record recordTo);

/**
  Represents a complete game of chess (or a variant with different staring
  position). This struct along with associated functions allows to easily
  implement a chess game that allows undoing moves, detecting draws, recording
  the moves etc. On platforms with extremely little RAM one can reduce
  SCL_RECORD_MAX_LENGTH to reduce the size of this struct (which will however
  possibly limit how many moves can be undone).
*/
typedef struct {
    SCL_Board board;
    SCL_Record record; /**< Holds the game record. This record is here 
                              firstly because games are usually recorded and
                              secondly this allows undoing moves up to the 
                              beginning of the game. This infinite undoing will
                              only work as long as the record is able to hold
                              the whole game; if the record is full, undoing is
                              no longet possible. */
    uint16_t state;
    uint16_t ply; ///< ply count (board ply counter is only 8 bit)

    uint32_t prevMoves[14]; ///< stores last moves, for repetition detection

    const char* startState; /**< Optional pointer to the starting board state.
                              If this is null, standard chess start position is
                              assumed. This is needed for undoing moves with
                              game record. */
} SCL_Game;

/**
  Initializes a new chess game. The startState parameter is optional and allows
  for setting up chess variants that differ by starting positions, setting this
  to 0 will assume traditional starting position. WARNING: if startState is
  provided, the pointed to board mustn't be deallocated afterwards, the string
  is not internally copied (for memory saving reasons).
*/
void SCL_gameInit(SCL_Game* game, const SCL_Board startState);

void SCL_gameMakeMove(SCL_Game* game, uint8_t squareFrom, uint8_t squareTo, char promoteTo);

uint8_t SCL_gameUndoMove(SCL_Game* game);

/**
  Gets a move which if played now would cause a draw by repetition. Returns 1
  if such move exists, 0 otherwise. The results parameters can be set to 0 in
  which case they will be ignored and only the existence of a draw move will be
  tested.
*/
uint8_t SCL_gameGetRepetiotionMove(SCL_Game* game, uint8_t* squareFrom, uint8_t* squareTo);

/**
  Leads a game record from PGN string. The function will probably not strictly
  adhere to the PGN input format, but should accept most sanely written PGN
  strings.
*/
void SCL_recordFromPGN(SCL_Record r, const char* pgn);

uint16_t SCL_recordLength(const SCL_Record r);

/**
  Gets the move out of a game record, returns the end state of the move
  (SCL_RECORD_CONT, SCL_RECORD_END etc.)
*/
uint8_t SCL_recordGetMove(
    const SCL_Record r,
    uint16_t index,
    uint8_t* squareFrom,
    uint8_t* squareTo,
    char* promotedPiece);

/**
  Adds another move to the game record. Terminating the record is handled so
  that the last move is always marked with end flag, endState is here to only
  indicate possible game result (otherwise pass SCL_RECORD_CONT). Returns 1 if
  the item was added, otherwise 0 (replay was already of maximum size).
*/
uint8_t SCL_recordAdd(
    SCL_Record r,
    uint8_t squareFrom,
    uint8_t squareTo,
    char promotePiece,
    uint8_t endState);

/**
  Removes the last move from the record, returns 1 if the replay is non-empty
  after the removal, otherwise 0.
*/
uint8_t SCL_recordRemoveLast(SCL_Record r);

/**
  Applies given number of half-moves (ply) to a given board (the board is
  automatically initialized at the beginning).
*/
void SCL_recordApply(const SCL_Record r, SCL_Board b, uint16_t moves);

int16_t SCL_pieceValue(char piece);
int16_t SCL_pieceValuePositive(char piece);

#define SCL_PRINT_FORMAT_NONE 0
#define SCL_PRINT_FORMAT_NORMAL 1
#define SCL_PRINT_FORMAT_COMPACT 2
#define SCL_PRINT_FORMAT_UTF8 3
#define SCL_PRINT_FORMAT_COMPACT_UTF8 4

/**
  Gets the best move for the currently moving player as computed by AI. The
  return value is the value of the move (with the same semantics as the value
  of an evaluation function). baseDepth is depth in plys to which all moves will
  be checked. If baseDepth 0 is passed, the function makes a random move and
  returns the evaluation of the board. extensionExtraDepth is extra depth for
  checking specific situations like exchanges and checks. endgameExtraDepth is
  extra depth which is added to baseDepth in the endgame. If the randomness
  function is 0, AI will always make the first best move it finds, if it is
  not 0 and randomness is 0, AI will randomly pick between the equally best
  moves, if it is not 0 and randomness is positive, AI will randomly choose
  between best moves with some bias (may not pick the best rated move).
*/
int16_t SCL_getAIMove(
    SCL_Board board,
    uint8_t baseDepth,
    uint8_t extensionExtraDepth,
    uint8_t endgameExtraDepth,
    SCL_StaticEvaluationFunction evalFunc,
    SCL_RandomFunction randFunc,
    uint8_t randomness,
    uint8_t repetitionMoveFrom,
    uint8_t repetitionMoveTo,
    uint8_t* resultFrom,
    uint8_t* resultTo,
    char* resultProm);

/**
  Function that prints out a single character. This is passed to printing
  functions.
*/
typedef void (*SCL_PutCharFunction)(char);

/**
  Prints given chessboard using given format and an abstract printing function.
*/
void SCL_printBoard(
    SCL_Board board,
    SCL_PutCharFunction putCharFunc,
    SCL_SquareSet highlightSquares,
    uint8_t selectSquare,
    uint8_t format,
    uint8_t offset,
    uint8_t labels,
    uint8_t blackDown);

void SCL_printBoardSimple(
    SCL_Board board,
    SCL_PutCharFunction putCharFunc,
    uint8_t selectSquare,
    uint8_t format);

void SCL_printSquareUTF8(uint8_t square, SCL_PutCharFunction putCharFunc);
void SCL_printPGN(SCL_Record r, SCL_PutCharFunction putCharFunc, SCL_Board initialState);

/**
  Reads a move from string (the notation format is described at the top of this
  file). The function is safe as long as the string is 0 terminated. Returns 1
  on success or 0 on fail (invalid move string).
*/
uint8_t SCL_stringToMove(
    const char* moveString,
    uint8_t* resultFrom,
    uint8_t* resultTo,
    char* resultPromotion);

char* SCL_moveToString(SCL_Board board, uint8_t s0, uint8_t s1, char promotion, char* string);

/**
  Function used in drawing, it is called to draw the next pixel. The first
  parameter is the pixel color, the second one if the sequential number of the
  pixel.
*/
typedef void (*SCL_PutPixelFunction)(uint8_t, uint16_t);

#define SCL_BOARD_PICTURE_WIDTH 64

/**
  Draws a simple 1bit 64x64 pixels board using a provided abstract function for
  drawing pixels. The function renders from top left to bottom right, i.e. no
  frame buffer is required.
*/
void SCL_drawBoard(
    SCL_Board board,
    SCL_PutPixelFunction putPixel,
    uint8_t selectedSquare,
    SCL_SquareSet highlightSquares,
    uint8_t blackDown);

/**
  Converts square number to string representation (e.g. "d2"). This function
  will modify exactly the first two bytes of the provided string.
*/
static inline char* SCL_squareToString(uint8_t square, char* string);

/**
  Converts a string, such as "A1" or "b4", to square number. The string must
  start with a letter (lower or upper case) and be followed by a number <1,8>.
  Validity of the string is NOT checked.
*/
uint8_t SCL_stringToSquare(const char* square);

//=============================================================================
// privates:

#define SCL_UNUSED(v) (void)(v)

uint8_t SCL_currentRandom8 = 0;

uint16_t SCL_currentRandom16 = 0;

void SCL_randomSimpleSeed(uint8_t seed) {
    SCL_currentRandom8 = seed;
}

uint8_t SCL_randomSimple(void) {
    SCL_currentRandom8 *= 13;
    SCL_currentRandom8 += 7;
    return SCL_currentRandom8;
}

uint8_t SCL_randomBetter(void) {
    SCL_currentRandom16 *= 13;
    SCL_currentRandom16 += 7;
    return (SCL_currentRandom16 % 256) ^ (SCL_currentRandom16 / 256);
}

void SCL_randomBetterSeed(uint16_t seed) {
    SCL_currentRandom16 = seed;
}

void SCL_squareSetClear(SCL_SquareSet squareSet) {
    for(uint8_t i = 0; i < 8; ++i) squareSet[i] = 0;
}

uint8_t SCL_stringToSquare(const char* square) {
    return (square[1] - '1') * 8 +
           (square[0] - ((square[0] >= 'A' && square[0] <= 'Z') ? 'A' : 'a'));
}

char* SCL_moveToString(SCL_Board board, uint8_t s0, uint8_t s1, char promotion, char* string) {
    char* result = string;

    SCL_squareToString(s0, string);
    string += 2;
    string = SCL_squareToString(s1, string);
    string += 2;

    char c = board[s0];

    if(c == 'p' || c == 'P') {
        uint8_t rank = s1 / 8;

        if(rank == 0 || rank == 7) {
            *string = promotion;
            string++;
        }
    }

    *string = 0;

    return result;
}

uint8_t SCL_boardWhitesTurn(SCL_Board board) {
    return (board[SCL_BOARD_PLY_BYTE] % 2) == 0;
}

uint8_t SCL_coordsToSquare(uint8_t row, uint8_t column) {
    return row * 8 + column;
}

uint8_t SCL_pieceIsWhite(char piece) {
    return piece < 'a';
}

char* SCL_squareToString(uint8_t square, char* string) {
    string[0] = 'a' + square % 8;
    string[1] = '1' + square / 8;

    return string;
}

uint8_t SCL_squareIsWhite(uint8_t square) {
    return (square % 2) != ((square / 8) % 2);
}

char SCL_pieceToColor(uint8_t piece, uint8_t toWhite) {
    return (SCL_pieceIsWhite(piece) == toWhite) ? piece : (piece + (toWhite ? -32 : 32));
}

/**
  Records the rook starting positions in the board state. This is required in
  chess 960 in order to be able to correctly perform castling (castling rights
  knowledge isn't enough as one rook might have moved to the other side and we
  wouldn't know which one can castle and which not).
*/
void _SCL_board960RememberRookPositions(SCL_Board board) {
    uint8_t pos = 0;
    uint8_t rooks = 2;

    while(pos < 8 && rooks != 0) {
        if(board[pos] == 'R') {
            board[SCL_BOARD_EXTRA_BYTE] = rooks == 2 ? pos :
                                                       (board[SCL_BOARD_EXTRA_BYTE] | (pos << 3));

            rooks--;
        }

        pos++;
    }
}

void SCL_boardInit(SCL_Board board) {
    /*
    We might use SCL_BOARD_START_STATE and copy it to the board, but that might
    waste RAM on Arduino, so we init the board by code.
  */

    char* b = board;

    *b = 'R';
    b++;
    *b = 'N';
    b++;
    *b = 'B';
    b++;
    *b = 'Q';
    b++;
    *b = 'K';
    b++;
    *b = 'B';
    b++;
    *b = 'N';
    b++;
    *b = 'R';
    b++;

    char* b2 = board + 48;

    for(uint8_t i = 0; i < 8; ++i, b++, b2++) {
        *b = 'P';
        *b2 = 'p';
    }

    for(uint8_t i = 0; i < 32; ++i, b++) *b = '.';

    b += 8;

    *b = 'r';
    b++;
    *b = 'n';
    b++;
    *b = 'b';
    b++;
    *b = 'q';
    b++;
    *b = 'k';
    b++;
    *b = 'b';
    b++;
    *b = 'n';
    b++;
    *b = 'r';
    b++;

    for(uint8_t i = 0; i < SCL_BOARD_STATE_SIZE - SCL_BOARD_SQUARES; ++i, ++b) *b = 0;

    board[SCL_BOARD_ENPASSANT_CASTLE_BYTE] = (char)0xff;

#if SCL_960_CASTLING
    _SCL_board960RememberRookPositions(board);
#endif
}

void _SCL_boardPlaceOnNthAvailable(SCL_Board board, uint8_t pos, char piece) {
    char* c = board;

    while(1) {
        if(*c == '.') {
            if(pos == 0) break;

            pos--;
        }

        c++;
    }

    *c = piece;
}

void SCL_boardInit960(SCL_Board board, uint16_t positionNumber) {
    SCL_Board b;

    SCL_boardInit(b);

    for(uint8_t i = 0; i < SCL_BOARD_STATE_SIZE; ++i)
        board[i] = ((i >= 8 && i < 56) || i >= 64) ? b[i] : '.';

    uint8_t helper = positionNumber % 16;

    board[(helper / 4) * 2] = 'B';
    board[1 + (helper % 4) * 2] = 'B';

    helper = positionNumber / 16;

    // maybe there's a simpler way :)

    _SCL_boardPlaceOnNthAvailable(board, helper % 6, 'Q');
    _SCL_boardPlaceOnNthAvailable(board, 0, helper <= 23 ? 'N' : 'R');

    _SCL_boardPlaceOnNthAvailable(
        board, 0, (helper >= 7 && helper <= 23) ? 'R' : (helper > 41 ? 'K' : 'N'));

    _SCL_boardPlaceOnNthAvailable(
        board,
        0,
        (helper <= 5 || helper >= 54) ?
            'R' :
            (((helper >= 12 && helper <= 23) || (helper >= 30 && helper <= 41)) ? 'K' : 'N'));

    _SCL_boardPlaceOnNthAvailable(
        board,
        0,
        (helper <= 11 || (helper <= 29 && helper >= 24)) ?
            'K' :
            (((helper >= 18 && helper <= 23) || (helper >= 36 && helper <= 41) ||
              (helper >= 48 && helper <= 53)) ?
                 'R' :
                 'N'));

    uint8_t rooks = 0;

    for(uint8_t i = 0; i < 8; ++i)
        if(board[i] == 'R') rooks++;

    _SCL_boardPlaceOnNthAvailable(board, 0, rooks == 2 ? 'N' : 'R');

    for(uint8_t i = 0; i < 8; ++i) board[56 + i] = SCL_pieceToColor(board[i], 0);

#if SCL_960_CASTLING
    _SCL_board960RememberRookPositions(board);
#else
    SCL_boardDisableCastling(board);
#endif
}

uint8_t SCL_boardsDiffer(SCL_Board b1, SCL_Board b2) {
    const char *p1 = b1, *p2 = b2;

    while(p1 < b1 + SCL_BOARD_STATE_SIZE) {
        if(*p1 != *p2) return 1;

        p1++;
        p2++;
    }

    return 0;
}

void SCL_recordInit(SCL_Record r) {
    r[0] = 0 | SCL_RECORD_END;
    r[1] = 0;
}

void SCL_recordFromPGN(SCL_Record r, const char* pgn) {
    SCL_Board board;

    SCL_boardInit(board);

    SCL_recordInit(r);

    uint8_t state = 0;
    uint8_t evenMove = 0;

    while(*pgn != 0) {
        switch(state) {
        case 0: // skipping tags and spaces, outside []
            if(*pgn == '1')
                state = 2;
            else if(*pgn == '[')
                state = 1;

            break;

        case 1: // skipping tags and spaces, inside []
            if(*pgn == ']') state = 0;

            break;

        case 2: // reading move number
            if(*pgn == '{')
                state = 3;
            else if((*pgn >= 'a' && *pgn <= 'h') || (*pgn >= 'A' && *pgn <= 'Z')) {
                state = 4;
                pgn--;
            }

            break;

        case 3: // initial comment
            if(*pgn == '}') state = 2;

            break;

        case 4: // reading move
        {
            char piece = 'p';
            char promoteTo = 'q';
            uint8_t castle = 0;
            uint8_t promotion = 0;

            int8_t coords[4];

            uint8_t ranks = 0, files = 0;

            for(uint8_t i = 0; i < 4; ++i) coords[i] = -1;

            while(*pgn != ' ' && *pgn != '\n' && *pgn != '\t' && *pgn != '{' && *pgn != 0) {
                if(*pgn == '=') promotion = 1;
                if(*pgn == 'O' || *pgn == '0') castle++;
                if(*pgn >= 'A' && *pgn <= 'Z') {
                    if(promotion)
                        promoteTo = *pgn;
                    else
                        piece = *pgn;
                } else if(*pgn >= 'a' && *pgn <= 'h') {
                    coords[files * 2] = *pgn - 'a';
                    files++;
                } else if(*pgn >= '1' && *pgn <= '8') {
                    coords[1 + ranks * 2] = *pgn - '1';
                    ranks++;
                }

                pgn++;
            }

            if(castle) {
                piece = 'K';

                coords[0] = 4;
                coords[1] = 0;
                coords[2] = castle < 3 ? 6 : 2;
                coords[3] = 0;

                if(evenMove) {
                    coords[1] = 7;
                    coords[3] = 7;
                }
            }

            piece = SCL_pieceToColor(piece, evenMove == 0);

            if(coords[2] < 0) {
                coords[2] = coords[0];
                coords[0] = -1;
            }

            if(coords[3] < 0) {
                coords[3] = coords[1];
                coords[1] = -1;
            }

            uint8_t squareTo = coords[3] * 8 + coords[2];

            if(coords[0] < 0 || coords[1] < 0) {
                // without complete starting coords we have to find the piece

                for(int i = 0; i < SCL_BOARD_SQUARES; ++i)
                    if(board[i] == piece) {
                        SCL_SquareSet s;

                        SCL_squareSetClear(s);

                        SCL_boardGetMoves(board, i, s);

                        if(SCL_squareSetContains(s, squareTo) &&
                           (coords[0] < 0 || coords[0] == i % 8) &&
                           (coords[1] < 0 || coords[1] == i / 8)) {
                            coords[0] = i % 8;
                            coords[1] = i / 8;
                            break;
                        }
                    }
            }

            uint8_t squareFrom = coords[1] * 8 + coords[0];

            SCL_boardMakeMove(board, squareFrom, squareTo, promoteTo);

            // for some reason tcc bugs here, the above line sets squareFrom to 0 lol
            // can be fixed with doing "squareFrom = coords[1] * 8 + coords[0];" again

            SCL_recordAdd(r, squareFrom, squareTo, promoteTo, SCL_RECORD_CONT);

            while(*pgn == ' ' || *pgn == '\n' || *pgn == '\t' || *pgn == '{') {
                if(*pgn == '{')
                    while(*pgn != '}') pgn++;

                pgn++;
            }

            if(*pgn == 0) return;

            pgn--;

            if(evenMove) state = 2;

            evenMove = !evenMove;

            break;
        }

        default:
            break;
        }

        pgn++;
    }
}

uint16_t SCL_recordLength(const SCL_Record r) {
    if((r[0] & 0x3f) == (r[1] & 0x3f)) // empty record that's only terminator
        return 0;

    uint16_t result = 0;

    while((r[result] & 0xc0) == 0) result += 2;

    return (result / 2) + 1;
}

uint8_t SCL_recordGetMove(
    const SCL_Record r,
    uint16_t index,
    uint8_t* squareFrom,
    uint8_t* squareTo,
    char* promotedPiece) {
    index *= 2;

    uint8_t b = r[index];

    *squareFrom = b & 0x3f;
    uint8_t result = b & 0xc0;

    index++;

    b = r[index];

    *squareTo = b & 0x3f;

    b &= 0xc0;

    switch(b) {
    case SCL_RECORD_PROM_Q:
        *promotedPiece = 'q';
        break;
    case SCL_RECORD_PROM_R:
        *promotedPiece = 'r';
        break;
    case SCL_RECORD_PROM_B:
        *promotedPiece = 'b';
        break;
    case SCL_RECORD_PROM_N:
    default:
        *promotedPiece = 'n';
        break;
    }

    return result;
}

uint8_t SCL_recordAdd(
    SCL_Record r,
    uint8_t squareFrom,
    uint8_t squareTo,
    char promotePiece,
    uint8_t endState) {
    uint16_t l = SCL_recordLength(r);

    if(l >= SCL_RECORD_MAX_LENGTH) return 0;

    l *= 2;

    if(l != 0) r[l - 2] &= 0x3f; // remove the end flag from previous item

    if(endState == SCL_RECORD_CONT) endState = SCL_RECORD_END;

    r[l] = squareFrom | endState;

    uint8_t p;

    switch(promotePiece) {
    case 'n':
    case 'N':
        p = SCL_RECORD_PROM_N;
        break;
    case 'b':
    case 'B':
        p = SCL_RECORD_PROM_B;
        break;
    case 'r':
    case 'R':
        p = SCL_RECORD_PROM_R;
        break;
    case 'q':
    case 'Q':
    default:
        p = SCL_RECORD_PROM_Q;
        break;
    }

    l++;

    r[l] = squareTo | p;

    return 1;
}

uint8_t SCL_recordRemoveLast(SCL_Record r) {
    uint16_t l = SCL_recordLength(r);

    if(l == 0) return 0;

    if(l == 1)
        SCL_recordInit(r);
    else {
        l = (l - 2) * 2;

        r[l] = (r[l] & 0x3f) | SCL_RECORD_END;
    }

    return 1;
}

void SCL_recordApply(const SCL_Record r, SCL_Board b, uint16_t moves) {
    SCL_boardInit(b);

    uint16_t l = SCL_recordLength(r);

    if(moves > l) moves = l;

    for(uint16_t i = 0; i < moves; ++i) {
        uint8_t s0, s1;
        char p;

        SCL_recordGetMove(r, i, &s0, &s1, &p);
        SCL_boardMakeMove(b, s0, s1, p);
    }
}

void SCL_boardUndoMove(SCL_Board board, SCL_MoveUndo moveUndo) {
#if SCL_960_CASTLING
    char squareToNow = board[moveUndo.squareTo];
#endif

    board[moveUndo.squareFrom] = board[moveUndo.squareTo];
    board[moveUndo.squareTo] = moveUndo.other & 0x7f;
    board[SCL_BOARD_PLY_BYTE]--;
    board[SCL_BOARD_ENPASSANT_CASTLE_BYTE] = moveUndo.enPassantCastle;
    board[SCL_BOARD_MOVE_COUNT_BYTE] = moveUndo.moveCount;

    if(moveUndo.other & 0x80) {
        moveUndo.squareTo /= 8;

        if(moveUndo.squareTo == 0 || moveUndo.squareTo == 7)
            board[moveUndo.squareFrom] = SCL_pieceIsWhite(board[moveUndo.squareFrom]) ? 'P' : 'p';
        // ^ was promotion
        else
            board[(moveUndo.squareFrom / 8) * 8 + (moveUndo.enPassantCastle & 0x0f)] =
                (board[moveUndo.squareFrom] == 'P') ? 'p' : 'P'; // was en passant
    }
#if !SCL_960_CASTLING
    else if(
        board[moveUndo.squareFrom] == 'k' && // black castling
        moveUndo.squareFrom == 60) {
        if(moveUndo.squareTo == 58) {
            board[59] = '.';
            board[56] = 'r';
        } else if(moveUndo.squareTo == 62) {
            board[61] = '.';
            board[63] = 'r';
        }
    } else if(
        board[moveUndo.squareFrom] == 'K' && // white castling
        moveUndo.squareFrom == 4) {
        if(moveUndo.squareTo == 2) {
            board[3] = '.';
            board[0] = 'R';
        } else if(moveUndo.squareTo == 6) {
            board[5] = '.';
            board[7] = 'R';
        }
    }
#else // 960 castling
    else if(
        ((moveUndo.other & 0x7f) == 'r') && // black castling
        (squareToNow == '.' || !SCL_pieceIsWhite(squareToNow))) {
        board[moveUndo.squareTo < moveUndo.squareFrom ? 59 : 61] = '.';
        board[moveUndo.squareTo < moveUndo.squareFrom ? 58 : 62] = '.';

        board[moveUndo.squareFrom] = 'k';
        board[moveUndo.squareTo] = 'r';
    } else if(
        ((moveUndo.other & 0x7f) == 'R') && // white castling
        (squareToNow == '.' || SCL_pieceIsWhite(squareToNow))) {
        board[moveUndo.squareTo < moveUndo.squareFrom ? 3 : 5] = '.';
        board[moveUndo.squareTo < moveUndo.squareFrom ? 2 : 6] = '.';

        board[moveUndo.squareFrom] = 'K';
        board[moveUndo.squareTo] = 'R';
    }
#endif
}

/**
  Potentially disables castling rights according to whether something moved from
  or to a square with a rook.
*/
void _SCL_handleRookActivity(SCL_Board board, uint8_t rookSquare) {
#if !SCL_960_CASTLING
    switch(rookSquare) {
    case 0:
        board[SCL_BOARD_ENPASSANT_CASTLE_BYTE] &= (uint8_t)~0x20;
        break;
    case 7:
        board[SCL_BOARD_ENPASSANT_CASTLE_BYTE] &= (uint8_t)~0x10;
        break;
    case 56:
        board[SCL_BOARD_ENPASSANT_CASTLE_BYTE] &= (uint8_t)~0x80;
        break;
    case 63:
        board[SCL_BOARD_ENPASSANT_CASTLE_BYTE] &= (uint8_t)~0x40;
        break;
    default:
        break;
    }
#else // 960 castling
    if(rookSquare == (board[SCL_BOARD_EXTRA_BYTE] & 0x07))
        board[SCL_BOARD_ENPASSANT_CASTLE_BYTE] &= (uint8_t)~0x20;
    else if(rookSquare == (board[SCL_BOARD_EXTRA_BYTE] >> 3))
        board[SCL_BOARD_ENPASSANT_CASTLE_BYTE] &= (uint8_t)~0x10;
    else if(rookSquare == 56 + (board[SCL_BOARD_EXTRA_BYTE] & 0x07))
        board[SCL_BOARD_ENPASSANT_CASTLE_BYTE] &= (uint8_t)~0x80;
    else if(rookSquare == 56 + (board[SCL_BOARD_EXTRA_BYTE] >> 3))
        board[SCL_BOARD_ENPASSANT_CASTLE_BYTE] &= (uint8_t)~0x40;
#endif
}

SCL_MoveUndo
    SCL_boardMakeMove(SCL_Board board, uint8_t squareFrom, uint8_t squareTo, char promotePiece) {
    char s = board[squareFrom];

    SCL_MoveUndo moveUndo;

    moveUndo.squareFrom = squareFrom;
    moveUndo.squareTo = squareTo;
    moveUndo.moveCount = board[SCL_BOARD_MOVE_COUNT_BYTE];
    moveUndo.enPassantCastle = board[SCL_BOARD_ENPASSANT_CASTLE_BYTE];
    moveUndo.other = board[squareTo];

    // reset the en-passant state
    board[SCL_BOARD_ENPASSANT_CASTLE_BYTE] |= 0x0f;

    if(SCL_boardMoveResetsCount(board, squareFrom, squareTo))
        board[SCL_BOARD_MOVE_COUNT_BYTE] = 0;
    else
        board[SCL_BOARD_MOVE_COUNT_BYTE]++;

#if SCL_960_CASTLING
    uint8_t castled = 0;
#endif

    if((s == 'k') || (s == 'K')) {
#if !SCL_960_CASTLING
        if((squareFrom == 4) || (squareFrom == 60)) // check castling
        {
            int8_t difference = squareTo - squareFrom;

            char rook = SCL_pieceToColor('r', SCL_pieceIsWhite(s));

            if(difference == 2) // short
            {
                board[squareTo - 1] = rook;
                board[squareTo + 1] = '.';
            } else if(difference == -2) // long
            {
                board[squareTo - 2] = '.';
                board[squareTo + 1] = rook;
            }
        }
#else // 960 castling
        uint8_t isWhite = SCL_pieceIsWhite(s);
        char rook = SCL_pieceToColor('r', isWhite);

        if(board[squareTo] == rook) {
            castled = 1;

            board[squareFrom] = '.';
            board[squareTo] = '.';

            if(squareTo > squareFrom) // short
            {
                board[isWhite ? 6 : (56 + 6)] = s;
                board[isWhite ? 5 : (56 + 5)] = rook;
            } else // long
            {
                board[isWhite ? 2 : (56 + 2)] = s;
                board[isWhite ? 3 : (56 + 3)] = rook;
            }
        }
#endif

        // after king move disable castling
        board[SCL_BOARD_ENPASSANT_CASTLE_BYTE] &= ~(0x03 << ((s == 'K') ? 4 : 6));
    } else if((s == 'p') || (s == 'P')) {
        uint8_t row = squareTo / 8;

        int8_t rowDiff = squareFrom / 8 - row;

        if(rowDiff == 2 || rowDiff == -2) // record en passant column
        {
            board[SCL_BOARD_ENPASSANT_CASTLE_BYTE] =
                (board[SCL_BOARD_ENPASSANT_CASTLE_BYTE] & 0xf0) | (squareFrom % 8);
        }

        if(row == 0 || row == 7) {
            // promotion
            s = SCL_pieceToColor(promotePiece, SCL_pieceIsWhite(s));

            moveUndo.other |= 0x80;
        } else {
            // check en passant move

            int8_t columnDiff = (squareTo % 8) - (squareFrom % 8);

            if((columnDiff != 0) && (board[squareTo] == '.')) {
                board[squareFrom + columnDiff] = '.';
                moveUndo.other |= 0x80;
            }
        }
    } else if((s == 'r') || (s == 'R'))
        _SCL_handleRookActivity(board, squareFrom);

    char taken = board[squareTo];

    // taking a rook may also disable castling:

    if(taken == 'R' || taken == 'r') _SCL_handleRookActivity(board, squareTo);

#if SCL_960_CASTLING
    if(!castled)
#endif
    {
        board[squareTo] = s;
        board[squareFrom] = '.';
    }

    board[SCL_BOARD_PLY_BYTE]++; // increase ply count

    return moveUndo;
}

void SCL_boardSetPosition(
    SCL_Board board,
    const char* pieces,
    uint8_t castlingEnPassant,
    uint8_t moveCount,
    uint8_t ply) {
    for(uint8_t i = 0; i < SCL_BOARD_SQUARES; ++i, pieces++)
        if(*pieces != 0)
            board[i] = *pieces;
        else
            break;

    board[SCL_BOARD_ENPASSANT_CASTLE_BYTE] = castlingEnPassant;
    board[SCL_BOARD_PLY_BYTE] = ply;
    board[SCL_BOARD_MOVE_COUNT_BYTE] = moveCount;
    board[SCL_BOARD_STATE_SIZE - 1] = 0;
}

void SCL_squareSetAdd(SCL_SquareSet squareSet, uint8_t square) {
    squareSet[square / 8] |= 0x01 << (square % 8);
}

uint8_t SCL_squareSetContains(const SCL_SquareSet squareSet, uint8_t square) {
    return squareSet[square / 8] & (0x01 << (square % 8));
}

uint8_t SCL_squareSetSize(const SCL_SquareSet squareSet) {
    uint8_t result = 0;

    for(uint8_t i = 0; i < 8; ++i) {
        uint8_t byte = squareSet[i];

        for(uint8_t j = 0; j < 8; ++j) {
            result += byte & 0x01;
            byte >>= 1;
        }
    }

    return result;
}

uint8_t SCL_squareSetEmpty(const SCL_SquareSet squareSet) {
    for(uint8_t i = 0; i < 8; ++i)
        if(squareSet[i] != 0) return 0;

    return 1;
}

uint8_t SCL_squareSetGetRandom(const SCL_SquareSet squareSet, SCL_RandomFunction randFunc) {
    uint8_t size = SCL_squareSetSize(squareSet);

    if(size == 0) return 0;

    uint8_t n = (randFunc() % size) + 1;
    uint8_t i = 0;

    while(i < SCL_BOARD_SQUARES) {
        if(SCL_squareSetContains(squareSet, i)) {
            n--;

            if(n == 0) break;
        }

        ++i;
    }

    return i;
}

void SCL_boardCopy(const SCL_Board boardFrom, SCL_Board boardTo) {
    for(uint8_t i = 0; i < SCL_BOARD_STATE_SIZE; ++i) boardTo[i] = boardFrom[i];
}

uint8_t SCL_boardSquareAttacked(SCL_Board board, uint8_t square, uint8_t byWhite) {
    const char* currentSquare = board;

    /* We need to place a temporary piece on the tested square in order to test if
     the square is attacked (consider testing if attacked by a pawn). */

    char previous = board[square];

    board[square] = SCL_pieceToColor('r', !byWhite);

    for(uint8_t i = 0; i < SCL_BOARD_SQUARES; ++i, ++currentSquare) {
        char s = *currentSquare;

        if((s == '.') || (SCL_pieceIsWhite(s) != byWhite)) continue;

        SCL_SquareSet moves;
        SCL_boardGetPseudoMoves(board, i, 0, moves);

        if(SCL_squareSetContains(moves, square)) {
            board[square] = previous;
            return 1;
        }
    }

    board[square] = previous;
    return 0;
}

uint8_t SCL_boardCheck(SCL_Board board, uint8_t white) {
    const char* square = board;
    char kingChar = white ? 'K' : 'k';

    for(uint8_t i = 0; i < SCL_BOARD_SQUARES; ++i, ++square)
        if((*square == kingChar && SCL_boardSquareAttacked(board, i, !white))) return 1;

    return 0;
}

uint8_t SCL_boardGameOver(SCL_Board board) {
    uint8_t position = SCL_boardGetPosition(board);

    return (position == SCL_POSITION_MATE) || (position == SCL_POSITION_STALEMATE) ||
           (position == SCL_POSITION_DEAD);
}

uint8_t SCL_boardMovePossible(SCL_Board board) {
    uint8_t white = SCL_boardWhitesTurn(board);

    for(uint8_t i = 0; i < SCL_BOARD_SQUARES; ++i) {
        char s = board[i];

        if((s != '.') && (SCL_pieceIsWhite(s) == white)) {
            SCL_SquareSet moves;

            SCL_boardGetMoves(board, i, moves);

            if(SCL_squareSetSize(moves) != 0) return 1;
        }
    }

    return 0;
}

uint8_t SCL_boardMate(SCL_Board board) {
    return SCL_boardGetPosition(board) == SCL_POSITION_MATE;
}

void SCL_boardGetPseudoMoves(
    SCL_Board board,
    uint8_t pieceSquare,
    uint8_t checkCastling,
    SCL_SquareSet result) {
    char piece = board[pieceSquare];

    SCL_squareSetClear(result);

    uint8_t isWhite = SCL_pieceIsWhite(piece);
    int8_t horizontalPosition = pieceSquare % 8;
    int8_t pawnOffset = -8;

    switch(piece) {
    case 'P':
        pawnOffset = 8;
        /* FALLTHROUGH */
    case 'p': {
        uint8_t square = pieceSquare + pawnOffset;
        uint8_t verticalPosition = pieceSquare / 8;

        if(board[square] == '.') // forward move
        {
            SCL_squareSetAdd(result, square);

            if(verticalPosition == (1 + (piece == 'p') * 5)) // start position?
            {
                uint8_t square2 = square + pawnOffset;

                if(board[square2] == '.') SCL_squareSetAdd(result, square2);
            }
        }

#define checkDiagonal(hor, add)                                                           \
    if(horizontalPosition != hor) {                                                       \
        uint8_t square2 = square + add;                                                   \
        char c = board[square2];                                                          \
        if(c != '.' && SCL_pieceIsWhite(c) != isWhite) SCL_squareSetAdd(result, square2); \
    }

        // diagonal moves
        checkDiagonal(0, -1) checkDiagonal(7, 1)

            uint8_t enPassantRow = 4;
        uint8_t enemyPawn = 'p';

        if(piece == 'p') {
            enPassantRow = 3;
            enemyPawn = 'P';
        }

        // en-passant moves
        if(verticalPosition == enPassantRow) {
            uint8_t enPassantColumn = board[SCL_BOARD_ENPASSANT_CASTLE_BYTE] & 0x0f;
            uint8_t column = pieceSquare % 8;

            for(int8_t offset = -1; offset < 2; offset += 2)
                if((enPassantColumn == column + offset) &&
                   (board[pieceSquare + offset] == enemyPawn)) {
                    SCL_squareSetAdd(result, pieceSquare + pawnOffset + offset);
                    break;
                }
        }

#undef checkDiagonal
    } break;

    case 'r': // rook
    case 'R':
    case 'b': // bishop
    case 'B':
    case 'q': // queen
    case 'Q': {
        const int8_t offsets[8] = {-8, 1, 8, -1, -7, 9, -9, 7};
        const int8_t columnDirs[8] = {0, 1, 0, -1, 1, 1, -1, -1};

        uint8_t from = (piece == 'b' || piece == 'B') * 4;
        uint8_t to = 4 + (piece != 'r' && piece != 'R') * 4;

        for(uint8_t i = from; i < to; ++i) {
            int8_t offset = offsets[i];
            int8_t columnDir = columnDirs[i];
            int8_t square = pieceSquare;
            int8_t col = horizontalPosition;

            while(1) {
                square += offset;
                col += columnDir;

                if(square < 0 || square > 63 || col < 0 || col > 7) break;

                char squareC = board[square];

                if(squareC == '.')
                    SCL_squareSetAdd(result, square);
                else {
                    if(SCL_pieceIsWhite(squareC) != isWhite) SCL_squareSetAdd(result, square);

                    break;
                }
            }
        }
    } break;

    case 'n': // knight
    case 'N': {
        const int8_t offsets[4] = {6, 10, 15, 17};
        const int8_t columnsMinus[4] = {2, -2, 1, -1};
        const int8_t columnsPlus[4] = {-2, 2, -1, 1};
        const int8_t *off, *col;

#define checkOffsets(op, comp, limit, dir)                             \
    off = offsets;                                                     \
    col = columns##dir;                                                \
    for(uint8_t i = 0; i < 4; ++i, ++off, ++col) {                     \
        int8_t square = pieceSquare op(*off);                          \
        if(square comp limit) /* out of board? */                      \
            break;                                                     \
        int8_t horizontalCheck = horizontalPosition + (*col);          \
        if(horizontalCheck < 0 || horizontalCheck >= 8) continue;      \
        char squareC = board[square];                                  \
        if((squareC == '.') || (SCL_pieceIsWhite(squareC) != isWhite)) \
            SCL_squareSetAdd(result, square);                          \
    }

        checkOffsets(-, <, 0, Minus) checkOffsets(+, >=, SCL_BOARD_SQUARES, Plus)

#undef checkOffsets
    } break;

    case 'k': // king
    case 'K': {
        uint8_t verticalPosition = pieceSquare / 8;

        uint8_t u = verticalPosition != 0, d = verticalPosition != 7, l = horizontalPosition != 0,
                r = horizontalPosition != 7;

        uint8_t square2 = pieceSquare - 9;

#define checkSquare(cond, add)                                                             \
    if(cond && ((board[square2] == '.') || (SCL_pieceIsWhite(board[square2])) != isWhite)) \
        SCL_squareSetAdd(result, square2);                                                 \
    square2 += add;

        checkSquare(l && u, 1) checkSquare(u, 1) checkSquare(r && u, 6) checkSquare(l, 2)
            checkSquare(r, 6) checkSquare(l && d, 1) checkSquare(d, 1) checkSquare(r && d, 0)
#undef checkSquare

            // castling:

            if(checkCastling) {
            uint8_t bitShift = 4 + 2 * (!isWhite);

            if((board[SCL_BOARD_ENPASSANT_CASTLE_BYTE] & (0x03 << bitShift)) &&
               !SCL_boardSquareAttacked(board, pieceSquare, !isWhite)) // no check?
            {
#if !SCL_960_CASTLING
                // short castle:
                pieceSquare++;

                if((board[SCL_BOARD_ENPASSANT_CASTLE_BYTE] & (0x01 << bitShift)) &&
                   (board[pieceSquare] == '.') && (board[pieceSquare + 1] == '.') &&
                   (board[pieceSquare + 2] == SCL_pieceToColor('r', isWhite)) &&
                   !SCL_boardSquareAttacked(board, pieceSquare, !isWhite))
                    SCL_squareSetAdd(result, pieceSquare + 1);

                /* note: don't check the final square for check, it will potentially
             be removed later (can't end up in check) */

                // long castle:
                pieceSquare -= 2;

                if((board[SCL_BOARD_ENPASSANT_CASTLE_BYTE] & (0x02 << bitShift)) &&
                   (board[pieceSquare] == '.') && (board[pieceSquare - 1] == '.') &&
                   (board[pieceSquare - 2] == '.') &&
                   (board[pieceSquare - 3] == SCL_pieceToColor('r', isWhite)) &&
                   !SCL_boardSquareAttacked(board, pieceSquare, !isWhite))
                    SCL_squareSetAdd(result, pieceSquare - 1);
#else // 960 castling
                for(int i = 0; i < 2; ++i) // short and long
                    if(board[SCL_BOARD_ENPASSANT_CASTLE_BYTE] & ((i + 1) << bitShift)) {
                        uint8_t rookPos = board[SCL_BOARD_EXTRA_BYTE] >> 3, targetPos = 5;

                        if(i == 1) {
                            rookPos = board[SCL_BOARD_EXTRA_BYTE] & 0x07, targetPos = 3;
                        }

                        if(!isWhite) {
                            rookPos += 56;
                            targetPos += 56;
                        }

                        uint8_t ok = board[rookPos] == SCL_pieceToColor('r', isWhite);

                        if(!ok) continue;

                        int8_t inc = 1 - 2 * (targetPos > rookPos);

                        while(targetPos != rookPos) // check vacant squares for the rook
                        {
                            if(board[targetPos] != '.' && targetPos != pieceSquare) {
                                ok = 0;
                                break;
                            }

                            targetPos += inc;
                        }

                        if(!ok) continue;

                        targetPos = i == 0 ? 6 : 2;

                        if(!isWhite) targetPos += 56;

                        inc = 1 - 2 * (targetPos > pieceSquare);

                        while(targetPos != pieceSquare) // check squares for the king
                        {
                            if((board[targetPos] != '.' && targetPos != rookPos) ||
                               SCL_boardSquareAttacked(board, targetPos, !isWhite)) {
                                ok = 0;
                                break;
                            }

                            targetPos += inc;
                        }

                        if(ok) SCL_squareSetAdd(result, rookPos);
                    }
#endif
            }
        }
    } break;

    default:
        break;
    }
}

void SCL_printSquareSet(SCL_SquareSet set, SCL_PutCharFunction putCharFunc) {
    uint8_t first = 1;

    putCharFunc('(');

    for(uint8_t i = 0; i < SCL_BOARD_SQUARES; ++i) {
        if(!SCL_squareSetContains(set, i)) continue;

        if(!first)
            putCharFunc(',');
        else
            first = 0;

        putCharFunc('A' + i % 8);
        putCharFunc('1' + i / 8);
    }

    putCharFunc(')');
}

void SCL_printSquareUTF8(uint8_t square, SCL_PutCharFunction putCharFunc) {
    uint32_t val = 0;

    switch(square) {
    case 'r':
        val = 0x9c99e200;
        break;
    case 'n':
        val = 0x9e99e200;
        break;
    case 'b':
        val = 0x9d99e200;
        break;
    case 'q':
        val = 0x9b99e200;
        break;
    case 'k':
        val = 0x9a99e200;
        break;
    case 'p':
        val = 0x9f99e200;
        break;
    case 'R':
        val = 0x9699e200;
        break;
    case 'N':
        val = 0x9899e200;
        break;
    case 'B':
        val = 0x9799e200;
        break;
    case 'Q':
        val = 0x9599e200;
        break;
    case 'K':
        val = 0x9499e200;
        break;
    case 'P':
        val = 0x9999e200;
        break;
    case '.':
        val = 0x9296e200;
        break;
    case ',':
        val = 0x9196e200;
        break;
    default:
        putCharFunc(square);
        return;
        break;
    }

    uint8_t count = 4;

    while((val % 256 == 0) && (count > 0)) {
        val /= 256;
        count--;
    }

    while(count > 0) {
        putCharFunc(val % 256);
        val /= 256;
        count--;
    }
}

void SCL_boardGetMoves(SCL_Board board, uint8_t pieceSquare, SCL_SquareSet result) {
    SCL_SquareSet allMoves;

    SCL_squareSetClear(allMoves);

    for(uint8_t i = 0; i < 8; ++i) result[i] = 0;

    SCL_boardGetPseudoMoves(board, pieceSquare, 1, allMoves);

    // Now only keep moves that don't lead to one's check:

    SCL_SQUARE_SET_ITERATE_BEGIN(allMoves)

    SCL_MoveUndo undo = SCL_boardMakeMove(board, pieceSquare, iteratedSquare, 'q');

    if(!SCL_boardCheck(board, !SCL_boardWhitesTurn(board)))
        SCL_squareSetAdd(result, iteratedSquare);

    SCL_boardUndoMove(board, undo);

    SCL_SQUARE_SET_ITERATE_END
}

uint8_t SCL_boardDead(SCL_Board board) {
    /*
    This byte represents material by bits:

    MSB _ _ _ _ _ _ _ _ LSB
          | | |   | | \_ white knight
          | | |   |  \__ white bishop on white
          | | |    \____ white bishop on black
          | |  \________ black knight
          |  \__________ black bishop on white
           \____________ black bishop on black
  */
    uint8_t material = 0;

    const char* p = board;

    for(uint8_t i = 0; i < SCL_BOARD_SQUARES; ++i) {
        char c = *p;

        switch(c) {
        case 'n':
            material |= 0x01;
            break;
        case 'N':
            material |= 0x10;
            break;
        case 'b':
            material |= (0x02 << (!SCL_squareIsWhite(i)));
            break;
        case 'B':
            material |= (0x20 << (!SCL_squareIsWhite(i)));
            break;
        case 'p':
        case 'P':
        case 'r':
        case 'R':
        case 'q':
        case 'Q':
            return 0; // REMOVE later if more complex check are performed
            break;

        default:
            break;
        }

        p++;
    }

    // TODO: add other checks than only insufficient material

    // possible combinations of insufficient material:

    return (material == 0x00) || // king vs king
           (material == 0x01) || // king and knight vs king
           (material == 0x10) || // king and knight vs king
           (material == 0x02) || // king and bishop vs king
           (material == 0x20) || // king and bishop vs king
           (material == 0x04) || // king and bishop vs king
           (material == 0x40) || // king and bishop vs king
           (material == 0x22) || // king and bishop vs king and bishop (same color)
           (material == 0x44); // king and bishop vs king and bishop (same color)
}

uint8_t SCL_boardGetPosition(SCL_Board board) {
    uint8_t check = SCL_boardCheck(board, SCL_boardWhitesTurn(board));
    uint8_t moves = SCL_boardMovePossible(board);

    if(check)
        return moves ? SCL_POSITION_CHECK : SCL_POSITION_MATE;
    else if(!moves)
        return SCL_POSITION_STALEMATE;

    if(SCL_boardDead(board)) return SCL_POSITION_DEAD;

    return SCL_POSITION_NORMAL;
}

uint8_t SCL_stringToMove(
    const char* moveString,
    uint8_t* resultFrom,
    uint8_t* resultTo,
    char* resultPromotion) {
    char c;

    uint8_t* dst = resultFrom;

    for(uint8_t i = 0; i < 2; ++i) {
        c = *moveString;

        *dst = (c >= 'a') ? (c - 'a') : (c - 'A');

        if(*dst > 7) return 0;

        moveString++;
        c = *moveString;

        *dst += 8 * (c - '1');

        if(*dst > 63) return 0;

        moveString++;

        dst = resultTo;
    }

    c = *moveString;

    if(c < 'A') c = c - 'A' + 'a';

    switch(c) {
    case 'N':
    case 'n':
        *resultPromotion = 'n';
        break;
    case 'B':
    case 'b':
        *resultPromotion = 'b';
        break;
    case 'R':
    case 'r':
        *resultPromotion = 'r';
        break;
    case 'Q':
    case 'q':
    default:
        *resultPromotion = 'q';
        break;
    }

    return 1;
}

void SCL_printBoard(
    SCL_Board board,
    SCL_PutCharFunction putCharFunc,
    SCL_SquareSet highlightSquares,
    uint8_t selectSquare,
    uint8_t format,
    uint8_t offset,
    uint8_t labels,
    uint8_t blackDown) {
    if(labels) {
        for(uint8_t i = 0; i < offset + 2; ++i) putCharFunc(' ');

        for(uint8_t i = 0; i < 8; ++i) {
            if((format != SCL_PRINT_FORMAT_COMPACT) && (format != SCL_PRINT_FORMAT_COMPACT_UTF8))
                putCharFunc(' ');

            putCharFunc(blackDown ? ('H' - i) : ('A' + i));
        }

        putCharFunc('\n');
    }

    int8_t i = 7;
    int8_t add = 1;

    if(!blackDown) {
        i = 56;
        add = -1;
    }

    for(int8_t row = 0; row < 8; ++row) {
        for(uint8_t j = 0; j < offset; ++j) putCharFunc(' ');

        if(labels) {
            putCharFunc(!blackDown ? ('8' - row) : ('1' + row));
            putCharFunc(' ');
        }

        const char* square = board + i;

        for(int8_t col = 0; col < 8; ++col) {
            switch(format) {
            case SCL_PRINT_FORMAT_COMPACT:
                putCharFunc(
                    (*square == '.') ?
                        (((i != selectSquare) ?
                              (!SCL_squareSetContains(highlightSquares, i) ? *square : '*') :
                              '#')) :
                        *square);
                break;

            case SCL_PRINT_FORMAT_UTF8: {
                char squareChar = SCL_squareIsWhite(i) ? '.' : ',';
                char pieceChar = (*square == '.') ? squareChar : *square;

                if(i == selectSquare) {
                    putCharFunc('(');

                    if(*square == '.')
                        putCharFunc(')');
                    else
                        SCL_printSquareUTF8(pieceChar, putCharFunc);
                } else if(!SCL_squareSetContains(highlightSquares, i)) {
                    SCL_printSquareUTF8(squareChar, putCharFunc);
                    SCL_printSquareUTF8(pieceChar, putCharFunc);
                } else {
                    putCharFunc('[');

                    if(*square == '.')
                        putCharFunc(']');
                    else
                        SCL_printSquareUTF8(*square, putCharFunc);
                }

                break;
            }

            case SCL_PRINT_FORMAT_COMPACT_UTF8:
                SCL_printSquareUTF8(
                    (*square == '.') ?
                        (SCL_squareSetContains(highlightSquares, i) ?
                             '*' :
                             (i == selectSquare ? '#' : ((SCL_squareIsWhite(i) ? '.' : ',')))) :
                        *square,
                    putCharFunc);
                break;

            case SCL_PRINT_FORMAT_NORMAL:
            default: {
                uint8_t c = *square;

                char squareColor = SCL_squareIsWhite(i) ? ' ' : ':';

                putCharFunc(
                    (i != selectSquare) ?
                        (!SCL_squareSetContains(highlightSquares, i) ? squareColor : '#') :
                        '@');

                putCharFunc(c == '.' ? squareColor : *square);
                break;
            }
            }

            i -= add;
            square -= add;
        }

        putCharFunc('\n');

        i += add * 16;
    } // for rows
}

int16_t SCL_pieceValuePositive(char piece) {
    switch(piece) {
    case 'p':
    case 'P':
        return SCL_VALUE_PAWN;
        break;
    case 'n':
    case 'N':
        return SCL_VALUE_KNIGHT;
        break;
    case 'b':
    case 'B':
        return SCL_VALUE_BISHOP;
        break;
    case 'r':
    case 'R':
        return SCL_VALUE_ROOK;
        break;
    case 'q':
    case 'Q':
        return SCL_VALUE_QUEEN;
        break;
    case 'k':
    case 'K':
        return SCL_VALUE_KING;
        break;
    default:
        break;
    }

    return 0;
}

int16_t SCL_pieceValue(char piece) {
    switch(piece) {
    case 'P':
        return SCL_VALUE_PAWN;
        break;
    case 'N':
        return SCL_VALUE_KNIGHT;
        break;
    case 'B':
        return SCL_VALUE_BISHOP;
        break;
    case 'R':
        return SCL_VALUE_ROOK;
        break;
    case 'Q':
        return SCL_VALUE_QUEEN;
        break;
    case 'K':
        return SCL_VALUE_KING;
        break;
    case 'p':
        return -1 * SCL_VALUE_PAWN;
        break;
    case 'n':
        return -1 * SCL_VALUE_KNIGHT;
        break;
    case 'b':
        return -1 * SCL_VALUE_BISHOP;
        break;
    case 'r':
        return -1 * SCL_VALUE_ROOK;
        break;
    case 'q':
        return -1 * SCL_VALUE_QUEEN;
        break;
    case 'k':
        return -1 * SCL_VALUE_KING;
        break;
    default:
        break;
    }

    return 0;
}

#define ATTACK_BONUS 3
#define MOBILITY_BONUS 10
#define CENTER_BONUS 7
#define CHECK_BONUS 5
#define KING_CASTLED_BONUS 30
#define KING_BACK_BONUS 15
#define KING_NOT_CENTER_BONUS 15
#define PAWN_NON_DOUBLE_BONUS 3
#define PAWN_PAIR_BONUS 3
#define KING_CENTERNESS 10

int16_t _SCL_rateKingEndgamePosition(uint8_t position) {
    int16_t result = 0;
    uint8_t rank = position / 8;
    position %= 8;

    if(position > 1 && position < 6) result += KING_CENTERNESS;

    if(rank > 1 && rank < 6) result += KING_CENTERNESS;

    return result;
}

int16_t SCL_boardEvaluateStatic(SCL_Board board) {
    uint8_t position = SCL_boardGetPosition(board);

    int16_t total = 0;

    switch(position) {
    case SCL_POSITION_MATE:
        return SCL_boardWhitesTurn(board) ? -1 * SCL_EVALUATION_MAX_SCORE :
                                            SCL_EVALUATION_MAX_SCORE;
        break;

    case SCL_POSITION_STALEMATE:
    case SCL_POSITION_DEAD:
        return 0;
        break;

        /*
      main points are assigned as follows:
      - points for material as a sum of all material on board
      - for playing side: if a piece attacks piece of greater value, a fraction
        of the value difference is gained (we suppose exchange), this is only
        gained once per every attacking piece (maximum gain is taken), we only
        take fraction so that actually taking the piece is favored
      - ATTACK_BONUS points for any attacked piece

      other points are assigned as follows (in total these shouldn't be more
      than the value of one pawn)
      - mobility: MOBILITY_BONUS points for each piece with at least 4 possible
        moves
      - center control: CENTER_BONUS points for a piece on a center square
      - CHECK_BONUS points for check
      - king:
        - safety (non endgame): KING_BACK_BONUS points for king on staring rank,
          additional KING_CASTLED_BONUS if the kind if on castled square or
          closer to the edge, additional KING_NOT_CENTER_BONUS for king not on
          its start neighbouring center square
        - center closeness (endgame): up to 2 * KING_CENTERNESS points for
          being closer to center
      - non-doubled pawns: PAWN_NON_DOUBLE_BONUS points for each pawn without
        same color pawn directly in front of it 
      - pawn structure: PAWN_PAIR_BONUS points for each pawn guarding own pawn
      - advancing pawns: 1 point for each pawn's rank in its move
        direction
    */

    case SCL_POSITION_CHECK:
        total += SCL_boardWhitesTurn(board) ? -1 * CHECK_BONUS : CHECK_BONUS;
        /* FALLTHROUGH */
    case SCL_POSITION_NORMAL:
    default: {
        SCL_SquareSet moves;

        const char* p = board;

        int16_t positiveMaterial = 0;
        uint8_t endgame = 0;

        // first count material to see if this is endgame or not
        for(uint8_t i = 0; i < SCL_BOARD_SQUARES; ++i, ++p) {
            char s = *p;

            if(s != '.') {
                positiveMaterial += SCL_pieceValuePositive(s);
                total += SCL_pieceValue(s);
            }
        }

        endgame = positiveMaterial <= SCL_ENDGAME_MATERIAL_LIMIT;

        p = board;

        for(uint8_t i = 0; i < SCL_BOARD_SQUARES; ++i, ++p) {
            char s = *p;

            if(s != '.') {
                uint8_t white = SCL_pieceIsWhite(s);

                switch(s) {
                case 'k': // king safety
                    if(endgame)
                        total -= _SCL_rateKingEndgamePosition(i);
                    else if(i >= 56) {
                        total -= KING_BACK_BONUS;

                        if(i != 59) {
                            total -= KING_NOT_CENTER_BONUS;

                            if(i >= 62 || i <= 58) total -= KING_CASTLED_BONUS;
                        }
                    }
                    break;

                case 'K':
                    if(endgame)
                        total += _SCL_rateKingEndgamePosition(i);
                    else if(i <= 7) {
                        total += KING_BACK_BONUS;

                        if(i != 3) {
                            total += KING_NOT_CENTER_BONUS;

                            if(i <= 2 || i >= 6) total += KING_CASTLED_BONUS;
                        }
                    }
                    break;

                case 'P': // pawns
                case 'p': {
                    int8_t rank = i / 8;

                    if(rank != 0 && rank != 7) {
                        if(s == 'P') {
                            total += rank;

                            char* tmp = board + i + 8;

                            if(*tmp != 'P') total += PAWN_NON_DOUBLE_BONUS;

                            if(i % 8 != 7) {
                                tmp++;

                                if(*tmp == 'P') total += PAWN_PAIR_BONUS;

                                if(*(tmp - 16) == 'P') total += PAWN_PAIR_BONUS;
                            }
                        } else {
                            total -= 7 - rank;

                            char* tmp = board + i - 8;

                            if(*tmp != 'p') total -= PAWN_NON_DOUBLE_BONUS;

                            if(i % 8 != 7) {
                                tmp += 17;

                                if(*tmp == 'p') total -= PAWN_PAIR_BONUS;

                                if(*(tmp - 16) == 'p') total -= PAWN_PAIR_BONUS;
                            }
                        }
                    }

                    break;
                }

                default:
                    break;
                }

                if(i >= 27 && i <= 36 && (i >= 35 || i <= 28)) // center control
                    total += white ? CENTER_BONUS : (-1 * CENTER_BONUS);

                // for performance we only take pseudo moves
                SCL_boardGetPseudoMoves(board, i, 0, moves);

                if(SCL_squareSetSize(moves) >= 4) // mobility
                    total += white ? MOBILITY_BONUS : (-1 * MOBILITY_BONUS);

                int16_t exchangeBonus = 0;

                SCL_SQUARE_SET_ITERATE_BEGIN(moves)

                if(board[iteratedSquare] != '.') {
                    total += white ? ATTACK_BONUS : (-1 * ATTACK_BONUS);

                    if(SCL_boardWhitesTurn(board) == white) {
                        int16_t valueDiff = SCL_pieceValuePositive(board[iteratedSquare]) -
                                            SCL_pieceValuePositive(s);

                        valueDiff /= 4; // only take a fraction to favor taking

                        if(valueDiff > exchangeBonus) exchangeBonus = valueDiff;
                    }
                }

                SCL_SQUARE_SET_ITERATE_END

                if(exchangeBonus != 0) total += white ? exchangeBonus : -1 * exchangeBonus;
            }
        } // for each square

        return total;

        break;

    } // normal position
    } // switch

    return 0;
}

#undef ATTACK_BONUS
#undef MOBILITY_BONUS
#undef CENTER_BONUS
#undef CHECK_BONUS
#undef KING_CASTLED_BONUS
#undef KING_BACK_BONUS
#undef PAWN_NON_DOUBLE_BONUS
#undef PAWN_PAIR_BONUS
#undef KING_CENTERNESS

SCL_StaticEvaluationFunction _SCL_staticEvaluationFunction;
int16_t _SCL_currentEval;
int8_t _SCL_depthHardLimit;

/**
  Inner recursive function for SCL_boardEvaluateDynamic. It is passed a square
  (or -1) at which last capture happened, to implement capture extension.
*/
int16_t _SCL_boardEvaluateDynamic(
    SCL_Board board,
    int8_t depth,
    int16_t alphaBeta,
    int8_t takenSquare) {
#if SCL_COUNT_EVALUATED_POSITIONS
    SCL_positionsEvaluated++;
#endif

#if SCL_CALL_WDT_RESET
    wdt_reset();
#endif

    uint8_t whitesTurn = SCL_boardWhitesTurn(board);
    int8_t valueMultiply = whitesTurn ? 1 : -1;
    int16_t bestMoveValue = -1 * SCL_EVALUATION_MAX_SCORE;
    uint8_t shouldCompute = depth > 0;
    uint8_t extended = 0;
    uint8_t positionType = SCL_boardGetPosition(board);

    if(!shouldCompute) {
        /* here we do two extensions (deeper search): taking on a same square 
      (exchanges) and checks (good for mating and preventing mates): */
        extended = (depth > _SCL_depthHardLimit) &&
                   (takenSquare >= 0 || (SCL_boardGetPosition(board) == SCL_POSITION_CHECK));

        shouldCompute = extended;
    }

#if SCL_DEBUG_AI
    char moveStr[8];
    uint8_t debugFirst = 1;
#endif

    if(shouldCompute &&
       (positionType == SCL_POSITION_NORMAL || positionType == SCL_POSITION_CHECK)) {
#if SCL_DEBUG_AI
        putchar('(');
#endif

        alphaBeta *= valueMultiply;
        uint8_t end = 0;
        const char* b = board;

        depth--;

        for(uint8_t i = 0; i < SCL_BOARD_SQUARES; ++i, ++b) {
            char s = *b;

            if(s != '.' && SCL_pieceIsWhite(s) == whitesTurn) {
                SCL_SquareSet moves;

                SCL_squareSetClear(moves);

                SCL_boardGetMoves(board, i, moves);

                if(!SCL_squareSetEmpty(moves)) {
                    SCL_SQUARE_SET_ITERATE_BEGIN(moves)

                    int8_t captureExtension = -1;

                    if(board[iteratedSquare] != '.' && // takes a piece
                       (takenSquare == -1 || // extend on first taken sq.
                        (extended && takenSquare != -1) || // ignore check extension
                        (iteratedSquare == takenSquare))) // extend on same sq. taken
                        captureExtension = iteratedSquare;

                    SCL_MoveUndo undo = SCL_boardMakeMove(board, i, iteratedSquare, 'q');

                    uint8_t s0Dummy, s1Dummy;
                    char pDummy;

                    SCL_UNUSED(s0Dummy);
                    SCL_UNUSED(s1Dummy);
                    SCL_UNUSED(pDummy);

#if SCL_DEBUG_AI
                    if(debugFirst)
                        debugFirst = 0;
                    else
                        putchar(',');

                    if(extended) putchar('*');

                    printf("%s ", SCL_moveToString(board, i, iteratedSquare, 'q', moveStr));
#endif

                    int16_t value = _SCL_boardEvaluateDynamic(
                                        board,
                                        depth, // this is depth - 1, we decremented it
#if SCL_ALPHA_BETA
                                        valueMultiply * bestMoveValue,
#else
                                        0,
#endif
                                        captureExtension) *
                                    valueMultiply;

                    SCL_boardUndoMove(board, undo);

                    if(value > bestMoveValue) {
                        bestMoveValue = value;

#if SCL_ALPHA_BETA
                        // alpha-beta pruning:

                        if(value > alphaBeta) // no, >= can't be here
                        {
                            end = 1;
                            iterationEnd = 1;
                        }
#endif
                    }

                    SCL_SQUARE_SET_ITERATE_END
                } // !squre set empty?
            } // valid piece?

            if(end) break;

        } // for each square

#if SCL_DEBUG_AI
        putchar(')');
#endif
    } else // don't dive recursively, evaluate statically
    {
        bestMoveValue = valueMultiply *
#ifndef SCL_EVALUATION_FUNCTION
                        _SCL_staticEvaluationFunction(board);
#else
                        SCL_EVALUATION_FUNCTION(board);
#endif

        /* For stalemate return the opposite value of the board, i.e. if the
       position is good for white, then stalemate is good for black and vice
       versa. */
        if(positionType == SCL_POSITION_STALEMATE) bestMoveValue *= -1;
    }

    /* Here we either improve (if the move worsens the situation) or devalve (if
     it improves the situation) the result: this needs to be done so that good
     moves far away are seen as worse compared to equally good moves achieved
     in fewer moves. Without this an AI in winning situation may just repeat
     random moves and draw by repetition even if it has mate in 1 (it sees all
     moves as leading to mate). */
    bestMoveValue += bestMoveValue > _SCL_currentEval * valueMultiply ? -1 : 1;

#if SCL_DEBUG_AI
    printf("%d", bestMoveValue * valueMultiply);
#endif

    return bestMoveValue * valueMultiply;
}

int16_t SCL_boardEvaluateDynamic(
    SCL_Board board,
    uint8_t baseDepth,
    uint8_t extensionExtraDepth,
    SCL_StaticEvaluationFunction evalFunction) {
    _SCL_staticEvaluationFunction = evalFunction;
    _SCL_currentEval = evalFunction(board);
    _SCL_depthHardLimit = 0;
    _SCL_depthHardLimit -= extensionExtraDepth;

    return _SCL_boardEvaluateDynamic(
        board,
        baseDepth,
        SCL_boardWhitesTurn(board) ? SCL_EVALUATION_MAX_SCORE : (-1 * SCL_EVALUATION_MAX_SCORE),
        -1);
}

void SCL_boardRandomMove(
    SCL_Board board,
    SCL_RandomFunction randFunc,
    uint8_t* squareFrom,
    uint8_t* squareTo,
    char* resultProm) {
    *resultProm = (randFunc() < 128) ? ((randFunc() < 128) ? 'r' : 'n') :
                                       ((randFunc() < 128) ? 'b' : 'q');

    SCL_SquareSet set;
    uint8_t white = SCL_boardWhitesTurn(board);
    const char* s = board;

    SCL_squareSetClear(set);

    // find squares with pieces that have legal moves

    for(uint8_t i = 0; i < SCL_BOARD_SQUARES; ++i, ++s) {
        char c = *s;

        if(c != '.' && SCL_pieceIsWhite(c) == white) {
            SCL_SquareSet moves;

            SCL_boardGetMoves(board, i, moves);

            if(SCL_squareSetSize(moves) != 0) SCL_squareSetAdd(set, i);
        }
    }

    *squareFrom = SCL_squareSetGetRandom(set, randFunc);

    SCL_boardGetMoves(board, *squareFrom, set);

    *squareTo = SCL_squareSetGetRandom(set, randFunc);
}

void SCL_printBoardSimple(
    SCL_Board board,
    SCL_PutCharFunction putCharFunc,
    uint8_t selectSquare,
    uint8_t format) {
    SCL_SquareSet s;

    SCL_squareSetClear(s);

    SCL_printBoard(board, putCharFunc, s, selectSquare, format, 1, 1, 0);
}

int16_t SCL_getAIMove(
    SCL_Board board,
    uint8_t baseDepth,
    uint8_t extensionExtraDepth,
    uint8_t endgameExtraDepth,
    SCL_StaticEvaluationFunction evalFunc,
    SCL_RandomFunction randFunc,
    uint8_t randomness,
    uint8_t repetitionMoveFrom,
    uint8_t repetitionMoveTo,
    uint8_t* resultFrom,
    uint8_t* resultTo,
    char* resultProm) {
#if SCL_DEBUG_AI
    puts("===== AI debug =====");
    putchar('(');
    unsigned char debugFirst = 1;
    char moveStr[8];
#endif

    if(baseDepth == 0) {
        SCL_boardRandomMove(board, randFunc, resultFrom, resultTo, resultProm);
#ifndef SCL_EVALUATION_FUNCTION
        return evalFunc(board);
#else
        return SCL_EVALUATION_FUNCTION(board);
#endif
    }

    if(SCL_boardEstimatePhase(board) == SCL_PHASE_ENDGAME) baseDepth += endgameExtraDepth;

    *resultFrom = 0;
    *resultTo = 0;
    *resultProm = 'q';

    int16_t bestScore = SCL_boardWhitesTurn(board) ? -1 * SCL_EVALUATION_MAX_SCORE - 1 :
                                                     (SCL_EVALUATION_MAX_SCORE + 1);

    for(uint8_t i = 0; i < SCL_BOARD_SQUARES; ++i)
        if(board[i] != '.' && SCL_boardWhitesTurn(board) == SCL_pieceIsWhite(board[i])) {
            SCL_SquareSet moves;

            SCL_squareSetClear(moves);

            SCL_boardGetMoves(board, i, moves);

            SCL_SQUARE_SET_ITERATE_BEGIN(moves)

            int16_t score = 0;

#if SCL_DEBUG_AI
            if(debugFirst)
                debugFirst = 0;
            else
                putchar(',');

            printf("%s ", SCL_moveToString(board, i, iteratedSquare, 'q', moveStr));

#endif

            if(i != repetitionMoveFrom || iteratedSquare != repetitionMoveTo) {
                SCL_MoveUndo undo = SCL_boardMakeMove(board, i, iteratedSquare, 'q');

                score =
                    SCL_boardEvaluateDynamic(board, baseDepth - 1, extensionExtraDepth, evalFunc);

                SCL_boardUndoMove(board, undo);
            }

            if(randFunc != 0 && randomness > 1 && score < 16000 && score > -16000) {
                /*^ We limit randomizing by about half the max score for two reasons:
            to prevent over/under flows and secondly we don't want to alter
            the highest values for checkmate -- these are modified by tiny
            values depending on their depth so as to prevent endless loops in
            which most moves are winning, biasing such values would completely
            kill that algorithm */

                int16_t bias = randFunc();
                bias = (bias - 128) / 2;
                bias *= randomness - 1;
                score += bias;
            }

            uint8_t comparison = score == bestScore;

            if((comparison != 1) && ((SCL_boardWhitesTurn(board) && score > bestScore) ||
                                     (!SCL_boardWhitesTurn(board) && score < bestScore)))
                comparison = 2;

            uint8_t replace = 0;

            if(randFunc == 0)
                replace = comparison == 2;
            else
                replace =
                    (comparison == 2) ||
                    ((comparison == 1) && (randFunc() < 160)); // not uniform distr. but simple

            if(replace) {
                *resultFrom = i;
                *resultTo = iteratedSquare;
                bestScore = score;
            }

            SCL_SQUARE_SET_ITERATE_END
        }

#if SCL_DEBUG_AI
    printf(")%d %s\n", bestScore, SCL_moveToString(board, *resultFrom, *resultTo, 'q', moveStr));
    puts("===== AI debug end ===== ");
#endif

    return bestScore;
}

uint8_t SCL_boardToFEN(SCL_Board board, char* string) {
    uint8_t square = 56;
    uint8_t spaces = 0;
    uint8_t result = 0;

#define put(c)         \
    {                  \
        *string = (c); \
        string++;      \
        result++;      \
    }

    while(1) // pieces
    {
        char s = board[square];

        if(s == '.') {
            spaces++;
        } else {
            if(spaces != 0) {
                put('0' + spaces) spaces = 0;
            }

            put(s)
        }

        square++;

        if(square % 8 == 0) {
            if(spaces != 0) {
                put('0' + spaces) spaces = 0;
            }

            if(square == 8) break;

            put('/');

            square -= 16;
        }
    }

    put(' ');
    put(SCL_boardWhitesTurn(board) ? 'w' : 'b');
    put(' ');

    uint8_t b = board[SCL_BOARD_ENPASSANT_CASTLE_BYTE] & 0xf0;

    if(b != 0) // castling
    {
        if(b & 0x10) put('K');
        if(b & 0x20) put('Q');
        if(b & 0x40) put('k');
        if(b & 0x80) put('q');
    } else
        put('-');

    put(' ');

    b = board[SCL_BOARD_ENPASSANT_CASTLE_BYTE] & 0x0f;

    if(b < 8) {
        put('a' + b);
        put(SCL_boardWhitesTurn(board) ? '6' : '3');
    } else
        put('-');

    for(uint8_t i = 0; i < 2; ++i) {
        put(' ');

        uint8_t moves = i == 0 ? ((uint8_t)board[SCL_BOARD_MOVE_COUNT_BYTE]) :
                                 (((uint8_t)board[SCL_BOARD_PLY_BYTE]) / 2 + 1);

        uint8_t hundreds = moves / 100;
        uint8_t tens = (moves % 100) / 10;

        if(hundreds != 0) {
            put('0' + hundreds);
            put('0' + tens);
        } else if(tens != 0)
            put('0' + tens);

        put('0' + moves % 10);
    }

    *string = 0; // terminate the string

    return result + 1;

#undef put
}

uint8_t SCL_boardFromFEN(SCL_Board board, const char* string) {
    uint8_t square = 56;

    while(1) {
        char c = *string;

        if(c == 0) return 0;

        if(c != '/' && c != ' ') // ignore line separators
        {
            if(c < '9') // empty square sequence
            {
                while(c > '0') {
                    board[square] = '.';
                    square++;
                    c--;
                }
            } else // piece
            {
                board[square] = c;
                square++;
            }
        } else {
            if(square == 8) break;

            square -= 16;
        }

        string++;
    }

#define nextChar \
    string++;    \
    if(*string == 0) return 0;

    nextChar // space

        board[SCL_BOARD_PLY_BYTE] = *string == 'b';
    nextChar

        nextChar // space

            uint8_t castleEnPassant = 0x0;

    while(*string != ' ') {
        switch(*string) {
        case 'K':
            castleEnPassant |= 0x10;
            break;
        case 'Q':
            castleEnPassant |= 0x20;
            break;
        case 'k':
            castleEnPassant |= 0x40;
            break;
        case 'q':
            castleEnPassant |= 0x80;
            break;
        default:
            castleEnPassant |= 0xf0;
            break; // for partial XFEN compat.
        }

        nextChar
    }

    nextChar // space

        if(*string != '-') {
        castleEnPassant |= *string - 'a';
        nextChar
    }
    else castleEnPassant |= 0x0f;

    nextChar

        board[SCL_BOARD_ENPASSANT_CASTLE_BYTE] = castleEnPassant;

    for(uint8_t i = 0; i < 2; ++i) {
        nextChar // space

            uint8_t ply = 0;

        while(1) {
            char c = *string;

            if(c < '0' || c > '9') break;

            ply = ply * 10 + (c - '0');

            string++;
        }

        if(i == 0 && *string == 0) return 0;

        if(i == 0)
            board[SCL_BOARD_MOVE_COUNT_BYTE] = ply;
        else
            board[SCL_BOARD_PLY_BYTE] += (ply - 1) * 2;
    }

#if SCL_960_CASTLING
    _SCL_board960RememberRookPositions(board);
#endif

    return 1;
#undef nextChar
}

uint8_t SCL_boardEstimatePhase(SCL_Board board) {
    uint16_t totalMaterial = 0;

    uint8_t ply = board[SCL_BOARD_PLY_BYTE];

    for(uint8_t i = 0; i < SCL_BOARD_SQUARES; ++i) {
        char s = *board;

        if(s != '.') {
            int16_t v = SCL_pieceValue(s);

            if(!SCL_pieceIsWhite(s)) v *= -1;

            totalMaterial += v;
        }

        board++;
    }

    if(totalMaterial < SCL_ENDGAME_MATERIAL_LIMIT) return SCL_PHASE_ENDGAME;

    if(ply <= 10 && (totalMaterial >= SCL_START_MATERIAL - 3 * SCL_VALUE_PAWN))
        return SCL_PHASE_OPENING;

    return SCL_PHASE_MIDGAME;
}

#define SCL_IMAGE_COUNT 12

static const uint8_t SCL_images[8 * SCL_IMAGE_COUNT] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x81,
    0xff, 0xff, 0xff, 0xff, 0xff, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0x81, 0xe7, 0xf7,
    0xf7, 0xaa, 0xff, 0xbd, 0xe7, 0xf7, 0xf7, 0xaa, 0xff, 0xc3, 0xc3, 0xe3, 0xc1, 0x80,
    0xff, 0x99, 0xdb, 0xeb, 0xc9, 0x94, 0xe7, 0xc3, 0x81, 0xc1, 0x94, 0x80, 0xe7, 0xdb,
    0xbd, 0xdd, 0xbe, 0xbe, 0xc3, 0xc3, 0x91, 0xe3, 0x80, 0x80, 0xdb, 0x99, 0x8d, 0xeb,
    0xaa, 0xbe, 0xc3, 0x81, 0xe1, 0xc1, 0xc1, 0xc1, 0xdb, 0xbd, 0xdd, 0xe3, 0xdd, 0xdd,
    0x81, 0x81, 0xc1, 0x9c, 0xc1, 0xc1, 0x81, 0x81, 0xc1, 0x9c, 0xc1, 0xc1};

void SCL_drawBoard(
    SCL_Board board,
    SCL_PutPixelFunction putPixel,
    uint8_t selectedSquare,
    SCL_SquareSet highlightSquares,
    uint8_t blackDown) {
    uint8_t row = 0;
    uint8_t col = 0;
    uint8_t x = 0;
    uint8_t y = 0;
    uint16_t n = 0;
    uint8_t s = 0;

    uint8_t pictureLine = 0;
    uint8_t loadLine = 1;

    while(row < 8) {
        if(loadLine) {
            s = blackDown ? (row * 8 + (7 - col)) : ((7 - row) * 8 + col);

            char piece = board[s];

            if(piece == '.')
                pictureLine = (y == 4) ? 0xef : 0xff;
            else {
                uint8_t offset = SCL_pieceIsWhite(piece) ? 6 : 0;
                piece = SCL_pieceToColor(piece, 1);

                switch(piece) {
                case 'R':
                    offset += 1;
                    break;
                case 'N':
                    offset += 2;
                    break;
                case 'B':
                    offset += 3;
                    break;
                case 'K':
                    offset += 4;
                    break;
                case 'Q':
                    offset += 5;
                    break;
                default:
                    break;
                }

                pictureLine = SCL_images[y * SCL_IMAGE_COUNT + offset];
            }

            if(SCL_squareSetContains(highlightSquares, s)) pictureLine &= (y % 2) ? 0xaa : 0x55;

            if(s == selectedSquare) pictureLine &= (y == 0 || y == 7) ? 0x00 : ~0x81;

            loadLine = 0;
        }

        putPixel(pictureLine & 0x80, n);
        pictureLine <<= 1;

        n++;
        x++;

        if(x == 8) {
            col++;
            loadLine = 1;
            x = 0;
        }

        if(col == 8) {
            y++;
            col = 0;
            x = 0;
        }

        if(y == 8) {
            row++;
            y = 0;
        }
    }
}

uint32_t SCL_boardHash32(const SCL_Board board) {
    uint32_t result = (board[SCL_BOARD_PLY_BYTE] & 0x01) +
                      (((uint32_t)((uint8_t)board[SCL_BOARD_ENPASSANT_CASTLE_BYTE])) << 24) +
                      board[SCL_BOARD_MOVE_COUNT_BYTE];

    const char* b = board;

    for(uint8_t i = 0; i < SCL_BOARD_SQUARES; ++i, ++b) {
        switch(*b) {
#define C(p, n)                \
    case p:                    \
        result ^= (i + 1) * n; \
        break;
            // the below number are primes
            C('P', 4003)
            C('R', 84673)
            C('N', 93911)
            C('B', 999331)
            C('Q', 909091)
            C('K', 2796203)
            C('p', 4793)
            C('r', 19391)
            C('n', 391939)
            C('b', 108301)
            C('q', 174763)
            C('k', 2474431)
#undef C
        default:
            break;
        }
    }

    // for extra spread of values we swap the low/high parts:
    result = (result >> 16) | (result << 16);

    return result;
}

void SCL_boardDisableCastling(SCL_Board board) {
    board[SCL_BOARD_ENPASSANT_CASTLE_BYTE] &= 0x0f;
}

uint8_t SCL_boardMoveResetsCount(SCL_Board board, uint8_t squareFrom, uint8_t squareTo) {
    return board[squareFrom] == 'P' || board[squareFrom] == 'p' || board[squareTo] != '.';
}

void SCL_printPGN(SCL_Record r, SCL_PutCharFunction putCharFunc, SCL_Board initialState) {
    if(SCL_recordLength(r) == 0) return;

    uint16_t pos = 0;

    SCL_Board board;

    if(initialState != 0)
        for(uint8_t i = 0; i < SCL_BOARD_STATE_SIZE; ++i) board[i] = initialState[i];
    else
        SCL_boardInit(board);

    while(1) {
        uint8_t s0, s1;
        char p;

        uint8_t state = SCL_recordGetMove(r, pos, &s0, &s1, &p);

        pos++;

        if(pos % 2) {
            uint8_t move = pos / 2 + 1;

            if(move / 100 != 0) putCharFunc('0' + move / 100);

            if(move / 10 != 0 || move / 100 != 0) putCharFunc('0' + (move % 100) / 10);

            putCharFunc('0' + move % 10);

            putCharFunc('.');
            putCharFunc(' ');
        }

#if !SCL_960_CASTLING
        if((board[s0] == 'K' && s0 == 4 && (s1 == 2 || s1 == 6)) ||
           (board[s0] == 'k' && s0 == 60 && (s1 == 62 || s1 == 58)))
#else
        if((board[s0] == 'K' && board[s1] == 'R') || (board[s0] == 'k' && board[s1] == 'r'))
#endif
        {
            putCharFunc('O');
            putCharFunc('-');
            putCharFunc('O');

#if !SCL_960_CASTLING
            if(s1 == 58 || s1 == 2)
#else
            if((s1 == (board[SCL_BOARD_EXTRA_BYTE] & 0x07)) ||
               (s1 == 56 + (board[SCL_BOARD_EXTRA_BYTE] & 0x07)))
#endif
            {
                putCharFunc('-');
                putCharFunc('O');
            }
        } else {
            uint8_t pawn = board[s0] == 'P' || board[s0] == 'p';

            if(!pawn) {
                putCharFunc(SCL_pieceToColor(board[s0], 1));

                // disambiguation:

                uint8_t specify = 0;

                for(int i = 0; i < SCL_BOARD_SQUARES; ++i)
                    if(i != s0 && board[i] == board[s0]) {
                        SCL_SquareSet s;

                        SCL_squareSetClear(s);

                        SCL_boardGetMoves(board, i, s);

                        if(SCL_squareSetContains(s, s1)) specify |= (s0 % 8 != s1 % 8) ? 1 : 2;
                    }

                if(specify & 0x01) putCharFunc('a' + s0 % 8);

                if(specify & 0x02) putCharFunc('1' + s0 / 8);
            }

            if(board[s1] != '.' || (pawn && s0 % 8 != s1 % 8 && board[s1] == '.')) // capture?
            {
                if(pawn) putCharFunc('a' + s0 % 8);

                putCharFunc('x');
            }

            putCharFunc('a' + s1 % 8);
            putCharFunc('1' + s1 / 8);

            if(pawn && (s1 >= 56 || s1 <= 7)) // promotion?
            {
                putCharFunc('=');
                putCharFunc(SCL_pieceToColor(p, 1));
            }
        }

        SCL_boardMakeMove(board, s0, s1, p);

        uint8_t position = SCL_boardGetPosition(board);

        if(position == SCL_POSITION_CHECK) putCharFunc('+');

        if(position == SCL_POSITION_MATE) {
            putCharFunc('#');
            break;
        } else if(state != SCL_RECORD_CONT) {
            putCharFunc('*');
            break;
        }

        putCharFunc(' ');
    }
}

void SCL_recordCopy(SCL_Record recordFrom, SCL_Record recordTo) {
    for(uint16_t i = 0; i < SCL_RECORD_MAX_SIZE; ++i) recordTo[i] = recordFrom[i];
}

void SCL_gameInit(SCL_Game* game, const SCL_Board startState) {
    game->startState = startState;

    if(startState != 0)
        SCL_boardCopy(startState, game->board);
    else
        SCL_boardInit(game->board);

    SCL_recordInit(game->record);

    for(uint8_t i = 0; i < 14; ++i) game->prevMoves[i] = 0;

    game->state = SCL_GAME_STATE_PLAYING;
    game->ply = 0;

    SCL_recordInit(game->record);
}

uint8_t SCL_gameGetRepetiotionMove(SCL_Game* game, uint8_t* squareFrom, uint8_t* squareTo) {
    if(squareFrom != 0 && squareTo != 0) {
        *squareFrom = 0;
        *squareTo = 0;
    }

    /*  pos. 1st         2nd         3rd
            |           |           |
            v           v           v       
             01 23 45 67 89 AB CD EF
     move    ab cd ba dc ab cd ba dc */

    if(game->ply >= 7 && game->prevMoves[0] == game->prevMoves[5] &&
       game->prevMoves[0] == game->prevMoves[8] && game->prevMoves[0] == game->prevMoves[13] &&

       game->prevMoves[1] == game->prevMoves[4] && game->prevMoves[1] == game->prevMoves[9] &&
       game->prevMoves[1] == game->prevMoves[12] &&

       game->prevMoves[2] == game->prevMoves[7] && game->prevMoves[2] == game->prevMoves[10] &&

       game->prevMoves[3] == game->prevMoves[6] && game->prevMoves[3] == game->prevMoves[11]) {
        if(squareFrom != 0 && squareTo != 0) {
            *squareFrom = game->prevMoves[3];
            *squareTo = game->prevMoves[2];
        }

        return 1;
    }

    return 0;
}

void SCL_gameMakeMove(SCL_Game* game, uint8_t squareFrom, uint8_t squareTo, char promoteTo) {
    uint8_t repetitionS0, repetitionS1;

    SCL_gameGetRepetiotionMove(game, &repetitionS0, &repetitionS1);
    SCL_boardMakeMove(game->board, squareFrom, squareTo, promoteTo);
    SCL_recordAdd(game->record, squareFrom, squareTo, promoteTo, SCL_RECORD_CONT);
    // ^ TODO: SCL_RECORD_CONT

    game->ply++;

    for(uint8_t i = 0; i < 14 - 2; ++i) game->prevMoves[i] = game->prevMoves[i + 2];

    game->prevMoves[12] = squareFrom;
    game->prevMoves[13] = squareTo;

    if(squareFrom == repetitionS0 && squareTo == repetitionS1)
        game->state = SCL_GAME_STATE_DRAW_REPETITION;
    else if(game->board[SCL_BOARD_MOVE_COUNT_BYTE] >= 50)
        game->state = SCL_GAME_STATE_DRAW_50;
    else {
        uint8_t position = SCL_boardGetPosition(game->board);

        switch(position) {
        case SCL_POSITION_MATE:
            game->state = SCL_boardWhitesTurn(game->board) ? SCL_GAME_STATE_BLACK_WIN :
                                                             SCL_GAME_STATE_WHITE_WIN;
            break;

        case SCL_POSITION_STALEMATE:
            game->state = SCL_GAME_STATE_DRAW_STALEMATE;
            break;

        case SCL_POSITION_DEAD:
            game->state = SCL_GAME_STATE_DRAW_DEAD;
            break;

        default:
            break;
        }
    }
}

uint8_t SCL_gameUndoMove(SCL_Game* game) {
    if(game->ply == 0) return 0;

    if((game->ply - 1) > SCL_recordLength(game->record)) return 0; // can't undo, lacking record

    SCL_Record r;

    SCL_recordCopy(game->record, r);

    uint16_t applyMoves = game->ply - 1;

    SCL_gameInit(game, game->startState);

    for(uint16_t i = 0; i < applyMoves; ++i) {
        uint8_t s0, s1;
        char p;

        SCL_recordGetMove(r, i, &s0, &s1, &p);
        SCL_gameMakeMove(game, s0, s1, p);
    }

    return 1;
}

uint8_t SCL_boardMoveIsLegal(SCL_Board board, uint8_t squareFrom, uint8_t squareTo) {
    if(squareFrom >= SCL_BOARD_SQUARES || squareTo >= SCL_BOARD_SQUARES) return 0;

    char piece = board[squareFrom];

    if((piece == '.') || (SCL_boardWhitesTurn(board) != SCL_pieceIsWhite(piece))) return 0;

    SCL_SquareSet moves;

    SCL_boardGetMoves(board, squareFrom, moves);

    return SCL_squareSetContains(moves, squareTo);
}

#endif // guard
