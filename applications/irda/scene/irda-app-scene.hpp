#pragma once
#include "../irda-app.hpp"
#include <api-hal-irda.h>
#include "irda.h"
#include <vector>
#include <string>
#include "../irda-app-file-reader.hpp"

class IrdaApp;

class IrdaAppScene {
public:
    virtual void on_enter(IrdaApp* app) = 0;
    virtual bool on_event(IrdaApp* app, IrdaAppEvent* event) = 0;
    virtual void on_exit(IrdaApp* app) = 0;
    virtual ~IrdaAppScene(){};

private:
};

class IrdaAppSceneStart : public IrdaAppScene {
public:
    void on_enter(IrdaApp* app) final;
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    void on_exit(IrdaApp* app) final;
private:
    uint32_t submenu_item_selected = 0;
};

class IrdaAppSceneUniversal : public IrdaAppScene {
public:
    void on_enter(IrdaApp* app) final;
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    void on_exit(IrdaApp* app) final;
private:
    uint32_t submenu_item_selected = 0;
};

class IrdaAppSceneLearn : public IrdaAppScene {
public:
    void on_enter(IrdaApp* app) final;
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    void on_exit(IrdaApp* app) final;
};

class IrdaAppSceneLearnSuccess : public IrdaAppScene {
public:
    void on_enter(IrdaApp* app) final;
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    void on_exit(IrdaApp* app) final;
};

class IrdaAppSceneLearnEnterName : public IrdaAppScene {
public:
    void on_enter(IrdaApp* app) final;
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    void on_exit(IrdaApp* app) final;
};

class IrdaAppSceneLearnDone : public IrdaAppScene {
public:
    void on_enter(IrdaApp* app) final;
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    void on_exit(IrdaApp* app) final;
};

class IrdaAppSceneLearnDoneAfter : public IrdaAppScene {
public:
    void on_enter(IrdaApp* app) final;
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    void on_exit(IrdaApp* app) final;
};

class IrdaAppSceneRemote : public IrdaAppScene {
public:
    void on_enter(IrdaApp* app) final;
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    void on_exit(IrdaApp* app) final;
private:
    std::vector<std::string> buttons_names;
    uint32_t buttonmenu_item_selected = 0;
};

class IrdaAppSceneRemoteList : public IrdaAppScene {
public:
    void on_enter(IrdaApp* app) final;
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    void on_exit(IrdaApp* app) final;
private:
    uint32_t submenu_item_selected = 0;
    std::vector<std::string> remote_names;
};

class IrdaAppSceneEdit : public IrdaAppScene {
public:
    void on_enter(IrdaApp* app) final;
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    void on_exit(IrdaApp* app) final;
private:
    uint32_t submenu_item_selected = 0;
};

class IrdaAppSceneEditKeySelect : public IrdaAppScene {
public:
    void on_enter(IrdaApp* app) final;
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    void on_exit(IrdaApp* app) final;
private:
    std::vector<std::string> buttons_names;
};

class IrdaAppSceneEditRename : public IrdaAppScene {
public:
    void on_enter(IrdaApp* app) final;
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    void on_exit(IrdaApp* app) final;
};

class IrdaAppSceneEditDelete : public IrdaAppScene {
public:
    void on_enter(IrdaApp* app) final;
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    void on_exit(IrdaApp* app) final;
};

class IrdaAppSceneEditRenameDone : public IrdaAppScene {
public:
    void on_enter(IrdaApp* app) final;
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    void on_exit(IrdaApp* app) final;
};

class IrdaAppSceneEditDeleteDone : public IrdaAppScene {
public:
    void on_enter(IrdaApp* app) final;
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    void on_exit(IrdaApp* app) final;
};

class IrdaAppSceneUniversalTV : public IrdaAppScene {
    typedef enum {
        ButtonPanelIndexNA = -1,
        ButtonPanelIndexPower = 0,
        ButtonPanelIndexMute,
        ButtonPanelIndexChu,
        ButtonPanelIndexChd,
        ButtonPanelIndexVolu,
        ButtonPanelIndexVold,
        ButtonPanelIndexMAX,
    } ButtonPanelIndex;

    ButtonPanelIndex pressed_button;
    IrdaAppFileReader* file_reader;
    int messages[ButtonPanelIndexMAX];
    const char* selected_button = "";

//    typedef struct {
//        int messages_in_file;
//        const char* button_name;
//        const char* bruteforce_filename;
//    } MessageBrut;
//
//    MessageBrut message_brut[6] = {
//        {.button_name = "POWER",    .bruteforce_filename = "tv.ir",},
//        {.button_name = "MUTE",     .bruteforce_filename = "",},
//        {.button_name = "CH+",      .bruteforce_filename = "",},
//        {.button_name = "CH-",      .bruteforce_filename = "",},
//        {.button_name = "VOL+",     .bruteforce_filename = "",},
//        {.button_name = "VOL-",     .bruteforce_filename = "",},
//    };

    static const char* universal_tv_db_name;
    File file;

    bool start_bruteforce(IrdaApp* app, ButtonPanelIndex index);
    void stop_bruteforce(IrdaApp* app);
    bool send_next_bruteforce(IrdaApp* app);
    void calculate_messages(IrdaApp* app);

public:
    IrdaAppSceneUniversalTV()
        : file_reader(new (IrdaAppFileReader)) {}
    ~IrdaAppSceneUniversalTV() {
        delete file_reader;
    }
    void on_enter(IrdaApp* app) final;
    bool on_event(IrdaApp* app, IrdaAppEvent* event) final;
    void on_exit(IrdaApp* app) final;
};

