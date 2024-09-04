#ifdef __cplusplus
extern "C" {
#endif

typedef struct JsGui JsGui;

/**
 * @brief Tells the main GUI module to free a view when it's torn down
 * 
 * This API function is mainly of use to glue code for the various views. For
 * more info on this design choice, look at `JsViewGhost`.
 */
void js_gui_defer_free(JsGui* module, void (*freer)(void*), void* instance);

/**
 * @brief Like `js_gui_defer_free`, but auto-casts the function pointer
 */
#define JS_GUI_DEFER_FREE(module, freer, instance) \
    js_gui_defer_free((module), (void (*)(void*))(freer), (instance))

#ifdef __cplusplus
}
#endif
