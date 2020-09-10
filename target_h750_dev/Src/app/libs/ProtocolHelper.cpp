#include "ProtocolHelper.h"

bool ProtocolHelper::open(const char* tty)
{
  this->tty = tty;
  this->xWriteSemaphore = xSemaphoreCreateMutexStatic( &this->xWriteMutexBuffer );

  tx_tty = furi_open(string(this->tty + "/tx").c_str(), false, false, NULL, NULL, NULL);

  if(tx_tty == NULL) 
  {
    return false;
  }

  auto callback = cbc::obtain_connector(this, &ProtocolHelper::receive);

  rx_tty = furi_open(string(this->tty + "/rx").c_str(), false, false, callback, NULL, this);

  if(rx_tty == NULL) 
  {
    furi_close(tx_tty);
    return false;
  }

  return true;
}


void ProtocolHelper::clearScreen()
{
  // move the cursor to the upper left corner of the screen
  // \033 = ESCAPE (ASCII 27, OCT 33)
  // ESCAPE [ 2 J — Erase Data
  // ESCAPE [ H — Cursor Position 1,1 
  const char command_clear_screen[] = "\033[H\033[J";
  this->write(command_clear_screen, sizeof(command_clear_screen));
}


void ProtocolHelper::write(const char * buffer, uint16_t length)
{
  while( xSemaphoreTake( this->xWriteSemaphore, portMAX_DELAY ) != pdTRUE ){ };
  furi_write(tx_tty, buffer, length);
  xSemaphoreGive( this->xWriteSemaphore );
}


void ProtocolHelper::printf(const char * format, ...)
{
	char buffer[256];
	uint16_t length = 0;

	va_list args;
	va_start(args, format);
	length = vsnprintf(buffer, 256, format, args);
	va_end(args);

  this->write(buffer, length);
}


void ProtocolHelper::close(void)
{
  furi_close(tx_tty);
  furi_close(rx_tty);
}


void ProtocolHelper::receive(const void* data, size_t size, void* ctx)
{
  ProtocolHelper *proto = static_cast<ProtocolHelper*>(ctx);
  if(proto->mode == modeNone){
    return;
  }

  if((proto->position + size) <= MAX_LINE_LENGTH)
  {
    memcpy((proto->read_buffer + proto->position), data, size);
    proto->position += size;
  } 
  else 
  {
    // TODO: this can lose data
    if(size <= MAX_LINE_LENGTH)
    {
      proto->position = 0;
      memcpy((proto->read_buffer + proto->position), data, size);
      proto->position += size;
    }
    else
    {
      proto->position = 0;
    }
  }
}

char ProtocolHelper::read(void)
{
  this->clearBuffer();

  volatile uint16_t old_position = this->position;
  this->mode = modeRead;
  while(old_position == this->position){}
  this->mode = modeNone;
  return this->read_buffer[this->position - 1];
}


char* ProtocolHelper::readUntil(const char symbol)
{
  uint16_t scan_position = 0;
  this->clearBuffer();
  this->mode = modeReadUntil;
  while(1)
  {
    // TODO: rewrite ugly buffer-losing algo
    if(this->read_buffer[scan_position] == symbol)
    {
      break;
    }

    scan_position++;

    if(scan_position > MAX_LINE_LENGTH)
    {
      scan_position = 0;
    }
  }
  this->mode = modeNone;
  this->clearBufferFrom(scan_position);
  return this->read_buffer;
}

char* ProtocolHelper::readLength(uint8_t length)
{
  this->clearBuffer();

  this->mode = modeReadLength;
  while(this->position < length){}
  this->mode = modeNone;
  return this->read_buffer;
}

void ProtocolHelper::clearBuffer(void)
{
  this->position = 0;
  memset(this->read_buffer, 0, MAX_LINE_LENGTH);
}

void ProtocolHelper::clearBufferFrom(uint16_t clear_position)
{
  memset(this->read_buffer + clear_position, 0, MAX_LINE_LENGTH - clear_position);
}