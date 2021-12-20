#pragma once

#define API_METHOD(x)                                    \
    sym_entry {                                          \
        .hash = elf_gnu_hash(#x), .address = (uint32_t)x \
    }

struct sym_entry {
    uint32_t hash;
    uint32_t address;
};

constexpr bool operator<(const sym_entry& k1, const sym_entry& k2) {
    return k1.hash < k2.hash;
}

static constexpr uint32_t elf_gnu_hash(const char* s) {
    uint32_t h = 0x1505;
    for(unsigned char c = *s; c != '\0'; c = *++s) h = (h << 5) + h + c;
    return h;
}

