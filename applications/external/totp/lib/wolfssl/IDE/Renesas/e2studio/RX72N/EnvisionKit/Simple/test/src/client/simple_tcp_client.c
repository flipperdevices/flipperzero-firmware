/* simple_tcp_client.c
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

#include <wolfssl_simple_demo.h>
#include <stdio.h>
#include <string.h>
#include "r_t4_itcpip.h"

#define SIMPLE_TCPSEVER_IP       "192.168.10.10"
#define SIMPLE_TCPSERVER_PORT    "11111"

ER    t4_tcp_callback(ID cepid, FN fncd , VP p_parblk);

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

static int getIPaddr(char *arg)
{
    int a1, a2, a3, a4;
    if(sscanf(arg, "%d.%d.%d.%d", &a1, &a2, &a3, &a4) == 4)
         return (a1 << 24) | (a2 << 16) | (a3 << 8) | a4;
    else return 0;
}

static int getPort(char *arg)
{
    int port;
    if(sscanf(arg, "%d", &port) == 1)
         return port;
    else return 0;
}


void simple_tcp_client( )
{
    ID  cepid = 1;
    ER  ercd;
    int ret;

    #define BUFF_SIZE 256
    static const char sendBuff[]= "Hello Server\n" ;
    
    char    rcvBuff[BUFF_SIZE] = {0};
    
    static T_IPV4EP my_addr = { 0, 0 };
    
    T_IPV4EP dst_addr;

    if((dst_addr.ipaddr = getIPaddr(SIMPLE_TCPSEVER_IP)) == 0){
        printf("ERROR: IP address\n");
        goto out;
    }
    if((dst_addr.portno = getPort(SIMPLE_TCPSERVER_PORT)) == 0){
        printf("ERROR: Port number\n");
        goto out;
    }

    if((ercd = tcp_con_cep(cepid, &my_addr, &dst_addr, TMO_FEVR)) != E_OK) {
        printf("ERROR TCP Connect: %d\n", ercd);
        goto out;
    }

    if (my_IOSend((char*)sendBuff, strlen(sendBuff), (void*)&cepid) != 
                                                            strlen(sendBuff)) {
        printf("ERROR TCP write \n");
        goto out;
    }

    if ((ret=my_IORecv(rcvBuff, BUFF_SIZE, (void*)&cepid)) < 0) {
        printf("ERROR TCP read\n");
        goto out;
    }

    rcvBuff[ret] = '\0' ;
    printf("Received: %s\n\n", rcvBuff);

 out:

    tcp_sht_cep(cepid);
    tcp_cls_cep(cepid, TMO_FEVR);

    return;
}