/**
 * @file irda_app_scene.h
 * Infrared: Application scenes
 */
#pragma once
#include "../irda_app_event.h"
#include <furi_hal_irda.h>
#include "irda.h"
#include <vector>
#include <string>
#include "../irda_app_brute_force.h"

/** Anonymous class */
class IrdaApp;

/** Base Scene class */
class IrdaAppScene {
public:
    /** Called when enter scene */
    virtual void on_enter(IrdaApp* app) = 0;
    /** Events handler callback */
    virtual bool on_event(IrdaApp* app, IrdaAppEvent* event) = 0;
    /** Called when exit scene */
    virtual void on_exit(IrdaApp* app) = 0;
    /** Virtual destructor of base class */
    virtual ~IrdaAppScene(){};

private:
};

/** Start scene
 * Main Infrared application menu
 */
class IrdaAppSceneStart : public IrdaAppScene {
public:
    /** Called when enter scene */
    void on_enter(IrdaApp* app) final;
    /** Events handler callback */
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    /** Called when exit scene */
    void on_exit(IrdaApp* app) final;

private:
    /** Save previously selected submenu index
     * to highlight it when get back */
    uint32_t submenu_item_selected = 0;
};

/** Universal menu scene
 * Scene to select universal remote
 */
class IrdaAppSceneUniversal : public IrdaAppScene {
public:
    /** Called when enter scene */
    void on_enter(IrdaApp* app) final;
    /** Events handler callback */
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    /** Called when exit scene */
    void on_exit(IrdaApp* app) final;

private:
    /** Save previously selected submenu index
     * to highlight it when get back */
    uint32_t submenu_item_selected = 0;
};

/** Learn new signal scene
 * On this scene catching new IR signal performed.
 */
class IrdaAppSceneLearn : public IrdaAppScene {
public:
    /** Called when enter scene */
    void on_enter(IrdaApp* app) final;
    /** Events handler callback */
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    /** Called when exit scene */
    void on_exit(IrdaApp* app) final;
};

/** New signal learn succeeded scene
 */
class IrdaAppSceneLearnSuccess : public IrdaAppScene {
public:
    /** Called when enter scene */
    void on_enter(IrdaApp* app) final;
    /** Events handler callback */
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    /** Called when exit scene */
    void on_exit(IrdaApp* app) final;
    bool button_pressed = false;
};

/** Scene to enter name for new button in remote
 */
class IrdaAppSceneLearnEnterName : public IrdaAppScene {
public:
    /** Called when enter scene */
    void on_enter(IrdaApp* app) final;
    /** Events handler callback */
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    /** Called when exit scene */
    void on_exit(IrdaApp* app) final;
};

/** Scene where signal is learnt
 */
class IrdaAppSceneLearnDone : public IrdaAppScene {
public:
    /** Called when enter scene */
    void on_enter(IrdaApp* app) final;
    /** Events handler callback */
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    /** Called when exit scene */
    void on_exit(IrdaApp* app) final;
};

/** Remote interface scene
 * On this scene you can send IR signals from selected remote
 */
class IrdaAppSceneRemote : public IrdaAppScene {
public:
    /** Called when enter scene */
    void on_enter(IrdaApp* app) final;
    /** Events handler callback */
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    /** Called when exit scene */
    void on_exit(IrdaApp* app) final;

private:
    /** container of button names in current remote. */
    std::vector<std::string> buttons_names;
    /** Save previously selected index
     * to highlight it when get back */
    uint32_t buttonmenu_item_selected = 0;
    /** state flag to show button is pressed.
     * As long as send-signal button pressed no other button
     * events are handled. */
    bool button_pressed = false;
};

/** List of remotes scene
 * Every remote is a file, located on internal/external storage.
 * Every file has same format, and same extension.
 * Files are parsed as you enter 'Remote scene' and showed
 * as a buttons.
 */
class IrdaAppSceneRemoteList : public IrdaAppScene {
public:
    /** Called when enter scene */
    void on_enter(IrdaApp* app) final;
    /** Events handler callback */
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    /** Called when exit scene */
    void on_exit(IrdaApp* app) final;

private:
    /** Save previously selected index
     * to highlight it when get back */
    uint32_t submenu_item_selected = 0;
    /** Remote names to show them in submenu */
    std::vector<std::string> remote_names;
};

class IrdaAppSceneAskBack : public IrdaAppScene {
public:
    /** Called when enter scene */
    void on_enter(IrdaApp* app) final;
    /** Events handler callback */
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    /** Called when exit scene */
    void on_exit(IrdaApp* app) final;
};

class IrdaAppSceneEdit : public IrdaAppScene {
public:
    /** Called when enter scene */
    void on_enter(IrdaApp* app) final;
    /** Events handler callback */
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    /** Called when exit scene */
    void on_exit(IrdaApp* app) final;

private:
    /** Save previously selected index
     * to highlight it when get back */
    uint32_t submenu_item_selected = 0;
};

class IrdaAppSceneEditKeySelect : public IrdaAppScene {
public:
    /** Called when enter scene */
    void on_enter(IrdaApp* app) final;
    /** Events handler callback */
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    /** Called when exit scene */
    void on_exit(IrdaApp* app) final;

private:
    /** Button names to show them in submenu */
    std::vector<std::string> buttons_names;
};

class IrdaAppSceneEditRename : public IrdaAppScene {
public:
    /** Called when enter scene */
    void on_enter(IrdaApp* app) final;
    /** Events handler callback */
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    /** Called when exit scene */
    void on_exit(IrdaApp* app) final;
};

class IrdaAppSceneEditDelete : public IrdaAppScene {
public:
    /** Called when enter scene */
    void on_enter(IrdaApp* app) final;
    /** Events handler callback */
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    /** Called when exit scene */
    void on_exit(IrdaApp* app) final;
};

class IrdaAppSceneEditRenameDone : public IrdaAppScene {
public:
    /** Called when enter scene */
    void on_enter(IrdaApp* app) final;
    /** Events handler callback */
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    /** Called when exit scene */
    void on_exit(IrdaApp* app) final;
};

class IrdaAppSceneEditDeleteDone : public IrdaAppScene {
public:
    /** Called when enter scene */
    void on_enter(IrdaApp* app) final;
    /** Events handler callback */
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    /** Called when exit scene */
    void on_exit(IrdaApp* app) final;
};

class IrdaAppSceneUniversalCommon : public IrdaAppScene {
    /** Brute force started flag */
    bool brute_force_started = false;

protected:
    /** Events handler callback */
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    /** Called when exit scene */
    void on_exit(IrdaApp* app) final;

    /** Show popup window
     *
     * @param app - application instance
     */
    void show_popup(IrdaApp* app, int record_amount);

    /** Hide popup window
     *
     * @param app - application instance
     */
    void hide_popup(IrdaApp* app);

    /** Propagate progress in popup window
     *
     * @param app - application instance
     */
    bool progress_popup(IrdaApp* app);

    /** Item selected callback
     *
     * @param context - context
     * @param index - selected item index
     */
    static void irda_app_item_callback(void* context, uint32_t index);

    /** Brute Force instance */
    IrdaAppBruteForce brute_force;

    /** Constructor */
    IrdaAppSceneUniversalCommon(const char* filename)
        : brute_force(filename) {
    }

    /** Destructor */
    ~IrdaAppSceneUniversalCommon() {
    }
};

class IrdaAppSceneUniversalTV : public IrdaAppSceneUniversalCommon {
public:
    /** Called when enter scene */
    void on_enter(IrdaApp* app) final;

    /** Constructor
     * Specifies path to brute force db library */
    IrdaAppSceneUniversalTV()
        : IrdaAppSceneUniversalCommon("/ext/irda/assets/tv.ir") {
    }

    /** Destructor */
    ~IrdaAppSceneUniversalTV() {
    }
};

