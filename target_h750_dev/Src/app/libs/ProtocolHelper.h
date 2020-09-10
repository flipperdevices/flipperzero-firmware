#include "main.h"
#include "../core/furi.h"
#include <string>
#include "string.h"
#include "CallbackConnector.h"
#include "helpers.hpp"
#include <stdarg.h>

#define MAX_LINE_LENGTH 256

using namespace std;

typedef enum {
  modeNone = 0,
  modeRead = 1,
  modeReadUntil = 2,
  modeReadLength = 3,
} ProtocolHelper_Read_Mode;

class ProtocolHelper
{
  public:
    bool open(const char* tty);
    void close(void);
    void write(const char * buffer, uint16_t length);
    void printf(const char * format, ...);
    void clearScreen(void);
    char read(void);
    char* readUntil(const char symbol);
    char* readLength(uint8_t length);
  private:
    void clearBuffer(void);
    void clearBufferFrom(uint16_t clear_position);
    string tty;
    char read_buffer[MAX_LINE_LENGTH + 1];
    volatile uint16_t position = 0;
    FuriRecordSubscriber* tx_tty;
    FuriRecordSubscriber* rx_tty;
    void receive(const void* data, size_t size, void* ctx);
    volatile ProtocolHelper_Read_Mode mode = modeNone;
    SemaphoreHandle_t xWriteSemaphore = NULL;
    StaticSemaphore_t xWriteMutexBuffer;
};