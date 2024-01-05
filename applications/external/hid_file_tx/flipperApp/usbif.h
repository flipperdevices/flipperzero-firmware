#include "furi_hal_usb.h"

#define NUM_OF_INTERFACES 6

typedef struct ThreadMessage {
    void* dataPointer;
} ThreadMessage;

void sendBulkData(uint8_t* data, uint8_t len);

void initializeSendingData(int numberOfInterfaces);
void stopSendingData();

FuriMessageQueue* initializeReceivingData();
void stopReceivingData();

FuriHalUsbInterface *getUsbHidBulk();

void sendViaEP(uint8_t* data, int interfaceNumber);
void receiveFromEP(uint8_t* outBuf, int interfaceNumber);
