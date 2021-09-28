#pragma once
#include <gui/view.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Menu Menu;
typedef struct MenuItem MenuItem;
typedef void (*MenuItemCallback)(void* context, uint32_t index);

Menu* menu_alloc();

void menu_free(Menu* menu);

View* menu_get_view(Menu* menu);

MenuItem* menu_add_item(
    Menu* menu,
    const char* label,
    IconAnimation* icon,
    uint32_t index,
    MenuItemCallback callback,
    void* context);

void menu_clean(Menu* menu);

#ifdef __cplusplus
}
#endif
