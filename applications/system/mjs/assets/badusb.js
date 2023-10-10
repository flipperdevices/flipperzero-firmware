let badusb = require("badusb");

badusb.setup({ vid: 0xAAAA, pid: 0xBBBB, mfr_name: "manufacturer", prod_name: "product" });
delay(1000);

if (badusb.isConnected()) {
    print("Connected");
    badusb.press("a");
    badusb.press("A");
    badusb.press(9);
    badusb.press(0x0f);
    badusb.press("SHIFT", 0x0C);
    badusb.press("CTRL", "a");
    delay(500);
    badusb.press("RIGHT");
    badusb.hold("a");
    delay(2000);
    badusb.release("a");
    badusb.hold("SHIFT", "b");
    delay(2000);
    badusb.release();
    badusb.press("ENTER");
    badusb.print("Hello")
    badusb.println("abcd")
    badusb.print("12345", 1000);
    delay(1000);
} else {
    print("USB not connected");
}
