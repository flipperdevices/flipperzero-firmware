#pragma once
#include <map>
#include <list>

#include "ibutton-view.h"
#include "scene/ibutton-scene-generic.h"
#include "scene/ibutton-scene-start.h"
#include "scene/ibutton-scene-read.h"

#include "one_wire_master.h"
#include "maxim_crc.h"

class iButtonApp {
public:
    void run(void);

    iButtonApp();
    ~iButtonApp();

    enum class Scene : uint8_t {
        iButtonAppSceneExit,
        iButtonAppSceneStart,
        iButtonAppSceneRead,
    };

    iButtonAppView* get_view();
    void switch_to_next_scene(Scene index);
    bool switch_to_prevous_scene();
    const GpioPin* get_ibutton_pin();
    OneWireMaster* get_onewire_master();

    void notify_green_blink();
    void notify_red_blink();

    void notify_green_on();
    void notify_green_off();
    void notify_red_on();
    void notify_red_off();

private:
    std::list<Scene> prevous_scene = {Scene::iButtonAppSceneExit};
    Scene current_scene = Scene::iButtonAppSceneStart;
    iButtonAppView view;

    std::map<Scene, iButtonScene*> scenes = {
        {Scene::iButtonAppSceneStart, new iButtonSceneStart()},
        {Scene::iButtonAppSceneRead, new iButtonSceneRead()}};

    OneWireMaster* onewire_master;

    void notify_init();
};