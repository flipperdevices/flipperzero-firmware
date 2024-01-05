#include <stdbool.h>
#include "types.h"
#include <string.h>

typedef enum MessageType {
    MessageMetadata = 0,
    MessageFullPayload = 1,
    MessagePartPayload = 2
} MessageType;

// 3 byte
typedef struct {
    uint32_t counter; // 22 bit LSB
    MessageType messageType; // 2 bit 
} MessageHeader;


// 5 byte + len(fileName)
typedef struct {
    MessageHeader header;
    uint32_t fileSize;
    const char* fileName;
} FileMetadataMessage;

// max. 64 byte, see payloadLength
typedef struct {
    // 3 byte
    MessageHeader header;
    // 61 byte
    uint8_t* payload;
} FullPayloadMessage;

// max. 64 byte, see payloadLength
typedef struct {
    // 3 byte
    MessageHeader header;
    uint8_t payloadLength;
    // 61 byte
    uint8_t* payload;
} PartPayloadMessage;



static void* parseMessage(MessageType *outMsg, void *msgBuffer) {
    uint32_t header = 0;
    memcpy(&header, msgBuffer, 3);
    //printf("D: Parse message, header: %lu\n", header);

    MessageType msgType = header & 3;
    uint32_t counter = header >> 2;

    *outMsg = msgType;

    if (msgType == MessageMetadata) {
        printf("D: Parse Metadata message\n");
        uint32_t fileSize;

        int strl = strlen(msgBuffer + 7);
        char *fileName = calloc(strl + 3, 1);

        memcpy(&fileSize, msgBuffer + 3, sizeof(fileSize));
        strncpy(fileName, msgBuffer + 7, strl + 3);

        FileMetadataMessage *msg = malloc(sizeof(FileMetadataMessage));
        memset(msg, 0, sizeof(FileMetadataMessage));

        *msg = (FileMetadataMessage) {
            .header = {
                .counter = counter,
                .messageType = MessageMetadata
            },
            .fileName = fileName,
            .fileSize = fileSize
        };
        return msg;
    } else if (msgType == MessageFullPayload) {
        FullPayloadMessage *msg = calloc(1, sizeof(FullPayloadMessage));
        *msg = (FullPayloadMessage) {
            .header = {
                .counter = counter,
                .messageType = MessageFullPayload
            },
            .payload = msgBuffer + 3
        };
        return msg;
    } else if (msgType == MessagePartPayload) {
        PartPayloadMessage *msg = calloc(1, sizeof(PartPayloadMessage));
        uint8_t length;
        memcpy(&length, msgBuffer + 3, 1);

        *msg = (PartPayloadMessage) {
            .header = {
                .counter = counter,
                .messageType = MessageFullPayload
            },
            .payloadLength = length,
            .payload = msgBuffer + 4
        };
        return msg;
    }

    printf("E: Tried to parse unknown msg! %d\n", msgType);
}
