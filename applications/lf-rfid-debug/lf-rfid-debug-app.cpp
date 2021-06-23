#include "lf-rfid-debug-app.h"
#include "scene/lf-rfid-debug-app-scene-start.h"
#include "scene/lf-rfid-debug-app-scene-tune.h"

LfRfidDebugApp::LfRfidDebugApp()
    : scene_controller{this} {
}

LfRfidDebugApp::~LfRfidDebugApp() {
}

void LfRfidDebugApp::run() {
    scene_controller.add_scene(SceneType::Start, new LfRfidDebugAppSceneStart());
    scene_controller.add_scene(SceneType::TuneScene, new LfRfidDebugAppSceneTune());
    scene_controller.process(100);
}