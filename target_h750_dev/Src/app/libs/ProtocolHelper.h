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

// Tag used for difference beetween callback (cbc::obtain_connector)
// because it work like a "get static method"

template <std::size_t Tag>
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

template <std::size_t Tag>
bool ProtocolHelper<Tag>::open(const char* tty)
{
  this->tty = tty;
  this->xWriteSemaphore = xSemaphoreCreateMutexStatic( &this->xWriteMutexBuffer );

  tx_tty = furi_open(string(this->tty + "/tx").c_str(), false, false, NULL, NULL, NULL);

  if(tx_tty == NULL) 
  {
    return false;
  }

  auto callback = cbc::obtain_connector<Tag>(this, &ProtocolHelper::receive);
  rx_tty = furi_open(string(this->tty + "/rx").c_str(), false, false, callback, NULL, NULL);

  if(rx_tty == NULL) 
  {
    furi_close(tx_tty);
    return false;
  }

  return true;
}

template <std::size_t Tag>
void ProtocolHelper<Tag>::clearScreen()
{
  // move the cursor to the upper left corner of the screen
  // \033 = ESCAPE (ASCII 27, OCT 33)
  // ESCAPE [ 2 J — Erase Data
  // ESCAPE [ H — Cursor Position 1,1 
  const char command_clear_screen[] = "\033[H\033[J";
  this->write(command_clear_screen, sizeof(command_clear_screen));
}

template <std::size_t Tag>
void ProtocolHelper<Tag>::write(const char * buffer, uint16_t length)
{
  while( xSemaphoreTake( this->xWriteSemaphore, portMAX_DELAY ) != pdTRUE ){ };
  furi_write(tx_tty, buffer, length);
  xSemaphoreGive( this->xWriteSemaphore );
}

template <std::size_t Tag>
void ProtocolHelper<Tag>::printf(const char * format, ...)
{
	char buffer[256];
	uint16_t length = 0;

	va_list args;
	va_start(args, format);
	length = vsnprintf(buffer, 256, format, args);
	va_end(args);

  this->write(buffer, length);
}

template <std::size_t Tag>
void ProtocolHelper<Tag>::close(void)
{
  furi_close(tx_tty);
  furi_close(rx_tty);
}

template <std::size_t Tag>
void ProtocolHelper<Tag>::receive(const void* data, size_t size, void* ctx)
{
  if(this->mode == modeNone){
    return;
  }

  if((this->position + size) <= MAX_LINE_LENGTH)
  {
    memcpy((this->read_buffer + this->position), data, size);
    this->position += size;
  }
}

template <std::size_t Tag>
char ProtocolHelper<Tag>::read(void)
{
  this->clearBuffer();

  volatile uint16_t old_position = this->position;
  this->mode = modeRead;
  while(old_position == this->position){}
  this->mode = modeNone;
  return this->read_buffer[this->position - 1];
}

template <std::size_t Tag>
char* ProtocolHelper<Tag>::readUntil(const char symbol)
{
  this->clearBuffer();

  this->mode = modeReadUntil;
  while(this->read_buffer[this->position] != symbol){}
  this->mode = modeNone;
  return this->read_buffer;
}

template <std::size_t Tag>
char* ProtocolHelper<Tag>::readLength(uint8_t length)
{
  this->clearBuffer();

  this->mode = modeReadLength;
  while(this->position < length){}
  this->mode = modeNone;
  return this->read_buffer;
}

template <std::size_t Tag>
void ProtocolHelper<Tag>::clearBuffer(void)
{
  this->position = 0;
  memset(this->read_buffer, 0, MAX_LINE_LENGTH);
}