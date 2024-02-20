
#include "dmcomm-lib/DMComm.h"

void setup()
{
    LED_Indicator led = LED_Indicator();
    DComOutput output = FcomOutput(C3, DMCOMM_NO_PIN);
    DigitalProngInput input = DigitalProngInput(A4);
    //AnalogProngInput analog_input = AnalogProngInput(A3, 5000, 10);
    //ProngTester prong_tester = ProngTester(output, input, analog_input);

    ClassicCommunicator classic_comm = ClassicCommunicator(output, input);
    ColorCommunicator color_comm = ColorCommunicator(output, input);
    Controller controller = Controller();
    SerialFollower serial_follower = SerialFollower(controller, Stream);

    controller.add(classic_comm);
    controller.add(color_comm);
    serial_follower.setIndicator(led);
    //serial_follower.setProngTester(prong_tester);
}

void loop()
{
    serial_follower.loop();
}