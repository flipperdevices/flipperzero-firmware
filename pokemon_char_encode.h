#ifndef POKEMON_CHAR_ENCODE_H
#define POKEMON_CHAR_ENCODE_H
/* NOTE: These map to the Gen 1 character set! */
/* NOTE: These map to English */
/* TODO: It may make more sense to put this in a const array as a LUT,
 * e.g. t['F'], t['l'], t['i'], t['p'], t['e'], t['r'], t['\0']
 * As this could be an easier translation for each letter to build a string
 * to set names and things on the fly in the flipper. Need to explore that.
 * once I get to that point.
 */
#define TERM_ 0x50
#define SPACE_ 0x7f
#define A_ 0x80
#define B_ 0x81
#define C_ 0x82
#define D_ 0x83
#define E_ 0x84
#define F_ 0x85
#define G_ 0x86
#define H_ 0x87
#define I_ 0x88
#define J_ 0x89
#define K_ 0x8a
#define L_ 0x8b
#define M_ 0x8c
#define N_ 0x8d
#define O_ 0x8e
#define P_ 0x8f
#define Q_ 0x90
#define R_ 0x91
#define S_ 0x92
#define T_ 0x93
#define U_ 0x94
#define V_ 0x95
#define W_ 0x96
#define X_ 0x97
#define Y_ 0x98
#define Z_ 0x99
#define O_PAREN_ 0x9a
#define C_PAREN_ 0x9b
#define COLON_ 0x9c
#define SEMI_ 0x9d
#define O_BRACKET_ 0x9e
#define C_BRACKET_ 0x9f
#define a_ 0xa0
#define b_ 0xa1
#define c_ 0xa2
#define d_ 0xa3
#define e_ 0xa4
#define f_ 0xa5
#define g_ 0xa6
#define h_ 0xa7
#define i_ 0xa8
#define j_ 0xa9
#define k_ 0xaa
#define l_ 0xab
#define m_ 0xac
#define n_ 0xad
#define o_ 0xae
#define p_ 0xaf
#define q_ 0xb0
#define r_ 0xb1
#define s_ 0xb2
#define t_ 0xb3
#define u_ 0xb4
#define v_ 0xb5
#define w_ 0xb6
#define x_ 0xb7
#define y_ 0xb8
#define z_ 0xb9
#define e_ACCENT_ 0xba
#define d_TICK_ 0xbb
#define l_TICK_ 0xbc
#define s_TICK_ 0xbd
#define t_TICK_ 0xbe
#define v_TICK_ 0xbf
#define S_QUOTE_ 0xe0
#define PK_ 0xe1
#define MN_ 0xe2
#define DASH_ 0xe3
#define r_TICK_ 0xe4
#define m_TICK_ 0xe5
#define QUESTION_ 0xe6
#define EXCLAIM_ 0xe7
#define PERIOD_ 0xe8
#define R_ARR_ 0xec
#define D_ARR_ 0xee
#define MALE_ 0xef
#define FEMALE_ 0xf5
#define HYPHEN_ 0xe3
#define _0_ 0xf6
#define _1_ 0xf7
#define _2_ 0xf8
#define _3_ 0xf9
#define _4_ 0xfa
#define _5_ 0xfb
#define _6_ 0xfc
#define _7_ 0xfd
#define _8_ 0xfe
#define _9_ 0xff

#include <stdint.h>
#include <stddef.h>

char pokemon_char_to_encoded(int byte);
int pokemon_encoded_to_char(char byte);

void pokemon_str_to_encoded_array(uint8_t* dest, char* src, size_t n);
void pokemon_encoded_array_to_str(char* dest, uint8_t* src, size_t n);

#endif // POKEMON_CHAR_ENCODE_H
