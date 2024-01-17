import logging
import os
import sys
import time

import allure
import pytest
import serial
from flipperzero_protobuf_py.flipperzero_protobuf.flipper_base import (
    FlipperProtoException,
)
from flipperzero_protobuf_py.flipperzero_protobuf.flipper_proto import FlipperProto
from flippigator.extensions.proxmark_wrapper import proxmark_wrapper
from flippigator.flippigator import (
    FlippigatorException,
    Gator,
    Navigator,
    Reader,
    Relay,
)

os.system("color")


def is_windows():
    # check for windows
    return sys.platform.startswith("win")


def pytest_addoption(parser):
    # here you can pass any arguments you want
    """
    You can add static name for usb devices by adding rules to /etc/udev/rules.d/99-usb-serial.rules
    SUBSYSTEM=="tty", ATTRS{idVendor}=="%your_device_VID%", ATTRS{idProduct}=="%your_device_PID%",
    ATTRS{serial}=="%your_device_serial%", SYMLINK+="name"
    pytest -v --scale=8 --bench-nfc-rfid --bench-ibutton-ir --port=/dev/bench_flipper --reader-port=/dev/reader_arduino --flipper-r-port=/dev/ireader_flipper --flipper-k-port=/dev/ikey_flipper --relay-port=/dev/relay_arduino
    """
    parser.addoption("--port", action="store", default=None, help="flipper serial port")
    parser.addoption(
        "--bench-port", action="store", default=None, help="bench serial port"
    )
    parser.addoption(
        "--reader-port", action="store", default=None, help="reader serial port"
    )
    parser.addoption(
        "--relay-port",
        action="store",
        default=None,
        help="relay controller serial port",
    )
    parser.addoption(
        "--flipper-r-port",
        action="store",
        default=None,
        help="flipper ibutton reader and IR serial port",
    )
    parser.addoption(
        "--flipper-k-port",
        action="store",
        default=None,
        help="flipper ibutton key and IR serial port",
    )
    parser.addoption(
        "--path", action="store", default="./img/ref/", help="path to reference images"
    )
    parser.addoption("--debugger", action="store", default=True, help="debug flag")
    parser.addoption("--gui", action="store", default=True, help="gui flag")
    parser.addoption("--scale", action="store", default=8, help="scale factor")
    parser.addoption("--threshold", action="store", default=0.99, help="threshold")
    parser.addoption(
        "--bench-nfc-rfid",
        action="store_true",
        default=False,
        help="use this flag for E2E rfid and nfc bench tests",
    )
    parser.addoption(
        "--bench-ibutton-ir",
        action="store_true",
        default=False,
        help="use this flag for E2E ibutton and ir bench tests",
    )
    parser.addoption(
        "--no-init",
        action="store_true",
        default=False,
        help="use this flag for skip setup cycle",
    )
    parser.addoption(
        "--px3-path",
        action="store",
        default="C:\\Users\\User\\Desktop\\proxmark3\\client\\proxmark3.exe",
        help="path to proxmark3 executable file",
        type=str,
    )
    parser.addoption(
        "--update-flippers",
        action="store",
        default=None,
        help="use this parameter for update all flippers for exact update channel",
    )


def pytest_configure(config):
    # here you can add setup before launching session!
    if config.getoption("--update-flippers"):
        chan = config.getoption("--update-flippers")
        update_firmware(config.getoption("--port"), chan)
        if config.getoption("--bench-ibutton-ir"):
            update_firmware(config.getoption("--flipper-r-port"), chan)
            update_firmware(config.getoption("--flipper-k-port"), chan)


def pytest_unconfigure(config):
    # here you can add teardown after session!
    pass


def update_firmware(port, channel):
    #check flipper on defined adress and it availability
    try:
        flipper_serial = serial.Serial(port, timeout=1)
    except serial.serialutil.SerialException:
        raise Exception('NoFlipper', 'There are no flipper on defined port, or it is unavailable')
    flipper_serial.close()

    #update flipper's firmware via ufbt
    os.system("ufbt dotenv_create")
    os.system("ufbt update --channel=" + channel)
    os.system("ufbt flash_usb FLIP_PORT=" + port)
    time.sleep(2)

    start_time = time.time()
    while time.time() - start_time < 120:
        try:
            flipper_serial = serial.Serial(port, timeout=1)
        except serial.serialutil.SerialException:
            time.sleep(1)
            print(
                "Waiting for flipper boot after update "
                + str(int(time.time() - start_time))
                + "s"
            )
            logging.debug(
                "Waiting for flipper boot after update "
                + str(int(time.time() - start_time))
                + "s"
            )
            if time.time() - start_time > 120:
                logging.error("can not open serial port")
                raise Exception('FlipperDisapear', 'Can\'t connect to flipper after update')
        else:
            break
    return 0

def pytest_collection_modifyitems(config, items):
    if config.getoption("--debugger"):
        logging.basicConfig(level=logging.DEBUG)
    if config.getoption("--bench-nfc-rfid"):
        logging.info("NFC and RFID bench option selected")
    else:
        logging.info("NFC and RFID bench option unselected")
        skip_bench_nfc_rfid = pytest.mark.skip(
            reason="need --bench-nfc-rfid option to run"
        )

        for item in items:
            if "bench-nfc-rfid" in item.keywords:
                item.add_marker(skip_bench_nfc_rfid)
    if config.getoption("--bench-ibutton-ir"):
        logging.info("IButton and Infrared bench option selected")
    else:
        logging.info("IButton and Infrared bench option unselected")
        skip_bench_ibutton_ir = pytest.mark.skip(
            reason="need --bench-ibutton-ir option to run"
        )

        for item in items:
            if "bench-ibutton-ir" in item.keywords:
                item.add_marker(skip_bench_ibutton_ir)


@pytest.hookimpl(tryfirst=True, hookwrapper=True)
def pytest_runtest_makereport(item, call):
    outcome = yield
    report = outcome.get_result()
    if report.when == "call" and report.failed:
        with allure.step("Failure screenshot"):
            logging.warning("Recognition failure, screenshot will be saved")
            nav = item.funcargs.get("nav")

            if nav:
                nav.save_screen("failed.bmp")
                allure.attach.file(
                    "img/failed.bmp",
                    name="last_screenshot",
                    attachment_type=allure.attachment_type.BMP,
                )


@pytest.fixture(scope="session")
def flipper_serial(request):
    # taking port from config or returning OS based default
    port = request.config.getoption("--port")
    if port:
        pass
    elif is_windows():
        port = "COM4"
    else:
        port = "/dev/ttyACM0"

    try:
        flipper_serial = serial.Serial(port, timeout=1)
        flipper_serial.baudrate = 230400
        flipper_serial.flushOutput()
        flipper_serial.flushInput()
        flipper_serial.timeout = 5
    except serial.serialutil.SerialException:
        logging.error("can not open serial port")
        sys.exit(0)

    logging.debug("Flipper serial port opened on" + port)
    return flipper_serial


@pytest.fixture(scope="session")
def bench_serial(request):
    # taking port from config or returning OS based default
    port = request.config.getoption("--bench-port")
    if port:
        pass
    elif is_windows():
        port = "COM5"
    else:
        port = "/dev/ttyUSB0"

    bench_serial = serial.Serial(port, timeout=1)
    bench_serial.baudrate = 115200

    time.sleep(3)

    bench_serial.flushOutput()
    bench_serial.flushInput()

    logging.debug("NFC and RFID bench serial port opened on" + port)
    return bench_serial


@pytest.fixture(scope="session")
def reader_serial(request):
    # taking port from config or returning OS based default
    port = request.config.getoption("--reader-port")
    if port:
        pass
    elif is_windows():
        port = "COM6"
    else:
        port = "/dev/ttyACM1"

    reader_serial = serial.Serial(port, timeout=1)  # Надо будет переделать!!!
    reader_serial.baudrate = 115200

    time.sleep(3)

    reader_serial.flushOutput()
    reader_serial.flushInput()

    logging.debug("NFC and RFID Readers serial port opened on" + port)
    return reader_serial


@pytest.fixture(scope="session")
def flipper_reader_serial(request):
    # taking port from config or returning OS based default
    port = request.config.getoption("--flipper-r-port")
    if port:
        pass
    elif is_windows():
        port = "COM7"
    else:
        port = "/dev/ttyACM2"

    flipper_reader_serial = serial.Serial(port, timeout=1)  # Надо будет переделать!!!
    flipper_reader_serial.baudrate = 2304000

    time.sleep(3)

    flipper_reader_serial.flushOutput()
    flipper_reader_serial.flushInput()

    logging.debug("Flipper 'reader' serial port opened on" + port)
    return flipper_reader_serial


@pytest.fixture(scope="session")
def flipper_key_serial(request):
    # taking port from config or returning OS based default
    port = request.config.getoption("--flipper-k-port")
    if port:
        pass
    elif is_windows():
        port = "COM8"
    else:
        port = "/dev/ttyACM3"

    flipper_key_serial = serial.Serial(port, timeout=1)  # Надо будет переделать!!!
    flipper_key_serial.baudrate = 2304000

    time.sleep(3)

    flipper_key_serial.flushOutput()
    flipper_key_serial.flushInput()

    logging.debug("Flipper 'key' serial port opened on" + port)
    return flipper_key_serial


@pytest.fixture(scope="session")
def relay_serial(request):
    # taking port from config or returning OS based default
    port = request.config.getoption("--relay-port")
    if port:
        pass
    elif is_windows():
        port = "COM9"
    else:
        port = "/dev/ttyACM4"

    relay_serial = serial.Serial(port, timeout=1)  # Надо будет переделать!!!
    relay_serial.baudrate = 115200

    time.sleep(3)

    relay_serial.flushOutput()
    relay_serial.flushInput()

    logging.debug("Relay serial port opened on" + port)
    return relay_serial


@pytest.fixture(scope="session")
def nav(flipper_serial, request):
    proto = FlipperProto(serial_port=flipper_serial, debug=True)
    proto.start_rpc_session()
    logging.debug("RPC session of main flipper started")

    path = request.config.getoption("--path")
    gui = request.config.getoption("--gui")
    debug = request.config.getoption("--debugger")
    scale = request.config.getoption("--scale")
    threshold = request.config.getoption("--threshold")

    nav = Navigator(
        proto,
        debug=debug,
        gui=gui,
        scale=int(scale),
        threshold=threshold,
        path=path,
        window_name="Main flipper",
    )
    nav.update_screen()

    if not (request.config.getoption("--no-init")):
        # Enabling of bluetooth
        nav.go_to_main_screen()
        nav.press_ok()
        nav.go_to("Settings")
        nav.press_ok()
        nav.go_to("Bluetooth")
        nav.press_ok()
        nav.update_screen()
        menu = nav.get_menu_list()
        if "BluetoothIsON" in menu:
            pass
        elif "BluetoothIsOFF" in menu:
            nav.press_right()
        else:
            raise FlippigatorException("Can not enable bluetooth")

        # Enabling Debug
        nav.press_back()
        nav.go_to("System")
        nav.press_ok()
        menu = nav.get_menu_list()
        if "DebugIsON" in menu:
            pass
        elif "DebugIsOFF" in menu:
            nav.go_to("DebugIsOFF")
            nav.press_right()
        else:
            raise FlippigatorException("Can not enable debug")

    return nav


@pytest.fixture(scope="session")
def px(request):
    px = proxmark_wrapper(request.config.getoption("--px3-path"))
    return px


@pytest.fixture(scope="session")
def nav_reader(flipper_reader_serial, request):
    proto = FlipperProto(serial_port=flipper_reader_serial, debug=True)
    proto.start_rpc_session()
    logging.debug("RPC session of flipper 'reader' started")

    path = request.config.getoption("--path")
    gui = request.config.getoption("--gui")
    debug = request.config.getoption("--debugger")
    scale = request.config.getoption("--scale")
    threshold = request.config.getoption("--threshold")

    nav_reader = Navigator(
        proto,
        debug=debug,
        gui=gui,
        scale=int(scale),
        threshold=threshold,
        path=path,
        window_name="Reader flipper",
    )
    nav_reader.update_screen()
    if not (request.config.getoption("--no-init")):
        # Enabling of bluetooth
        nav_reader.go_to_main_screen()
        nav_reader.press_ok()
        nav_reader.go_to("Settings")
        nav_reader.press_ok()
        nav_reader.go_to("Bluetooth")
        nav_reader.press_ok()
        nav_reader.update_screen()
        menu = nav_reader.get_menu_list()
        if "BluetoothIsON" in menu:
            pass
        elif "BluetoothIsOFF" in menu:
            nav_reader.press_right()
        else:
            raise FlippigatorException("Can not enable bluetooth")

        # Enabling Debug
        nav_reader.press_back()
        nav_reader.go_to("System")
        nav_reader.press_ok()
        menu = nav_reader.get_menu_list()
        if "DebugIsON" in menu:
            pass
        elif "DebugIsOFF" in menu:
            nav_reader.go_to("DebugIsOFF")
            nav_reader.press_right()
        else:
            raise FlippigatorException("Can not enable debug")

    return nav_reader


@pytest.fixture(scope="session")
def nav_key(flipper_key_serial, request):
    proto = FlipperProto(serial_port=flipper_key_serial, debug=True)
    proto.start_rpc_session()
    logging.debug("RPC session of flipper 'key' started")

    path = request.config.getoption("--path")
    gui = request.config.getoption("--gui")
    debug = request.config.getoption("--debugger")
    scale = request.config.getoption("--scale")
    threshold = request.config.getoption("--threshold")

    nav_key = Navigator(
        proto,
        debug=debug,
        gui=gui,
        scale=int(scale),
        threshold=threshold,
        path=path,
        window_name="Key flipper",
    )
    nav_key.update_screen()
    if not (request.config.getoption("--no-init")):
        # Enabling of bluetooth
        nav_key.go_to_main_screen()
        nav_key.press_ok()
        nav_key.go_to("Settings")
        nav_key.press_ok()
        nav_key.go_to("Bluetooth")
        nav_key.press_ok()
        nav_key.update_screen()
        menu = nav_key.get_menu_list()
        if "BluetoothIsON" in menu:
            pass
        elif "BluetoothIsOFF" in menu:
            nav_key.press_right()
        else:
            raise FlippigatorException("Can not enable bluetooth")

        # Enabling Debug
        nav_key.press_back()
        nav_key.go_to("System")
        nav_key.press_ok()
        menu = nav_key.get_menu_list()
        if "DebugIsON" in menu:
            pass
        elif "DebugIsOFF" in menu:
            nav_key.go_to("DebugIsOFF")
            nav_key.press_right()
        else:
            raise FlippigatorException("Can not enable debug")

    return nav_key


@pytest.fixture(scope="session", autouse=False)
def gator(bench_serial, request) -> Gator:
    bench = request.config.getoption("--bench-nfc-rfid")
    if bench:
        logging.debug("Gator initialization")

        gator = Gator(bench_serial, 900, 900)
        gator.home()

        logging.debug("Gator initialization complete")
        return gator


@pytest.fixture(scope="session", autouse=False)
def reader_nfc(reader_serial, gator, request) -> Reader:
    bench = request.config.getoption("--bench-nfc-rfid")
    if bench:
        logging.debug("NFC reader initialization")

        reader = Reader(reader_serial, gator, -925.0, -890.0)

        logging.debug("NFC reader initialization complete")
        return reader


@pytest.fixture(scope="session", autouse=False)
def reader_em_hid(reader_serial, gator, request) -> Reader:
    bench = request.config.getoption("--bench-nfc-rfid")
    if bench:
        logging.debug("Reader RFID EM and HID initialization")

        reader = Reader(reader_serial, gator, -665.0, -875.0)

        logging.debug("Reader RFID EM and HID initialization complete")
        return reader


@pytest.fixture(scope="session", autouse=False)
def reader_indala(reader_serial, gator, request) -> Reader:
    bench = request.config.getoption("--bench-nfc-rfid")
    if bench:
        logging.debug("Reader RFID Indala initialization")

        reader = Reader(reader_serial, gator, -925.0, -635.0)

        logging.debug("Reader RFID Indala initialization complete")
        return reader


@pytest.fixture(scope="session", autouse=False)
def relay(relay_serial, gator, request) -> Relay:
    bench = request.config.getoption("--bench-ibutton-ir")
    if bench:
        logging.debug("Relay module initialization")

        relay = Relay(relay_serial)

        logging.debug("Relay module initialization complete")
        return relay
