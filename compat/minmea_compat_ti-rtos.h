/*
 * Copyright © 2017 Kosma Moczek <kosma@cloudyourcar.com>
 * This program is free software. It comes without any warranty, to the extent
 * permitted by applicable law. You can redistribute it and/or modify it under
 * the terms of the Do What The Fuck You Want To Public License, Version 2, as
 * published by Sam Hocevar. See the COPYING file for more details.
 */

#if defined(__TI_ARM__)

// timespec definition
#include <ti/sysbios/posix/types.h>

#define timegm  mktime

#endif

/* vim: set ts=4 sw=4 et: */
