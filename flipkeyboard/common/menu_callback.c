#include "menu_callback.h"

/**
 * @brief Callback for menu navigation
 * @param _context Unused parameter.
 * @return returns index 0.
*/
static uint32_t menu_0(void* _context) {
    UNUSED(_context);
    return (uint32_t)0;
}

/**
 * @brief Callback for menu navigation
 * @param _context Unused parameter.
 * @return returns index 1.
*/
static uint32_t menu_1(void* _context) {
    UNUSED(_context);
    return (uint32_t)1;
}

/**
 * @brief Callback for menu navigation
 * @param _context Unused parameter.
 * @return returns index 2.
*/
static uint32_t menu_2(void* _context) {
    UNUSED(_context);
    return (uint32_t)2;
}

/**
 * @brief Callback for menu navigation
 * @param _context Unused parameter.
 * @return returns index 3.
*/
static uint32_t menu_3(void* _context) {
    UNUSED(_context);
    return (uint32_t)3;
}

/**
 * @brief Callback for menu navigation
 * @param _context Unused parameter.
 * @return returns index 4.
*/
static uint32_t menu_4(void* _context) {
    UNUSED(_context);
    return (uint32_t)4;
}

/**
 * @brief Callback for menu navigation
 * @param _context Unused parameter.
 * @return returns index 5.
*/
static uint32_t menu_5(void* _context) {
    UNUSED(_context);
    return (uint32_t)5;
}

/**
 * @brief Callback for menu navigation
 * @param _context Unused parameter.
 * @return returns index 6.
*/
static uint32_t menu_6(void* _context) {
    UNUSED(_context);
    return (uint32_t)6;
}

/**
 * @brief Callback for menu navigation
 * @param _context Unused parameter.
 * @return returns index 7.
*/
static uint32_t menu_7(void* _context) {
    UNUSED(_context);
    return (uint32_t)7;
}

/**
 * @brief Callback for menu navigation
 * @param _context Unused parameter.
 * @return returns index 8.
*/
static uint32_t menu_8(void* _context) {
    UNUSED(_context);
    return (uint32_t)8;
}

/**
 * @brief Callback for menu navigation
 * @param _context Unused parameter.
 * @return returns index 9.
*/
static uint32_t menu_9(void* _context) {
    UNUSED(_context);
    return (uint32_t)9;
}

/**
 * @brief Callback for menu navigation
 * @param _context Unused parameter.
 * @return returns index 10.
*/
static uint32_t menu_10(void* _context) {
    UNUSED(_context);
    return (uint32_t)10;
}

/**
 * @brief Callback for menu navigation
 * @param _context Unused parameter.
 * @return returns index 11.
*/
static uint32_t menu_11(void* _context) {
    UNUSED(_context);
    return (uint32_t)11;
}

/**
 * @brief Callback for menu navigation
 * @param _context Unused parameter.
 * @return returns index 12.
*/
static uint32_t menu_12(void* _context) {
    UNUSED(_context);
    return (uint32_t)12;
}

/**
 * @brief Callback for menu navigation
 * @param _context Unused parameter.
 * @return returns index 13.
*/
static uint32_t menu_13(void* _context) {
    UNUSED(_context);
    return (uint32_t)13;
}

/**
 * @brief Callback for menu navigation
 * @param _context Unused parameter.
 * @return returns index 14.
*/
static uint32_t menu_14(void* _context) {
    UNUSED(_context);
    return (uint32_t)14;
}

/**
 * @brief Callback for menu navigation
 * @note if return_index is out of range, it will return a callback that
 * returns 0.
 * @param return_index The value the callback should return (1-14).
 * @return The callback to be used for menu navigation.
 */
ViewNavigationCallback get_menu_callback(uint32_t return_index) {
    switch(return_index) {
    case 1:
        return menu_1;
    case 2:
        return menu_2;
    case 3:
        return menu_3;
    case 4:
        return menu_4;
    case 5:
        return menu_5;
    case 6:
        return menu_6;
    case 7:
        return menu_7;
    case 8:
        return menu_8;
    case 9:
        return menu_9;
    case 10:
        return menu_10;
    case 11:
        return menu_11;
    case 12:
        return menu_12;
    case 13:
        return menu_13;
    case 14:
        return menu_14;
    default:
        FURI_LOG_E("Flipboard", "Invalid menu index %ld", return_index);
        furi_assert(return_index < 15);
        return menu_0;
    }
}