#include <furi.h>
#include <api-hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <cli/cli.h>
#include <api-hal-irda.h>
#include <irda.h>

//#include "irda_nec.h"
//#include "irda_samsung.h"
//#include "irda_protocols.h"
//#include "irda-decoder/irda-decoder.h"

#include "callback-connector.h"



class IrdaApp {
    void callback(void* ctx, bool level, uint32_t duration);

    IrdaHandler* decoder;
    osMessageQueueId_t events;

public:
    void run(void);

    IrdaApp(void)
        : decoder(irda_init_decoder()),
          events(osMessageQueueNew(5, sizeof(IrdaMessage), 0))
    {
        api_hal_irda_rx_irq_init();
        auto callback = cbc::obtain_connector(this, &IrdaApp::callback);
        api_hal_irda_rx_irq_set_callback(callback, this);
    }

    void run_test(const uint32_t *delays, uint32_t size);
    ~IrdaApp(void) {
        irda_free_decoder(decoder);
    }
};


static uint32_t dbg_missed = 0;
static uint32_t dbg_got = 0;
static uint32_t dbg_fq = 0;

#define COUNT_OF(x)             (sizeof(x) / sizeof(x[0]))

extern "C" {
void print_m(void);
}

void IrdaApp::run(void) {
    IrdaMessage message;
    while (osOK == osMessageQueueGet (events, &message, 0, osWaitForever)) {
        printf("irda_msg: protocol: %s, address: %#08lX, command: %#08lX %s\r\n",
            message.protocol_name,
            message.address,
            message.command,
            message.repeat ? "R" : "" );
    }

    printf("osMessageQueueGet FAILED\r\n");
}

extern "C" void IrdaApp::callback(void* ctx, bool level, uint32_t duration) {
    (void) ctx;
    const IrdaMessage* message = irda_decode(decoder, level, duration);

    if (message) {
        if (osOK != osMessageQueuePut(events, message, 0, 0)) {
            dbg_fq++;
        }
        dbg_got++;
    }
    else {
        dbg_missed++;
    }
}


// app enter function
extern "C" int32_t irda(void* p) {
    IrdaApp* app = new IrdaApp();
    app->run();
    delete app;

    return 255;
}

