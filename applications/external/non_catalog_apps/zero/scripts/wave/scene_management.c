#include "scene_management.h"
#include <stdlib.h>
#include <gui/gui.h>

const int SCENE_MANAGER_NO_SCENE = -1000;

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
    Gui* gui;
    bool isAutoManaged;
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

void scene_manager_register_scene(SceneManager* sceneManager, int id, Scene* scene) {
    ScenesListNode* node = (ScenesListNode*)malloc(sizeof(ScenesListNode));
    node->id = id;
    node->scene = scene;
    node->next = sceneManager->scenesListHead;
    sceneManager->scenesListHead = node;
}

void scene_manager_set_scene(SceneManager* sceneManager, int to) {
    ScenesListNode* nextNode = NULL;
    for(nextNode = sceneManager->scenesListHead; nextNode; nextNode = nextNode->next) {
        if(nextNode->id == to) break;
    }

    int from = sceneManager->currentScene ? sceneManager->currentScene->id :
                                            SCENE_MANAGER_NO_SCENE;
    Scene* currentScene = scene_manager_get_current_scene(sceneManager);

    if(currentScene && currentScene->transitionCallback)
        currentScene->transitionCallback(from, to, currentScene->context);

    if(nextNode && nextNode->scene && nextNode->scene->transitionCallback)
        nextNode->scene->transitionCallback(from, to, nextNode->scene->context);

    sceneManager->currentScene = nextNode;
}

int scene_manager_get_current_scene_id(SceneManager* sceneManager) {
    if(sceneManager->currentScene == NULL) return SCENE_MANAGER_NO_SCENE;

    return sceneManager->currentScene->id;
}

bool scene_manager_has_scene(SceneManager* sceneManager) {
    return sceneManager->currentScene != NULL;
}

SceneManager* scene_manager_alloc(ViewPort* viewPort, Gui* gui, FuriMessageQueue* eventQueue) {
    SceneManager* sceneManager = (SceneManager*)malloc(sizeof(SceneManager));
    sceneManager->viewPort = viewPort;
    sceneManager->eventQueue = eventQueue;
    sceneManager->gui = gui;
    gui_add_view_port(gui, viewPort, GuiLayerFullscreen);

    view_port_draw_callback_set(viewPort, scene_manager_draw_callback, sceneManager);
    view_port_input_callback_set(viewPort, scene_manager_input_callback, sceneManager);
    sceneManager->scenesListHead = sceneManager->currentScene = NULL;
    return sceneManager;
}

SceneManager* scene_manager_alloc_auto() {
    ViewPort* viewPort = view_port_alloc();
    Gui* gui = furi_record_open(RECORD_GUI);
    FuriMessageQueue* eventQueue = furi_message_queue_alloc(8, sizeof(InputEvent));

    SceneManager* sceneManager = scene_manager_alloc(viewPort, gui, eventQueue);
    sceneManager->isAutoManaged = true;
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

    gui_remove_view_port(sceneManager->gui, sceneManager->viewPort);

    if(sceneManager->isAutoManaged) {
        furi_message_queue_free(sceneManager->eventQueue);
        view_port_free(sceneManager->viewPort);
        furi_record_close(RECORD_GUI);
    }

    free(sceneManager);
}