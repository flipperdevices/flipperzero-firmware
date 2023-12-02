/*
 * Copyright © 2017 Kosma Moczek <kosma@cloudyourcar.com>
 * This program is free software. It comes without any warranty, to the extent
 * permitted by applicable law. You can redistribute it and/or modify it under
 * the terms of the Do What The Fuck You Want To Public License, Version 2, as
 * published by Sam Hocevar. See the COPYING file for more details.
 */

#if defined(_MSC_VER)

#if !defined(HAVE_STRUCT_TIMESPEC)
struct timespec {
    time_t tv_sec;
    long tv_nsec;
};
#endif

#define inline __inline
#define timegm _mkgmtime

#endif

/* vim: set ts=4 sw=4 et: */
