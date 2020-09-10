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
    отправляет OK, ERR когда готово принимать данные
    отправляет size байт из открытого файла, затем \n
    отправляет OK, ERR в зависимости от результата чтения
  f_size
    отправляет размер открытого файла в байтах или ERR
  f_close
    закрывает файл
    отправляет OK
  f_lseek size
    перемещает указатель чтения на size от начала файла
    отправляет OK, ERR в зависимости от результата перемещения
  f_tell
    отправляет текущую позицию указателя 
  id
    уникальный идентификатор устройства
  ls
    выводит список файлов

  пример: 
  -> f_open test.txt r\n
  <- OK\n
  -> f_valid\n
  <- OK\n
  -> f_size\n
  <- 4\n
  -> f_read 4\n
  <- OK\n
  <- 1234\n
  <- OK\n
  -> f_close\n
  <- OK\n
*/

FRESULT scan_files(char *path, ProtocolHelper* proto);

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
          break;
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
          break;
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
        result = f_open(&file, filename, f_open_mode);

        if (result != FR_OK)
        {
          LOGE("cannot open file: %d" EOL, result);
          furi_give(fs_record);
          fs_path = NULL;
          proto->printf("ERR\n");
          break;
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
        uint16_t bytes_to_write = 0;
        uint16_t bytes_written = 0;
        bytes_to_write = atoi(size);
        
        if(bytes_to_write == 0 || bytes_to_write > 128)
        {
          LOGE("size param error" EOL);
          proto->printf("ERR\n");
          break;
        }

        if(fs_path == NULL)
        {
          LOGE("no file" EOL);
          proto->printf("ERR\n");
          break;
        }

        proto->printf("OK\n");

        command_data = proto->readLength(bytes_to_write);
        result = f_write(&file, command_data, bytes_to_write, reinterpret_cast<unsigned int*>(&bytes_written));
        
        if (result != FR_OK)
        {
          LOGE("cannot write to file: %d" EOL, result);
          proto->printf("ERR\n");
          break;
        }

        LOGD("to_write %d, written %d" EOL, bytes_to_write, bytes_written);

        if(bytes_to_write != bytes_written)
        {
          LOGE("disk full, to_write %d, written %d" EOL, bytes_to_write, bytes_written);
          proto->printf("ERR\n");
          break;
        }

        proto->printf("OK\n");
        break;
      }
      case strhash("f_read"):
      {
        char* size = strtok_r(command_data, " ", &command_data);
        uint16_t bytes_to_read = 0;
        uint16_t bytes_readed = 0;
        bytes_to_read = atoi(size);
        char bytes[128];

        if(bytes_to_read == 0 || bytes_to_read > 128)
        {
          LOGE("size param error" EOL);
          proto->printf("ERR\n");
          break;
        }

        if(fs_path == NULL)
        {
          LOGE("no file" EOL);
          proto->printf("ERR\n");
          break;
        }

        proto->printf("OK\n");

        result = f_read(&file, bytes, bytes_to_read, reinterpret_cast<unsigned int*>(&bytes_readed));
        
        if (result != FR_OK)
        {
          LOGE("cannot read from file: %d" EOL, result);
          proto->printf("ERR\n");
          break;
        }

        proto->write(bytes, bytes_readed);
        proto->printf("\n");
        LOGD("to_read %d, readed %d" EOL, bytes_to_read, bytes_readed);

        if(bytes_to_read != bytes_readed)
        {
          LOGE("readed more than file, to_read %d, readed %d" EOL, bytes_to_read, bytes_readed);
          proto->printf("ERR\n");
          break;
        }

        proto->printf("OK\n");
        break;
      }
      case strhash("f_size"):
      {
        if(fs_path == NULL)
        {
          LOGE("no file" EOL);
          proto->printf("ERR\n");
          break;
        }

        proto->printf("%lu" EOL, f_size(&file));
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
        char* size = strtok_r(command_data, " ", &command_data);
        uint32_t bytes_to_seek = 0;
        bytes_to_seek = atoi(size);

        if(fs_path == NULL)
        {
          LOGE("no file" EOL);
          proto->printf("ERR\n");
          break;
        }

        result = f_lseek(&file, bytes_to_seek);
        
        if(result != FR_OK)
        {
          LOGE("cant seek, %d" EOL, result);
          proto->printf("ERR\n");
          break;
        }

        proto->printf("OK\n");
        break;
      }
      case strhash("f_tell"):
      {
        if(fs_path == NULL)
        {
          LOGE("no file" EOL);
          proto->printf("ERR\n");
          break;
        }

        proto->printf("%lu" EOL, f_tell(&file));

        break;
      }
      case strhash("id"):
      {
        proto->printf("%lu" EOL, get_flipper_unique_id());
        break;
      }
      case strhash("ls"):
      {
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

        scan_files(fs_path, proto);

        furi_give(fs_record);
        fs_path = NULL;

        proto->printf("OK\n");
        break;
      }
      default:
        LOGE("unknown command" EOL);
        break;
    }
  }
}

FRESULT scan_files(char *path, ProtocolHelper* proto)
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
				res = scan_files(path, proto);
				if (res != FR_OK) break;
				path[i] = 0;
			}
			else
			{ 
        /* It is a file. */
				proto->printf("%s/%s\r\n", path, fno.fname);
			}
		}
		f_closedir(&dir);
	} 

	return res;
}