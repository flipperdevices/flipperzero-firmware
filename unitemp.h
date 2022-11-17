#ifndef UNITEMP
#define UNITEMP

/* Подключение стандартных библиотек */

/* Подключение API Flipper Zero */
//Файловый поток
#include <toolbox/stream/file_stream.h>
//Экран
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/widget.h>
//Уведомления
#include <notification/notification.h>
#include <notification/notification_messages.h>

/* Внутренние библиотеки */
//Интерфейсы подключения датчиков
#include "Sensors.h"

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
typedef enum { CELSIUS, FAHRENHEIT } measureUnit;

/* Объявление структур */
//Настройки плагина
typedef struct {
    //Бесконечная работа подсветки
    bool infinityBacklight;
    //Единица измерения температуры
    measureUnit unit;
    //Последнее состояние OTG
    bool lastOTGState;
} UnitempSettings;

//Основная структура плагина
typedef struct {
    //Система
    bool processing; //Флаг работы приложения. При ложном значении приложение закрывается
    bool sensors_ready; //Флаг готовности датчиков к опросу
    //Основные настройки
    UnitempSettings settings;
    //Список указателей на датчики
    Sensor* sensors[MAX_SENSORS];
    //Количество загруженных датчиков
    uint8_t sensors_count;

    //SD-карта
    Storage* storage; //Хранилище
    Stream* file_stream; //Файловый поток

    //Экран
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    NotificationApp* notifications;
} Unitemp;

/* Объявление прототипов функций */

/**
 * @brief Перевод значения температуры датчика из Цельсия в Фаренгейты
 * 
 * @param sensor Указатель на датчик
 */
void uintemp_celsiumToFarengate(Sensor* sensor);

/**
 * @brief Сохранение настроек на SD-карту
 * 
 * @return Истина если сохранение успешное
 */
bool unitemp_saveSettings(void);
/**
 * @brief Загрузка настроек с SD-карты
 * 
 * @return Истина если загрузка успешная
 */
bool unitemp_loadSettings(void);

extern Unitemp* app;
#endif