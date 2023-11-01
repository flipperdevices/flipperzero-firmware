# Princeton Sender

This is a demo application that sends a Princeton 24-bit code using the internal radio, then
it sends a 24-bit code using an external radio. It also enables the 5v pin for the external
radio, in case the radio has a 5-volt to 3.3-volt regulator attached.

There is no UI for this project, it just sends the two signals and then exits. Hopefully you
can borrow this code for other projects, such as sending a beacon on a time interval.
