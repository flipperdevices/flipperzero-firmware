#pragma once
#include <map>
#include <list>

#include "ibutton-view.h"
#include "mode/ibutton-mode-generic.h"
#include "mode/ibutton-mode-start.h"
#include "mode/ibutton-mode-read.h"

#include "one_wire_master.h"
#include "maxim_crc.h"

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
    const GpioPin* get_ibutton_pin();
    OneWireMaster* get_onewire_master();

    void notify_green_blink();
    void notify_red_blink();

    void notify_green_on();
    void notify_green_off();
    void notify_red_on();
    void notify_red_off();

private:
    std::list<Mode> prevous_mode = {Mode::iButtonAppModeExit};
    Mode current_mode = Mode::iButtonAppModeStart;
    iButtonAppView view;

    std::map<Mode, iButtonMode*> mode_data = {
        {Mode::iButtonAppModeStart, new iButtonModeStart()},
        {Mode::iButtonAppModeRead, new iButtonModeRead()}};

    OneWireMaster* onewire_master;

    void notify_init();
};