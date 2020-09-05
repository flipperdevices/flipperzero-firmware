#include <stdint.h>
#include <stdlib.h>
#include <limits.h>

#define HAL_MAX_DELAY INT_MAX

typedef struct device* SerialDevice;
void app_serial_write(SerialDevice* serial, uint8_t* buffer, uint16_t size, uint32_t timeout);

typedef uint32_t TIM_HandleTypeDef;
