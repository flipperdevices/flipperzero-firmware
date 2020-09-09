#include "app.h"
#include "main.h"
#include "fatfs.h"
#include "usb_device.h"

#include "drivers/flipper_gpio.h"
#include "drivers/flipper_tty_hw.h"
#include "drivers/flipper_tty_usb.h"
#include "sd_diskio.h"

#include "libs/version.h"
#include "libs/unique_id.h"
#include <inttypes.h>
#include "libs/ProtocolHelper.h"

//extern UART_HandleTypeDef huart1;
//UART uart1(&huart1);

extern "C" void main_loop(void){
	app();
}

void app_led_blinker(void *p);
void app_fs(void *p);

ProtocolHelper<1> proto;
ProtocolHelper<2> proto_usb;

void app(void){
  initGpioDriver();
  initTTYHWDriver();
  initTTYUSBDriver();

  proto_usb.open("/dev/tty/usb");

  proto.open("/dev/tty/hw");
  proto.clearScreen();
  proto.printf("System start\r\n");
  proto.printf("Flipper id: %lu\r\n", get_flipper_unique_id());
  proto.printf("Build time: %s %s\r\n", VERSION_DATE, VERSION_TIME);
  proto.printf("Heap:\r\n");
  proto.printf("  %lu bytes total\r\n", configTOTAL_HEAP_SIZE);
  proto.printf("  %lu bytes free\r\n", xPortGetFreeHeapSize());

  furiac_start(app_led_blinker, "led_blinker", NULL);

  while (true)
  {
    char a = proto_usb.read();
    proto_usb.printf("%c", a);
    proto.printf("%c", a);
  }

  while (true)
  {
    if (HAL_GPIO_ReadPin(KEY1_INPUT_GPIO_Port, KEY1_INPUT_Pin) == GPIO_PIN_RESET)
    {
      proto_usb.printf("Hello\r\n");
    }

    if (HAL_GPIO_ReadPin(KEY2_INPUT_GPIO_Port, KEY2_INPUT_Pin) == GPIO_PIN_RESET)
    {
      proto.printf("Heap: %lu B free\r\n", xPortGetFreeHeapSize());
    }

    osDelay(250);
  }
};

void app_led_blinker(void *p) 
{
  proto.printf("led task start\r\n");
  FuriRecordSubscriber* gpio_a1_record = furi_open("/dev/gpio/a/1", true, false, NULL, NULL, NULL);

  if(gpio_a1_record == NULL) 
  {
    proto.printf("pin not found\r\n");
    vTaskDelete(NULL);
  }

  flp_GPIO* pin_a1 = static_cast<flp_GPIO*>(furi_take(gpio_a1_record, 1000));

  if(pin_a1 == NULL)
  {
    proto.printf("pin is busy\r\n");
    vTaskDelete(NULL);
  }

  pinMode(pin_a1, OutputPushPull, NoPullup, LowSpeed);

  while(1)
  {
    digitalWrite(pin_a1, 0);
    osDelay(500);
    digitalWrite(pin_a1, 1);
    osDelay(500);
  }
  
  furi_give(gpio_a1_record);
  vTaskDelete(NULL);
};

FRESULT scan_files(char *path)
{
	FRESULT res;
	DIR dir;
	UINT i;
	static FILINFO fno;

	res = f_opendir(&dir, path); /* Open the directory */
	if (res == FR_OK)
	{
		for (;;)
		{
			res = f_readdir(&dir, &fno); /* Read a directory item */
			if (res != FR_OK || fno.fname[0] == 0)
				break; /* Break on error or end of dir */
			if (fno.fattrib & AM_DIR)
			{ /* It is a directory */
				i = strlen(path);
				sprintf(&path[i], "/%s", fno.fname);
				res = scan_files(path); /* Enter the directory */
				if (res != FR_OK) break;
				path[i] = 0;
			}
			else
			{ /* It is a file. */
				proto.printf("%s/%s\r\n", path, fno.fname);
			}
		}
		f_closedir(&dir);
	}

	return res;
}

void app_fs(void *p){
  proto.printf("fs task start\r\n");
  
  uint8_t res = 0;
	FATFS SDFatFs;
  FIL MyFile;
	char SD_Path[4];

  res = f_mount(&SDFatFs, (TCHAR const*)SD_Path, 1);
	if(res != FR_OK)
  {
    proto.printf("f_mount = %d\r\n", res);
    return;
  }

  FATFS *fs2;
  DWORD fre_clust, fre_sect, tot_sect;
  res = f_getfree(SDPath, &fre_clust, &fs2);
  if(res != FR_OK)
  {
    proto.printf("f_getfree = %d\r\n", res);
    return;
  }
  
  // Get total sectors and free sectors
  tot_sect = (fs2->n_fatent - 2) * fs2->csize;
  fre_sect = fre_clust * fs2->csize;
  // Print the free space (assuming 512 bytes/sector)
  proto.printf("Sector size: %hu b\r\n", (fs2->ssize) );
  proto.printf("%10lu kB total disk space\r\n", (tot_sect / (fs2->ssize / 256)) );
  proto.printf("%10lu kB available\r\n", (fre_sect / (fs2->ssize / 256)));

  scan_files(SDPath);

  proto.printf("fs task end\r\n");
  while(1)
  {
    osDelay(1000);
  }
}