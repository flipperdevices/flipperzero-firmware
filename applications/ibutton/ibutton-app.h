#pragma once
#include <map>
#include <list>

#include "ibutton-view-manager.h"
#include "scene/ibutton-scene-generic.h"
#include "scene/ibutton-scene-start.h"
#include "scene/ibutton-scene-read.h"
#include "scene/ibutton-scene-read-crc-error.h"

#include "one_wire_master.h"
#include "maxim_crc.h"
#include "ibutton-key.h"

class iButtonApp {
public:
    void run(void);

    iButtonApp();
    ~iButtonApp();

    enum class Scene : uint8_t {
        SceneExit,
        SceneStart,
        SceneRead,
        SceneReadNotKeyError,
        SceneReadCRCError,
        SceneReadSuccess,
    };

    iButtonAppViewManager* get_view_manager();
    void switch_to_next_scene(Scene index);
    bool switch_to_prevous_scene();
    const GpioPin* get_ibutton_pin();
    OneWireMaster* get_onewire_master();
    iButtonKey* get_key();

    void notify_green_blink();
    void notify_red_blink();

    void notify_green_on();
    void notify_green_off();
    void notify_red_on();
    void notify_red_off();

    void notify_error();
    void notify_success();

    void set_text_store(const char* text...);
    const char* get_text_store();

private:
    std::list<Scene> prevous_scene = {Scene::SceneExit};
    Scene current_scene = Scene::SceneStart;
    iButtonAppViewManager view;

    std::map<Scene, iButtonScene*> scenes = {
        {Scene::SceneStart, new iButtonSceneStart()},
        {Scene::SceneRead, new iButtonSceneRead()},
        {Scene::SceneReadCRCError, new iButtonSceneReadCRCError()}};

    OneWireMaster* onewire_master;
    iButtonKey key;

    static const uint8_t text_store_size = 64;
    char text_store[text_store_size + 1];

    void notify_init();
};