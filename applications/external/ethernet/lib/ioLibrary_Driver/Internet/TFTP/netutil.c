#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "netutil.h"

/**
 * Convert a 32bit Address into a Dotted Decimal Format string.
 *
 * @param addr 32bit address.
 * @return Dotted Decimal Format string.
 */
int8_t* inet_ntoa(uint32_t addr)
{
	static int8_t addr_str[16];
	memset(addr_str,0,16);
	sprintf((char*)addr_str,"%d.%d.%d.%d",(int32_t)(addr>>24 & 0xFF),(int32_t)(addr>>16 & 0xFF),(int32_t)(addr>>8 & 0xFF),(int32_t)(addr & 0xFF));
	return addr_str;
}

/**
 * Convert a 32bit Address into a Dotted Decimal Format string.
 * This is differ from inet_ntoa in fixed length.
 *
 * @param addr 32bit address.
 * @return Dotted Decimal Format string.
 */
int8_t* inet_ntoa_pad(uint32_t addr)
{
	static int8_t addr_str[16];
	memset(addr_str,0,16);
	sprintf((char*)addr_str,"%03d.%03d.%03d.%03d",(int32_t)(addr>>24 & 0xFF),(int32_t)(addr>>16 & 0xFF),(int32_t)(addr>>8 & 0xFF),(int32_t)(addr & 0xFF));
	return addr_str;
}

/**
 * Converts a string containing an (Ipv4) Internet Protocol decimal dotted address into a 32bit address.
 *
 * @param addr Dotted Decimal Format string.
 * @return 32bit address.
 */
uint32_t inet_addr(uint8_t* addr)
{
	int8_t i;
	uint32_t inetaddr = 0;
	int8_t taddr[30];
	int8_t * nexttok;
	int32_t num;
	strcpy((char*)taddr,(char*)addr);
	
	nexttok = taddr;
	for(i = 0; i < 4 ; i++)
	{
		nexttok = (int8_t*)strtok((char*)nexttok,".");
		if(nexttok[0] == '0' && nexttok[1] == 'x') num = strtol((char*)nexttok+2, NULL, 16);
		else num = strtol((char*)nexttok, NULL, 10);
		inetaddr = inetaddr << 8;		
		inetaddr |= (num & 0xFF);
		nexttok = NULL;
	}
	return inetaddr;	
}	

/**
 * Swap the byte order of 16bit(short) wide variable.
 *
 * @param i 16bit value to swap
 * @return Swapped value
 */
uint16_t swaps(uint16_t i)
{
	uint16_t ret=0;
	ret = (i & 0xFF) << 8;
	ret |= ((i >> 8)& 0xFF);
	return ret;	
}

/**
 * Swap the byte order of 32bit(long) wide variable.
 *
 * @param l 32bit value to convert
 * @return Swapped value
 */
uint32_t swapl(uint32_t l)
{
	uint32_t ret=0;
	ret = (l & 0xFF) << 24;
	ret |= ((l >> 8) & 0xFF) << 16;
	ret |= ((l >> 16) & 0xFF) << 8;
	ret |= ((l >> 24) & 0xFF);
	return ret;
}

/**
 * htons function converts a unsigned short from host to TCP/IP network byte order (which is big-endian).
 *
 * @param hostshort The value to convert.
 * @return The value in TCP/IP network byte order.
 */ 
uint16_t htons(uint16_t hostshort)
{
#ifdef SYSTEM_LITTLE_ENDIAN
	return swaps(hostshort);
#else
	return hostshort;
#endif		
}


/**
 * htonl function converts a unsigned long from host to TCP/IP network byte order (which is big-endian).
 *
 * @param hostlong The value to convert.
 * @return The value in TCP/IP network byte order.
 */ 
uint32_t htonl(uint32_t hostlong)
{
#ifdef SYSTEM_LITTLE_ENDIAN
	return swapl(hostlong);
#else
	return hostlong;
#endif	
}


/**
 * ntohs function converts a unsigned short from TCP/IP network byte order
 * to host byte order (which is little-endian on Intel processors).
 *
 * @param netshort The value to convert.
 * @return A 16-bit number in host byte order
 */ 
uint32_t ntohs(uint16_t netshort)
{
#ifdef SYSTEM_LITTLE_ENDIAN
	return htons(netshort);
#else
	return netshort;
#endif		
}

/**
 * converts a unsigned long from TCP/IP network byte order to host byte order 
 * (which is little-endian on Intel processors).
 *
 * @param netlong The value to convert.
 * @return A 16-bit number in host byte order
 */ 
uint32_t ntohl(uint32_t netlong)
{
#ifdef SYSTEM_LITTLE_ENDIAN
	return swapl(netlong);
#else
	return netlong;
#endif		
}
/**
 * @}
 */
