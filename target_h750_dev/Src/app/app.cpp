#include "app.h"
#include "main.h"
#include "fatfs.h"
#include "usb_device.h"

#include "drivers/flipper_gpio.h"
#include "drivers/flipper_tty_hw.h"
#include "drivers/flipper_tty_usb.h"
#include "drivers/flipper_fs.h"

#include "libs/version.h"
#include "libs/unique_id.h"
#include <inttypes.h>
#include "libs/ProtocolHelper.h"

#include "apps/app_fileprotocol.h"

extern "C" void main_loop(void){
	app();
}

void app_led_blinker(void *p);
void app_filesystem(void *p);

ProtocolHelper proto;
ProtocolHelper proto_usb;

void app(void){
  initGpioDriver();
  initTTYHWDriver();
  initTTYUSBDriver();
  initFsDriver();

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
  //furiac_start(app_filesystem, "filesystem", NULL);

  /*while (true)
  {
    char* a = proto_usb.readUntil('z');
    proto_usb.printf("%s", a);
    proto.printf("%s", a);
  }*/

  app_fileprotocol(&proto);

  while (true)
  {
    /*if (HAL_GPIO_ReadPin(KEY1_INPUT_GPIO_Port, KEY1_INPUT_Pin) == GPIO_PIN_RESET)
    {
      proto_usb.printf("Hello\r\n");
    }

    if (HAL_GPIO_ReadPin(KEY2_INPUT_GPIO_Port, KEY2_INPUT_Pin) == GPIO_PIN_RESET)
    {
      proto.printf("Heap: %lu B free\r\n", xPortGetFreeHeapSize());
    }*/

    osDelay(250);
  }
};

void app_led_blinker(void* argument) 
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

  /* Open the directory */
	res = f_opendir(&dir, path); 
	if (res == FR_OK)
	{
		while(1)
		{
      /* Read a directory item */
			res = f_readdir(&dir, &fno); 
			if (res != FR_OK || fno.fname[0] == 0)
      {
        /* Break on error or end of dir */
        break;
      }
				
			if (fno.fattrib & AM_DIR)
			{ 
        /* It is a directory */
				i = strlen(path);
				sprintf(&path[i], "/%s", fno.fname);

        /* Enter the directory */
				res = scan_files(path);
				if (res != FR_OK) break;
				path[i] = 0;
			}
			else
			{ 
        /* It is a file. */
				proto.printf("%s/%s\r\n", path, fno.fname);
			}
		}
		f_closedir(&dir);
	} 

	return res;
}

void app_filesystem(void* argument){
  proto.printf("fs task start\r\n");

  uint8_t res = 0;
  FuriRecordSubscriber* fs_record = furi_open("/dev/filesystem", true, false, NULL, NULL, NULL);

  if(fs_record == NULL) 
  {
    proto.printf("filesystem not found\r\n");
    vTaskDelete(NULL);
  }

  char* fs_path = static_cast<char*>(furi_take(fs_record, 1000));

  if(fs_path == NULL)
  {
    proto.printf("filesystem is busy\r\n");
    vTaskDelete(NULL);
  }

  FATFS *fs_data;
  DWORD fre_clust, fre_sect, tot_sect;
  res = f_getfree(fs_path, &fre_clust, &fs_data);
  if(res != FR_OK)
  {
    proto.printf("f_getfree = %d\r\n", res);
    return;
  }

  // Get total sectors and free sectors
  tot_sect = (fs_data->n_fatent - 2) * fs_data->csize;
  fre_sect = fre_clust * fs_data->csize;

  // Print the free space (assuming 512 bytes/sector)
  proto.printf("Sector size: %hu b\r\n", (fs_data->ssize) );
  proto.printf("%10lu kB total disk space\r\n", (tot_sect / (fs_data->ssize / 256)) );
  proto.printf("%10lu kB available\r\n", (fre_sect / (fs_data->ssize / 256)));

  scan_files(fs_path);

  furi_give(fs_record);
  furi_close(fs_record);

  proto.printf("fs task end\r\n");

  while(1)
  {
    osDelay(1000);
  }
}