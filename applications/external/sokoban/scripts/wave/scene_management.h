#include <furi.h>
#include <gui/gui.h>

extern const int SCENE_MANAGER_NO_SCENE;

typedef void (*RenderCallback)(Canvas*, void*);
typedef void (*InputCallback)(InputKey, InputType, void*);
typedef void (*TickCallback)(void*);
typedef void (*TransitionCallback)(int from, int to, void*);

typedef struct Scene Scene;
typedef struct SceneManager SceneManager;

Scene* scene_alloc(RenderCallback render_callback, TickCallback tick_callback, InputCallback input_callback, TransitionCallback transition_callback, void* context);
void scene_destroy(Scene* s);

SceneManager* scene_manager_alloc_auto();
SceneManager* scene_manager_alloc(ViewPort* viewPort, Gui* gui, FuriMessageQueue* eventQueue);
void scene_manager_free(SceneManager* sm);
void scene_manager_attach_to_gui(SceneManager* sceneManager, Gui* gui);
void scene_manager_detach_from_gui(SceneManager* sceneManager, Gui* gui);
void scene_manager_register_scene(SceneManager* sm, int id, Scene* scene);
void scene_manager_set_scene(SceneManager* sm, int id);
void scene_manager_tick(SceneManager* sm);
int scene_manager_get_current_scene_id(SceneManager* sm);
bool scene_manager_has_scene(SceneManager* sm);