#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct bucket {
    uint32_t *head;
    uint32_t *bp;
} bucket_t;

typedef bucket_t bucket_array_t[2][0x100];

typedef struct bucket_info {
    struct {
        uint32_t *head, *tail;
    } bucket_info[2][0x100];
    uint32_t numbuckets;
} bucket_info_t;

struct Crypto1State {uint32_t odd, even;};

#define LF_POLY_ODD (0x29CE5C)
#define LF_POLY_EVEN (0x870804)
#define BIT(x, n) ((x) >> (n) & 1)
#define BEBIT(x, n) BIT(x, (n) ^ 24)

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

extern void bucket_sort_intersect(uint32_t *const estart, uint32_t *const estop,
                                  uint32_t *const ostart, uint32_t *const ostop,
                                  bucket_info_t *bucket_info, bucket_array_t bucket) {
    uint32_t *p1, *p2;
    uint32_t *start[2];
    uint32_t *stop[2];

    start[0] = estart;
    stop[0] = estop;
    start[1] = ostart;
    stop[1] = ostop;

    for (uint32_t i = 0; i < 2; i++) {
        for (uint32_t j = 0x00; j <= 0xff; j++) {
            bucket[i][j].bp = bucket[i][j].head;
        }
    }

    for (uint32_t i = 0; i < 2; i++) {
        for (p1 = start[i]; p1 <= stop[i]; p1++) {
            uint32_t bucket_index = (*p1 & 0xff000000) >> 24;
            *(bucket[i][bucket_index].bp++) = *p1;
        }
    }

    for (uint32_t i = 0; i < 2; i++) {
        p1 = start[i];
        uint32_t nonempty_bucket = 0;
        for (uint32_t j = 0x00; j <= 0xff; j++) {
            if (bucket[0][j].bp != bucket[0][j].head && bucket[1][j].bp != bucket[1][j].head) { // non-empty intersecting buckets only
                bucket_info->bucket_info[i][nonempty_bucket].head = p1;
                for (p2 = bucket[i][j].head; p2 < bucket[i][j].bp; *p1++ = *p2++);
                bucket_info->bucket_info[i][nonempty_bucket].tail = p1 - 1;
                nonempty_bucket++;
            }
        }
        bucket_info->numbuckets = nonempty_bucket;
    }
}

#if !defined LOWMEM && defined __GNUC__
static uint8_t filterlut[1 << 20];
static void __attribute__((constructor)) fill_lut(void) {
    uint32_t i;
    for (i = 0; i < 1 << 20; ++i)
        filterlut[i] = filter(i);
}
#define filter(x) (filterlut[(x) & 0xfffff])
#endif

static inline void update_contribution(uint32_t *item, const uint32_t mask1, const uint32_t mask2) {
    uint32_t p = *item >> 25;

    p = p << 1 | (evenparity32(*item & mask1));
    p = p << 1 | (evenparity32(*item & mask2));
    *item = p << 24 | (*item & 0xffffff);
}

static inline void extend_table(uint32_t *tbl, uint32_t **end, int bit, int m1, int m2, uint32_t in) {
    in <<= 24;
    for (*tbl <<= 1; tbl <= *end; *++tbl <<= 1)
        if (filter(*tbl) ^ filter(*tbl | 1)) {
            *tbl |= filter(*tbl) ^ bit;
            update_contribution(tbl, m1, m2);
            *tbl ^= in;
        } else if (filter(*tbl) == bit) {
            *++*end = tbl[1];
            tbl[1] = tbl[0] | 1;
            update_contribution(tbl, m1, m2);
            *tbl++ ^= in;
            update_contribution(tbl, m1, m2);
            *tbl ^= in;
        } else
            *tbl-- = *(*end)--;
}

static inline void extend_table_simple(uint32_t *tbl, uint32_t **end, int bit) {
    for (*tbl <<= 1; tbl <= *end; *++tbl <<= 1) {
        if (filter(*tbl) ^ filter(*tbl | 1)) { // replace
            *tbl |= filter(*tbl) ^ bit;
        } else if (filter(*tbl) == bit) {     // insert
            *++*end = *++tbl;
            *tbl = tbl[-1] | 1;
        } else {                              // drop
            *tbl-- = *(*end)--;
        }
    }
}

static struct Crypto1State *
recover(uint32_t *o_head, uint32_t *o_tail, uint32_t oks,
        uint32_t *e_head, uint32_t *e_tail, uint32_t eks, int rem,
        struct Crypto1State *sl, uint32_t in, bucket_array_t bucket) {
    bucket_info_t bucket_info;

    if (rem == -1) {
        for (uint32_t *e = e_head; e <= e_tail; ++e) {
            *e = *e << 1 ^ (evenparity32(*e & LF_POLY_EVEN)) ^ (!!(in & 4));
            for (uint32_t *o = o_head; o <= o_tail; ++o, ++sl) {
                sl->even = *o;
                sl->odd = *e ^ (evenparity32(*o & LF_POLY_ODD));
                sl[1].odd = sl[1].even = 0;
            }
        }
        return sl;
    }

    for (uint32_t i = 0; i < 4 && rem--; i++) {
        oks >>= 1;
        eks >>= 1;
        in >>= 2;
        extend_table(o_head, &o_tail, oks & 1, LF_POLY_EVEN << 1 | 1, LF_POLY_ODD << 1, 0);
        if (o_head > o_tail)
            return sl;

        extend_table(e_head, &e_tail, eks & 1, LF_POLY_ODD, LF_POLY_EVEN << 1 | 1, in & 3);
        if (e_head > e_tail)
            return sl;
    }

    bucket_sort_intersect(e_head, e_tail, o_head, o_tail, &bucket_info, bucket);

    for (int i = bucket_info.numbuckets - 1; i >= 0; i--) {
        sl = recover(bucket_info.bucket_info[1][i].head, bucket_info.bucket_info[1][i].tail, oks,
                     bucket_info.bucket_info[0][i].head, bucket_info.bucket_info[0][i].tail, eks,
                     rem, sl, in, bucket);
    }

    return sl;
}


#if !defined(__arm__) || defined(__linux__) || defined(_WIN32) || defined(__APPLE__) // bare metal ARM Proxmark lacks malloc()/free()
struct Crypto1State *lfsr_recovery32(uint32_t ks2, uint32_t in) {
    struct Crypto1State *statelist;
    uint32_t *odd_head = 0, *odd_tail = 0, oks = 0;
    uint32_t *even_head = 0, *even_tail = 0, eks = 0;
    int i;

    for (i = 31; i >= 0; i -= 2)
        oks = oks << 1 | BEBIT(ks2, i);
    for (i = 30; i >= 0; i -= 2)
        eks = eks << 1 | BEBIT(ks2, i);

    odd_head = odd_tail = calloc(1, sizeof(uint32_t) << 21);
    even_head = even_tail = calloc(1, sizeof(uint32_t) << 21);
    statelist =  calloc(1, sizeof(struct Crypto1State) << 18);
    if (!odd_tail-- || !even_tail-- || !statelist) {
        free(statelist);
        statelist = 0;
        goto out;
    }

    statelist->odd = statelist->even = 0;

    bucket_array_t bucket;

    for (i = 0; i < 2; i++) {
        for (uint32_t j = 0; j <= 0xff; j++) {
            bucket[i][j].head = calloc(1, sizeof(uint32_t) << 14);
            if (!bucket[i][j].head) {
                goto out;
            }
        }
    }

    for (i = 1 << 20; i >= 0; --i) {
        if (filter(i) == (oks & 1))
            *++odd_tail = i;
        if (filter(i) == (eks & 1))
            *++even_tail = i;
    }

    for (i = 0; i < 4; i++) {
        extend_table_simple(odd_head,  &odd_tail, (oks >>= 1) & 1);
        extend_table_simple(even_head, &even_tail, (eks >>= 1) & 1);
    }

    in = (in >> 16 & 0xff) | (in << 16) | (in & 0xff00); // Byte swapping
    recover(odd_head, odd_tail, oks, even_head, even_tail, eks, 11, statelist, in << 1, bucket);

out:
    for (i = 0; i < 2; i++)
        for (uint32_t j = 0; j <= 0xff; j++)
            free(bucket[i][j].head);
    free(odd_head);
    free(even_head);
    return statelist;
}
#endif

uint8_t lfsr_rollback_bit(struct Crypto1State *s, uint32_t in, int fb) {
    int out;
    uint8_t ret;
    uint32_t t;

    s->odd &= 0xffffff;
    t = s->odd, s->odd = s->even, s->even = t;

    out = s->even & 1;
    out ^= LF_POLY_EVEN & (s->even >>= 1);
    out ^= LF_POLY_ODD & s->odd;
    out ^= !!in;
    out ^= (ret = filter(s->odd)) & (!!fb);

    s->even |= (evenparity32(out)) << 23;
    return ret;
}

uint32_t lfsr_rollback_word(struct Crypto1State *s, uint32_t in, int fb) {
    uint32_t ret = 0;
    ret |= lfsr_rollback_bit(s, BEBIT(in, 31), fb) << (24 ^ 31);
    ret |= lfsr_rollback_bit(s, BEBIT(in, 30), fb) << (24 ^ 30);
    ret |= lfsr_rollback_bit(s, BEBIT(in, 29), fb) << (24 ^ 29);
    ret |= lfsr_rollback_bit(s, BEBIT(in, 28), fb) << (24 ^ 28);
    ret |= lfsr_rollback_bit(s, BEBIT(in, 27), fb) << (24 ^ 27);
    ret |= lfsr_rollback_bit(s, BEBIT(in, 26), fb) << (24 ^ 26);
    ret |= lfsr_rollback_bit(s, BEBIT(in, 25), fb) << (24 ^ 25);
    ret |= lfsr_rollback_bit(s, BEBIT(in, 24), fb) << (24 ^ 24);

    ret |= lfsr_rollback_bit(s, BEBIT(in, 23), fb) << (24 ^ 23);
    ret |= lfsr_rollback_bit(s, BEBIT(in, 22), fb) << (24 ^ 22);
    ret |= lfsr_rollback_bit(s, BEBIT(in, 21), fb) << (24 ^ 21);
    ret |= lfsr_rollback_bit(s, BEBIT(in, 20), fb) << (24 ^ 20);
    ret |= lfsr_rollback_bit(s, BEBIT(in, 19), fb) << (24 ^ 19);
    ret |= lfsr_rollback_bit(s, BEBIT(in, 18), fb) << (24 ^ 18);
    ret |= lfsr_rollback_bit(s, BEBIT(in, 17), fb) << (24 ^ 17);
    ret |= lfsr_rollback_bit(s, BEBIT(in, 16), fb) << (24 ^ 16);

    ret |= lfsr_rollback_bit(s, BEBIT(in, 15), fb) << (24 ^ 15);
    ret |= lfsr_rollback_bit(s, BEBIT(in, 14), fb) << (24 ^ 14);
    ret |= lfsr_rollback_bit(s, BEBIT(in, 13), fb) << (24 ^ 13);
    ret |= lfsr_rollback_bit(s, BEBIT(in, 12), fb) << (24 ^ 12);
    ret |= lfsr_rollback_bit(s, BEBIT(in, 11), fb) << (24 ^ 11);
    ret |= lfsr_rollback_bit(s, BEBIT(in, 10), fb) << (24 ^ 10);
    ret |= lfsr_rollback_bit(s, BEBIT(in, 9), fb) << (24 ^ 9);
    ret |= lfsr_rollback_bit(s, BEBIT(in, 8), fb) << (24 ^ 8);

    ret |= lfsr_rollback_bit(s, BEBIT(in, 7), fb) << (24 ^ 7);
    ret |= lfsr_rollback_bit(s, BEBIT(in, 6), fb) << (24 ^ 6);
    ret |= lfsr_rollback_bit(s, BEBIT(in, 5), fb) << (24 ^ 5);
    ret |= lfsr_rollback_bit(s, BEBIT(in, 4), fb) << (24 ^ 4);
    ret |= lfsr_rollback_bit(s, BEBIT(in, 3), fb) << (24 ^ 3);
    ret |= lfsr_rollback_bit(s, BEBIT(in, 2), fb) << (24 ^ 2);
    ret |= lfsr_rollback_bit(s, BEBIT(in, 1), fb) << (24 ^ 1);
    ret |= lfsr_rollback_bit(s, BEBIT(in, 0), fb) << (24 ^ 0);
    return ret;
}

#define SWAPENDIAN(x)\
    (x = (x >> 8 & 0xff00ff) | (x & 0xff00ff) << 8, x = x >> 16 | x << 16)

void crypto1_get_lfsr(struct Crypto1State *state, uint64_t *lfsr) {
    int i;
    for (*lfsr = 0, i = 23; i >= 0; --i) {
        *lfsr = *lfsr << 1 | BIT(state->odd, i ^ 3);
        *lfsr = *lfsr << 1 | BIT(state->even, i ^ 3);
    }
}
uint8_t crypto1_bit(struct Crypto1State *s, uint8_t in, int is_encrypted) {
    uint32_t feedin, t;
    uint8_t ret = filter(s->odd);

    feedin  = ret & (!!is_encrypted);
    feedin ^= !!in;
    feedin ^= LF_POLY_ODD & s->odd;
    feedin ^= LF_POLY_EVEN & s->even;
    s->even = s->even << 1 | (evenparity32(feedin));

    t = s->odd;
    s->odd = s->even;
    s->even = t;

    return ret;
}
uint32_t crypto1_word(struct Crypto1State *s, uint32_t in, int is_encrypted) {
    uint32_t ret = 0;
    ret |= crypto1_bit(s, BEBIT(in, 0), is_encrypted) << (24 ^ 0);
    ret |= crypto1_bit(s, BEBIT(in, 1), is_encrypted) << (24 ^ 1);
    ret |= crypto1_bit(s, BEBIT(in, 2), is_encrypted) << (24 ^ 2);
    ret |= crypto1_bit(s, BEBIT(in, 3), is_encrypted) << (24 ^ 3);
    ret |= crypto1_bit(s, BEBIT(in, 4), is_encrypted) << (24 ^ 4);
    ret |= crypto1_bit(s, BEBIT(in, 5), is_encrypted) << (24 ^ 5);
    ret |= crypto1_bit(s, BEBIT(in, 6), is_encrypted) << (24 ^ 6);
    ret |= crypto1_bit(s, BEBIT(in, 7), is_encrypted) << (24 ^ 7);

    ret |= crypto1_bit(s, BEBIT(in, 8), is_encrypted) << (24 ^ 8);
    ret |= crypto1_bit(s, BEBIT(in, 9), is_encrypted) << (24 ^ 9);
    ret |= crypto1_bit(s, BEBIT(in, 10), is_encrypted) << (24 ^ 10);
    ret |= crypto1_bit(s, BEBIT(in, 11), is_encrypted) << (24 ^ 11);
    ret |= crypto1_bit(s, BEBIT(in, 12), is_encrypted) << (24 ^ 12);
    ret |= crypto1_bit(s, BEBIT(in, 13), is_encrypted) << (24 ^ 13);
    ret |= crypto1_bit(s, BEBIT(in, 14), is_encrypted) << (24 ^ 14);
    ret |= crypto1_bit(s, BEBIT(in, 15), is_encrypted) << (24 ^ 15);

    ret |= crypto1_bit(s, BEBIT(in, 16), is_encrypted) << (24 ^ 16);
    ret |= crypto1_bit(s, BEBIT(in, 17), is_encrypted) << (24 ^ 17);
    ret |= crypto1_bit(s, BEBIT(in, 18), is_encrypted) << (24 ^ 18);
    ret |= crypto1_bit(s, BEBIT(in, 19), is_encrypted) << (24 ^ 19);
    ret |= crypto1_bit(s, BEBIT(in, 20), is_encrypted) << (24 ^ 20);
    ret |= crypto1_bit(s, BEBIT(in, 21), is_encrypted) << (24 ^ 21);
    ret |= crypto1_bit(s, BEBIT(in, 22), is_encrypted) << (24 ^ 22);
    ret |= crypto1_bit(s, BEBIT(in, 23), is_encrypted) << (24 ^ 23);

    ret |= crypto1_bit(s, BEBIT(in, 24), is_encrypted) << (24 ^ 24);
    ret |= crypto1_bit(s, BEBIT(in, 25), is_encrypted) << (24 ^ 25);
    ret |= crypto1_bit(s, BEBIT(in, 26), is_encrypted) << (24 ^ 26);
    ret |= crypto1_bit(s, BEBIT(in, 27), is_encrypted) << (24 ^ 27);
    ret |= crypto1_bit(s, BEBIT(in, 28), is_encrypted) << (24 ^ 28);
    ret |= crypto1_bit(s, BEBIT(in, 29), is_encrypted) << (24 ^ 29);
    ret |= crypto1_bit(s, BEBIT(in, 30), is_encrypted) << (24 ^ 30);
    ret |= crypto1_bit(s, BEBIT(in, 31), is_encrypted) << (24 ^ 31);
    return ret;
}

uint32_t prng_successor(uint32_t x, uint32_t n) {
    SWAPENDIAN(x);
    while (n--)
        x = x >> 1 | (x >> 16 ^ x >> 18 ^ x >> 19 ^ x >> 21) << 31;

    return SWAPENDIAN(x);
}

int main(int argc, char *argv[]) {
    struct Crypto1State *s, *t;
    uint64_t key;     // recovered key
    uint32_t uid;     // serial number
    uint32_t nt0;      // tag challenge first
    uint32_t nt1;      // tag challenge second
    uint32_t nr0_enc; // first encrypted reader challenge
    uint32_t ar0_enc; // first encrypted reader response
    uint32_t nr1_enc; // second encrypted reader challenge
    uint32_t ar1_enc; // second encrypted reader response
    uint32_t ks2;     // keystream used to encrypt reader response

    // TODO: Read from mfkey capture file instead
    // TODO: Loop for every key
    if (argc < 8) {
        printf("syntax: %s <uid> <nt> <nr_0> <ar_0> <nt1> <nr_1> <ar_1>\n\n", argv[0]);
        return 1;
    }

    sscanf(argv[1], "%x", &uid);
    sscanf(argv[2], "%x", &nt0);
    sscanf(argv[3], "%x", &nr0_enc);
    sscanf(argv[4], "%x", &ar0_enc);
    sscanf(argv[5], "%x", &nt1);
    sscanf(argv[6], "%x", &nr1_enc);
    sscanf(argv[7], "%x", &ar1_enc);
    uint32_t p64 = prng_successor(nt0, 64);
    uint32_t p64b = prng_successor(nt1, 64);
    ks2 = ar0_enc ^ p64;
    s = lfsr_recovery32(ar0_enc ^ p64, 0);
    for (t = s; t->odd | t->even; ++t) {
        lfsr_rollback_word(t, 0, 0);
        lfsr_rollback_word(t, nr0_enc, 1);
        lfsr_rollback_word(t, uid ^ nt0, 0);
        crypto1_get_lfsr(t, &key);
        crypto1_word(t, uid ^ nt1, 0);
        crypto1_word(t, nr1_enc, 1);
        if (ar1_enc == (crypto1_word(t, 0, 0) ^ p64b)) {
            // TODO: Append key to user dictionary file if missing in file
            printf("%012" PRIx64 , key);
            printf("\n");
            break;
        }
    }
    free(s);
    return 0;
}
