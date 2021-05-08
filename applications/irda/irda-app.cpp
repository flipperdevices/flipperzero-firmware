#include <furi.h>
#include <api-hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <cli/cli.h>
#include <api-hal-irda.h>
#include <irda.h>

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

    ~IrdaApp(void) {
        irda_free_decoder(decoder);
    }
};


void IrdaApp::run(void) {
    IrdaMessage message;
    while (osOK == osMessageQueueGet (events, &message, 0, osWaitForever)) {
        printf("irda_msg: protocol: %s, address: %#08lX, command: %#08lX %s\r\n",
            irda_get_protocol_name(message.protocol),
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
            furi_assert(0);
        }
    }
}

extern "C" int32_t irda(void* p) {
    IrdaApp* app = new IrdaApp();
    app->run();
    delete app;

    return 0;
}

