#ifndef __STATS_H__
#define __STATS_H__

#pragma once

typedef enum {
    /* Base stats */
    STAT_BASE = 0, // Sentry value
    STAT_BASE_ATK = 0,
    STAT_BASE_DEF,
    STAT_BASE_SPD,
    /* NOTE! While accessing SPC/APC_AT will do the correct thing for both
     * Gen I and Gen II, accessing SPC_DEF for Gen I will return a value
     * that is not used in Gen I games. This normally isn't an issue, but
     * is a potential gotcha to be aware of.
     */
    STAT_BASE_SPC,
    STAT_BASE_SPC_ATK,
    STAT_BASE_SPC_DEF,
    STAT_BASE_HP,
    STAT_BASE_TYPE,
    STAT_BASE_MOVE,
    STAT_BASE_GROWTH,
    STAT_BASE_GENDER_RATIO,
    STAT_BASE_INDEX,
    STAT_BASE_END, // Sentry value

    /* In-party stats */
    STAT = 0, // Sentry value
    STAT_ATK = 0,
    STAT_DEF,
    STAT_SPD,
    /* Gen I uses SPC, Gen II uses SPC_ATK and SPC_DEF */
    STAT_SPC,
    STAT_SPC_ATK,
    STAT_SPC_DEF,
    STAT_HP,
    STAT_END = 7, // Sentry value
    STAT_TYPE = 7,
    /* Move is left here to line up with the table base stats */
    STAT_MOVE,

    STAT_EV = 10, // Sentry value
    STAT_EV_OFFS = 10,
    STAT_ATK_EV = 10,
    STAT_DEF_EV,
    STAT_SPD_EV,
    /* There is only SPC EV, there is no SPC_ATK/DEF. However, for the sake
     * of making calculations easier, we pretend there are. This means that
     * SPC/SPC_ATK/SPC_DEF calculations all grab the SPC EV.
     */
    STAT_SPC_EV,
    STAT_SPC_ATK_EV,
    STAT_SPC_DEF_EV,
    STAT_HP_EV = 16,
    STAT_EV_END, // Sentry value

    STAT_IV = 19,
    STAT_IV_OFFS = 20, // Sentry value
    STAT_ATK_IV = 20,
    STAT_DEF_IV,
    STAT_SPD_IV,
    /* There is only SPC IV, there is no SPC_ATK/DEF. However, for the sake
     * of making calculations easier, we pretend there are. This means that
     * SPC/SPC_ATK/SPC_DEF calculations all grab the SPC IV.
     */
    STAT_SPC_IV,
    STAT_SPC_ATK_IV,
    STAT_SPC_DEF_IV,
    STAT_HP_IV = 26,
    STAT_IV_END, // Sentry value

    /* These won't ever really be needed in groups */
    STAT_LEVEL = 28,
    STAT_INDEX,
    STAT_NUM,
    STAT_CONDITION,
    STAT_NICKNAME,
    STAT_OT_NAME,
    STAT_OT_ID,
    STAT_TRAINER_NAME,
    STAT_SEL, // which EV/IV calc to use
    STAT_EXP,
    STAT_HELD_ITEM,
    STAT_POKERUS,
} DataStat;

typedef enum {
    MOVE_0 = 0,
    MOVE_1,
    MOVE_2,
    MOVE_3,

    TYPE_0 = 0,
    TYPE_1,

    EXP_0 = 0,
    EXP_1,
    EXP_2,

    NONE = 0, // Just a filler value
} DataStatSub;

#endif // __STATS_H__
