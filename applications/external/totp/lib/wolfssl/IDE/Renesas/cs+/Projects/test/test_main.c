/* test_main.c
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

#include "typedefine.h"
#ifdef __cplusplus
#include <ios>                        // Remove the comment when you use ios
_SINT ios_base::Init::init_cnt;       // Remove the comment when you use ios
#endif

#include "stdio.h"

void main(void);
#ifdef __cplusplus
extern "C" {
void abort(void);
}
#endif

typedef struct func_args {
    int    argc;
    char** argv;
    int    return_code;
} func_args;

void wolfcrypt_test(func_args args);

void main(void)
{
    func_args args = { 1 };
  
    printf("Start wolfCrypt Test\n");
    wolfcrypt_test(args);
    printf("End wolfCrypt Test\n");

}

#ifdef __cplusplus
void abort(void)
{

}
#endif
