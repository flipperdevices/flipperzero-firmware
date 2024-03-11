#include "fcom.h"
#include "app_state.h"
#include "dmcomm_lib/DMComm.h"
#include "dmcomm_lib/flipper_led.h"
#include "dmcomm_lib/flipper_pin_control.h"

using namespace DMComm;

/*
Just a static class to hold a bunch of pointers
so we can deallocate em later
*/
class FComState {
public:
    FComState(){};
    Stream* stream;
    FlipperIndicator* led;
    FComOutput* output;
    FComInput* input;

    ClassicCommunicator* classic_comm;
    ColorCommunicator* color_comm;
    Controller* controller;
    SerialFollower* serial_follower;
};

static FComState fcom_state;

void set_serial_callback(DmcommCallback cb) {
    //FURI_LOG_I(TAG, "set_serial_callback %p", cb);
    fcom_state.stream->set_callback(cb);
}

void setup_fcom(void* context) {
    App* app = (App*)context;
    fcom_state.stream = new Stream(app);
    fcom_state.led = new FlipperIndicator(app);
    fcom_state.output = new FComOutput(&gpio_ext_pc3);
    fcom_state.input = new FComInput(&gpio_ext_pb2);

    // Maybe one day I'll copy the flipper oscilloscope analog STM code
    // and make this work...
    //AnalogProngInput analog_input = AnalogProngInput(A3, 5000, 10);
    //ProngTester prong_tester = ProngTester(output, input, analog_input);

    fcom_state.classic_comm = new ClassicCommunicator(*fcom_state.output, *fcom_state.input);
    fcom_state.color_comm = new ColorCommunicator(*fcom_state.output, *fcom_state.input);
    fcom_state.controller = new Controller();
    fcom_state.serial_follower = new SerialFollower(*fcom_state.controller, *fcom_state.stream);

    fcom_state.controller->add(*fcom_state.classic_comm);
    fcom_state.controller->add(*fcom_state.color_comm);
    fcom_state.serial_follower->setIndicator(*fcom_state.led);
    //serial_follower.setProngTester(prong_tester);
}

void loop_fcom() {
    fcom_state.serial_follower->loop();
}

void destroy_fcom() {
    // Deallocate everything
    delete fcom_state.serial_follower;
    delete fcom_state.controller;
    delete fcom_state.color_comm;
    delete fcom_state.classic_comm;
    delete fcom_state.input;
    delete fcom_state.output;
    delete fcom_state.led;
    delete fcom_state.stream;
}