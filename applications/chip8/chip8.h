#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <furi.h>
#include <m-string.h>
#include <storage/storage.h>

typedef struct Chip8Emulator Chip8Emulator;

typedef struct {

} Chip8State;

Chip8Emulator* chip8_make_emulator(string_t file_path);

void chip8_close_emulator(Chip8Emulator* chip8);

Chip8State* chip8_get_state(Chip8Emulator* chip8);

void chip8_toggle(Chip8Emulator* chip8);

uint16_t read_rom_data(File* file, uint8_t* data);

#ifdef __cplusplus
}
#endif
