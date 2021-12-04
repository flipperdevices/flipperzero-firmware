//
// Created by dwdraugr on 24.11.2021.
//

#ifndef FLIPPER_CHIP_FLIPPER_CHIP_H
#define FLIPPER_CHIP_FLIPPER_CHIP_H

#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define CPU_REGISTER_NUMBER 0x10
#define CPU_STACK_DEPTH 0x10

#define MEMORY_SIZE 0x1000
#define MEMORY_START_POSITION 0x200
#define MEMORY_ROM_SIZE (MEMORY_SIZE - MEMORY_START_POSITION)

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define SCREEN_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)

#define KEYS_NUMBER 0x10

typedef struct s_flipper_state {
	uint16_t I;
	uint16_t PC;
	uint8_t SP;
	uint8_t V[CPU_REGISTER_NUMBER];
	uint16_t stack[CPU_STACK_DEPTH];
	uint8_t memory[MEMORY_SIZE];
	uint8_t delay_timer;
	uint8_t sound_timer;
	uint8_t screen[SCREEN_HEIGHT][SCREEN_WIDTH];
	uint8_t key[KEYS_NUMBER];
	bool stop;
} t_flipper_state;

// keyboard layout
// 1  2  3  C
// 4  5  6  D
// 7  8  9  E
// A  0  B  F
typedef enum e_keys {
	k_x,
	k_1,
	k_2,
	k_3,
	k_q,
	k_w,
	k_e,
	k_a,
	k_s,
	k_d,
	k_z,
	k_c,
	k_4,
	k_r,
	k_f,
	k_v,
} t_keys;

void init(t_flipper_state *flipper);
bool load_game(t_flipper_state *state, const char *filename);
void execute_next_opcode(t_flipper_state *state);
void tick(t_flipper_state* state);
uint8_t** get_screen(t_flipper_state* state);

#endif //FLIPPER_CHIP_FLIPPER_CHIP_H
