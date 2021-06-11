#pragma once
#include "../scened-app.h"

class ScenedAppSceneStart : public GenericScene<ScenedApp> {
public:
    void on_enter(ScenedApp* app, bool need_restore) final;
    bool on_event(ScenedApp* app, ScenedApp::EventType* event) final;
    void on_exit(ScenedApp* app) final;
};