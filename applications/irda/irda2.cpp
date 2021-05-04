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

#define COUNT_OF(x)             (sizeof(x) / sizeof(x[0]))

static const uint32_t delays[] = {2640671, 
9071, 4445, 601, 497, 578, 500, 604, 501, 603, 502, 581, 496, 615, 498, 606, 499, 584, 493, 610, 1630, 576, 1640, 601, 1615, 605, 1638, 581, 1634, 606, 1610, 610, 1633, 577, 1639, 601, 504, 580, 498, 604, 501, 603, 500, 582, 496, 607, 498, 606, 499, 585, 485, 610, 1633, 576, 1640, 596, 1615, 605, 1638, 582, 1634, 605, 1610, 609, 1634, 586, 1630, 600, 40015, 

9077, 2208, 593, 1457713, 

9076, 4440, 607, 508, 585, 493, 610, 494, 598, 506, 577, 501, 603, 502, 601, 504, 580, 498, 605, 1638, 582, 1634, 606, 1610, 610, 1633, 577, 1639, 600, 1616, 605, 1638, 582, 1634, 606, 499, 585, 493, 609, 495, 608, 496, 586, 502, 612, 493, 601, 504, 579, 498, 605, 1638, 582, 1633, 606, 1610, 610, 1633, 577, 1639, 602, 1614, 574, 1668, 582, 1634, 606, 1415838, 

9080, 4436, 611, 494, 600, 505, 578, 500, 608, 501, 602, 502, 580, 498, 606, 508, 605, 500, 583, 1633, 608, 1608, 611, 1631, 578, 1638, 602, 1614, 606, 1637, 583, 1633, 607, 1609, 611, 494, 600, 505, 570, 500, 604, 501, 602, 502, 581, 497, 606, 499, 605, 499, 583, 1633, 617, 1608, 611, 1631, 579, 1638, 602};
//static const uint32_t delays[] = {1081467, 169, 4580896, 9059, 4475, 590, 507, 614, 509, 613, 510, 592, 505, 617, 506, 616, 507, 594, 503, 619, 504, 608, 1626, 622, 1639, 589, 1645, 614, 1622, 617, 1645, 594, 1641, 618, 1616, 622, 1640, 588, 507, 614};

void IrdaApp::run(void) {
#if 1
    IrdaMessage* msg = 0;
    bool level = 0;

    delay(1000);

    for (uint32_t i = 0; i < COUNT_OF(delays); ++i) {
        level = !level;
//        int32_t delta = (int32_t) (delays[i] > 1000 ? delays[i] - 1600 : delays[i] - 560);
//        printf("delay[%lu]: %lu (%ld)\r\n", i, delays[i], delta);
        msg = irda_decode(decoder, level, delays[i]);
        if (msg) {
            printf("irda_msg: protocol: %s, adr: %#08lX, cmd: %#08lX %s\r\n",
                irda_get_protocol_name(msg->protocol),
                msg->adr,
                msg->cmd,
                msg->repeat ? "" : "R" );
        }
    }

    printf("finished\r\n");
    while(1) {
        delay(1000);
    };

#else
    IrdaMessage msg;
    while (osOK == osMessageQueueGet (events, &msg, 0, osWaitForever)) {
        printf("irda_msg: protocol: %s, adr: %#08lX, cmd: %#08lX %s\r\n",
            irda_get_protocol_name(msg.protocol),
            msg.adr,
            msg.cmd,
            msg.repeat ? "" : "R" );
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

