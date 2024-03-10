#include <named_list.h>
#include <stat_nl.h>

/* These line up with the DataStat enum */
/* XXX ???
const char *stat_text[] = {
    "ATK",
    "DEF",
    "SPD",
    "SPC_ATK",
    "SPC_DEF",
    "HP",
    "Type",
    "Move",
    "Growth",
    "Gender Ratio",
    "ATK_EV",
    "DEF_EV",
    "SPD_EV",
    "SPC_EV",
    "HP_EV",
    "IV",
    "ATK_IV",
    "DEF_IV",
    "SPD_IV",
    "SPC_IV",
    "HP_IV",
    "Lvl.",
    "Idx.",
    "Num.",
    "Cond.",
    "Nick.",
    "OT Name",
    "OT ID",
    "Trainer Name",
    "EV/IV Sel.",
    "Exp.",
};
*/

const NamedList stat_list[] = {
    {"Random IV, Zero EV", RANDIV_ZEROEV, 0},
    {"Random IV, Max EV / Level", RANDIV_LEVELEV, 0},
    {"Random IV, Max EV", RANDIV_MAXEV, 0},
    {"Max IV, Zero EV", MAXIV_ZEROEV, 0},
    {"Max IV, Max EV / Level", MAXIV_LEVELEV, 0},
    {"Max IV, Max EV", MAXIV_MAXEV, 0},
    {},
};
