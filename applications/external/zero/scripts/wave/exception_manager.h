#include "scene_management.h"
#include <gui/gui.h>
#include <stdbool.h>

extern const int EXCEPTION_SCENE_ID;

void throw_exception(const char* message);
void throw_exception_if(bool condition, const char* message);

bool exception_manager_has_exception();

const char* get_exception_message();

void exception_manager_handle_exceptions(SceneManager* sceneManager);