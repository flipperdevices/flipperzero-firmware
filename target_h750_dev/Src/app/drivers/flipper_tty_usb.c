#include "flipper_tty_hw.h"
#include "usbd_cdc_if.h"

static FuriRecordSubscriber* rx_usb_tty;

void handle_usb_uart_write(const void* data, size_t size, void* ctx) 
{
  CDC_Transmit_FS((uint8_t*)data, (uint16_t)size);
}

void handle_usb_uart_read(uint8_t* data, uint32_t *size) 
{
  furi_write(rx_usb_tty, data, *size);
}

bool initTTYUSBDriver(void)
{
  if(!furi_create("/dev/tty/usb/tx", NULL, 0))
  {
    return false;
  }

  if(furi_open("/dev/tty/usb/tx", false, false, handle_usb_uart_write, NULL, NULL) == NULL) 
  {
		return false;
	}
  
  if(!furi_create("/dev/tty/usb/rx", NULL, 0))
  {
    return false;
  }

  rx_usb_tty = furi_open("/dev/tty/usb/rx", false, false, NULL, NULL, NULL);

  if(rx_usb_tty == NULL) 
  {
		return false;
	}

  return true;
}