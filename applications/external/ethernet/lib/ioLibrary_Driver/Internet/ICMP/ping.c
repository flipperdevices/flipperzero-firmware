#include "ping.h"
#include "socket.h"
#include "w5500.h"

PINGMSGR PingRequest; // Variable for Ping Request
PINGMSGR PingReply; // Variable for Ping Reply
static uint16_t RandomID = 0x1234;
static uint16_t RandomSeqNum = 0x4321;
uint8_t ping_reply_received = 0;
uint8_t req = 0;
uint8_t rep = 0;

extern void ping_wait_ms(int ms);

uint8_t ping_auto(uint8_t s, uint8_t* addr) {
    uint8_t i;
    int32_t len = 0;
    uint8_t cnt = 0;
    int ret = 0;
    rep = req = 0;
    for(i = 0; i <= 3; i++) {
        uint8_t sr = getSn_SR(s);
        eth_printf("SR: %02X", sr);
        switch(sr) {
        case SOCK_CLOSED:
            close(s);
            IINCHIP_WRITE(Sn_PROTO(s), IPPROTO_ICMP); // set ICMP Protocol
            if((ret = socket(s, Sn_MR_IPRAW, 3000, 0)) !=
               s) { // open the SOCKET with IPRAW mode, if fail then Error
                eth_printf("  socket %d fail %d", s, ret);
#ifdef PING_DEBUG
                return SOCKET_ERROR;
#endif
            }
            /* Check socket register */
            while(getSn_SR(s) != SOCK_IPRAW)
                ;
            ping_wait_ms(1000); // wait 1000ms
            ping_wait_ms(1000); // wait 1000ms
            break;
        case SOCK_IPRAW:
            ping_request(s, addr);
            req++;
            while(1) {
                if((len = getSn_RX_RSR(s)) > 0) {
                    ping_reply(s, addr, len);
                    rep++;
                    break;
                } else if(cnt > 100) {
                    eth_printf("Request Time out.");
                    cnt = 0;
                    break;
                } else {
                    cnt++;
                    ping_wait_ms(50); // wait 50ms
                }
                // wait_time for 2 seconds, Break on fail
            }
            break;
        default:
            break;
        }
#ifdef PING_DEBUG
        if(req >= 3) {
            eth_printf("Ping Request = %d, PING_Reply = %d", req, rep);

            if(rep == req)
                return PING_SUCCESS;
            else
                return REPLY_ERROR;
        }
#endif
    }
    return FUNCTION_ERROR;
}

uint8_t ping_count(uint8_t s, uint16_t pCount, uint8_t* addr) {
    uint16_t rlen, cnt, i;
    cnt = 0;

    for(i = 0; i < pCount + 1; i++) {
        if(i != 0) {
            /* Output count number */
            eth_printf("No.%d", (i - 1));
        }

        uint8_t sr = getSn_SR(s);
        eth_printf("SR: %d", sr);
        switch(sr) {
        case SOCK_CLOSED:
            close(s);
            // close the SOCKET
            /* Create Socket */
            IINCHIP_WRITE(Sn_PROTO(s), IPPROTO_ICMP); // set ICMP Protocol
            if(socket(s, Sn_MR_IPRAW, 3000, 0) !=
               s) { // open the SOCKET with IPRAW mode, if fail then Error
                eth_printf(" socket %d fail ", (s));
#ifdef PING_DEBUG
                return SOCKET_ERROR;
#endif
            }
            /* Check socket register */
            while(getSn_SR(s) != SOCK_IPRAW)
                ;
            ping_wait_ms(1000);
            break;
        case SOCK_IPRAW:
            ping_request(s, addr);
            req++;
            while(1) {
                if((rlen = getSn_RX_RSR(s)) > 0) {
                    ping_reply(s, addr, rlen);
                    rep++;
                    if(ping_reply_received) break;
                }
                /* wait_time for 2 seconds, Break on fail*/
                if((cnt > 100)) {
                    eth_printf("Request Time out.");
                    cnt = 0;
                    break;
                } else {
                    cnt++;
                    ping_wait_ms(20);
                }
            }
            break;
        default:
            break;
        }
#ifdef PING_DEBUG
        if(req >= pCount) {
            eth_printf("Ping Request = %d, PING_Reply = %d", req, rep);
            if(rep == req)
                return PING_SUCCESS;
            else
                return REPLY_ERROR;
        }
#endif
    }
    return FUNCTION_ERROR;
}

uint8_t ping_request(uint8_t s, uint8_t* addr) {
    uint16_t i;

    //Initailize flag for ping reply
    ping_reply_received = 0;
    /* make header of the ping-request  */
    PingRequest.Type = PING_REQUEST; // Ping-Request
    PingRequest.Code = CODE_ZERO; // Always '0'
    PingRequest.ID = htons(RandomID++); // set ping-request's ID to random integer value
    PingRequest.SeqNum =
        htons(RandomSeqNum++); // set ping-request's sequence number to ramdom integer value
    //size = 32;                                 // set Data size

    /* Fill in Data[]  as size of BIF_LEN (Default = 32)*/
    for(i = 0; i < BUF_LEN; i++) {
        PingRequest.Data[i] = (i) % 8; //'0'~'8' number into ping-request's data
    }
    /* Do checksum of Ping Request */
    PingRequest.CheckSum =
        0; // value of checksum before calucating checksum of ping-request packet
    PingRequest.CheckSum =
        htons(checksum((uint8_t*)&PingRequest, sizeof(PingRequest))); // Calculate checksum

    /* sendto ping_request to destination */
    if(sendto(s, (uint8_t*)&PingRequest, sizeof(PingRequest), addr, 3000) ==
       0) { // Send Ping-Request to the specified peer.
        eth_printf(" Fail to send ping-reply packet ");
    } else {
        eth_printf(
            "Send Ping Request  to Destination (%d.%d.%d.%d )",
            (addr[0]),
            (addr[1]),
            (addr[2]),
            (addr[3]));
        eth_printf(
            " ID:%x  SeqNum:%x CheckSum:%x",
            htons(PingRequest.ID),
            htons(PingRequest.SeqNum),
            htons(PingRequest.CheckSum));
    }
    return 0;
} // ping request

uint8_t ping_reply(uint8_t s, uint8_t* addr, uint16_t rlen) {
    uint16_t tmp_checksum;
    uint16_t len;
    uint16_t i;
    uint8_t data_buf[128];
    uint16_t port = 3000;
    PINGMSGR PingReply;
    /* receive data from a destination */
    len = recvfrom(s, (uint8_t*)data_buf, rlen, addr, &port);
    if(data_buf[0] == PING_REPLY) {
        PingReply.Type = data_buf[0];
        PingReply.Code = data_buf[1];
        PingReply.CheckSum = (data_buf[3] << 8) + data_buf[2];
        PingReply.ID = (data_buf[5] << 8) + data_buf[4];
        PingReply.SeqNum = (data_buf[7] << 8) + data_buf[6];

        for(i = 0; i < len - 8; ++i) {
            PingReply.Data[i] = data_buf[8 + i];
        }
        /* check Checksum of Ping Reply */
        tmp_checksum = ~checksum(data_buf, len);
        if(tmp_checksum != 0xffff) {
            eth_printf("tmp_checksum = %x", tmp_checksum);
        } else {
            /*  Output the Destination IP and the size of the Ping Reply Message*/
            eth_printf(
                "Reply from %d.%d.%d.%d  ID:%x SeqNum:%x  :data size %d bytes",
                (addr[0]),
                (addr[1]),
                (addr[2]),
                (addr[3]),
                htons(PingReply.ID),
                htons(PingReply.SeqNum),
                (rlen + 6));
            /*  SET ping_reply_receiver to '1' and go out the while_loop (waitting for ping reply)*/
            ping_reply_received = 1;
        }
    } else if(data_buf[0] == PING_REQUEST) {
        PingReply.Code = data_buf[1];
        PingReply.Type = data_buf[2];
        PingReply.CheckSum = (data_buf[3] << 8) + data_buf[2];
        PingReply.ID = (data_buf[5] << 8) + data_buf[4];
        PingReply.SeqNum = (data_buf[7] << 8) + data_buf[6];

        for(i = 0; i < len - 8; ++i) {
            PingReply.Data[i] = data_buf[8 + i];
        }
        /* check Checksum of Ping Reply */
        tmp_checksum = PingReply.CheckSum;
        PingReply.CheckSum = 0;
        PingReply.CheckSum = htons(checksum((uint8_t*)&PingReply, len));

        if(tmp_checksum != PingReply.CheckSum) {
            eth_printf(
                "CheckSum is in correct %x shold be %x",
                (tmp_checksum),
                htons(PingReply.CheckSum));
        } else {
            eth_printf("Checksum is correct");
        }

        /*  Output the Destination IP and the size of the Ping Reply Message*/
        eth_printf(
            "Request from %d.%d.%d.%d  ID:%x SeqNum:%x  :data size %d bytes",
            (addr[0]),
            (addr[1]),
            (addr[2]),
            (addr[3]),
            (PingReply.ID),
            (PingReply.SeqNum),
            (rlen + 6));
        /*  SET ping_reply_receiver to '1' and go out the while_loop (waitting for ping reply)*/
        ping_reply_received = 1;
    } else {
        eth_printf(" Unkonwn msg.");
    }

    return 0;
} // ping_reply

uint16_t checksum(uint8_t* data_buf, uint16_t len) {
    uint16_t sum, tsum, i, j;
    uint32_t lsum;

    j = len >> 1;
    lsum = 0;
    tsum = 0;
    for(i = 0; i < j; ++i) {
        tsum = data_buf[i * 2];
        tsum = tsum << 8;
        tsum += data_buf[i * 2 + 1];
        lsum += tsum;
    }
    if(len % 2) {
        tsum = data_buf[i * 2];
        lsum += (tsum << 8);
    }
    sum = (uint16_t)lsum;
    sum = ~(sum + (lsum >> 16));
    return sum;
}

uint16_t htons(uint16_t hostshort) {
#if 1
    //#ifdef LITTLE_ENDIAN
    uint16_t netshort = 0;
    netshort = (hostshort & 0xFF) << 8;
    netshort |= ((hostshort >> 8) & 0xFF);
    return netshort;
#else
    return hostshort;
#endif
}
