#include <hidapi.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "messages.c"

long long timeInMilliseconds(void) {
    struct timeval tv;

    gettimeofday(&tv,NULL);
    return (((long long)(tv.tv_sec))*1000)+(tv.tv_usec/1000);
}

int main() {
    hid_init();

    hid_device *handle;
    handle = hid_open(0x485, 0xffff, NULL);
	if (!handle) {
		printf("Unable to open device\n\t%ls\n", hid_error(NULL));
		hid_exit();
 		return 1;
	}

    hid_set_nonblocking(handle, 0);

    printf("Waiting for metadata message...\n");
    
    unsigned char buf[64];
    hid_read(handle, buf, 64);

    MessageType msgType;
    FileMetadataMessage *metadata;
    metadata = parseMessage(&msgType, buf);

    if (msgType != MessageMetadata) {
        printf("Did not receive metadata message. Exit.\n");
        exit(1);
    }

    printf("strlen %d\n", strlen(metadata->fileName));
    printf("Receiving %s %.2fMB\n", metadata->fileName, metadata->fileSize / 1000000.0);

    bool received = false;
    void* data = calloc(metadata->fileSize, 1);
    uint64_t bytesReceived = 0;
    long long startTime = (long long)timeInMilliseconds();
    long long lastLogTime = (long long)timeInMilliseconds();

    while (true) {
        unsigned char tmpBuf[65];
        int res = hid_read_timeout(handle, tmpBuf, 65, 2000);
        if (res == 0) {
            printf("Timeout...\n");
            break;
        }

        received = true;

        MessageType tmpMsgType;
        void *tmpMsg;
        tmpMsg = parseMessage(&tmpMsgType, tmpBuf);

        if (tmpMsgType == MessageFullPayload) {
            FullPayloadMessage *msg = tmpMsg;
            // counter = 0 for metadata message, 1 for first payload
            uint32_t counter = msg->header.counter;
            memcpy(data + 61 * (counter - 1), msg->payload, 61);
            bytesReceived += 61;
        } else if (tmpMsgType == MessagePartPayload) {
            PartPayloadMessage *msg = tmpMsg;
            uint32_t counter = msg->header.counter;
            memcpy(data + 61 * (counter - 1), msg->payload, msg->payloadLength);
            bytesReceived += msg->payloadLength;
            printf("Received partPayload message\n");
        } else {
            printf("Received unexpected message\n");
        }

        
        if ((long long)timeInMilliseconds() - lastLogTime >= 2000) {
            lastLogTime = (long long)timeInMilliseconds();
            long long diff = lastLogTime - startTime;
            printf("Received %.2f%% (%.2fMB) @ %.2fkB/s\n", 
                bytesReceived / (double)metadata->fileSize * 100, 
                bytesReceived / 1000000.0, 
                (bytesReceived / 1000.0) / (diff / 1000.0)
            );
        }

        free(tmpMsg);
    }

    if (metadata->fileSize != bytesReceived) {
        printf("Filesize does not match!\n");
    }

    if (received) {
        printf("Writing to file %s (%lu)\n", metadata->fileName, metadata->fileSize);
        FILE *f = fopen(metadata->fileName, "wb");
        int writtenBytes = fwrite(data, metadata->fileSize, 1, f);
        if (writtenBytes == 0) {
            printf("0 bytes written\n");
        }
        if (fclose(f) != 0) {
            printf("Closing file failed\n");
        }
    }

    free(metadata->fileName);
    free(metadata);
    free(data);

    hid_close(handle);
    hid_exit();

    return 0;
}

