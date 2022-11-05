#ifndef UNITEMP_ONE_WIRE
#define UNITEMP_ONE_WIRE

#include "../unitemp.h"

//Интерфейс One Wire
typedef struct {
    //Тип интерфейса подключения датчика
    Interface interface;
    //Порт подключения датчика
    const GPIO* gpio;
    //Время последнего опроса
    uint32_t lastPollingTime;
} OneWireSensor;

#endif