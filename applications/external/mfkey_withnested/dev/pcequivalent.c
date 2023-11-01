#pragma GCC push_options
#pragma GCC optimize ("unroll-loops")
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MSB_LIMIT 16
#define LF_POLY_ODD (0x29CE5C)
#define LF_POLY_EVEN (0x870804)
#define CONST_M1_1 (LF_POLY_EVEN << 1 | 1)
#define CONST_M2_1 (LF_POLY_ODD << 1)
#define CONST_M1_2 (LF_POLY_ODD)
#define CONST_M2_2 (LF_POLY_EVEN << 1 | 1)
#define BIT(x, n) ((x) >> (n) & 1)
#define BEBIT(x, n) BIT(x, (n) ^ 24)
#define SWAPENDIAN(x) (x = (x >> 8 & 0xff00ff) | (x & 0xff00ff) << 8, x = x >> 16 | x << 16)
#define SIZEOF(arr) sizeof(arr) / sizeof(*arr)
struct Crypto1State {uint32_t odd, even;};
struct Crypto1Params {uint64_t key;uint32_t nr0_enc, uid_xor_nt0, uid_xor_nt1, nr1_enc, p64b, ar1_enc;};
uint32_t prng_successor(uint32_t x, uint32_t n) {
    SWAPENDIAN(x);
    while (n--)
        x = x >> 1 | (x >> 16 ^ x >> 18 ^ x >> 19 ^ x >> 21) << 31;
    return SWAPENDIAN(x);
}
static inline int filter(uint32_t const x) {
    uint32_t f;
    f  = 0xf22c0 >> (x       & 0xf) & 16;
    f |= 0x6c9c0 >> (x >>  4 & 0xf) &  8;
    f |= 0x3c8b0 >> (x >>  8 & 0xf) &  4;
    f |= 0x1e458 >> (x >> 12 & 0xf) &  2;
    f |= 0x0d938 >> (x >> 16 & 0xf) &  1;
    return BIT(0xEC57E80A, f);
}
static inline uint8_t evenparity32(uint32_t x) {
    return (__builtin_parity(x) & 0xFF);
}
void update_contribution(int data[], int item, int mask1, int mask2) {
    int p = data[item] >> 25;
    p = p << 1 | evenparity32(data[item] & mask1);
    p = p << 1 | evenparity32(data[item] & mask2);
    data[item] = p << 24 | (data[item] & 0xffffff);
}
void crypto1_get_lfsr(struct Crypto1State *state, uint64_t *lfsr) {
    int i;
    for (*lfsr = 0, i = 23; i >= 0; --i) {
        *lfsr = *lfsr << 1 | BIT(state->odd, i ^ 3);
        *lfsr = *lfsr << 1 | BIT(state->even, i ^ 3);
    }
}
uint8_t crypt_or_rollback_bit(struct Crypto1State *s, uint32_t in, int x, int is_crypt) {
    uint8_t ret;
    uint32_t feedin, t;
    if (is_crypt == 0) {
        s->odd &= 0xffffff;
        t = s->odd, s->odd = s->even, s->even = t;
    }
    ret = filter(s->odd);
    feedin = ret & (!!x);
    if (is_crypt == 0) {
        feedin ^= s->even & 1;
        feedin ^= LF_POLY_EVEN & (s->even >>= 1);
    } else {
        feedin ^= LF_POLY_EVEN & s->even;
    }
    feedin ^= LF_POLY_ODD & s->odd;
    feedin ^= !!in;
    if (is_crypt == 0) {
        s->even |= (evenparity32(feedin)) << 23;
    } else {
        s->even = s->even << 1 | (evenparity32(feedin));
        t = s->odd, s->odd = s->even, s->even = t;
    }
    return ret;
}
uint32_t crypt_or_rollback_word(struct Crypto1State *s, uint32_t in, int x, int is_crypt) {
    uint32_t ret = 0;
    int i;
    if (is_crypt == 0) {
        for (i = 31; i >= 0; i--) {
            ret |= crypt_or_rollback_bit(s, BEBIT(in, i), x, 0) << (24 ^ i);
        }
    } else {
        for (i = 0; i <= 31; i++) {
            ret |= crypt_or_rollback_bit(s, BEBIT(in, i), x, 1) << (24 ^ i);
        }
    }
    return ret;
}
int key_already_found_for_nonce(uint64_t *keyarray, size_t keyarray_size, uint32_t uid_xor_nt1, uint32_t nr1_enc, uint32_t p64b, uint32_t ar1_enc) {
    int k = 0, found = 0;
    for(k = 0; k < keyarray_size; k++) {
        struct Crypto1State temp = {0, 0};
        int i;
        for (i = 0; i < 24; i++) {
            (&temp)->odd |= (BIT(keyarray[k], 2*i+1) << (i ^ 3));
            (&temp)->even |= (BIT(keyarray[k], 2*i) << (i ^ 3));
        }
        crypt_or_rollback_word(&temp, uid_xor_nt1, 0, 1);
        crypt_or_rollback_word(&temp, nr1_enc, 1, 1);
        if (ar1_enc == (crypt_or_rollback_word(&temp, 0, 0, 1) ^ p64b)) {
            found = 1;
            break;
        }
    }
    return found;
}
int check_state(struct Crypto1State *t, struct Crypto1Params *p) {
    uint64_t key = 0;
    int found = 0;
    struct Crypto1State temp = {0, 0};
    if (t->odd | t->even) {
        crypt_or_rollback_word(t, 0, 0, 0);
        crypt_or_rollback_word(t, p->nr0_enc, 1, 0);
        crypt_or_rollback_word(t, p->uid_xor_nt0, 0, 0);
        temp.odd = t->odd;
        temp.even = t->even;
        crypt_or_rollback_word(t, p->uid_xor_nt1, 0, 1);
        crypt_or_rollback_word(t, p->nr1_enc, 1, 1);
        if (p->ar1_enc == (crypt_or_rollback_word(t, 0, 0, 1) ^ p->p64b)) {
            crypto1_get_lfsr(&temp, &key);
            p->key = key;
            found = 1;
        }
    }
    return found;
}


static inline int state_loop(unsigned int* states_buffer, int xks, int m1, int m2) {
    int states_tail = 0;
    int round = 0, s = 0, xks_bit = 0;

    for (round = 1; round <= 15; round++) {
        xks_bit = BIT(xks, round);

        for (s = 0; s <= states_tail; s++) {
            states_buffer[s] <<= 1;

            if ((filter(states_buffer[s]) ^ filter(states_buffer[s] | 1)) != 0) {
                states_buffer[s] |= filter(states_buffer[s]) ^ xks_bit;
                if (round > 4) {
                    update_contribution(states_buffer, s, m1, m2);
                }
            } else if (filter(states_buffer[s]) == xks_bit) {
                // TODO: Refactor
                if (round > 4) {
                    states_buffer[++states_tail] = states_buffer[s + 1];
                    states_buffer[s + 1] = states_buffer[s] | 1;
                    update_contribution(states_buffer, s, m1, m2);
                    s++;
                    update_contribution(states_buffer, s, m1, m2);
                } else {
                    states_buffer[++states_tail] = states_buffer[++s];
                    states_buffer[s] = states_buffer[s - 1] | 1;
                }
            } else {
                states_buffer[s--] = states_buffer[states_tail--];
            }
        }
    }

    return states_tail;
}

struct Msb {
    int tail;
    int states[512];
};

int calculate_msb_tables(int oks, int eks, int msb_round, struct Crypto1Params *p) {
    // TODO: Combine Odd and Even loops
    int msb_head = (MSB_LIMIT * msb_round); // msb_iter ranges from 0 to (256/MSB_LIMIT)-1
    int msb_tail = (MSB_LIMIT * (msb_round+1));
    unsigned int *states_buffer = malloc(sizeof(unsigned int)*(2<<9));
    int states_tail = 0;
    int i = 0, j = 0, y = 0, semi_state = 0, tail = 0, found = 0;
    unsigned int msb = 0;
    struct Crypto1State temp = {0, 0};

    //FURI_LOG_I(TAG, "MSB GO %i", msb_iter); // DEBUG

    struct Msb *odd_msbs  = (struct Msb*)malloc(MSB_LIMIT * sizeof(struct Msb));
    struct Msb *even_msbs = (struct Msb*)malloc(MSB_LIMIT * sizeof(struct Msb));
    memset(odd_msbs, 0, MSB_LIMIT * sizeof(struct Msb));
    memset(even_msbs, 0, MSB_LIMIT * sizeof(struct Msb));

    // Odd
    for (semi_state = 1 << 20; semi_state >= 0; semi_state--) {
        //if (main_iter % 2048 == 0) {
        //    FURI_LOG_I(TAG, "On main_iter %i", main_iter); // DEBUG
        //}
        if (filter(semi_state) == (oks & 1)) {
            states_buffer[0] = semi_state;
            states_tail = state_loop(states_buffer, oks, CONST_M1_1, CONST_M2_1);

            for (i = 0; i <= states_tail; i++) {
                msb = states_buffer[i] >> 24;
                if ((msb >= msb_head) && (msb < msb_tail)) {
                    found = 0;
                    for(j = 0; j < odd_msbs[msb - msb_head].tail; j++) {
                        if(odd_msbs[msb - msb_head].states[j] == states_buffer[i]) {
                            found = 1;
                            break;
                        }
                    }

                    if (!found) {
                        tail = odd_msbs[msb - msb_head].tail++;
                        odd_msbs[msb - msb_head].states[tail] = states_buffer[i];
                    }
                }
            }
        }
    }

    //FURI_LOG_I(TAG, "MSB GE %i", msb_iter); // DEBUG

    // Even
    for (semi_state = 1 << 20; semi_state >= 0; semi_state--) {
        if (filter(semi_state) == (eks & 1)) {
            states_buffer[0] = semi_state;
            states_tail = state_loop(states_buffer, eks, CONST_M1_2, CONST_M2_2);

            for (i = 0; i <= states_tail; i++) {
                msb = states_buffer[i] >> 24;
                if ((msb >= msb_head) && (msb < msb_tail)) {
                    found = 0;

                    for(j = 0; j < even_msbs[msb - msb_head].tail; j++) {
                        if(even_msbs[msb - msb_head].states[j] == states_buffer[i]) {
                            found = 1;
                            break;
                        }
                    }

                    if (found)
                        continue;

                    tail = even_msbs[msb - msb_head].tail++;
                    even_msbs[msb - msb_head].states[tail] = states_buffer[i];
                    states_buffer[i] = (states_buffer[i] << 1) ^ evenparity32(states_buffer[i] & LF_POLY_EVEN);
                    // Check against all odd states for MSB
                    for(y = 0; y <= odd_msbs[msb - msb_head].tail; y++) {
                        temp.even = odd_msbs[msb - msb_head].states[y];
                        temp.odd = states_buffer[i] ^ evenparity32(odd_msbs[msb - msb_head].states[y] & LF_POLY_ODD);
                        if (check_state(&temp, p)) {
                            free(states_buffer);
                            free(odd_msbs);
                            free(even_msbs);
                            return 1;
                        }
                    }
                }
            }
        }
    }

    free(states_buffer);
    free(odd_msbs);
    free(even_msbs);
    return 0;
}

int recover(struct Crypto1Params *p, int ks2) {
    int oks = 0, eks = 0;
    int i = 0, msb = 0;
    for (i = 31; i >= 0; i -= 2) {
        oks = oks << 1 | BEBIT(ks2, i);
    }
    for (i = 30; i >= 0; i -= 2) {
        eks = eks << 1 | BEBIT(ks2, i);
    }
    for (msb = 0; msb <= ((256/MSB_LIMIT)-1); msb++) {
        //printf("MSB: %i\n", msb);
        if (calculate_msb_tables(oks, eks, msb, p)) {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    struct Crypto1State *s, *t;
    uint64_t found_key; // recovered key
    uint32_t uid;     // serial number
    uint32_t nt0;      // tag challenge first
    uint32_t nt1;      // tag challenge second
    uint32_t nr0_enc; // first encrypted reader challenge
    uint32_t ar0_enc; // first encrypted reader response
    uint32_t nr1_enc; // second encrypted reader challenge
    uint32_t ar1_enc; // second encrypted reader response
    int i;
    char *rest;
    char *token;
    size_t keyarray_size;
    uint64_t *keyarray = malloc(sizeof(uint64_t)*1);

    keyarray_size = 0;
    FILE* filePointer;
    int bufferLength = 255;
    char buffer[bufferLength];
    filePointer = fopen(".mfkey32.log", "r"); // TODO: In FAP, use full path

    while(fgets(buffer, bufferLength, filePointer)) {
        rest = buffer;
        for (i = 0; i <= 17; i++) {
            token = strtok_r(rest, " ", &rest);
            switch(i){
                case 5: sscanf(token, "%x", &uid); break;
                case 7: sscanf(token, "%x", &nt0); break;
                case 9: sscanf(token, "%x", &nr0_enc); break;
                case 11: sscanf(token, "%x", &ar0_enc); break;
                case 13: sscanf(token, "%x", &nt1); break;
                case 15: sscanf(token, "%x", &nr1_enc); break;
                case 17: sscanf(token, "%x", &ar1_enc); break;
                default: break; // Do nothing
            }
        }
        uint32_t p64 = prng_successor(nt0, 64);
        uint32_t p64b = prng_successor(nt1, 64);
        struct Crypto1Params p = {0, nr0_enc, uid ^ nt0, uid ^ nt1, nr1_enc, p64b, ar1_enc};
        if (recover(&p, ar0_enc ^ p64) == 0) {
            // No key found in recover()
            continue;
        }
        found_key = p.key;
        int found = 0;
        for(i = 0; i < keyarray_size; i++) {
            if (keyarray[i] == found_key) {
                found = 1;
                break;
            }
        }
        if (found == 0) {
            // Not already found
            keyarray = realloc(keyarray, sizeof(uint64_t)*(keyarray_size+1));
            keyarray_size += 1;
            keyarray[keyarray_size-1] = found_key;
        }
    }

    fclose(filePointer);

    printf("Keys found: %li\n", keyarray_size);
    printf("Unique keys found:\n");
    for(i = 0; i < keyarray_size; i++) {
        printf("%012" PRIx64 , keyarray[i]);
        printf("\n");
    }
    // TODO: Append key to user dictionary file if missing in file
    return 0;
}
