#include "unitemp.h"

/**
 * @brief Выделение места под переменные плагина
 * 
 * @return true Если всё прошло успешно
 * @return false Если в процессе загрузки произошла ошибка
 */
static bool unitemp_alloc(void) {
    return true;
}

/**
 * @brief Освыбождение памяти после работы приложения
 */
static void unitemp_free(void) {
}

/**
 * @brief Точка входа в приложение
 * 
 * @return Код ошибки
 */
int32_t unitemp_app() {
    //Выделение памяти под переменные
    //Выход если произошла ошибка
    if(unitemp_alloc() == false) {
        //Освобождение памяти
        unitemp_free();
        //Выход
        return 0;
    }

    //Тут рабочий код

    //Освобождение памяти
    unitemp_free();
    //Выход
    return 0;
}