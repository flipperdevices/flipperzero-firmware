// TODO: How is it narrowing the state search space in the original?
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
int* extend_table_precompute_msb(int head, int *tail, int xks, int iterations, int m1, int m2, char msb) {
    int *data = malloc(sizeof(int) * (5 << 12));

    for (int main_iter = 1 << 20; main_iter >= 0; --main_iter) {
        if (filter(main_iter) == (xks & 1)) {
            int *temp_states_buffer = malloc(sizeof(int)*(2<<12)); // TODO: Set to 8192 for testing
            int temp_states_tail = 0;

            temp_states_buffer[0] = main_iter;

            for (int extend_iter = 1; extend_iter <= iterations; extend_iter++) {
                int xks_bit = BIT(xks, extend_iter);

                for (int s = 0; s <= temp_states_tail; s++) {
                    temp_states_buffer[s] <<= 1;
                    int t = temp_states_buffer[s];

                    if ((filter(t) ^ filter(t | 1)) != 0 ) {
                        temp_states_buffer[s] |= filter(t) ^ xks_bit;
                        if (extend_iter > 4) {
                            update_contribution(temp_states_buffer, s, m1, m2);
                        }
                    } else if (filter(t) == xks_bit) {
                        // TODO: Refactor
                        if (extend_iter > 4) {
                            temp_states_buffer[++temp_states_tail] = temp_states_buffer[s + 1];
                            temp_states_buffer[s + 1] = temp_states_buffer[s] | 1;
                            update_contribution(temp_states_buffer, s, m1, m2);
                            s++;
                            update_contribution(temp_states_buffer, s, m1, m2);
                        } else {
                            temp_states_buffer[++temp_states_tail] = temp_states_buffer[++s];
                            temp_states_buffer[s] = temp_states_buffer[ s - 1 ] | 1;
                        }
                    } else {
                        temp_states_buffer[s--] = temp_states_buffer[temp_states_tail--];
                    }
                }
            }

            for (int s = 0; s <= temp_states_tail; s++) {
                char s_msb = temp_states_buffer[s] >> 24;
                if (s_msb == msb) {
                    data[*tail] = temp_states_buffer[s];
                    (*tail)++;
                }
            }

            free(temp_states_buffer);
        }
    }

    return data;
}
int recover(struct Crypto1Params *p, int ks2) {
    int o, e, i, x, y;
    int o_head = 0, o_tail = 0, oks = 0, e_head = 0, e_tail = 0, eks = 0;
    for (i = 31; i >= 0; i -= 2) {
        oks = oks << 1 | BEBIT(ks2, i);
    }
    for (i = 30; i >= 0; i -= 2) {
        eks = eks << 1 | BEBIT(ks2, i);
    }
    for (int msb = 0x00; msb <= 0xff; msb++) {
        printf("Current MSB value: %i\n", msb);
        o_tail = 0;
        e_tail = 0;
        int *odd = extend_table_precompute_msb(e_head, &o_tail, oks, 15, CONST_M1_1, CONST_M2_1, (char) msb);
        int *even = extend_table_precompute_msb(e_head, &e_tail, eks, 15, CONST_M1_2, CONST_M2_2, (char) msb);
        printf("o_tail: %i, e_tail: %i\n", o_tail, e_tail);
        printf("Have %i permutations to search\n", ((e_tail+1)*(o_tail+1)));
        for (x = e_head; x <= e_tail; ++x) {
            even[x] = (even[x] << 1) ^ evenparity32(even[x] & LF_POLY_EVEN);
            for (y = o_head; y <= o_tail; ++y) {
                struct Crypto1State temp = {0, 0};
                temp.even = odd[y];
                temp.odd = even[x] ^ evenparity32(odd[y] & LF_POLY_ODD);
                if (check_state(&temp, p)) {
                    free(odd);
                    free(even);
                    return 1;
                }
            }
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
