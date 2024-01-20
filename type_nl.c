#include <named_list.h>
#include <pokemon_data.h>

const NamedList type_list[] = {
    {"Bug", 0x07, GEN_I},
    {"Dragon", 0x1A, GEN_I},
    {"Electric", 0x17, GEN_I},
    {"Fighting", 0x01, GEN_I},
    {"Fire", 0x14, GEN_I},
    {"Flying", 0x02, GEN_I},
    {"Ghost", 0x08, GEN_I},
    {"Grass", 0x16, GEN_I},
    {"Ground", 0x04, GEN_I},
    {"Ice", 0x19, GEN_I},
    {"Normal", 0x00, GEN_I},
    {"Poison", 0x03, GEN_I},
    {"Psychic", 0x18, GEN_I},
    {"Rock", 0x05, GEN_I},
    {"Water", 0x15, GEN_I},
    /* Types are not transferred in gen ii */
    {},
};
