/*
 * Copyright © 2017 Kosma Moczek <kosma@cloudyourcar.com>
 * This program is free software. It comes without any warranty, to the extent
 * permitted by applicable law. You can redistribute it and/or modify it under
 * the terms of the Do What The Fuck You Want To Public License, Version 2, as
 * published by Sam Hocevar. See the COPYING file for more details.
 */

#ifndef MINMEA_COMPAT_H_
#define MINMEA_COMPAT_H_

#if defined(__TI_ARM__)
    #include <ti/posix/ccs/sys/types.h>
    #include <ti/posix/ccs/time.h>
#elif defined(__IAR_SYSTEMS_ICC__)
    #include <ti/posix/iar/sys/types.h>
    #include <ti/posix/iar/time.h>
#elif defined(gcc)
    #include <ti/posix/gcc/sys/types.h>
    #include <ti/posix/gcc/time.h>
#endif /* __TI_ARM__ */

#define timegm  mktime

#endif /* MINMEA_COMPAT_H */

/* vim: set ts=4 sw=4 et: */
