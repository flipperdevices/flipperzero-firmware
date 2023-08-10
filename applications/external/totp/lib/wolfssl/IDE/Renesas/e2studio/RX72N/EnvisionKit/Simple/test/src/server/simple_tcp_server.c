/* simple_tcp_server.c
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
#include <stdio.h>
#include <string.h>
#include <wolfssl_simple_demo.h>
#include <stdio.h>
#include <string.h>
#include "r_t4_itcpip.h"

static int my_IORecv(char* buff, int sz, void* ctx)
{
    int ret;
    ID  cepid;

    if(ctx != NULL)
        cepid = *(ID *)ctx;
    else
        return 0;

    ret = tcp_rcv_dat(cepid, buff, sz, TMO_FEVR);
    if(ret > 0)
        return ret;
    else
        return 0;
}

static int my_IOSend(char* buff, int sz, void* ctx)
{
    int ret;
    ID  cepid;

    if(ctx != NULL)
        cepid = *(ID *)ctx;
    else
        return 0;

    ret = tcp_snd_dat(cepid, buff, sz, TMO_FEVR);
    if(ret == sz)
        return ret;
    else
        return 0;
}

void simple_tcp_server( )
{
    ID cepid = 1;
    ID repid = 1;
    ER ercd;
    int len;
    #define BUFF_SIZE 256
    char buff[BUFF_SIZE];
    T_IPV4EP dst_addr = {0, 0};

    if((ercd = tcp_acp_cep(cepid, repid, &dst_addr, TMO_FEVR)) != E_OK) {
        printf("ERROR TCP Accept: %d\n", ercd);
        goto out;
    }

    if ((len = my_IORecv(buff, sizeof(buff) - 1, (void*)&cepid)) < 0) {
        printf("ERROR: Read \n");
        goto out;
    }

    buff[len] = '\0';
    printf("Received: %s\n", buff);

    if (my_IOSend(buff, len, (void*)&cepid) != len) {
        printf("ERROR: Write\n");
    }

out:
    tcp_sht_cep(cepid);
    tcp_cls_cep(cepid, TMO_FEVR);
}
