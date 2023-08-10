/* wolfssl_dummy.c
 *
 * Copyright (C) 2006-2023 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */

typedef unsigned long time_t;

#define YEAR 2019
#define MON  5

static int tick = 0;

time_t time(time_t *t)
{
    return ((YEAR-1970)*365+30*MON)*24*60*60 + tick++;
}

#include <ctype.h>
int strncasecmp(const char *s1, const char * s2, unsigned int sz)
{
    for( ; sz>0; sz--)
        if(toupper(s1++) != toupper(s2++))
        return 1;
    return 0;
}
