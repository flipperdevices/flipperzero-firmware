#include "lfrfid-app.h"
#include "scene/lfrfid-app-scene-start.h"
#include "scene/lfrfid-app-scene-read.h"
#include "scene/lfrfid-app-scene-read-success.h"
#include "scene/lfrfid-app-scene-readed-menu.h"
#include "scene/lfrfid-app-scene-write.h"

LfRfidApp::LfRfidApp()
    : scene_controller{this}
    , fs_api{"sdcard"}
    , sd_ex_api{"sdcard-ex"}
    , notification{"notification"} {
    api_hal_power_insomnia_enter();

    // we need random
    srand(DWT->CYCCNT);
}

LfRfidApp::~LfRfidApp() {
    api_hal_power_insomnia_exit();
}

void LfRfidApp::run() {
    scene_controller.add_scene(SceneType::Start, new LfRfidAppSceneStart());
    scene_controller.add_scene(SceneType::Read, new LfRfidAppSceneRead());
    scene_controller.add_scene(SceneType::ReadSuccess, new LfRfidAppSceneReadSuccess());
    scene_controller.add_scene(SceneType::ReadedMenu, new LfRfidAppSceneReadedMenu());
    scene_controller.add_scene(SceneType::Write, new LfRfidAppSceneWrite());
    scene_controller.process(100);
}