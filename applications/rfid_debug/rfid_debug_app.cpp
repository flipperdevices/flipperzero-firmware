#include "rfid_debug_app.h"
#include "scene/rfid_debug_app_scene_start.h"
#include "scene/rfid_debug_app_scene_lftune.h"

RfidDebugApp::RfidDebugApp()
    : scene_controller{this} {
}

RfidDebugApp::~RfidDebugApp() {
}

void RfidDebugApp::run() {
    view_controller.attach_to_gui(ViewDispatcherTypeFullscreen);
    scene_controller.add_scene(SceneType::Start, new RfidDebugAppSceneStart());
    scene_controller.add_scene(SceneType::LFTuneScene, new RfidDebugAppSceneLFTune());
    scene_controller.process(100);
}
