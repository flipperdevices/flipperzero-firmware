#include <stdio.h>
#include <stdbool.h>

#define WHITE 1
#define BLACK -1
#define BOARD_SIZE 8

typedef struct {
  int board[BOARD_SIZE][BOARD_SIZE];
  int current_player;
  int human_color;
} game_state;

bool isLegalMove(int board[BOARD_SIZE][BOARD_SIZE], int row, int col, int player);

void init_game(game_state* state) {
  // Fill the board with 0
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            state->board[i][j] = 0;
        }
    }

    // Place the initial pieces
    int mid = BOARD_SIZE / 2;
    state->board[mid - 1][mid - 1] = WHITE;
    state->board[mid][mid] = WHITE;
    state->board[mid - 1][mid] = BLACK;
    state->board[mid][mid - 1] = BLACK;
}

bool isGameOver(int board[BOARD_SIZE][BOARD_SIZE]) {
    /* Check if the game is over by checking if there are no more moves left for either player */
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (isLegalMove(board, i, j, BLACK) ||
                isLegalMove(board, i, j, WHITE)) {
                return false;
            }
        }
    }
    return true;
}

bool isLegalMove(int board[BOARD_SIZE][BOARD_SIZE], int row, int col, int player) {
    /* Check if the move is legal by checking if it results in any opponent pieces being captured */
    if (board[row][col] != 0) return false;
    int opponent = -player;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;
            int r = row + i, c = col + j;
            if (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE && board[r][c] == opponent) {
                int k = 2;
                while (true) {
                    r += i;
                    c += j;
                    if (r < 0 || r >= BOARD_SIZE || c < 0 || c >= BOARD_SIZE) break;
                    if (board[r][c] == player) return true;
                    if (board[r][c] == 0) break;
                    k++;
                }
            }
        }
    }
    return false;
}


int heuristic(int board[BOARD_SIZE][BOARD_SIZE]) {
    /* Calculate the heuristic value of the current board. This function can
       be replaced with a more complex evaluation function that takes into
       account factors such as mobility, piece square tables, etc. */
    int white = 0, black = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == 1) white++;
            if (board[i][j] == -1) black++;
        }
    }
    return white - black;
}

void makeMove(game_state* state, int x, int y, int player) {
    /* Make a move on the board and capture any opponent pieces */
    state->board[x][y] = player;
    int opponent = -player;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;
            int r = x + i, c = y + j;
            if (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE && state->board[r][c] == opponent) {
                int k = 2;
                while (true) {
                    r += i;
                    c += j;
                    if (r < 0 || r >= BOARD_SIZE || c < 0 || c >= BOARD_SIZE) break;
                    if (state->board[r][c] == player) {
                        r -= i;
                        c -= j;
                        while (r != x || c != y) {
                            state->board[r][c] = player;
                            r -= i;
                            c -= j;
                        }
                        break;
                    }
                    if (state->board[r][c] == 0) break;
                    k++;
                }
            }
        }
    }
}

// function to print the board to show black and white pieces for current game state
// and display numbers for the columns and letters for the rows
void print_board(int board[BOARD_SIZE][BOARD_SIZE]) {
    printf("  ");
    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("%d ", i);
    }
    printf("\n");
    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("%d ", i);
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == 0) {
                printf(". ");
            } else if (board[i][j] == 1) {
                printf("W ");
            } else {
                printf("B ");
            }
        }
        printf("\n");
    }
}

int main() {
  game_state state;

  init_game(&state);
  state.human_color = WHITE;
  state.current_player = WHITE;

  while (true) {
    if (isGameOver(state.board)) {
      // caluclate number of black and white pieces on the board
      // and determine the winner
      // print the winner
      int white = 0, black = 0;
      for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
          if (state.board[i][j] == 1) white++;
          if (state.board[i][j] == -1) black++;
        }
      }
      if (white > black) {
        printf("White wins!\n");
      } else if (black > white) {
        printf("Black wins!\n");
      } else {
        printf("Tie game!\n");
      }
      printf("%d - %d\n", white, black);
      break;
    }
    print_board(state.board);
    if (state.current_player == state.human_color) {
      int row, col;
      printf("Enter row and column (or -1 -1 to skip; -2 -2 to exit): ");
      scanf("%d %d", &row, &col);
      if (row == -2 && col == -2) {
        break;
      }
      if (row == -1 && col == -1) {
        state.current_player = -state.current_player;
        continue;
      }
      if (isLegalMove(state.board, row, col, state.current_player)) {
        makeMove(&state, row, col, state.current_player);
        state.current_player = -state.current_player;
      } else {
        printf("Illegal move!\n");
      }
    } else {
      int best_row = -1, best_col = -1, best_score = -1000000;
      for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
          if (isLegalMove(state.board, i, j, state.current_player)) {
            int score = heuristic(state.board);
            if (score > best_score) {
              best_score = score;
              best_row = i;
              best_col = j;
            }
          }
        }
      }
      if (best_row != -1) {
        makeMove(&state, best_row, best_col, state.current_player);
      } else {
        printf("No legal moves for computer!\n");
      }
      state.current_player = -state.current_player;
    }
  }

  return 0;
}
