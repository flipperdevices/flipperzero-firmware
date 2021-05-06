#include <furi.h>
#include <api-hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <cli/cli.h>
#include <api-hal-irda.h>
#include <libirda.h>

//#include "irda_nec.h"
//#include "irda_samsung.h"
//#include "irda_protocols.h"
//#include "irda-decoder/irda-decoder.h"

#include "callback-connector.h"



class IrdaApp {
    void callback(void* ctx, bool level, uint32_t duration);

    IrdaMessage im;
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

//static const uint32_t delays[] = {1081467, 169, 4580896, 9059, 4475, 590, 507, 614, 509, 613, 510, 592, 505, 617, 506, 616, 507, 594, 503, 619, 504, 608, 1626, 622, 1639, 589, 1645, 614, 1622, 617, 1645, 594, 1641, 618, 1616, 622, 1640, 588, 507, 614};


extern "C" {
void print_m(void);
}

void IrdaApp::run_test(const uint32_t *delays, uint32_t size) {
    const IrdaMessage* msg = 0;
    bool level = 1;

    for (uint32_t i = 0; i < size; ++i) {
//        int32_t delta = (int32_t) (delays[i] > 1000 ? delays[i] - 1600 : delays[i] - 560);
//        printf("delay[%lu]: %lu (%ld)\r\n", i, delays[i], delta);
        msg = irda_decode(decoder, level, delays[i]);
        level = !level;
        if (msg) {
            printf("irda_msg: protocol: %s, adr: %#08lX, cmd: %#08lX %s\r\n",
                msg->protocol_name,
                msg->adr,
                msg->cmd,
                msg->repeat ? "R" : "" );
        }
    }
}

void IrdaApp::run(void) {
#if 0
#include "test_data.h"

    delay(1000);
    run_test(delays, COUNT_OF(delays));
    run_test(delays2, COUNT_OF(delays2));

    printf("finished\r\n");
    while(1) {
        print_m();
        delay(1000);
    };

    delay(1000);
    printf("\r\n\r\n===========================================\r\n");
    while(1) {
        print_m();
        delay(1000);
    };
#else
    IrdaMessage msg;
    while (osOK == osMessageQueueGet (events, &msg, 0, osWaitForever)) {
        printf("irda_msg: protocol: %s, adr: %#08lX, cmd: %#08lX %s\r\n",
            msg.protocol_name,
            msg.adr,
            msg.cmd,
            msg.repeat ? "R" : "" );
    }


    printf("osMessageQueueGet FAILED\r\n");

    for (;;) {
        printf("got/missed/fq: %lu/%lu/%lu\r\n", dbg_got, dbg_missed, dbg_fq);
        delay(1000);
    }
#endif
}

extern "C" void IrdaApp::callback(void* ctx, bool level, uint32_t duration) {
    (void) ctx;
    const IrdaMessage* msg = irda_decode(decoder, level, duration);

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

