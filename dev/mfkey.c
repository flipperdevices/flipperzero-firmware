#pragma GCC push_options
#pragma GCC optimize ("unroll-loops")
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MSB_LIMIT 16
//#define MSB_LIMIT 256
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
static const char table[256] = {
0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};
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
    if ((table[x & 0xff] + table[(x >> 8) & 0xff] + table[(x >> 16) & 0xff] + table[x >> 24]) % 2 == 0) {
        return 0;
    } else {
        return 1;
    }
    //return ((table[x & 0xff] + table[(x >> 8) & 0xff] + table[(x >> 16) & 0xff] + table[x >> 24]) % 2) & 0xFF;
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
uint32_t crypt_word(struct Crypto1State *s) {
    // "in" and "x" are always 0 (last iteration)
    uint32_t res_ret = 0;
    uint32_t feedin, t;
    for (int i = 0; i <= 31; i++) {
        res_ret |= (filter(s->odd) << (24 ^ i));
        feedin = 0;
        feedin ^= LF_POLY_EVEN & s->even;
        feedin ^= LF_POLY_ODD & s->odd;
        s->even = s->even << 1 | (evenparity32(feedin));
        t = s->odd, s->odd = s->even, s->even = t;
    }
    return res_ret;
}
void crypt_word_noret(struct Crypto1State *s, uint32_t in, int x) {
    uint8_t ret;
    uint32_t feedin, t, next_in;
    for (int i = 0; i <= 31; i++) {
        next_in = BEBIT(in, i);
        ret = filter(s->odd);
        feedin = ret & (!!x);
        feedin ^= LF_POLY_EVEN & s->even;
        feedin ^= LF_POLY_ODD & s->odd;
        feedin ^= !!next_in;
        s->even = s->even << 1 | (evenparity32(feedin));
        t = s->odd, s->odd = s->even, s->even = t;
    }
    return;
}
void rollback_word_noret(struct Crypto1State *s, uint32_t in, int x) {
    uint8_t ret;
    uint32_t feedin, t, next_in;
    for (int i = 31; i >= 0; i--) {
        next_in = BEBIT(in, i);
        s->odd &= 0xffffff;
        t = s->odd, s->odd = s->even, s->even = t;
        ret = filter(s->odd);
        feedin = ret & (!!x);
        feedin ^= s->even & 1;
        feedin ^= LF_POLY_EVEN & (s->even >>= 1);
        feedin ^= LF_POLY_ODD & s->odd;
        feedin ^= !!next_in;
        s->even |= (evenparity32(feedin)) << 23;

    }
    return;
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
        crypt_word_noret(&temp, uid_xor_nt1, 0);
        crypt_word_noret(&temp, nr1_enc, 1);
        if (ar1_enc == (crypt_word(&temp) ^ p64b)) {
            found = 1;
            break;
        }
    }
    return found;
}
int check_state(struct Crypto1State *t, struct Crypto1Params *p) {
    //printf("check_state: %i, %i\n", t->odd, t->even);
    if (!(t->odd | t->even)) return 0;
    rollback_word_noret(t, 0, 0);
    rollback_word_noret(t, p->nr0_enc, 1);
    rollback_word_noret(t, p->uid_xor_nt0, 0);
    struct Crypto1State temp = {t->odd, t->even};
    crypt_word_noret(t, p->uid_xor_nt1, 0);
    crypt_word_noret(t, p->nr1_enc, 1);
    if (p->ar1_enc == (crypt_word(t) ^ p->p64b)) {
        crypto1_get_lfsr(&temp, &(p->key));
        return 1;
    }
    return 0;
}


static inline int state_loop(unsigned int* states_buffer, int xks, int m1, int m2) {
    int states_tail = 0;
    int round = 0, s = 0, xks_bit = 0;

    for (round = 1; round <= 8; round++) {
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
    //for (int i = 0; i <= states_tail; i++) {
    //        printf("states_buffer[%d] = %u\n", i, states_buffer[i]);
    //}

    return states_tail;
}

struct Msb {
    int tail;
    int states[4096];
};

int binsearch(int data[], int start, int stop) {
    int mid, val = data[stop] & 0xff000000;
    while (start != stop) {
        mid = (stop - start) >> 1;
        if ((data[start + mid] ^ 0x80000000) > (val ^ 0x80000000))
            stop = start + mid;
        else
            start += mid + 1;
    }
    return start;
}
void quicksort(int array[], int low, int high) {
    if (SIZEOF(array) == 0)
        return;
    if (low >= high)
        return;
    int middle = low + (high - low) / 2;
    int pivot = array[middle];
    int i = low, j = high;
    while (i <= j) {
        while (array[i] < pivot) {
            i++;
        }
        while (array[j] > pivot) {
            j--;
        }
        if (i <= j) { // swap
            int temp = array[i];
            array[i] = array[j];
            array[j] = temp;
            i++;
            j--;
        }
    }
    if (low < j) {
        quicksort(array, low, j);
    }
    if (high > i) {
        quicksort(array, i, high);
    }
}
int extend_table(int data[], int tbl, int end, int bit, int m1, int m2) {
    for (data[tbl] <<= 1; tbl <= end; data[++tbl] <<= 1) {
        if ((filter(data[tbl]) ^ filter(data[tbl] | 1)) != 0) {
            data[tbl] |= filter(data[tbl]) ^ bit;
            update_contribution(data, tbl, m1, m2);
        } else if (filter(data[tbl]) == bit) {
            data[++end] = data[tbl + 1];
            data[tbl + 1] = data[tbl] | 1;
            update_contribution(data, tbl, m1, m2);
            tbl++;
            update_contribution(data, tbl, m1, m2);
        } else {
            data[tbl--] = data[end--];
        }
    }
    return end;
}

int old_recover(int odd[], int o_head, int o_tail, int oks, int even[], int e_head, int e_tail, int eks, int rem, int s, struct Crypto1Params *p, int first_run) {
    int o, e, i;
    if (rem == -1) {
        for (e = e_head; e <= e_tail; ++e) {
            even[e] = (even[e] << 1) ^ evenparity32(even[e] & LF_POLY_EVEN);
            for (o = o_head; o <= o_tail; ++o, ++s) {
                struct Crypto1State temp = {0, 0};
                temp.even = odd[o];
                temp.odd = even[e] ^ evenparity32(odd[o] & LF_POLY_ODD);
                if (check_state(&temp, p)) {
                    return -1;
                }
            }
        }
        return s;
    }
    if (first_run == 0) {
        for (i = 0; (i < 4) && (rem-- != 0); i++) {
            oks >>= 1;
            eks >>= 1;
            o_tail = extend_table(odd, o_head, o_tail, oks & 1, LF_POLY_EVEN << 1 | 1, LF_POLY_ODD << 1);
            if (o_head > o_tail)
                return s;
            e_tail = extend_table(even, e_head, e_tail, eks & 1, LF_POLY_ODD, LF_POLY_EVEN << 1 | 1);
            if (e_head > e_tail)
                return s;
        }
    }
    first_run = 0;
    quicksort(odd, o_head, o_tail);
    quicksort(even, e_head, e_tail);
    while (o_tail >= o_head && e_tail >= e_head) {
        if (((odd[o_tail] ^ even[e_tail]) >> 24) == 0) {
            o_tail = binsearch(odd, o_head, o = o_tail);
            e_tail = binsearch(even, e_head, e = e_tail);
            s = old_recover(odd, o_tail--, o, oks, even, e_tail--, e, eks, rem, s, p, first_run);
            if (s == -1) {
                break;
            }
        } else if ((odd[o_tail] ^ 0x80000000) > (even[e_tail] ^ 0x80000000)) {
            o_tail = binsearch(odd, o_head, o_tail) - 1;
        } else {
            e_tail = binsearch(even, e_head, e_tail) - 1;
        }
    }
    return s;
}

int calculate_msb_tables(int oks, int eks, int msb_round, struct Crypto1Params *p) {
    // TODO: Combine Odd and Even loops
    int msb_head = (MSB_LIMIT * msb_round); // msb_iter ranges from 0 to (256/MSB_LIMIT)-1
    int msb_tail = (MSB_LIMIT * (msb_round+1));
    unsigned int *states_buffer = malloc(sizeof(unsigned int)*(1<<20));
    int states_tail = 0, tail = 0;
    int i = 0, j = 0, y = 0, semi_state = 0, found = 0;
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

            for (i = states_tail; i >= 0; i--) {
                msb = states_buffer[i] >> 24;
                if ((msb >= msb_head) && (msb < msb_tail)) {
                    found = 0;
                    for(j = 0; j < odd_msbs[msb - msb_head].tail-1; j++) {
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

                    if (!found) {
                        tail = even_msbs[msb - msb_head].tail++;
                        even_msbs[msb - msb_head].states[tail] = states_buffer[i];
                    }
                }
            }
        }
    }

    oks >>= 8;
    eks >>= 8;

    for (i = 0; i < MSB_LIMIT; i++) {
        int res = old_recover(odd_msbs[i].states, 0, odd_msbs[i].tail, oks, even_msbs[i].states, 0, even_msbs[i].tail, eks, 7, 0, p, 1);
        if (res == -1) {
            free(states_buffer);
            free(odd_msbs);
            free(even_msbs);
            return 1;
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
    int keyarray_size;
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
