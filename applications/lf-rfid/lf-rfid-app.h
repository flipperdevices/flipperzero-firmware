#pragma once
#include <map>
#include <list>
#include "lf-rfid-view-manager.h"

#include "scene/lf-rfid-scene-start.h"
#include "scene/lf-rfid-scene-read.h"

class LfrfidApp {
public:
    void run(void);

    LfrfidApp();
    ~LfrfidApp();

    enum class Scene : uint8_t {
        SceneExit,
        SceneStart,
        SceneRead,
    };

    LfrfidAppViewManager* get_view_manager();
    void switch_to_next_scene(Scene index);
    void search_and_switch_to_previous_scene(std::initializer_list<Scene> scenes_list);
    bool switch_to_previous_scene(uint8_t count = 1);
    Scene get_previous_scene();

    void notify_init();
    void notify_green_blink();

    char* get_text_store();
    uint8_t get_text_store_size();
    void set_text_store(const char* text...);

private:
    std::list<Scene> previous_scenes_list = {Scene::SceneExit};
    Scene current_scene = Scene::SceneStart;
    LfrfidAppViewManager view;

    std::map<Scene, LfrfidScene*> scenes = {
        {Scene::SceneStart, new LfrfidSceneStart()},
        {Scene::SceneRead, new LfrfidSceneRead()},
    };

    static const uint8_t text_store_size = 128;
    char text_store[text_store_size + 1];
};