#ifndef _SNMP_H_
#define _SNMP_H_

#ifdef __cplusplus
extern "C" {
#endif

// SNMP Debug Message (dump) Enable
#define _SNMP_DEBUG_

#define PORT_SNMP_AGENT				161
#define PORT_SNMP_TRAP				162

#define SNMP_V1						0

#define MAX_OID						12
#define MAX_STRING					64
#define MAX_SNMPMSG_LEN				512
#define MAX_TRAPMSG_LEN				512

// SNMP Error code
#define SNMP_SUCCESS				0
#define OID_NOT_FOUND				-1
#define TABLE_FULL					-2
#define ILLEGAL_LENGTH				-3
#define INVALID_ENTRY_ID			-4
#define INVALID_DATA_TYPE			-5

#define NO_SUCH_NAME				2
#define BAD_VALUE					3

// SNMPv1 Commands
#define GET_REQUEST					0xa0
#define GET_NEXT_REQUEST			0xa1
#define GET_RESPONSE				0xa2
#define SET_REQUEST					0xa3

// Macros: SNMPv1 request validation checker
#define VALID_REQUEST(x)			((x == GET_REQUEST) || (x == GET_NEXT_REQUEST) || (x == SET_REQUEST))

// SNMPv1 Return Types
#define SNMPDTYPE_INTEGER			0x02
#define SNMPDTYPE_OCTET_STRING		0x04
#define SNMPDTYPE_NULL_ITEM			0x05
#define SNMPDTYPE_OBJ_ID			0x06
#define SNMPDTYPE_SEQUENCE			0x30
#define SNMPDTYPE_SEQUENCE_OF		SNMPDTYPE_SEQUENCE

#define SNMPDTYPE_COUNTER			0x41
#define SNMPDTYPE_GAUGE  			0x42
#define SNMPDTYPE_TIME_TICKS		0x43
#define SNMPDTYPE_OPAQUE			0x44

// SNMP Trap: Standard Trap Types (Generic)
#define SNMPTRAP_COLDSTART			0x00	// Generic trap-type 0: Cold Start
#define SNMPTRAP_WARMSTART			0x01	// Generic trap-type 1: Warm Start
#define SNMPTRAP_LINKDOWN			0x02	// Generic trap-type 2: Link Down
#define SNMPTRAP_LINKUP				0x03	// Generic trap-type 3: Link Up
#define SNMPTRAP_AUTHENTICATION		0x04	// Generic trap-type 4: Authentication Failure
#define SNMPTRAP_EGPNEIGHBORLOSS	0x05	// Generic trap-type 5: EGP Neighbor Loss

// Macros
#define COPY_SEGMENT(x) \
{ \
	request_msg.index += seglen; \
	memcpy(&response_msg.buffer[response_msg.index], &request_msg.buffer[x.start], seglen ); \
	response_msg.index += seglen; \
}

#ifndef HTONL
    #define HTONL(x)                    \
        ((((x) >> 24) & 0x000000ff) |   \
         (((x) >>  8) & 0x0000ff00) |   \
         (((x) <<  8) & 0x00ff0000) |   \
         (((x) << 24) & 0xff000000))
#endif

typedef struct {
	uint8_t oidlen;
	uint8_t oid[MAX_OID];
	uint8_t dataType;
	uint8_t dataLen;
	union {
		uint8_t octetstring[MAX_STRING];
		uint32_t intval;
	} u;
	void (*getfunction)(void *, uint8_t *);
	void (*setfunction)(int32_t);
} dataEntryType;

struct messageStruct {
	uint8_t buffer[MAX_SNMPMSG_LEN];
	int32_t len;
	int32_t index;
};

typedef struct {
	int32_t start;		/* Absolute Index of the TLV */
	int32_t len;		/* The L value of the TLV */
	int32_t vstart;		/* Absolute Index of this TLV's Value */
	int32_t nstart;		/* Absolute Index of the next TLV */
} tlvStructType;


/********************************************************************************************/
/* SNMP : Functions                                                                         */
/********************************************************************************************/
// SNMP Main functions
void snmpd_init(uint8_t * managerIP, uint8_t * agentIP, uint8_t sn_agent, uint8_t sn_trap);
int32_t snmpd_run(void);
int32_t snmp_sendTrap(uint8_t * managerIP, uint8_t * agentIP, int8_t* community, dataEntryType enterprise_oid, uint32_t genericTrap, uint32_t specificTrap, uint32_t va_count, ...);

// SNMP Time handler functions
void SNMP_time_handler(void);
uint32_t getSNMPTimeTick(void);
void currentUptime(void *ptr, uint8_t *len);

#ifdef __cplusplus
}
#endif

#endif
