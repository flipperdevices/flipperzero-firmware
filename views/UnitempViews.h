#ifndef UNITEMP_SCENES
#define UNITEMP_SCENES

#include "../unitemp.h"

//Виды менюшек
typedef enum UnitempViews {
    VIEW_GENERAL,
    VIEW_MAIN_MENU,
    VIEW_SETTINGS,
    VIEW_SENSORS_LIST,
    VIEW_SENSOR_EDIT,
    VIEW_SENSOR_NAME_EDIT,
    VIEW_SENSOR_ACTIONS,
    VIEW_SENSOR_INFO,

    VIEWS_COUNT
} UnitempViews;

/* Общий вид на датчики */
void unitemp_General_alloc(void);
void unitemp_General_switch(void);
void unitemp_General_free(void);

/* Главное меню */
void unitemp_MainMenu_alloc(void);
void unitemp_MainMenu_switch(void);
void unitemp_MainMenu_free(void);

/* Настройки */
void unitemp_Settings_alloc(void);
void unitemp_Settings_switch(void);
void unitemp_Settings_free(void);

/* Список датчиков */
void unitemp_SensorsList_alloc(void);
void unitemp_SensorsList_switch(void);
void unitemp_SensorsList_free(void);

/* Редактор датчка */
void unitemp_SensorEdit_alloc(void);
//sensor - указатель на редактируемый датчик
void unitemp_SensorEdit_switch(Sensor* sensor);
void unitemp_SensorEdit_free(void);

/* Редактор имени датчика */
void unitemp_SensorNameEdit_alloc(void);
void unitemp_SensorNameEdit_switch(Sensor* sensor);
void unitemp_SensorNameEdit_free(void);

/* Список действий с датчиком */
void unitemp_SensorActions_alloc(void);
void unitemp_SensorActions_switch(Sensor* sensor);
void unitemp_SensorActions_free(void);

#endif