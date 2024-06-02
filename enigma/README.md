# Enigma Machine in C

https://github.com/LeonardoE95/enigma-machine

Implementation of Enigma machine used by German troops to encrypt communications during WWII. The machine is implemented using the C programming language as a header-only library. 

**NOTE**: The current implementation is able to work only with capital
letters taken from the english alphabet. That is, we work with the
alphabet `ABCDEFGHIJKLMNOPQRSTUVWXYZ`.

## Quick Start

To use the machine compile the examples and in particular the `examples/cli.c` file

```c
make examples
```

At this point you can use the CLI as follows

```c
./examples/cli
```

Within the cli you can use the following commands

```
Enigma> help
Enigma> List of commands...

        quit – exit from enigma cli
        help – list available commands
        info – print enigma configuration
        set <rotors> | set <reflector> | set <plugboard> – change enigma configuration
        encrypt <plaintext> – encrypt plaintext
        decrypt <ciphertext> – decrypt ciphertext
```

Some examples are shown below

```
Enigma> info
Enigma> Current configuration...
        Rotors (from left to right): M3-II, M3-I, M3-III
               Position: 0, 0, 0
                   Ring: 0, 0, 0
        Reflector: M3-B
        Plugboard: 6 plugs
                   (A, M)
                   (F, I)
                   (N, V)
                   (P, S)
                   (T, U)
                   (W, Z)
Enigma> encrypt HELLO
MIJEN
```

```
Enigma> set reflector M3-B
Enigma> set rotor left M3-I 1 2
Enigma> set rotor middle M3-II 3 4
Enigma> set rotor right M3-III 5 6
Enigma> set plugboard B-Q C-R
Enigma> info
Enigma> Current configuration...
        Rotors (from left to right): M3-I, M3-II, M3-III
               Position: 1, 3, 5
                   Ring: 2, 4, 6
        Reflector: M3-B
        Plugboard: 2 plugs
                   (B, Q)
                   (C, R)
Enigma> encrypt DSFSDFSDF
SCLMEEYMV
```

## Library Usage

To actually use the library in your own C sources, download the `enigma.h` file and import it in your code as follows

```c
#define ENIGMA_IMPLEMENTATION
#include "../enigma.h"
```

First you istantiate an enigma machine with a call to `init_enigma`

```c
Enigma *e = init_enigma (
			 // rotors model
			 (const char *[]){"M3-II", "M3-I", "M3-III"},
			 // rotor_positions
			 (const uint8_t [ROTORS_N]) {0, 0, 0},
			 // rotor_ring_settings
			 (const uint8_t [ROTORS_N]) {0, 0, 0},
			 // reflector model
			 "M3-B",
			 // plugboard switches
			 (uint8_t [][2]){
			   {'A', 'M'}, {'F', 'I'}, {'N', 'V'},
			   {'P', 'S'}, {'T', 'U'}, {'W', 'Z'}},
			 // plugboard size
			 6 
			 );
```
And then you can encrypt/decrypt your text. 

```c
enigma_encrypt(enigma, plaintext, length, ciphertext);
enigma_decrypt(enigma, ciphertext, length, plaintext);
```

Notice that before using the machine you have to take care of allocating the memory for the plaintext and the ciphertext buffers. This is shown below

```c
char *str = "HELLO";
size_t length = strlen(str);

char plaintext[length];
char ciphertext[length];
  
memcpy(plaintext, str, length + 1);
memcpy(ciphertext, str, length + 1);

enigma_encrypt(e, plaintext, length, ciphertext);
```

Finally, when you stop using enigma, you have to call `destroy_enigma` for freeing up the allocated memory.

```c
destroy_enigma(enigma);
```