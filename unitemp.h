#ifndef UNITEMP
#define UNITEMP

/* Подключение стандартных библиотек */

/* Подключение API Flipper Zero */
//Файловый поток
#include <toolbox/stream/file_stream.h>
//Уведомления
#include <notification/notification.h>
#include <notification/notification_messages.h>

/* Внутренние библиотеки */
//Интерфейсы подключения датчиков
#include "interfaces/Sensors.h"

/* Объявление макроподстановок */
//Имя приложения
#define APP_NAME "Unitemp"
//Путь хранения файлов плагина
#define APP_PATH_FOLDER "/ext/unitemp"
//Имя файла с настройками
#define APP_FILENAME_SETTINGS "settings.cfg"
//Имя файла с датчиками
#define APP_FILENAME_SENSORS "sensors.cfg"
//Максимальное количество датчиков
#define MAX_SENSORS 5 //Больше на экран не лезет

/* Объявление перечислений */
//Единицы измерения
typedef enum { CELSIUM, FARENGATE } measureUnit;

/* Объявление структур */
//Настройки плагина
typedef struct {
    //Бесконечная работа подсветки
    bool infinityBacklight;
    //Единица измерения температуры
    //Ложь - градусы Цельсия, истина - Фарeнгейты
    measureUnit unit;
    //Последнее состояние OTG
    bool lastOTGState;
} UnitempSettings;

//Основная структура плагина
typedef struct {
    //Основные настройки
    UnitempSettings settings;
    //Список указателей на датчики
    Sensor* sensors[MAX_SENSORS];
    //Количество загруженных датчиков
    size_t sensors_count;

    //SD-карта
    Storage* storage; //Хранилище
    Stream* file_stream; //Файловый поток

    //Уведомления
    NotificationApp* notifications;
} Unitemp;

/* Объявление прототипов функций */

extern Unitemp* app;
#endif