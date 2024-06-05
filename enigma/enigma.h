#ifndef ENIGMA_H_
#define ENIGMA_H_

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

// to test out
// - http://wiki.franklinheath.co.uk/index.php/Enigma/Sample_Messages
// - https://cryptii.com/pipes/enigma-machine

// --------------------------------------------------------------
// DATA STRUCTURES, DEFINES and TYPE ALIASES

#define ALPHABET_SIZE 26
#define PLUGBOARD_SIZE 10
#define LABEL_LENGTH 42
#define ROTORS_N 3

typedef uint8_t u8;
typedef size_t usize;
typedef u8 Wiring[ALPHABET_SIZE];

typedef enum {
    RO_FORWARD,
    RO_BACKWARD,
} RotorOrder;

// NOTE: for now we only handle single ring_setting values
typedef struct {
    Wiring forward_wiring;
    Wiring backward_wiring;
    u8 position;
    u8 notch[2];
    u8 ring;
    char name[LABEL_LENGTH];
} Rotor;

typedef struct {
    char name[LABEL_LENGTH];
    char wiring[ALPHABET_SIZE];
    u8 notch[2];
} RotorModel;

typedef struct {
    u8 board[PLUGBOARD_SIZE][2];
    usize board_size;
} Plugboard;

typedef struct {
    Wiring wiring;
    char name[LABEL_LENGTH];
} Reflector;

typedef struct {
    char name[LABEL_LENGTH];
    char wiring[ALPHABET_SIZE];
} ReflectorModel;

// To properly configure an Enigma machine you need four different settings:
//
// - Rotor order
// - Ring setting for each rotor
// - Starting position for each rotor
// - Plugboard connections
// - Type of reflector
//
typedef struct {
    Plugboard plugboard;
    Rotor rotors[ROTORS_N];
    Reflector reflector;
} Enigma;

// --------------------------------------------------------------
// SIGNATURES, MACROS

// TODO: expand this function so we can support a bigger alphabet and
// not just english uppercase.
#define CHAR2CODE(ch) ((u8)((ch) - 'A'))
#define CODE2CHAR(code) ((char)('A' + (code)))

// Function signatures
Enigma* init_enigma(
    const char* rotor_names[ROTORS_N],
    const u8 rotor_positions[ROTORS_N],
    const u8 rotor_ring_settings[ROTORS_N],
    const char* reflector_name,
    u8 (*plugboard)[2],
    usize plugboard_size);

void init_wiring(Wiring wiring, const char* alphabet, usize alphabet_len);
void reverse_wiring(Wiring new_wiring, Wiring old_wiring, usize wiring_len);
char* copy_str(const char* src, const usize length);

void init_rotor(Rotor* r, const char* rotor_name, const u8 position, const u8 ring_settings);
void init_rotors(
    Enigma* e,
    const char* rotor_names[ROTORS_N],
    const u8 rotor_positions[ROTORS_N],
    const u8 rotor_ring_settings[ROTORS_N]);
void init_reflector(Enigma* e, const char* reflector_name);
void destroy_enigma(Enigma* e);

u8 apply_rotor(Rotor* r, const u8 plaintext_code, RotorOrder order);
void move_rotors(Enigma* e);
u8 apply_rotors(Enigma* e, const u8 plaintext_code, RotorOrder order);
u8 apply_plugboard(Enigma* e, const u8 plaintext_code);
u8 apply_reflector(Enigma* e, const u8 plaintext_code);
void apply_enigma(Enigma* e, const u8* input, usize input_len, u8* output);

void enigma_encrypt(Enigma* e, const char* plaintext, usize plaintext_len, char* ciphertext);
void enigma_decrypt(Enigma* e, const char* ciphertext, usize ciphertext_len, char* plaintext);

#endif // ENIGMA_H_

#ifdef ENIGMA_IMPLEMENTATION

// --------------------------------------------------------------
// ENIGMA MODELS

RotorModel KNOWN_ROTORS[] = {
    {"M3-I", "EKMFLGDQVZNTOWYHXUSPAIBRCJ", {CHAR2CODE('Q'), CHAR2CODE('_')}},
    {"M3-II", "AJDKSIRUXBLHWTMCQGZNPYFVOE", {CHAR2CODE('E'), CHAR2CODE('_')}},
    {"M3-III", "BDFHJLCPRTXVZNYEIWGAKMUSQO", {CHAR2CODE('V'), CHAR2CODE('_')}},
    {"M3-IV", "ESOVPZJAYQUIRHXLNFTGKDCMWB", {CHAR2CODE('J'), CHAR2CODE('_')}},
    {"M3-V", "VZBRGITYUPSDNHLXAWMJQOFECK", {CHAR2CODE('Z'), CHAR2CODE('_')}},
    {"M4-VI", "JPGVOUMFYQBENHZRDKASXLICTW", {CHAR2CODE('Z'), CHAR2CODE('M')}},
    {"M4-VII", "NZJHGRCXMYSWBOUFAIVLPEKQDT", {CHAR2CODE('Z'), CHAR2CODE('M')}},
    {"M4-VIII", "FKQHTLXOCBJSPDZRAMEWNIUYGV", {CHAR2CODE('Z'), CHAR2CODE('M')}},
    {"M4-Beta", "LEYJVCNIXWPBQMDRTAKZGFUHOS", {CHAR2CODE('_'), CHAR2CODE('_')}}, // TODO: M4
    {"M4-Gamma", "FSOKANUERHMBTIYCWLQPZXVGJD", {CHAR2CODE('_'), CHAR2CODE('_')}}, // TODO: M4
};

ReflectorModel KNOWN_REFLECTORS[] = {
    {"M3-A", "EJMZALYXVBWFCRQUONTSPIKHGD"},
    {"M3-B", "YRUHQSLDPXNGOKMIEBFZCWVJAT"},
    {"M3-C", "FVPJIAOYEDRZXWGCTKUQSBNMHL"},
    {"M4-B Thin", "ENKQAUYWJICOPBLMDXZVFTHRGS"}, // TODO: M4
    {"M4-C Thin", "RDOBJNTKVEHMLFCWZAXGYIPSUQ"}, // TODO: M4
};

usize KNOWN_ROTORS_LENGTH = (sizeof(KNOWN_ROTORS) / sizeof(RotorModel));
usize KNOWN_REFLECTORS_LENGTH = (sizeof(KNOWN_REFLECTORS) / sizeof(ReflectorModel));

// --------------------------------------------------------------
// UTILS

void init_wiring(Wiring wiring, const char* alphabet, usize alphabet_len) {
    for(usize i = 0; i < alphabet_len; i++) {
        wiring[i] = CHAR2CODE(alphabet[i]);
    }
}

// Used to change the direction of old_wiring into new_wiring.
//
// old_wiring[X] = Y if and only if old_wiring[Y] = X
//
void reverse_wiring(Wiring new_wiring, Wiring old_wiring, usize wiring_len) {
    for(usize i = 0; i < wiring_len; i++) {
        new_wiring[old_wiring[i]] = (u8)i;
    }
}

// --------------------------------------------------------------
// DESTRUCTION LOGIC

// https://www.cryptomuseum.com/crypto/enigma/wiring.htm
void init_rotor(Rotor* r, const char* rotor_name, const u8 position, const u8 ring) {
    u8 found = 0;

    // Here we assume that
    for(usize i = 0; i < KNOWN_ROTORS_LENGTH; i++) {
        if(strcmp(KNOWN_ROTORS[i].name, rotor_name) == 0) {
            found = 1;

            char* known_name = KNOWN_ROTORS[i].name;
            char* known_wiring = KNOWN_ROTORS[i].wiring;
            u8* known_notch = KNOWN_ROTORS[i].notch;

            init_wiring(r->forward_wiring, known_wiring, ALPHABET_SIZE);
            reverse_wiring(r->backward_wiring, r->forward_wiring, ALPHABET_SIZE);

            memcpy(r->notch, known_notch, 2);
            r->position = position;
            r->ring = ring;

            // copy also NULL-terminating byte
            memcpy(r->name, known_name, strlen(known_name) + 1);

            break;
        }
    }

    if(!found) {
        assert(0 && "init_rotor(): unsupported rotor\n");
    }
}

// We specify rotors in the init array from left to right. Given
// however that the most frequent rotor is the right-most rotor, we
// save that on index-0.
void init_rotors(
    Enigma* e,
    const char* rotor_names[ROTORS_N],
    const u8 rotor_positions[ROTORS_N],
    const u8 rotor_ring_settings[ROTORS_N]) {
    for(usize i = 0; i < ROTORS_N; i++) {
        init_rotor(
            &e->rotors[i],
            rotor_names[ROTORS_N - 1 - i],
            rotor_positions[ROTORS_N - 1 - i],
            rotor_ring_settings[ROTORS_N - 1 - i]);
    }
}

void init_reflector(Enigma* e, const char* reflector_name) {
    u8 found = 0;

    for(usize i = 0; i < KNOWN_REFLECTORS_LENGTH; i++) {
        if(strcmp(KNOWN_REFLECTORS[i].name, reflector_name) == 0) {
            found = 1;

            char* known_name = KNOWN_REFLECTORS[i].name;
            char* known_wiring = KNOWN_REFLECTORS[i].wiring;

            init_wiring(e->reflector.wiring, known_wiring, ALPHABET_SIZE);

            // copy also NULL-terminating byte
            memcpy(e->reflector.name, known_name, strlen(known_name) + 1);
        }
    }

    if(!found) {
        assert(0 && "init_reflector(): unsupported reflector\n");
    }
}

void init_plugboard(Enigma* e, u8 (*board)[2], usize plugboard_size) {
    e->plugboard.board_size = plugboard_size;
    for(usize i = 0; i < plugboard_size; i++) {
        e->plugboard.board[i][0] = CHAR2CODE(board[i][0]);
        e->plugboard.board[i][1] = CHAR2CODE(board[i][1]);
    }
}

Enigma* init_enigma(
    const char* rotor_names[ROTORS_N],
    const u8 rotor_positions[ROTORS_N],
    const u8 rotor_ring_settings[ROTORS_N],
    const char* reflector_name,
    u8 (*plugboard)[2],
    usize plugboard_size) {
    if(plugboard_size > PLUGBOARD_SIZE) {
        printf(
            "[ERROR]: init_enigma() - supplied plugboard size (%d) greater than maxium (%d)\n",
            plugboard_size,
            PLUGBOARD_SIZE);
        // exit(0);
        return NULL;
    }

    Enigma* e = calloc(1, sizeof(Enigma));

    init_rotors(e, rotor_names, rotor_positions, rotor_ring_settings);
    init_reflector(e, reflector_name);
    init_plugboard(e, plugboard, plugboard_size);

    return e;
}

// --------------------------------------------------------------
// DESTRUCTION LOGIC

void reset_plugboard(Enigma* e) {
    e->plugboard.board_size = 0;
}

void destroy_enigma(Enigma* e) {
    if(e) {
        free(e);
    }
}

// --------------------------------------------------------------
// CORE LOGIC

u8 apply_rotor(Rotor* r, u8 char_code, RotorOrder order) {
    char_code = (char_code - r->ring + r->position + ALPHABET_SIZE) % ALPHABET_SIZE;
    if(order == RO_FORWARD) {
        char_code = r->forward_wiring[char_code];
    } else if(order == RO_BACKWARD) {
        char_code = r->backward_wiring[char_code];
    } else {
        assert(0 && "Unreachable");
    }
    char_code = (char_code + r->ring - r->position + ALPHABET_SIZE) % ALPHABET_SIZE;

    return char_code;
}

void move_rotors(Enigma* e) {
    // https://en.wikipedia.org/wiki/Enigma_machine
    // https://www.youtube.com/watch?v=ds8HoowfewA
    //
    // Double stepping caused by the claw mechanism used for rotating
    // the rotors makes the second rotor move twice in a row, if the
    // first movement brings it in the turnover position during the
    // first rotation.
    //
    //  https://www.youtube.com/watch?v=5StZlF-clPc
    //  https://www.youtube.com/watch?v=hcVhQeZ5gI4
    //
    if(e->rotors[1].position == e->rotors[1].notch[0] ||
       e->rotors[1].position == e->rotors[1].notch[1]) {
        e->rotors[2].position = (e->rotors[2].position + 1) % ALPHABET_SIZE;
        e->rotors[1].position = (e->rotors[1].position + 1) % ALPHABET_SIZE;
    } else if(
        e->rotors[0].position == e->rotors[0].notch[0] ||
        e->rotors[0].position == e->rotors[0].notch[1]) {
        e->rotors[1].position = (e->rotors[1].position + 1) % ALPHABET_SIZE;
    }
    e->rotors[0].position = (e->rotors[0].position + 1) % ALPHABET_SIZE;
}

u8 apply_rotors(Enigma* e, u8 char_code, RotorOrder order) {
    switch(order) {
    case RO_FORWARD: {
        char_code = apply_rotor(&e->rotors[0], char_code, RO_FORWARD);
        char_code = apply_rotor(&e->rotors[1], char_code, RO_FORWARD);
        char_code = apply_rotor(&e->rotors[2], char_code, RO_FORWARD);
    } break;

    case RO_BACKWARD: {
        char_code = apply_rotor(&e->rotors[2], char_code, RO_BACKWARD);
        char_code = apply_rotor(&e->rotors[1], char_code, RO_BACKWARD);
        char_code = apply_rotor(&e->rotors[0], char_code, RO_BACKWARD);
    } break;

    default:
        assert(0 && "apply_rotors(): Unreachable");
    }

    return char_code;
}

u8 apply_plugboard(Enigma* e, const u8 plaintext_code) {
    for(usize i = 0; i < e->plugboard.board_size; i++) {
        if(plaintext_code == e->plugboard.board[i][0]) {
            return e->plugboard.board[i][1];
        } else if(plaintext_code == e->plugboard.board[i][1]) {
            return e->plugboard.board[i][0];
        }
    }
    return plaintext_code;
}

u8 apply_reflector(Enigma* e, const u8 plaintext_code) {
    return e->reflector.wiring[plaintext_code];
}

void apply_enigma(Enigma* e, const u8* input, usize input_len, u8* output) {
    // Assumes output has been already allocated with a null-terminating
    // string and that len(output) == input_len.

    for(usize i = 0; i < input_len; i++) {
        u8 input_char = input[i];

        // Transform character into char_code
        u8 char_code = CHAR2CODE(input_char);

        // Movement is executed before encryption
        move_rotors(e);

        // FORWARD PASS
        char_code = apply_plugboard(e, char_code);
        char_code = apply_rotors(e, char_code, RO_FORWARD);

        // REFLECTOR
        char_code = apply_reflector(e, char_code);

        // BACKWARD PASS
        char_code = apply_rotors(e, char_code, RO_BACKWARD);
        char_code = apply_plugboard(e, char_code);

        // Transform char_code into character
        u8 output_char = CODE2CHAR(char_code);

        output[i] = output_char;
    }
}

void enigma_encrypt(Enigma* e, const char* plaintext, usize plaintext_len, char* ciphertext) {
    assert(
        plaintext_len == strlen(ciphertext) &&
        "enigma_encrypt(): strlen(ciphertext) != plaintext_len");
    apply_enigma(e, (const u8*)plaintext, plaintext_len, (u8*)ciphertext);
#ifdef ENIGMA_DEBUG
    printf("[INFO] enigma_encrypt(): '%s' -> '%s'\n", plaintext, ciphertext);
#endif
}

void enigma_decrypt(Enigma* e, const char* ciphertext, usize ciphertext_len, char* plaintext) {
    assert(
        ciphertext_len == strlen(plaintext) &&
        "enigma_encrypt(): strlen(ciphertext) != plaintext_len");
    apply_enigma(e, (const u8*)ciphertext, ciphertext_len, (u8*)plaintext);
#ifdef ENIGMA_DEBUG
    printf("[INFO] enigma_decrypt(): '%s' -> '%s'\n", ciphertext, plaintext);
#endif
}

#endif // ENIGMA_IMPLEMENTATION