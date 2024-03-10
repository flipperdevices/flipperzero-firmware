#ifndef __STAT_NL_H__
#define __STAT_NL_H__

#pragma once

extern const NamedList stat_list[];

typedef enum {
    RANDIV_ZEROEV,
    RANDIV_LEVELEV,
    RANDIV_MAXEV,
    MAXIV_ZEROEV,
    MAXIV_LEVELEV,
    MAXIV_MAXEV,
} EvIv;

#endif // __STAT_NL_H__
