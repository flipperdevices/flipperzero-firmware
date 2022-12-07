#ifndef UNITEMP_SCENES
#define UNITEMP_SCENES

#include "../unitemp.h"

//Виды менюшек
typedef enum UnitempViews {
    UnitempViewGeneral,
    UnitempViewMainMenu,
    UnitempViewSettings,
    UnitempViewSensorsList,
    UnitempViewSensorEdit,
    UnitempViewSensorNameEdit,
    UnitempViewSensorActions,
    UnitempViewWidget,
    UnitempViewPopup,

    UnitempViewsCount
} UnitempViews;

/**
 * @brief Вывести всплывающее окно
 * 
 * @param icon Указатель на иконку
 * @param header Заголовок
 * @param message Сообщение
 * @param prev_view_id ID вида куда в который нужно вернуться
 */
void unitemp_popup(const Icon* icon, char* header, char* message, uint32_t prev_view_id);

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

/* Виджеты */
void unitemp_widgets_alloc(void);
void unitemp_widgets_free(void);

/* Подтверждение удаления */
void unitemp_widget_delete_switch(Sensor* sensor);
/* Помощь */
void unitemp_widget_help_switch(void);
/* О приложении */
void unitemp_widget_about_switch(void);
#endif