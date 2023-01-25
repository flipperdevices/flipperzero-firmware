#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#define LF_POLY_ODD (0x29CE5C)
#define LF_POLY_EVEN (0x870804)
#define BIT(x, n) ((x) >> (n) & 1)
#define BEBIT(x, n) BIT(x, (n) ^ 24)
#define SWAPENDIAN(x) (x = (x >> 8 & 0xff00ff) | (x & 0xff00ff) << 8, x = x >> 16 | x << 16)
#define SIZEOF(arr) sizeof(arr) / sizeof(*arr)
struct Crypto1State {uint32_t odd, even;};
void *file_calloc(size_t nmemb, size_t size, FILE *file) {
    // Seek to the end of the file
    fseek(file, 0, SEEK_END);
    // Get the current position, which is the end of the file
    long int offset = ftell(file);
    // Write nmemb * size bytes of 0s to the file
    for (int i = 0; i < nmemb * size; i++) {
        fputc(0, file);
    }
    // Return the starting offset of the allocated memory
    return (void *) offset;
}

void file_write_at_offset(void *ptr, int element, void *src, size_t n, FILE *file) {
    // Seek to the specified offset in the file
    fseek(file, (long int) (ptr + (element*n)), SEEK_SET);
    // Write n bytes from src to the file
    fwrite(src, n, 1, file);
}

void *file_read_at_offset(void *ptr, int element, size_t n, FILE *file) {
    // Seek to the specified offset in the file
    fseek(file, (long int) (ptr + (element*n)), SEEK_SET);
    // Read n bytes from the file
    void *buf = malloc(n);
    fread(buf, n, 1, file);
    return buf;
}
int file_read_int_at_offset(void *ptr, int element, size_t n, FILE *file) {
    int res;
    // Seek to the specified offset in the file
    fseek(file, (long int) (ptr + (element*n)), SEEK_SET);
    // Read n bytes from the file
    fread(&res, n, 1, file);
    return res;
}
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
int binsearch(void *data_offset, FILE *mem_file, int start, int stop) {
    int mid, val = file_read_int_at_offset(data_offset, stop, sizeof(int), mem_file) & 0xff000000;
    while (start != stop) {
        mid = (stop - start) >> 1;
        int data_start_mid = file_read_int_at_offset(data_offset, start + mid, sizeof(int), mem_file);
        if ((data_start_mid ^ 0x80000000) > (val ^ 0x80000000))
            stop = start + mid;
        else
            start += mid + 1;
    }
    return start;
}
void quicksort(void *data_offset, FILE *mem_file, int low, int high) {
    /*
    if (SIZEOF(array) == 0)
        return;
    */
    if (low >= high)
        return;
    int middle = low + (high - low) / 2;
    int pivot = file_read_int_at_offset(data_offset, middle, sizeof(int), mem_file);
    int i = low, j = high;
    while (i <= j) {
        while (file_read_int_at_offset(data_offset, i, sizeof(int), mem_file) < pivot) {
            i++;
        }
        while (file_read_int_at_offset(data_offset, j, sizeof(int), mem_file) > pivot) {
            j--;
        }
        if (i <= j) { // swap
            int temp = file_read_int_at_offset(data_offset, i, sizeof(int), mem_file);
            int temp_2 = file_read_int_at_offset(data_offset, j, sizeof(int), mem_file);
            file_write_at_offset(data_offset, i, &temp_2, sizeof(int), mem_file);
            file_write_at_offset(data_offset, j, &temp, sizeof(int), mem_file);
            i++;
            j--;
        }
    }
    if (low < j) {
        quicksort(data_offset, mem_file, low, j);
    }
    if (high > i) {
        quicksort(data_offset, mem_file, i, high);
    }
}
void update_contribution(void *data_offset, FILE *mem_file, int item, int mask1, int mask2) {
    int data_item = file_read_int_at_offset(data_offset, item, sizeof(int), mem_file);
    int p = data_item >> 25;
    p = p << 1 | evenparity32(data_item & mask1);
    p = p << 1 | evenparity32(data_item & mask2);
    data_item = p << 24 | (data_item & 0xffffff);
    file_write_at_offset(data_offset, item, &data_item, sizeof(int), mem_file);
}
int extend_table(void *data_offset, FILE *mem_file, int tbl, int end, int bit, int m1, int m2) {
    int data_tbl_init = file_read_int_at_offset(data_offset, tbl, sizeof(int), mem_file);
    data_tbl_init <<= 1;
    file_write_at_offset(data_offset, tbl, &data_tbl_init, sizeof(int), mem_file);
    while (tbl <= end) {
        int data_tbl = file_read_int_at_offset(data_offset, tbl, sizeof(int), mem_file);
        if ((filter(data_tbl) ^ filter(data_tbl | 1)) != 0) {
            data_tbl |= filter(data_tbl) ^ bit;
            file_write_at_offset(data_offset, tbl, &data_tbl, sizeof(int), mem_file);
            update_contribution(data_offset, mem_file, tbl, m1, m2);
        } else if (filter(data_tbl) == bit) {
            end++;
            int data_tbl_2 = file_read_int_at_offset(data_offset, tbl + 1, sizeof(int), mem_file);
            file_write_at_offset(data_offset, end, &data_tbl_2, sizeof(int), mem_file);
            int data_tbl_3 = file_read_int_at_offset(data_offset, tbl, sizeof(int), mem_file);
            data_tbl = data_tbl_3 | 1;
            file_write_at_offset(data_offset, tbl + 1, &data_tbl, sizeof(int), mem_file);
            update_contribution(data_offset, mem_file, tbl, m1, m2);
            tbl++;
            update_contribution(data_offset, mem_file, tbl, m1, m2);
        } else {
            int data_end = file_read_int_at_offset(data_offset, end, sizeof(int), mem_file);
            file_write_at_offset(data_offset, tbl, &data_end, sizeof(int), mem_file);
            end--;
            tbl--;
        }
        tbl++;
        int data_tbl_update = file_read_int_at_offset(data_offset, tbl, sizeof(int), mem_file);
        data_tbl_update <<= 1;
        file_write_at_offset(data_offset, tbl, &data_tbl_update, sizeof(int), mem_file);
    }
    return end;
}
int extend_table_simple(void *data_offset, FILE *mem_file, int tbl, int end, int bit) {
    int data_tbl_init = file_read_int_at_offset(data_offset, tbl, sizeof(int), mem_file);
    data_tbl_init <<= 1;
    file_write_at_offset(data_offset, tbl, &data_tbl_init, sizeof(int), mem_file);
    while (tbl <= end) {
        int data_tbl = file_read_int_at_offset(data_offset, tbl, sizeof(int), mem_file);
        if ((filter(data_tbl) ^ filter(data_tbl | 1)) !=0 ) {
            data_tbl |= filter(data_tbl) ^ bit;
            file_write_at_offset(data_offset, tbl, &data_tbl, sizeof(int), mem_file);
        } else if (filter(data_tbl) == bit) {
            end++;
            tbl++;
            int data_tbl_2 = file_read_int_at_offset(data_offset, tbl, sizeof(int), mem_file);
            file_write_at_offset(data_offset, end, &data_tbl_2, sizeof(int), mem_file);
            int data_tbl_3 = file_read_int_at_offset(data_offset, tbl - 1, sizeof(int), mem_file);
            data_tbl = data_tbl_3 | 1;
            file_write_at_offset(data_offset, tbl, &data_tbl, sizeof(int), mem_file);
        } else {
            int data_end = file_read_int_at_offset(data_offset, end, sizeof(int), mem_file);
            file_write_at_offset(data_offset, tbl, &data_end, sizeof(int), mem_file);
            end--;
            tbl--;
        }
        tbl++;
        int data_tbl_update = file_read_int_at_offset(data_offset, tbl, sizeof(int), mem_file);
        data_tbl_update <<= 1;
        file_write_at_offset(data_offset, tbl, &data_tbl_update, sizeof(int), mem_file);
    }
    return end;
}
int recover(void *odd_start_offset, FILE *mem_file, int o_head, int o_tail, int oks, void *even_start_offset, int e_head, int e_tail, int eks, int rem,
            void *statelist_start_offset, int s) {
    int o = 0, e = 0, i = 0;
    if (rem == -1) {
        for (e = e_head; e <= e_tail; ++e) {
            int even_e = file_read_int_at_offset(even_start_offset, e, sizeof(int), mem_file);
            even_e = (even_e << 1) ^ evenparity32(even_e & LF_POLY_EVEN);
            file_write_at_offset(even_start_offset, e, &even_e, sizeof(int), mem_file);
            for (o = o_head; o <= o_tail; ++o, ++s) {
                int odd_o = file_read_int_at_offset(odd_start_offset, o, sizeof(int), mem_file);
                int even_e_xor = even_e ^ evenparity32(odd_o & LF_POLY_ODD);
                struct Crypto1State sl_s = {even_e_xor, odd_o};
                file_write_at_offset(statelist_start_offset, s, &sl_s, sizeof(struct Crypto1State), mem_file);
                struct Crypto1State sl_s_1 = {0, 0};
                file_write_at_offset(statelist_start_offset, s + 1, &sl_s_1, sizeof(struct Crypto1State), mem_file);
            }
        }
        return s;
    }
    for (i = 0; (i < 4) && (rem-- != 0); i++) {
        oks >>= 1;
        eks >>= 1;
        o_tail = extend_table(odd_start_offset, mem_file, o_head, o_tail, oks & 1, LF_POLY_EVEN << 1 | 1, LF_POLY_ODD << 1);
        if (o_head > o_tail)
            return s;
        e_tail = extend_table(even_start_offset, mem_file, e_head, e_tail, eks & 1, LF_POLY_ODD, LF_POLY_EVEN << 1 | 1);
        if (e_head > e_tail)
            return s;
    }
    quicksort(odd_start_offset, mem_file, o_head, o_tail);
    quicksort(even_start_offset, mem_file, e_head, e_tail);
    while (o_tail >= o_head && e_tail >= e_head) {
        int odd_o_tail = file_read_int_at_offset(odd_start_offset, o_tail, sizeof(int), mem_file);
        int even_e_tail = file_read_int_at_offset(even_start_offset, e_tail, sizeof(int), mem_file);
        if (((odd_o_tail ^ even_e_tail) >> 24) == 0) {
            o_tail = binsearch(odd_start_offset, mem_file, o_head, o = o_tail);
            e_tail = binsearch(even_start_offset, mem_file, e_head, e = e_tail);
            s = recover(odd_start_offset, mem_file, o_tail--, o, oks, even_start_offset, e_tail--, e, eks, rem, statelist_start_offset, s);
        } else if ((odd_o_tail ^ 0x80000000) > (even_e_tail ^ 0x80000000)) {
            o_tail = binsearch(odd_start_offset, mem_file, o_head, o_tail) - 1;
        } else {
            e_tail = binsearch(even_start_offset, mem_file, e_head, e_tail) - 1;
        }
    }
    return s;
}
struct Crypto1State* lfsr_recovery32(int ks2, FILE *mem_file) {
    struct Crypto1State *statelist;
    int odd_head = 0, odd_tail = -1, oks = 0;
    int even_head = 0, even_tail = -1, eks = 0;
    void *odd_start_offset = file_calloc(1, 5 << 19, mem_file);
    void *even_start_offset = file_calloc(1, 5 << 19, mem_file);
    void *statelist_start_offset = file_calloc(1, sizeof(struct Crypto1State) << 18, mem_file);
    int i;
    for (i = 31; i >= 0; i -= 2)
        oks = oks << 1 | BEBIT(ks2, i);
    for (i = 30; i >= 0; i -= 2)
        eks = eks << 1 | BEBIT(ks2, i);
    for (i = 1 << 20; i >= 0; --i) {
        if (filter(i) == (oks & 1))
            //odd[++odd_tail] = i;
            file_write_at_offset(odd_start_offset, ++odd_tail, &i, sizeof(int), mem_file);
        if (filter(i) == (eks & 1))
            //even[++even_tail] = i;
            file_write_at_offset(even_start_offset, ++even_tail, &i, sizeof(int), mem_file);
    }
    for (i = 0; i < 4; i++) {
        odd_tail = extend_table_simple(odd_start_offset, mem_file, odd_head, odd_tail, ((oks >>= 1) & 1));
        even_tail = extend_table_simple(even_start_offset, mem_file, even_head, even_tail, ((eks >>= 1) & 1));
    }
    recover(odd_start_offset, mem_file, odd_head, odd_tail, oks, even_start_offset, even_head, even_tail, eks, 11, statelist_start_offset, 0);
    return statelist_start_offset;
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

int main(int argc, char *argv[]) {
    uint64_t key;     // recovered key
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
    struct Crypto1State *temp;

    keyarray_size = 0;
    FILE* filePointer;
    int bufferLength = 255;
    char buffer[bufferLength];
    filePointer = fopen(".mfkey32.log", "r"); // TODO: In FAP, use full path
    char file_path[] = "/tmp/mem_file.bin";
    unlink(file_path);
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
        if (key_already_found_for_nonce(keyarray, keyarray_size, uid ^ nt1, nr1_enc, p64b, ar1_enc)) {
            continue;
        }
        FILE *mem_file = fopen(file_path, "wb+");
        void *s_offset = lfsr_recovery32(ar0_enc ^ p64, mem_file);
        int ti = 0;
        while (1) {
            temp = (struct Crypto1State *) file_read_at_offset(s_offset, ti, sizeof(struct Crypto1State), mem_file);
            if (!temp->odd && !temp->even) break;
            crypt_or_rollback_word(temp, 0, 0, 0);
            crypt_or_rollback_word(temp, nr0_enc, 1, 0);
            crypt_or_rollback_word(temp, uid ^ nt0, 0, 0);
            crypto1_get_lfsr(temp, &key);
            crypt_or_rollback_word(temp, uid ^ nt1, 0, 1);
            crypt_or_rollback_word(temp, nr1_enc, 1, 1);
            if (ar1_enc == (crypt_or_rollback_word(temp, 0, 0, 1) ^ p64b)) {
                int found = 0;
                for(i = 0; i < keyarray_size; i++) {
                    if (keyarray[i] == key) {
                        found = 1;
                        break;
                    }
                }
                if (found == 0) {
                    keyarray = realloc(keyarray, sizeof(uint64_t)*(keyarray_size+1));
                    keyarray_size += 1;
                    keyarray[keyarray_size-1] = key;
                }
                break;
            }
            //file_write_at_offset(s_offset, t, &temp, sizeof(struct Crypto1State), mem_file);
            free(temp);
            ti++;
        }
    }

    fclose(filePointer);
    printf("Unique keys found:\n");
    for(i = 0; i < keyarray_size; i++) {
        printf("%012" PRIx64 , keyarray[i]);
        printf("\n");
    }
    // TODO: Append key to user dictionary file if missing in file
    return 0;
}
