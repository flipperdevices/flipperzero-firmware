#define PARTIAL_STATE_CHUNK_SIZE 100

struct Crypto1PackedPartialState {uint32_t part1, part2, part3;}; // Four 24-bit values packed into three 32-bit values

struct EndPartialStateList {
    struct EndPartialStateList *next;
    struct Crypto1PackedPartialState state[PARTIAL_STATE_CHUNK_SIZE];
};

// A function that takes a 24-bit value, an index from 0 to 3, and a pointer to a Crypto1PackedPartialState, and packs the value into the Crypto1PackedPartialState (which contains part1, part2, and part3 integers) at the given index
static inline void pack_partial_state(uint32_t value, int index, struct Crypto1PackedPartialState *state) {
    switch (index) {
        case 0:
            state->part1 = (state->part1 & 0xff0000) | (value & 0xffff);
            break;
        case 1:
            state->part1 = (state->part1 & 0xffff) | ((value & 0xff) << 16);
            state->part2 = (state->part2 & 0xff0000) | ((value >> 8) & 0xffff);
            break;
        case 2:
            state->part2 = (state->part2 & 0xffff) | ((value & 0xff) << 16);
            state->part3 = (state->part3 & 0xff0000) | ((value >> 8) & 0xffff);
            break;
        case 3:
            state->part3 = (state->part3 & 0xffff) | ((value & 0xffff) << 16);
            break;
    }
}
// A function which takes a Crypto1PackedPartialState and an index from 0 to 3, and returns the 24-bit value at the given index
static inline uint32_t unpack_partial_state(struct Crypto1PackedPartialState *state, int index) {
    switch (index) {
        case 0:
            return state->part1 & 0xffffff;
        case 1:
            return ((state->part1 >> 16) & 0xff) | ((state->part2 & 0xffff) << 8);
        case 2:
            return ((state->part2 >> 16) & 0xff) | ((state->part3 & 0xffff) << 8);
        case 3:
            return state->part3 >> 16;
    }
    return 0;
}
