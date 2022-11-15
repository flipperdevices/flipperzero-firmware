#ifndef UNITEMP_SCENES
#define UNITEMP_SCENES

#include "../unitemp.h"

//Виды менюшек
typedef enum UnitempViews {
    SUMMARY_VIEW,
    MAINMENU_VIEW,
    SETTINGS_VIEW,

    VIEWS_COUNT
} UnitempViews;

/* Общий вид на датчики */
void unitemp_Summary_alloc(void);
void unitemp_Summary_free(void);

/* Главное меню */
void unitemp_MainMenu_alloc(void);
void unitemp_MainMenu_switch(void);
void unitemp_MainMenu_free(void);

/* Настройки */
void unitemp_Settings_alloc(void);
void unitemp_Settings_switch(void);
void unitemp_Settings_free(void);
#endif