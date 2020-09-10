#include "app_fileprotocol.h"
#include "fatfs.h"
#include "../libs/unique_id.h"
#include <charconv>

#define EOL "\r\n"

#define LOG_LEVEL_NONE 0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_DEBUG 2

#define LOG_LEVEL LOG_LEVEL_DEBUG

#if LOG_LEVEL >= LOG_LEVEL_ERROR 
  #define LOGE(...) (proto->printf("E: " __VA_ARGS__))
#else 
  #define LOGE(...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_DEBUG 
  #define LOGD(...) (proto->printf("D: " __VA_ARGS__))
#else 
  #define LOGD(...)
#endif

/*
  все команды сопровождаются \n

  f_open filename mode 
    открывает filename для чтения (r)/записи (w)
    возвращает OK в случае успешного открытия, ERR в случае неуспешного
  f_valid
    возвращает OK в случае успешного открытия, ERR в случае неуспешного
  f_write size
    отправляет OK, ERR когда готово принимать данные
    принимает size байт и записывает в открытый файл
    отправляет OK, ERR в зависимости от результата записи
  f_read size
    отправляет size байт из открытого файла или ERR 
  f_size
    отправляет размер открытого файла в байтах или ERR
  f_close
    закрывает файл
    отправляет OK
  f_lseek size
    перемещает указатель чтения на size от начала файла
    отправляет OK
  f_tell
    отправляет текущую позицию указателя
  id
    уникальный идентификатор устройства
  
  пример: 
  -> f_open test.txt r\n
  <- OK\n
  -> f_valid\n
  <- OK\n
  -> f_size\n
  <- 4\n
  -> f_read 4\n
  <- 1234\n
  -> f_close\n
  <- OK\n
*/

/*
0://System Volume Information/IndexerVolumeGuid
0://System Volume Information/WPSettings.dat
0://city.xlsx
0://vrack.png
*/

void app_fileprotocol(ProtocolHelper* proto)
{
  proto->printf("flipper file protocol\n");

  char* fs_path = NULL;
  FIL file;
  FuriRecordSubscriber* fs_record = furi_open("/dev/filesystem", true, false, NULL, NULL, NULL);
  uint8_t result = FR_OK;

  if(fs_record == NULL) 
  {
    LOGE("filesystem not found\r\n");
    vTaskDelete(NULL);
  }

  while(1)
  {
    char* command_data = proto->readUntil('\n');
    LOGD("%s" EOL, command_data);

    char* command = strtok_r(command_data, " ", &command_data);

    if(command == NULL)
    {
      LOGE("no command" EOL);
      continue;
    }

    switch(strhash(command))
    {
      case strhash("f_open"):
      {
        char* filename = strtok_r(command_data, " ", &command_data);
        char* mode = strtok_r(command_data, " ", &command_data);
        BYTE f_open_mode;
        bool f_open_mode_valid = false;

        if(filename == NULL || mode == NULL)
        {
          LOGE("usage f_open filename mode" EOL);
          continue;
        }

        if(strcmp(mode, "r") == 0)
        {
          f_open_mode = FA_READ | FA_OPEN_EXISTING;
          f_open_mode_valid = true; 
        }
        else if(strcmp(mode, "w") == 0)
        {
          f_open_mode = FA_WRITE | FA_OPEN_ALWAYS;
          f_open_mode_valid = true;
        }

        if(f_open_mode_valid == false)
        {
          LOGE("unknown mode" EOL);
          continue;
        }

        if(fs_path != NULL)
        {
          LOGE("file already opened" EOL);
          proto->printf("ERR\n");
          break;
        }

        fs_path = static_cast<char*>(furi_take(fs_record, 1000));

        if(fs_path == NULL)
        {
          LOGE("filesystem is busy" EOL);
          proto->printf("ERR\n");
          break;
        }

        LOGD("file \"%s\", mode = \"%s\"" EOL, filename, mode);
        result = f_open(&file, "0:dstfile.dat", f_open_mode);

        if (result != FR_OK)
        {
          LOGE("cannot open file: %d" EOL, result);
          furi_give(fs_record);
          fs_path = NULL;
          proto->printf("ERR\n");
        }

        proto->printf("OK\n");
        break;
      }
      case strhash("f_valid"):
      {
        if(fs_path == NULL)
        {
          proto->printf("ERR\n");
        }
        else
        {
          proto->printf("OK\n");
        }
        break;
      }
      case strhash("f_write"):
      {
        char* size = strtok_r(command_data, " ", &command_data);
        uint16_t usize = 0;
        usize = atoi(size);
        
        if(usize == 0 || usize > 128)
        {
          LOGE("size param error" EOL);
          proto->printf("ERR\n");
        }

        

        break;
      }
      case strhash("f_read"):
      {
        break;
      }
      case strhash("f_size"):
      {
        break;
      }
      case strhash("f_close"):
      {
        result = f_close(&file);

        if (result != FR_OK)
        {
          LOGE("cannot close file: %d" EOL, result);
          proto->printf("ERR\n");
        }

        furi_give(fs_record);
        fs_path = NULL;

        proto->printf("OK\n");

        break;
      }
      case strhash("f_lseek"):
      {
        break;
      }
      case strhash("f_tell"):
      {
        break;
      }
      case strhash("id"):
      {
        proto->printf("%lu" EOL, get_flipper_unique_id());
        break;
      }
      default:
        LOGE("unknown command" EOL);
        break;
    }
  }
}