#ifndef UNITEMP_SCENES
#define UNITEMP_SCENES

#include "../unitemp.h"

//Виды менюшек
typedef enum UnitempViews {
    TEMPHUM_VIEW,
} UnitempViews;

/**
 * @brief Создание сцены отображения данных температуры и влажности
 */
void TempHum_secene_alloc(void);

/**
 * @brief //Освобождение памяти сцены
 */
void TempHum_secene_free(void);

#endif