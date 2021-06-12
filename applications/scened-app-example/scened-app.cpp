#include "scened-app.h"
#include "scene/scened-app-scene-start.h"

ScenedApp::ScenedApp()
    : scene_controller{this}
    , text_store{128} {
}

ScenedApp::~ScenedApp() {
}

void ScenedApp::run() {
    scene_controller.add_scene(SceneType::Start, new ScenedAppSceneStart());
    scene_controller.process(100);
}