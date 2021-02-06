#pragma once
#include <map>
#include <list>

#include "ibutton-view.h"
#include "mode/ibutton-mode-generic.h"
#include "mode/ibutton-mode-start.h"
#include "mode/ibutton-mode-read.h"

class iButtonApp {
public:
    void run(void);

    iButtonApp();
    ~iButtonApp();

    enum class Mode : uint8_t {
        iButtonAppModeExit,
        iButtonAppModeStart,
        iButtonAppModeRead,
    };

    iButtonAppView* get_view();
    void switch_to_next(Mode mode);
    bool switch_to_prevous();

private:
    std::list<Mode> prevous_mode = {Mode::iButtonAppModeExit};
    Mode current_mode = Mode::iButtonAppModeStart;
    iButtonAppView view;

    std::map<Mode, iButtonMode*> mode_data = {
        {Mode::iButtonAppModeStart, new iButtonModeStart()},
        {Mode::iButtonAppModeRead, new iButtonModeRead()}};
};