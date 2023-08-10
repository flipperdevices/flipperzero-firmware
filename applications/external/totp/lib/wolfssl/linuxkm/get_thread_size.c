/* get_thread_size.c -- trivial program to determine stack frame size
 * for a Linux kernel thread, given a configured source tree.
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

#ifndef __KERNEL__
#define __KERNEL__
#endif
#include <linux/kconfig.h>
#include <linux/kernel.h>
#include <linux/kthread.h>

extern int dprintf(int fd, const char *format, ...);

int main(__maybe_unused int argc, __maybe_unused char **argv) {
  dprintf(1, "%lu\n",THREAD_SIZE);
  return 0;
}
