#include "scene_management.h"
#include <stdlib.h>
#include <gui/gui.h>

const int SceneManagerNoScene = -1;

typedef void (*RenderCallback)(Canvas*, void*);
typedef void (*InputCallback)(InputKey, InputType, void*);
typedef void (*TickCallback)(void*);
typedef void (*TransitionCallback)(int from, int to, void*);

struct Scene {
    RenderCallback renderCallback;
    TickCallback tickCallback;
    InputCallback inputCallback;
    TransitionCallback transitionCallback;
    void* context;
};

typedef struct ScenesListNode {
    int id;
    Scene* scene;
    struct ScenesListNode* next;
} ScenesListNode;

struct SceneManager {
    ScenesListNode* scenesListHead;
    ScenesListNode* currentScene;
    ViewPort* viewPort;
    FuriMessageQueue* eventQueue;
};

Scene* scene_alloc(
    RenderCallback renderCallback,
    TickCallback tickCallback,
    InputCallback inputCallback,
    TransitionCallback transitionCallback,
    void* context) {
    Scene* scene = (Scene*)malloc(sizeof(Scene));
    scene->renderCallback = renderCallback;
    scene->tickCallback = tickCallback;
    scene->inputCallback = inputCallback;
    scene->transitionCallback = transitionCallback;
    scene->context = context;
    return scene;
}

void scene_destroy(Scene* s) {
    free(s);
}

Scene* scene_manager_get_current_scene(SceneManager* sceneManager) {
    if(sceneManager == NULL || sceneManager->currentScene == NULL) return NULL;

    return sceneManager->currentScene->scene;
}

void scene_manager_tick(SceneManager* sceneManager) {
    while(furi_message_queue_get_count(sceneManager->eventQueue) > 0) {
        InputEvent input;
        furi_message_queue_get(sceneManager->eventQueue, &input, 0);
        Scene* currentScene = scene_manager_get_current_scene(sceneManager);
        if(currentScene && currentScene->inputCallback)
            currentScene->inputCallback(input.key, input.type, currentScene->context);
    }

    Scene* currentScene = scene_manager_get_current_scene(sceneManager);
    if(currentScene && currentScene->tickCallback)
        currentScene->tickCallback(currentScene->context);

    view_port_update(sceneManager->viewPort);
}

void scene_manager_draw_callback(Canvas* canvas, void* context) {
    SceneManager* sceneManager = (SceneManager*)context;
    Scene* currentScene = scene_manager_get_current_scene(sceneManager);
    if(currentScene && currentScene->renderCallback)
        currentScene->renderCallback(canvas, currentScene->context);
}

void scene_manager_input_callback(InputEvent* input_event, void* context) {
    SceneManager* sceneManager = (SceneManager*)context;
    Scene* currentScene = scene_manager_get_current_scene(sceneManager);
    if(currentScene)
        furi_message_queue_put(sceneManager->eventQueue, input_event, FuriWaitForever);
}

SceneManager* scene_manager_alloc() {
    SceneManager* sceneManager = (SceneManager*)malloc(sizeof(SceneManager));
    sceneManager->viewPort = view_port_alloc();
    sceneManager->eventQueue = furi_message_queue_alloc(8, sizeof(InputEvent));
    view_port_draw_callback_set(sceneManager->viewPort, scene_manager_draw_callback, sceneManager);
    view_port_input_callback_set(
        sceneManager->viewPort, scene_manager_input_callback, sceneManager);
    sceneManager->scenesListHead = sceneManager->currentScene = NULL;
    return sceneManager;
}

void scene_manager_free(SceneManager* sceneManager) {
    ScenesListNode* node = sceneManager->scenesListHead;
    while(node) {
        ScenesListNode* nextNode = node->next;
        scene_destroy(node->scene);
        free(node);
        node = nextNode;
    }

    furi_message_queue_free(sceneManager->eventQueue);
    view_port_free(sceneManager->viewPort);
    free(sceneManager);
}

void scene_manager_attach_to_gui(SceneManager* sceneManager, Gui* gui) {
    gui_add_view_port(gui, sceneManager->viewPort, GuiLayerFullscreen);
}

void scene_manager_detach_from_gui(SceneManager* sceneManager, Gui* gui) {
    gui_remove_view_port(gui, sceneManager->viewPort);
}

void scene_manager_register_scene(SceneManager* sceneManager, int id, Scene* scene) {
    ScenesListNode* node = (ScenesListNode*)malloc(sizeof(ScenesListNode));
    node->id = id;
    node->scene = scene;
    node->next = sceneManager->scenesListHead;
    sceneManager->scenesListHead = node;
}

void scene_manager_set_scene(SceneManager* sceneManager, int to) {
    ScenesListNode* nextNode;
    for(nextNode = sceneManager->scenesListHead; nextNode; nextNode = nextNode->next) {
        if(nextNode->id == to) break;
    }

    int from = sceneManager->currentScene ? sceneManager->currentScene->id : SceneManagerNoScene;
    Scene* currentScene = scene_manager_get_current_scene(sceneManager);

    if(currentScene && currentScene->transitionCallback)
        currentScene->transitionCallback(from, to, currentScene->context);

    if(nextNode && nextNode->scene && nextNode->scene->transitionCallback)
        nextNode->scene->transitionCallback(from, to, nextNode->scene->context);

    sceneManager->currentScene = nextNode;
}

int scene_manager_get_current_scene_id(SceneManager* sceneManager) {
    if(sceneManager->currentScene == NULL) return SceneManagerNoScene;

    return sceneManager->currentScene->id;
}

bool scene_manager_has_scene(SceneManager* sceneManager) {
    return sceneManager->currentScene != NULL;
}