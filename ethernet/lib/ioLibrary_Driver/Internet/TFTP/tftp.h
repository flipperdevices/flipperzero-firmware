/**
 * @file tftp.h
 * @brief TFTP Header File.
 * @version 0.1.0
 * @author Sang-sik Kim
 */
#ifndef __TFTP_H__
#define __TFTP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define F_APP_TFTP
#define __TFTP_DEBUG__

#define F_STORAGE // If your target support a storage, you have to activate this feature and implement.

#define SOCK_TFTP		1

#define INFO_DBG		0x01
#define ERROR_DBG		0x02
#define DEBUG_DBG		0x04
#define IPC_DBG			0x08

#define DBG_PRINT(level, format, args...)		{ \
											if(dbg_level & level) \
												printf(format, ##args); \
										}

#define NORMAL_MODE		0
#define TFTP_MODE		1

extern int dbg_level;

/* tftp message */
#define TFTP_RRQ		1
#define TFTP_WRQ		2
#define TFTP_DATA		3
#define TFTP_ACK		4
#define TFTP_ERROR		5
#define TFTP_OACK		6

/* tftp state */
#define STATE_NONE		0
#define STATE_RRQ		1
#define STATE_WRQ		2
#define STATE_DATA		3
#define STATE_ACK		4
#define STATE_OACK		5

/* tftp transfer mode */
#define TRANS_ASCII		"netascii"
#define TRANS_BINARY	"octet"

/* tftp progress state */
#define TFTP_PROGRESS	0
#define TFTP_FAIL		1
#define TFTP_SUCCESS	2

/* define */
#define TFTP_SERVER_PORT		69
#define TFTP_TEMP_PORT			51000
#define TFTP_BLK_SIZE			512
#define MAX_MTU_SIZE			1514
#define FILE_NAME_SIZE			20

//#define __TFTP_DEBUG__

/* typedef */ 
typedef struct tftp_data {
	uint16_t opcode;
	uint16_t block_num;
	uint8_t data[0];
} TFTP_DATA_T;

typedef struct tftp_error {
	uint16_t opcode;
	uint16_t error_code;
	uint8_t error_msg[0];
} TFTP_ERROR_T;

typedef struct tftp_option {
	uint8_t *code;
	uint8_t *value;
} TFTP_OPTION;

/* Functions */
void TFTP_init(uint8_t socket, uint8_t *buf);
void TFTP_exit(void);
int TFTP_run(void);
void TFTP_read_request(uint32_t server_ip, uint8_t *filename);
void tftp_timeout_handler(void);

#ifdef __cplusplus
}
#endif

#endif	/*__TFTP_H__ */
