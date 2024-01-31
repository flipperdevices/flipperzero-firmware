#include <stdio.h>

typedef enum {
    EntityPacman,
    EntityGhost,
    EntityNothing,
    EntityWall,
    EntityCandy,
    EntitySuperCandy,
    EntityCherry,
    EntityBase,
    EntityVoid
} Entity;

/**
 * @brief      Sets up the map matrix according to the config matrix.
 * @details    Parses the characters in the config matrix and creates the correspoding enum matrix.
 * @param      config The config matrix (actually array of strings).
 * @return     The initialized Entity matrix.
*/
static void setup_map(char* config[], Entity map[][17], int size) {
    for(int i = 0; i < 11; i++) {
        for(int j = 0; j < size; j++) {
            char symbol = config[i][j];
            switch(symbol) {
            case 'C':
                map[i][j] = EntityCandy;
                break;
            case 'W':
                map[i][j] = EntityWall;
                break;
            case 'N':
                map[i][j] = EntityNothing;
                break;
            case 'B':
                map[i][j] = EntityBase;
                break;
            case 'S':
                map[i][j] = EntitySuperCandy;
                break;
            default:
                map[i][j] = EntityVoid;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    static const char* map_config[] = {
        "CCCCCCCCWCCCCCCCC",
        "CWWCWWWCWCWWWCWWC",
        "CCCCCCCCCCCCCCCCC",
        "CWWCWCWWWWWCWCWWC",
        "CCCCWCCCWCCCWCCCC",
        "NNNCCCBBBBBCCCNNN",
        "CCCCWCCCWCCCWCCCC",
        "CWWCWCWWWWWCWCWWC",
        "CCCCCCCCCCCCCCCCC",
        "CWWCWWWCWCWWWCWWC",
        "CCCCCCCCWCCCCCCCC",
    };

    const Entity map[17][17];

    setup_map(map_config, map, 17);

    return 0;
}