#pragma once
#include "../mifare_fuzzer_custom_events.h"
#include <furi.h>
#include <furi_hal.h>
#include <gui/view.h>
#include <gui/elements.h>
#include <lib/nfc/nfc_device.h>

#include "lib/nfc/protocols/iso14443_3a/iso14443_3a.h"

typedef void (*MifareFuzzerEmulatorCallback)(MifareFuzzerEvent event, void* context);

typedef enum MifareCard {
    MifareCardClassic1k = 1,
    MifareCardClassic4k,
    MifareCardUltralight,
} MifareCard;

typedef enum MifareFuzzerAttack {
    MifareFuzzerAttackTestValues = 1,
    MifareFuzzerAttackRandomValues,
    MifareFuzzerAttackLoadUidsFromFile,
} MifareFuzzerAttack;

typedef struct MifareFuzzerEmulator {
    View* view;
    MifareFuzzerEmulatorCallback callback;
    void* context;
    bool is_attacking;
    uint8_t ticks_between_cards;
} MifareFuzzerEmulator;

typedef struct MifareFuzzerEmulatorModel {
    const char* title;
    MifareCard mifare_card;
    const char* mifare_card_dsc;
    MifareFuzzerAttack attack;
    const char* attack_dsc;
    Iso14443_3aData nfc_data;
    bool is_attacking;
    uint8_t tick_num;
    uint8_t ticks_between_cards;
} MifareFuzzerEmulatorModel;

MifareFuzzerEmulator* mifare_fuzzer_emulator_alloc();

void mifare_fuzzer_emulator_free(MifareFuzzerEmulator* context);

View* mifare_fuzzer_emulator_get_view(MifareFuzzerEmulator* context);

void mifare_fuzzer_emulator_set_card(
    MifareFuzzerEmulator* mifare_fuzzer_emulator,
    MifareCard mifare_card,
    FuriString* name);
void mifare_fuzzer_emulator_set_attack(
    MifareFuzzerEmulator* mifare_fuzzer_emulator,
    MifareFuzzerAttack mifare_attack);

void mifare_fuzzer_emulator_set_callback(
    MifareFuzzerEmulator* mifare_fuzzer_emulator,
    MifareFuzzerEmulatorCallback callback,
    void* context);

void mifare_fuzzer_emulator_set_nfc_data(
    MifareFuzzerEmulator* mifare_fuzzer_emulator,
    Iso14443_3aData nfc_data);

void mifare_fuzzer_emulator_set_ticks_between_cards(
    MifareFuzzerEmulator* mifare_fuzzer_emulator,
    uint8_t ticks);

void mifare_fuzzer_emulator_set_tick_num(
    MifareFuzzerEmulator* mifare_fuzzer_emulator,
    uint8_t tick_num);
