#pragma once
#include "../ibutton-event.h"

class iButtonApp;

class iButtonScene {
public:
    virtual void on_enter(iButtonApp* app);
    virtual bool on_event(iButtonApp* app, iButtonEvent* event);
    virtual void on_exit(iButtonApp* app);
};