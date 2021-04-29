#include <furi.h>
#include <api-hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <cli/cli.h>
#include <api-hal-irda.h>
#include <libirda.h>

#include "irda_nec.h"
#include "irda_samsung.h"
#include "irda_protocols.h"
#include "irda-decoder/irda-decoder.h"

#include "callback-connector.h"



#if 0
static void timer_callback(TimerIRQ source);

int32_t irda3(void* p) {
    api_hal_irda_rx_irq_init();

    IrdaDecoder* decoder = irda_new_decoder();
    api_hal_irda_rx_irq_set_callback(timer_callback, decoder);

    while(1) {

    }

    api_hal_irda_rx_irq_deinit();
}

static void timer_callback(TimerIRQ source)
{
    osMessageQueueId_t events = (osMessageQueueId_t)comp_ctx;
    bool got_msg = false;

    source
    got_msg = irda_decode(decoder, TimerIRQRisingEdge);

    osMessageQueuePut(events, &event, 0, 0);
}
#endif

//#include "ibutton-app.h"


class IrdaApp {
    void callback(void* ctx, bool level, uint32_t duration);

    IrdaMessage im;
    IrdaProtocol ip;
    IrdaDecoder* decoder;
    osMessageQueueId_t events;

public:
    void run(void);

    IrdaApp(void)
        : decoder(irda_new_decoder()),
          events(osMessageQueueNew(5, sizeof(IrdaMessage), 0))
    {
        api_hal_irda_rx_irq_init();
        auto callback = cbc::obtain_connector(this, &IrdaApp::callback);
        api_hal_irda_rx_irq_set_callback(callback, this);
    }

    ~IrdaApp(void) {
        irda_free_decoder(decoder);
    }
};


static uint32_t dbg_missed = 0;
static uint32_t dbg_got = 0;
static uint32_t dbg_fq = 0;

void IrdaApp::run(void) {

    IrdaMessage msg;
    while (osOK == osMessageQueueGet (events, &msg, 0, osWaitForever)) {
        printf("irda_msg: protocol: %d, adr: %#08lX, cmd: %#08lX\r\n",
            msg.protocol,
            msg.adr,
            msg.cmd);
    }


    printf("osMessageQueueGet FAILED\r\n");

    for (;;) {
        printf("got/missed/fq: %lu/%lu/%lu\r\n", dbg_got, dbg_missed, dbg_fq);
        delay(1000);
    }
}

extern "C" void IrdaApp::callback(void* ctx, bool level, uint32_t duration) {
    (void) ctx;
    IrdaMessage* msg = irda_decode(decoder, level, duration);

    if (msg) {
        if (osOK != osMessageQueuePut(events, msg, 0, 0)) {
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

