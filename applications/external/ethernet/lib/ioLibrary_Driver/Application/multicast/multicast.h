#ifndef _MULTICAST_H_
#define _MULTICAST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Multicast test debug message printout enable */
#define _MULTICAST_DEBUG_

#ifndef DATA_BUF_SIZE
	#define DATA_BUF_SIZE			2048
#endif

/* UDP Multicast Loopback test example */
int32_t multicast_loopback(uint8_t sn, uint8_t* buf, uint8_t* multicast_ip, uint16_t multicast_port);

/* UDP Multicast Recv test example */
int32_t multicast_recv(uint8_t sn, uint8_t* buf, uint8_t* multicast_ip, uint16_t multicast_port);


#ifdef __cplusplus
}
#endif

#endif
