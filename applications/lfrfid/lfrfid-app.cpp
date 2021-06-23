#include "lfrfid-app.h"
#include "scene/lfrfid-app-scene-start.h"

LfRfidApp::LfRfidApp()
    : scene_controller{this} {
}

LfRfidApp::~LfRfidApp() {
}

void LfRfidApp::run() {
    scene_controller.add_scene(SceneType::Start, new LfRfidAppSceneStart());
    scene_controller.process(100);
}