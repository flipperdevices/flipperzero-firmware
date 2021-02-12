#pragma once
#include <map>
#include <list>

#include "ibutton-view-manager.h"
#include "scene/ibutton-scene-generic.h"
#include "scene/ibutton-scene-start.h"
#include "scene/ibutton-scene-read.h"
#include "scene/ibutton-scene-read-crc-error.h"
#include "scene/ibutton-scene-read-not-key-error.h"
#include "scene/ibutton-scene-read-success.h"
#include "scene/ibutton-scene-readed-key-menu.h"
#include "scene/ibutton-scene-write.h"
#include "scene/ibutton-scene-saved.h"
#include "scene/ibutton-scene-saved-key-menu.h"
#include "scene/ibutton-scene-delete-confirm.h"
#include "scene/ibutton-scene-delete-confirmed.h"
#include "scene/ibutton-scene-emulate.h"
#include "scene/ibutton-scene-save-name.h"
#include "scene/ibutton-scene-save-confirmed.h"
#include "scene/ibutton-scene-info.h"

#include "helpers/key-store.h"
#include "helpers/key-worker.h"

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
        SceneReadedKeyMenu,
        SceneWrite,
        SceneEmulate,
        SceneSaved,
        SceneSavedKeyMenu,
        SceneDeleteConfirm,
        SceneDeleteConfirmed,
        SceneSaveName,
        SceneSaveConfirmed,
        SceneInfo,
    };

    iButtonAppViewManager* get_view_manager();
    void switch_to_next_scene(Scene index);
    bool switch_to_prevous_scene();
    bool switch_to_prevous_scene(uint8_t count);
    Scene get_prevous_scene();

    const GpioPin* get_ibutton_pin();
    KeyWorker* get_key_worker();
    iButtonKey* get_key();

    void notify_green_blink();
    void notify_red_blink();

    void notify_green_on();
    void notify_green_off();
    void notify_red_on();
    void notify_red_off();

    void notify_error();
    void notify_success();

    void notify_vibro_on();
    void notify_vibro_off();

    void set_text_store(const char* text...);
    char* get_text_store();
    uint8_t get_text_store_size();

    KeyStore* get_key_store();
    uint8_t get_stored_key_index();
    void set_stored_key_index(uint8_t index);

    void generate_random_name(char* name, uint8_t max_name_size);

private:
    std::list<Scene> prevous_scene = {Scene::SceneExit};
    Scene current_scene = Scene::SceneStart;
    iButtonAppViewManager view;

    std::map<Scene, iButtonScene*> scenes = {
        {Scene::SceneStart, new iButtonSceneStart()},
        {Scene::SceneRead, new iButtonSceneRead()},
        {Scene::SceneReadCRCError, new iButtonSceneReadCRCError()},
        {Scene::SceneReadNotKeyError, new iButtonSceneReadNotKeyError()},
        {Scene::SceneReadSuccess, new iButtonSceneReadSuccess()},
        {Scene::SceneReadedKeyMenu, new iButtonSceneReadedKeyMenu()},
        {Scene::SceneWrite, new iButtonSceneWrite()},
        {Scene::SceneEmulate, new iButtonSceneEmulate()},
        {Scene::SceneSaved, new iButtonSceneSaved()},
        {Scene::SceneSavedKeyMenu, new iButtonSceneSavedKeyMenu()},
        {Scene::SceneDeleteConfirm, new iButtonSceneDeleteConfirm()},
        {Scene::SceneDeleteConfirmed, new iButtonSceneDeleteConfirmed()},
        {Scene::SceneSaveName, new iButtonSceneSaveName()},
        {Scene::SceneSaveConfirmed, new iButtonSceneSaveConfirmed()},
        {Scene::SceneInfo, new iButtonSceneInfo()},
    };

    KeyWorker* key_worker;

    iButtonKey key;
    uint8_t key_index = 0;

    static const uint8_t text_store_size = 128;
    char text_store[text_store_size + 1];

    KeyStore store;

    void notify_init();
};