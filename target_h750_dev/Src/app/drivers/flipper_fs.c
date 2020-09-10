#include "flipper_fs.h"

bool initFsDriver(){
  uint8_t res = 0;

  // SDFatFS and SDPath prepared in MX_FATFS_Init()
  res = f_mount(&SDFatFS, (TCHAR const*)SDPath, 1);
  if(res != FR_OK)
  {
    return false;
  }

  if(!furi_create("/dev/filesystem", (void*)&SDPath, sizeof(SDPath))) {
    return false;
  }

  return true;
}