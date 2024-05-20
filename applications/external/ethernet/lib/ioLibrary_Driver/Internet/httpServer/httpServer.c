#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "socket.h"
#include "wizchip_conf.h"

#include "httpServer.h"
#include "httpParser.h"
#include "httpUtil.h"

#ifdef	_USE_SDCARD_
#include "ff.h" 	// header file for FatFs library (FAT file system)
#endif

#ifndef DATA_BUF_SIZE
	#define DATA_BUF_SIZE		2048
#endif

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
static uint8_t HTTPSock_Num[_WIZCHIP_SOCK_NUM_] = {0, };
static st_http_request * http_request;				/**< Pointer to received HTTP request */
static st_http_request * parsed_http_request;		/**< Pointer to parsed HTTP request */
static uint8_t * http_response;						/**< Pointer to HTTP response */

// ## For Debugging
//static uint8_t uri_buf[128];

// Number of registered web content in code flash memory
static uint16_t total_content_cnt = 0;
/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
uint8_t * pHTTP_TX;
uint8_t * pHTTP_RX;

volatile uint32_t httpServer_tick_1s = 0;
st_http_socket HTTPSock_Status[_WIZCHIP_SOCK_NUM_] = { {STATE_HTTP_IDLE, }, };
httpServer_webContent web_content[MAX_CONTENT_CALLBACK];

#ifdef	_USE_SDCARD_
FIL fs;		// FatFs: File object
FRESULT fr;	// FatFs: File function return code
#endif

/*****************************************************************************
 * Private functions
 ****************************************************************************/
void httpServer_Sockinit(uint8_t cnt, uint8_t * socklist);
static uint8_t getHTTPSocketNum(uint8_t seqnum);
static int8_t getHTTPSequenceNum(uint8_t socket);
static int8_t http_disconnect(uint8_t sn);

static void http_process_handler(uint8_t s, st_http_request * p_http_request);
static void send_http_response_header(uint8_t s, uint8_t content_type, uint32_t body_len, uint16_t http_status);
static void send_http_response_body(uint8_t s, uint8_t * uri_name, uint8_t * buf, uint32_t start_addr, uint32_t file_len);
static void send_http_response_cgi(uint8_t s, uint8_t * buf, uint8_t * http_body, uint16_t file_len);

/*****************************************************************************
 * Public functions
 ****************************************************************************/
// Callback functions definition: MCU Reset / WDT Reset
void default_mcu_reset(void) {;}
void default_wdt_reset(void) {;}
void (*HTTPServer_ReStart)(void) = default_mcu_reset;
void (*HTTPServer_WDT_Reset)(void) = default_wdt_reset;

void httpServer_Sockinit(uint8_t cnt, uint8_t * socklist)
{
	uint8_t i;

	for(i = 0; i < cnt; i++)
	{
		// Mapping the H/W socket numbers to the sequential index numbers
		HTTPSock_Num[i] = socklist[i];
	}
}

static uint8_t getHTTPSocketNum(uint8_t seqnum)
{
	// Return the 'H/W socket number' corresponding to the index number
	return HTTPSock_Num[seqnum];
}

static int8_t getHTTPSequenceNum(uint8_t socket)
{
	uint8_t i;

	for(i = 0; i < _WIZCHIP_SOCK_NUM_; i++)
		if(HTTPSock_Num[i] == socket) return i;

	return -1;
}

void httpServer_init(uint8_t * tx_buf, uint8_t * rx_buf, uint8_t cnt, uint8_t * socklist)
{
	// User's shared buffer
	pHTTP_TX = tx_buf;
	pHTTP_RX = rx_buf;

	// H/W Socket number mapping
	httpServer_Sockinit(cnt, socklist);
}


/* Register the call back functions for HTTP Server */
void reg_httpServer_cbfunc(void(*mcu_reset)(void), void(*wdt_reset)(void))
{
	// Callback: HW Reset and WDT reset function for each MCU platforms
	if(mcu_reset) HTTPServer_ReStart = mcu_reset;
	if(wdt_reset) HTTPServer_WDT_Reset = wdt_reset;
}


void httpServer_run(uint8_t seqnum)
{
	uint8_t s;	// socket number
	uint16_t len;
	uint32_t gettime = 0;

#ifdef _HTTPSERVER_DEBUG_
	uint8_t destip[4] = {0, };
	uint16_t destport = 0;
#endif

	http_request = (st_http_request *)pHTTP_RX;		// Structure of HTTP Request
	parsed_http_request = (st_http_request *)pHTTP_TX;

	// Get the H/W socket number
	s = getHTTPSocketNum(seqnum);

	/* HTTP Service Start */
	switch(getSn_SR(s))
	{
		case SOCK_ESTABLISHED:
			// Interrupt clear
			if(getSn_IR(s) & Sn_IR_CON)
			{
				setSn_IR(s, Sn_IR_CON);
			}

			// HTTP Process states
			switch(HTTPSock_Status[seqnum].sock_status)
			{

				case STATE_HTTP_IDLE :
					if ((len = getSn_RX_RSR(s)) > 0)
					{
						if (len > DATA_BUF_SIZE) len = DATA_BUF_SIZE;
						len = recv(s, (uint8_t *)http_request, len);

						*(((uint8_t *)http_request) + len) = '\0';

						parse_http_request(parsed_http_request, (uint8_t *)http_request);
#ifdef _HTTPSERVER_DEBUG_
						getSn_DIPR(s, destip);
						destport = getSn_DPORT(s);
						printf("\r\n");
						printf("> HTTPSocket[%d] : HTTP Request received ", s);
						printf("from %d.%d.%d.%d : %d\r\n", destip[0], destip[1], destip[2], destip[3], destport);
#endif
#ifdef _HTTPSERVER_DEBUG_
						printf("> HTTPSocket[%d] : [State] STATE_HTTP_REQ_DONE\r\n", s);
#endif
						// HTTP 'response' handler; includes send_http_response_header / body function
						http_process_handler(s, parsed_http_request);

						gettime = get_httpServer_timecount();
						// Check the TX socket buffer for End of HTTP response sends
						while(getSn_TX_FSR(s) != (getSn_TxMAX(s)))
						{
							if((get_httpServer_timecount() - gettime) > 3)
							{
#ifdef _HTTPSERVER_DEBUG_
								printf("> HTTPSocket[%d] : [State] STATE_HTTP_REQ_DONE: TX Buffer clear timeout\r\n", s);
#endif
								break;
							}
						}

						if(HTTPSock_Status[seqnum].file_len > 0) HTTPSock_Status[seqnum].sock_status = STATE_HTTP_RES_INPROC;
						else HTTPSock_Status[seqnum].sock_status = STATE_HTTP_RES_DONE; // Send the 'HTTP response' end
					}
					break;

				case STATE_HTTP_RES_INPROC :
					/* Repeat: Send the remain parts of HTTP responses */
#ifdef _HTTPSERVER_DEBUG_
					printf("> HTTPSocket[%d] : [State] STATE_HTTP_RES_INPROC\r\n", s);
#endif
					// Repeatedly send remaining data to client
					send_http_response_body(s, 0, http_response, 0, 0);

					if(HTTPSock_Status[seqnum].file_len == 0) HTTPSock_Status[seqnum].sock_status = STATE_HTTP_RES_DONE;
					break;

				case STATE_HTTP_RES_DONE :
#ifdef _HTTPSERVER_DEBUG_
					printf("> HTTPSocket[%d] : [State] STATE_HTTP_RES_DONE\r\n", s);
#endif
					// Socket file info structure re-initialize
					HTTPSock_Status[seqnum].file_len = 0;
					HTTPSock_Status[seqnum].file_offset = 0;
					HTTPSock_Status[seqnum].file_start = 0;
					HTTPSock_Status[seqnum].sock_status = STATE_HTTP_IDLE;

//#ifdef _USE_SDCARD_
//					f_close(&fs);
//#endif
#ifdef _USE_WATCHDOG_
					HTTPServer_WDT_Reset();
#endif
					http_disconnect(s);
					break;

				default :
					break;
			}
			break;

		case SOCK_CLOSE_WAIT:
#ifdef _HTTPSERVER_DEBUG_
		printf("> HTTPSocket[%d] : ClOSE_WAIT\r\n", s);	// if a peer requests to close the current connection
#endif
			disconnect(s);
			break;

		case SOCK_CLOSED:
#ifdef _HTTPSERVER_DEBUG_
			printf("> HTTPSocket[%d] : CLOSED\r\n", s);
#endif
			if(socket(s, Sn_MR_TCP, HTTP_SERVER_PORT, 0x00) == s)    /* Reinitialize the socket */
			{
#ifdef _HTTPSERVER_DEBUG_
				printf("> HTTPSocket[%d] : OPEN\r\n", s);
#endif
			}
			break;

		case SOCK_INIT:
			listen(s);
			break;

		case SOCK_LISTEN:
			break;

		default :
			break;

	} // end of switch

#ifdef _USE_WATCHDOG_
	HTTPServer_WDT_Reset();
#endif
}

////////////////////////////////////////////
// Private Functions
////////////////////////////////////////////
static void send_http_response_header(uint8_t s, uint8_t content_type, uint32_t body_len, uint16_t http_status)
{
	switch(http_status)
	{
		case STATUS_OK: 		// HTTP/1.1 200 OK
			if((content_type != PTYPE_CGI) && (content_type != PTYPE_XML)) // CGI/XML type request does not respond HTTP header
			{
#ifdef _HTTPSERVER_DEBUG_
			printf("> HTTPSocket[%d] : HTTP Response Header - STATUS_OK\r\n", s);
#endif
				make_http_response_head((char*)http_response, content_type, body_len);
			}
			else
			{
#ifdef _HTTPSERVER_DEBUG_
			printf("> HTTPSocket[%d] : HTTP Response Header - NONE / CGI or XML\r\n", s);
#endif
				// CGI/XML type request does not respond HTTP header to client
				http_status = 0;
			}
			break;
		case STATUS_BAD_REQ: 	// HTTP/1.1 400 OK
#ifdef _HTTPSERVER_DEBUG_
			printf("> HTTPSocket[%d] : HTTP Response Header - STATUS_BAD_REQ\r\n", s);
#endif
			memcpy(http_response, ERROR_REQUEST_PAGE, sizeof(ERROR_REQUEST_PAGE));
			break;
		case STATUS_NOT_FOUND:	// HTTP/1.1 404 Not Found
#ifdef _HTTPSERVER_DEBUG_
			printf("> HTTPSocket[%d] : HTTP Response Header - STATUS_NOT_FOUND\r\n", s);
#endif
			memcpy(http_response, ERROR_HTML_PAGE, sizeof(ERROR_HTML_PAGE));
			break;
		default:
			break;
	}

	// Send the HTTP Response 'header'
	if(http_status)
	{
#ifdef _HTTPSERVER_DEBUG_
		printf("> HTTPSocket[%d] : [Send] HTTP Response Header [ %d ]byte\r\n", s, (uint16_t)strlen((char *)http_response));
#endif
		send(s, http_response, strlen((char *)http_response));
	}
}

static void send_http_response_body(uint8_t s, uint8_t * uri_name, uint8_t * buf, uint32_t start_addr, uint32_t file_len)
{
	int8_t get_seqnum;
	uint32_t send_len;

	uint8_t flag_datasend_end = 0;

#ifdef _USE_SDCARD_
	uint16_t blocklen;
#endif
#ifdef _USE_FLASH_
	uint32_t addr = 0;
#endif

	if((get_seqnum = getHTTPSequenceNum(s)) == -1) return; // exception handling; invalid number

	// Send the HTTP Response 'body'; requested file
	if(!HTTPSock_Status[get_seqnum].file_len) // ### Send HTTP response body: First part ###
	{
		if (file_len > DATA_BUF_SIZE - 1)
		{
			HTTPSock_Status[get_seqnum].file_start = start_addr;
			HTTPSock_Status[get_seqnum].file_len = file_len;
			send_len = DATA_BUF_SIZE - 1;

/////////////////////////////////////////////////////////////////////////////////////////////////
// ## 20141219 Eric added, for 'File object structure' (fs) allocation reduced (8 -> 1)
			memset(HTTPSock_Status[get_seqnum].file_name, 0x00, MAX_CONTENT_NAME_LEN);
			strcpy((char *)HTTPSock_Status[get_seqnum].file_name, (char *)uri_name);
#ifdef _HTTPSERVER_DEBUG_
			printf("> HTTPSocket[%d] : HTTP Response body - file name [ %s ]\r\n", s, HTTPSock_Status[get_seqnum].file_name);
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _HTTPSERVER_DEBUG_
			printf("> HTTPSocket[%d] : HTTP Response body - file len [ %ld ]byte\r\n", s, file_len);
#endif
		}
		else
		{
			// Send process end
			send_len = file_len;

#ifdef _HTTPSERVER_DEBUG_
			printf("> HTTPSocket[%d] : HTTP Response end - file len [ %ld ]byte\r\n", s, send_len);
#endif
		}
#ifdef _USE_FLASH_
		if(HTTPSock_Status[get_seqnum]->storage_type == DATAFLASH) addr = start_addr;
#endif
	}
	else // remained parts
	{
#ifdef _USE_FLASH_
		if(HTTPSock_Status[get_seqnum]->storage_type == DATAFLASH)
		{
			addr = HTTPSock_Status[get_seqnum].file_start + HTTPSock_Status[get_seqnum].file_offset;
		}
#endif
		send_len = HTTPSock_Status[get_seqnum].file_len - HTTPSock_Status[get_seqnum].file_offset;

		if(send_len > DATA_BUF_SIZE - 1)
		{
			send_len = DATA_BUF_SIZE - 1;
			//HTTPSock_Status[get_seqnum]->file_offset += send_len;
		}
		else
		{
#ifdef _HTTPSERVER_DEBUG_
			printf("> HTTPSocket[%d] : HTTP Response end - file len [ %ld ]byte\r\n", s, HTTPSock_Status[get_seqnum].file_len);
#endif
			// Send process end
			flag_datasend_end = 1;
		}
#ifdef _HTTPSERVER_DEBUG_
			printf("> HTTPSocket[%d] : HTTP Response body - send len [ %ld ]byte\r\n", s, send_len);
#endif
	}

/*****************************************************/
	//HTTPSock_Status[get_seqnum]->storage_type == NONE
	//HTTPSock_Status[get_seqnum]->storage_type == CODEFLASH
	//HTTPSock_Status[get_seqnum]->storage_type == SDCARD
	//HTTPSock_Status[get_seqnum]->storage_type == DATAFLASH
/*****************************************************/

	if(HTTPSock_Status[get_seqnum].storage_type == CODEFLASH)
	{
		if(HTTPSock_Status[get_seqnum].file_len) start_addr = HTTPSock_Status[get_seqnum].file_start;
		read_userReg_webContent(start_addr, &buf[0], HTTPSock_Status[get_seqnum].file_offset, send_len);
	}
#ifdef _USE_SDCARD_
	else if(HTTPSock_Status[get_seqnum].storage_type == SDCARD)
	{
		// Data read from SD Card
		fr = f_read(&fs, &buf[0], send_len, (void *)&blocklen);
		if(fr != FR_OK)
		{
			send_len = 0;
#ifdef _HTTPSERVER_DEBUG_
		printf("> HTTPSocket[%d] : [FatFs] Error code return: %d (File Read) / HTTP Send Failed - %s\r\n", s, fr, HTTPSock_Status[get_seqnum].file_name);
#endif
		}
		else
		{
			*(buf+send_len+1) = 0; // Insert '/0' for indicates the 'End of String' (null terminated)
		}
	}
#endif

#ifdef _USE_FLASH_
	else if(HTTPSock_Status[get_seqnum]->storage_type == DATAFLASH)
	{
		// Data read from external data flash memory
		read_from_flashbuf(addr, &buf[0], send_len);
		*(buf+send_len+1) = 0; // Insert '/0' for indicates the 'End of String' (null terminated)
	}
#endif
	else
	{
		send_len = 0;
	}
	// Requested content send to HTTP client
#ifdef _HTTPSERVER_DEBUG_
	printf("> HTTPSocket[%d] : [Send] HTTP Response body [ %ld ]byte\r\n", s, send_len);
#endif

	if(send_len) send(s, buf, send_len);
	else flag_datasend_end = 1;

	if(flag_datasend_end)
	{
		HTTPSock_Status[get_seqnum].file_start = 0;
		HTTPSock_Status[get_seqnum].file_len = 0;
		HTTPSock_Status[get_seqnum].file_offset = 0;
		flag_datasend_end = 0;
	}
	else
	{
		HTTPSock_Status[get_seqnum].file_offset += send_len;
#ifdef _HTTPSERVER_DEBUG_
		printf("> HTTPSocket[%d] : HTTP Response body - offset [ %ld ]\r\n", s, HTTPSock_Status[get_seqnum].file_offset);
#endif
	}

// ## 20141219 Eric added, for 'File object structure' (fs) allocation reduced (8 -> 1)
#ifdef _USE_SDCARD_
	f_close(&fs);
#endif
// ## 20141219 added end
}

static void send_http_response_cgi(uint8_t s, uint8_t * buf, uint8_t * http_body, uint16_t file_len)
{
	uint16_t send_len = 0;

#ifdef _HTTPSERVER_DEBUG_
	printf("> HTTPSocket[%d] : HTTP Response Header + Body - CGI\r\n", s);
#endif
	send_len = sprintf((char *)buf, "%s%d\r\n\r\n%s", RES_CGIHEAD_OK, file_len, http_body);
#ifdef _HTTPSERVER_DEBUG_
	printf("> HTTPSocket[%d] : HTTP Response Header + Body - send len [ %d ]byte\r\n", s, send_len);
#endif

	send(s, buf, send_len);
}


static int8_t http_disconnect(uint8_t sn)
{
	setSn_CR(sn,Sn_CR_DISCON);
	/* wait to process the command... */
	while(getSn_CR(sn));

	return SOCK_OK;
}


static void http_process_handler(uint8_t s, st_http_request * p_http_request)
{
	uint8_t * uri_name;
	uint32_t content_addr = 0;
	uint16_t content_num = 0;
	uint32_t file_len = 0;

	uint8_t uri_buf[MAX_URI_SIZE]={0x00, };

	uint16_t http_status;
	int8_t get_seqnum;
	uint8_t content_found;

	if((get_seqnum = getHTTPSequenceNum(s)) == -1) return; // exception handling; invalid number

	http_status = 0;
	http_response = pHTTP_RX;
	file_len = 0;

	//method Analyze
	switch (p_http_request->METHOD)
	{
		case METHOD_ERR :
			http_status = STATUS_BAD_REQ;
			send_http_response_header(s, 0, 0, http_status);
			break;

		case METHOD_HEAD :
		case METHOD_GET :
			get_http_uri_name(p_http_request->URI, uri_buf);
			uri_name = uri_buf;

			if (!strcmp((char *)uri_name, "/")) strcpy((char *)uri_name, INITIAL_WEBPAGE);	// If URI is "/", respond by index.html
			if (!strcmp((char *)uri_name, "m")) strcpy((char *)uri_name, M_INITIAL_WEBPAGE);
			if (!strcmp((char *)uri_name, "mobile")) strcpy((char *)uri_name, MOBILE_INITIAL_WEBPAGE);
			find_http_uri_type(&p_http_request->TYPE, uri_name);	// Checking requested file types (HTML, TEXT, GIF, JPEG and Etc. are included)

#ifdef _HTTPSERVER_DEBUG_
			printf("\r\n> HTTPSocket[%d] : HTTP Method GET\r\n", s);
			printf("> HTTPSocket[%d] : Request Type = %d\r\n", s, p_http_request->TYPE);
			printf("> HTTPSocket[%d] : Request URI = %s\r\n", s, uri_name);
#endif

			if(p_http_request->TYPE == PTYPE_CGI)
			{
				content_found = http_get_cgi_handler(uri_name, pHTTP_TX, &file_len);
				if(content_found && (file_len <= (DATA_BUF_SIZE-(strlen(RES_CGIHEAD_OK)+8))))
				{
					send_http_response_cgi(s, http_response, pHTTP_TX, (uint16_t)file_len);
				}
				else
				{
					send_http_response_header(s, PTYPE_CGI, 0, STATUS_NOT_FOUND);
				}
			}
			else
			{
				// Find the User registered index for web content
				if(find_userReg_webContent(uri_buf, &content_num, &file_len))
				{
					content_found = 1; // Web content found in code flash memory
					content_addr = (uint32_t)content_num;
					HTTPSock_Status[get_seqnum].storage_type = CODEFLASH;
				}
				// Not CGI request, Web content in 'SD card' or 'Data flash' requested
#ifdef _USE_SDCARD_
#ifdef _HTTPSERVER_DEBUG_
				printf("\r\n> HTTPSocket[%d] : Searching the requested content\r\n", s);
#endif
				if((fr = f_open(&fs, (const char *)uri_name, FA_READ)) == 0)
				{
					content_found = 1; // file open succeed

					file_len = fs.fsize;
					content_addr = fs.sclust;
					HTTPSock_Status[get_seqnum].storage_type = SDCARD;
				}
#elif _USE_FLASH_
				else if(/* Read content from Dataflash */)
				{
					content_found = 1;
					HTTPSock_Status[get_seqnum]->storage_type = DATAFLASH;
					; // To do
				}
#endif
				else
				{
					content_found = 0; // fail to find content
				}

				if(!content_found)
				{
#ifdef _HTTPSERVER_DEBUG_
					printf("> HTTPSocket[%d] : Unknown Page Request\r\n", s);
#endif
					http_status = STATUS_NOT_FOUND;
				}
				else
				{
#ifdef _HTTPSERVER_DEBUG_
					printf("> HTTPSocket[%d] : Find Content [%s] ok - Start [%ld] len [ %ld ]byte\r\n", s, uri_name, content_addr, file_len);
#endif
					http_status = STATUS_OK;
				}

				// Send HTTP header
				if(http_status)
				{
#ifdef _HTTPSERVER_DEBUG_
					printf("> HTTPSocket[%d] : Requested content len = [ %ld ]byte\r\n", s, file_len);
#endif
					send_http_response_header(s, p_http_request->TYPE, file_len, http_status);
				}

				// Send HTTP body (content)
				if(http_status == STATUS_OK)
				{
					send_http_response_body(s, uri_name, http_response, content_addr, file_len);
				}
			}
			break;

		case METHOD_POST :
			mid((char *)p_http_request->URI, "/", " HTTP", (char *)uri_buf);
			uri_name = uri_buf;
			find_http_uri_type(&p_http_request->TYPE, uri_name);	// Check file type (HTML, TEXT, GIF, JPEG are included)

#ifdef _HTTPSERVER_DEBUG_
			printf("\r\n> HTTPSocket[%d] : HTTP Method POST\r\n", s);
			printf("> HTTPSocket[%d] : Request URI = %s ", s, uri_name);
			printf("Type = %d\r\n", p_http_request->TYPE);
#endif

			if(p_http_request->TYPE == PTYPE_CGI)	// HTTP POST Method; CGI Process
			{
				content_found = http_post_cgi_handler(uri_name, p_http_request, http_response, &file_len);
#ifdef _HTTPSERVER_DEBUG_
				printf("> HTTPSocket[%d] : [CGI: %s] / Response len [ %ld ]byte\r\n", s, content_found?"Content found":"Content not found", file_len);
#endif
				if(content_found && (file_len <= (DATA_BUF_SIZE-(strlen(RES_CGIHEAD_OK)+8))))
				{
					send_http_response_cgi(s, pHTTP_TX, http_response, (uint16_t)file_len);

					// Reset the H/W for apply to the change configuration information
					if(content_found == HTTP_RESET) HTTPServer_ReStart();
				}
				else
				{
					send_http_response_header(s, PTYPE_CGI, 0, STATUS_NOT_FOUND);
				}
			}
			else	// HTTP POST Method; Content not found
			{
				send_http_response_header(s, 0, 0, STATUS_NOT_FOUND);
			}
			break;

		default :
			http_status = STATUS_BAD_REQ;
			send_http_response_header(s, 0, 0, http_status);
			break;
	}
}

void httpServer_time_handler(void)
{
	httpServer_tick_1s++;
}

uint32_t get_httpServer_timecount(void)
{
	return httpServer_tick_1s;
}

void reg_httpServer_webContent(uint8_t * content_name, uint8_t * content)
{
	uint16_t name_len;
	uint32_t content_len;

	if(content_name == NULL || content == NULL)
	{
		return;
	}
	else if(total_content_cnt >= MAX_CONTENT_CALLBACK)
	{
		return;
	}

	name_len = strlen((char *)content_name);
	content_len = strlen((char *)content);

	web_content[total_content_cnt].content_name = malloc(name_len+1);
	strcpy((char *)web_content[total_content_cnt].content_name, (const char *)content_name);
	web_content[total_content_cnt].content_len = content_len;
	web_content[total_content_cnt].content = content;

	total_content_cnt++;
}

uint8_t display_reg_webContent_list(void)
{
	uint16_t i;
	uint8_t ret;

	if(total_content_cnt == 0)
	{
		printf(">> Web content file not found\r\n");
		ret = 0;
	}
	else
	{
		printf("\r\n=== List of Web content in code flash ===\r\n");
		for(i = 0; i < total_content_cnt; i++)
		{
			printf(" [%d] ", i+1);
			printf("%s, ", web_content[i].content_name);
			printf("%ld byte, ", web_content[i].content_len);

			if(web_content[i].content_len < 30) printf("[%s]\r\n", web_content[i].content);
			else printf("[ ... ]\r\n");
		}
		printf("=========================================\r\n\r\n");
		ret = 1;
	}

	return ret;
}

uint8_t find_userReg_webContent(uint8_t * content_name, uint16_t * content_num, uint32_t * file_len)
{
	uint16_t i;
	uint8_t ret = 0; // '0' means 'File Not Found'

	for(i = 0; i < total_content_cnt; i++)
	{
		if(!strcmp((char *)content_name, (char *)web_content[i].content_name))
		{
			*file_len = web_content[i].content_len;
			*content_num = i;
			ret = 1; // If the requested content found, ret set to '1' (Found)
			break;
		}
	}
	return ret;
}


uint16_t read_userReg_webContent(uint16_t content_num, uint8_t * buf, uint32_t offset, uint16_t size)
{
	uint16_t ret = 0;
	uint8_t * ptr;

	if(content_num > total_content_cnt) return 0;

	ptr = web_content[content_num].content;
	if(offset) ptr += offset;

	strncpy((char *)buf, (char *)ptr, size);
	*(buf+size) = 0; // Insert '/0' for indicates the 'End of String' (null terminated)

	ret = strlen((void *)buf);
	return ret;
}
