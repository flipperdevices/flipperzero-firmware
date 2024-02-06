#include <stdio.h>
#include <wchar.h>

#define MAP_SIZE_W 17
#define MAP_SIZE_H 31

typedef enum {
    EntityPacman,
    EntityGhost,
    EntityNothing,
    EntityTopLeftWall,
    EntityTopRightWall,
    EntityBottomLeftWall,
    EntityBottomRightWall,
    EntityHorizontalWall,
    EntityVerticalWall,
    EntityCandy,
    EntitySuperCandy,
    EntityCherry,
    EntityBase,
    EntityVoid
} Entity;

static const char* map_config[] = {
    "1------------21-----------2",  "|CCCCCCCCCCCC||CCCCCCCCCCCC|", "|C1--2C1---2C||C1---2C1--2C|",
    "|C|  |C|   |C||C|   |C|  |C|", "|C3--4C3---4C34C3---4C3--4C|", "|CCCCCCCCCCCCCCCCCCCCCCCCCC|",
    "|C1--2C12C1------2C12C1--2C|", "|C3--4C||C3--21--4C||C3--4C|", "|CCCCCC||CCCC||CCCC||CCCCCC|",
    "3----2C|3--2 || 1--4|C1----4", "     |C|1--4 34 3--2|C|     ", "     |C||          ||C|     ",
    "     |C|| 1--  --2 ||C|     ", "-----4C34 |BBBBBB| 34C3-----", "      C   |BBBBBB|   C      ",
    "-----2C12 |BBBBBB| 12C1-----", "     |C|| 3------4 ||C|     ", "     |C||          ||C|     ",
    "     |C|| 1------2 ||C|     ", "1----4C34 3--21--4 34C3----2", "|CCCCCCCCCCCC||CCCCCCCCCCCC|",
    "|C1--2C1---2C||C1---2C1--2C|", "|C3-2|C3---4C34C3---4C|1-4C|", "|CCC||CCCCCCCCCCCCCCCC||CCC|",
    "3-2C||C12C1------2C12C||C1-4", "1-4C34C||C3--21--4C||C34C3-2", "|CCCCCC||CCCC||CCCC||CCCCCC|",
    "|C1----43--2C||C1--43----2C|", "|C3--------4C34C3--------4C|", "|CCCCCCCCCCCCCCCCCCCCCCCCCC|",
    "3--------------------------4"};

/**
 * @brief      Sets up the map matrix according to the config matrix.
 * @details    Parses the characters in the config matrix and creates the correspoding enum matrix.
 * @param      config The config matrix (actually array of strings).
 * @return     The initialized Entity matrix.
*/
static void setup_map(Entity map[][MAP_SIZE_W]) {
    for(int i = 0; i < MAP_SIZE_H; i++) {
        for(int j = 0; j < MAP_SIZE_W; j++) {
            wchar_t symbol = map_config[i][j]; // Int to prevent to go beyond char range
            wprintf(L"%lc", symbol);
            switch(symbol) {
            case 'C':
                map[i][j] = EntityCandy;
                break;
            case '|':
                map[i][j] = EntityVerticalWall;
                break;
            case '-':
                map[i][j] = EntityHorizontalWall;
                break;
            case L'1':
                map[i][j] = EntityTopLeftWall;
                break;
            case L'2':
                map[i][j] = EntityTopRightWall;
                break;
            case L'3':
                map[i][j] = EntityBottomRightWall;
                break;
            case L'4':
                map[i][j] = EntityBottomLeftWall;
                break;
            default:
                map[i][j] = EntityVoid;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    Entity map[17][17];

    setup_map(map);

    return 0;
}