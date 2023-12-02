#include "blocks.h"
#include <stdlib.h>
#include <math.h>

Block get_house() {
    Line* house = (Line*)malloc(sizeof(Line) * HOUSE_LINE_CO);

    // Outline
    house[0].a.x = -10;
    house[0].a.y = 30;
    house[0].b.x = 10;
    house[0].b.y = 30;

    house[1].a.x = 10;
    house[1].a.y = 30;
    house[1].b.x = 10;
    house[1].b.y = 0;

    house[2].a.x = 10;
    house[2].a.y = 0;
    house[2].b.x = -10;
    house[2].b.y = 0;

    house[3].a.x = -10;
    house[3].a.y = 0;
    house[3].b.x = -10;
    house[3].b.y = 30;

    // Windows
    house[4].a.x = -10;
    house[4].a.y = 10;
    house[4].b.x = 10;
    house[4].b.y = 10;

    house[5].a.x = -10;
    house[5].a.y = 20;
    house[5].b.x = 10;
    house[5].b.y = 20;

    house[6].a.x = -2;
    house[6].a.y = 10;
    house[6].b.x = -2;
    house[6].b.y = 20;

    house[7].a.x = 2;
    house[7].a.y = 10;
    house[7].b.x = 2;
    house[7].b.y = 20;

    house[8].a.x = -6;
    house[8].a.y = 10;
    house[8].b.x = -6;
    house[8].b.y = 20;

    house[9].a.x = -6;
    house[9].a.y = 15;
    house[9].b.x = -2;
    house[9].b.y = 15;

    house[10].a.x = 6;
    house[10].a.y = 10;
    house[10].b.x = 6;
    house[10].b.y = 20;

    house[10].a.x = 2;
    house[10].a.y = 15;
    house[10].b.x = 10;
    house[10].b.y = 15;

    Block b = {HOUSE_LINE_CO, house};
    return b;
}

Block get_crane() {
    Line* crane = (Line*)malloc(sizeof(Line) * CRANE_LINE_CO);

    // Crane rope
    crane[0].a.x = 0;
    crane[0].a.y = 0;
    crane[0].b.x = 0;
    crane[0].b.y = 15;

    Block b = {CRANE_LINE_CO, crane};
    return b;
}

Block get_ground() {
    Line* ground = (Line*)malloc(sizeof(Line) * GROUND_LINE_CO);

    // Ground
    ground[0].a.x = -32;
    ground[0].a.y = 0;
    ground[0].b.x = 32;
    ground[0].b.y = 0;

    Block b = {GROUND_LINE_CO, ground};
    return b;
}
