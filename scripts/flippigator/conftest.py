import asyncio
import logging
import os
import sys
import time
from datetime import datetime

import allure
import pytest
import serial
from flippigator.extensions.proxmark_wrapper import proxmark_wrapper
from flippigator.flippigator import (
    FlippigatorException,
    Gator,
    Navigator,
    Reader,
    Relay,
)
from async_protopy.connectors.serial_connector import SerialConnector
from async_protopy.clients.protobuf_client import FlipperProtobufClient


os.system("color")


@pytest.fixture(scope="session")
def event_loop():
    loop = asyncio.get_event_loop()
    yield loop
    loop.close()


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
    if not os.path.exists('reports'):
        os.makedirs('reports')
    config.option.htmlpath = 'reports/'+datetime.now().strftime("%d-%m-%Y %H-%M-%S")+".html"

    if config.getoption("--relay-port") != None:
        print("Reboot flippers")
        relay_serial = serial.Serial(config.getoption("--relay-port"), timeout=1)
        relay_serial.baudrate = 115200
        relay_serial.write(("R9K0\n").encode("ASCII"))
        time.sleep(0.2)
        relay_serial.write(("R0K0\n").encode("ASCII"))
        time.sleep(5)
    else:
        if config.getoption("--port"):
            pass
            #reboot_flipper(config.getoption("--port"))
        if config.getoption("--bench-ibutton-ir"):
            reboot_flipper(config.getoption("--flipper-r-port"))
            reboot_flipper(config.getoption("--flipper-k-port"))
        time.sleep(2)
    if config.getoption("--update-flippers"):
        chan = config.getoption("--update-flippers")
        update_firmware(config.getoption("--port"), chan)
        if config.getoption("--bench-ibutton-ir"):
            update_firmware(config.getoption("--flipper-r-port"), chan)
            update_firmware(config.getoption("--flipper-k-port"), chan)


def pytest_unconfigure(config):
    # here you can add teardown after session!
    pass

async def reboot_flipper(port):
    try:
        flipper_serial = serial.Serial(port, timeout=1)
        flipper_serial.write("power reboot\n\r".encode())
        flipper_serial.close()
    except serial.serialutil.SerialException:
        raise Exception('NoFlipper', 'There are no flipper on defined port, or it is unavailable')
    start_time = time.time()
    time.sleep(1)
    while time.time() - start_time < 80:
        try:
            flipper_serial = serial.Serial(port, timeout=1)
            flipper_serial.close()
        except serial.serialutil.SerialException:
            await asyncio.sleep(1)
            print(
                "Waiting for flipper boot after reboot "
                + str(int(time.time() - start_time))
                + "s"
            )
            logging.debug(
                "Waiting for flipper boot after reboot "
                + str(int(time.time() - start_time))
                + "s"
            )
            if time.time() - start_time > 60:
                logging.error("can not open serial port")
                raise Exception('FlipperDisapear', 'Can\'t connect to flipper after reboot')
                sys.exit(0)
        else:
            break
    return 0

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
    os.system("ufbt flash_usb FLIP_PORT=" + str(port))
    time.sleep(2)

    start_time = time.time()
    while time.time() - start_time < 120:
        try:
            flipper_serial = serial.Serial(port, timeout=1)
            flipper_serial.close()
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
                nav.save_screen("failed_"+report.nodeid+".bmp")
                allure.attach.file(
                    "img/failed"+report.nodeid+".bmp",
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
        port = "/dev/ttyACM10"

    try:

        flipper_serial = SerialConnector(url=port, baud_rate=230400, timeout=5)
    except serial.serialutil.SerialException:  # TODO exception
        logging.error("can not open serial port")
        sys.exit(0)

    logging.debug("Flipper serial port opened on" + port)
    return flipper_serial


@pytest.fixture(scope="session")
async def bench_serial(request):
    # taking port from config or returning OS based default
    port = request.config.getoption("--bench-port")
    if port:
        pass
    elif is_windows():
        port = "COM5"
    else:
        port = "/dev/ttyUSB0"

    print("bTry to open")
    bench_serial = SerialConnector(url=port, baud_rate=115200, timeout=1)
    print("b"+str(port))
    await bench_serial.open_connection()
    print("bDone")
    await asyncio.sleep(3)

    await bench_serial.drain()
    print("bdrain")
    await bench_serial.clear_read_buffer()
    print("bclear")

    logging.debug("NFC and RFID bench serial port opened on" + port)
    return bench_serial


@pytest.fixture(scope="session")
async def reader_serial(request):
    # taking port from config or returning OS based default
    port = request.config.getoption("--reader-port")
    if port:
        pass
    elif is_windows():
        port = "COM6"
    else:
        port = "/dev/ttyACM11"

    reader_serial = SerialConnector(url=port, baud_rate=115200, timeout=1)

    await asyncio.sleep(3)
    #await reader_serial.open_connection()

    await reader_serial.drain()
    await reader_serial.clear_read_buffer()

    logging.debug("NFC and RFID Readers serial port opened on" + port)
    return reader_serial


@pytest.fixture(scope="session")
async def flipper_reader_serial(request):
    # taking port from config or returning OS based default
    port = request.config.getoption("--flipper-r-port")
    if port:
        pass
    elif is_windows():
        port = "COM7"
    else:
        port = "/dev/ttyACM12"

    print("Try to open reader flipper serial")
    flipper_reader_serial = SerialConnector(url=port, baud_rate=2304000)
    print("Wait for connection")
    await asyncio.sleep(1)
    #await flipper_reader_serial.open_connection()
    print("Connection established")
    #await flipper_reader_serial.drain()
    #await flipper_reader_serial.clear_read_buffer()
    print("Buffer drained")
    #await asyncio.sleep(1)
    #import pdb
    #pdb.set_trace()
    logging.debug("Flipper 'reader' serial port opened on" + port)
    return flipper_reader_serial


@pytest.fixture(scope="session")
async def flipper_key_serial(request):
    # taking port from config or returning OS based default
    port = request.config.getoption("--flipper-k-port")
    if port:
        pass
    elif is_windows():
        port = "COM8"
    else:
        port = "/dev/ttyACM13"

    print("Try to open key flipper serial")
    flipper_key_serial = SerialConnector(url=port, baud_rate=2304000)
    print("Wait for connection")
    #await asyncio.sleep(1)
    #await flipper_key_serial.open_connection()
    print("Connection established")
    #await flipper_key_serial.drain()
    #await flipper_key_serial.clear_read_buffer()
    print("Buffer drained")
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
        port = "/dev/ttyACM14"

    print("Try to open")
    relay_serial = serial.Serial(port, timeout=1)
    relay_serial.baudrate = 115200
    print(str(port))
    print("Done")


    relay_serial.flushOutput()
    print("drain")
    relay_serial.flushInput()
    print("clear")

    logging.debug("Relay serial port opened on" + port)
    return relay_serial


@pytest.fixture(scope="session")
async def nav(flipper_serial, request):
    proto = FlipperProtobufClient(flipper_serial)
    await proto.start()
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

    if not (request.config.getoption("--no-init")):
        # Enabling of bluetooth
        await nav.go_to_main_screen()
        await nav.press_ok()
        await nav.go_to("Settings")
        await nav.press_ok()
        await nav.go_to("Bluetooth")
        await nav.press_ok()
        menu = await nav.get_menu_list()
        if "BluetoothIsON" in menu:
            pass
        elif "BluetoothIsOFF" in menu:
            await nav.press_right()
        else:
            raise FlippigatorException("Can not enable bluetooth")

        # Enabling Debug
        await nav.press_back()
        await nav.go_to("System")
        await nav.press_ok()
        menu = await nav.get_menu_list()
        if "DebugIsON" in menu:
            pass
        elif "DebugIsOFF" in menu:
            await nav.go_to("DebugIsOFF")
            await nav.press_right()
        else:
            raise FlippigatorException("Can not enable debug")

    yield nav
    await proto.stop()



@pytest.fixture(scope="session")
def px(request):
    px = proxmark_wrapper(request.config.getoption("--px3-path"))
    return px


@pytest.fixture(scope="session")
async def nav_reader(flipper_reader_serial, request):
    proto = FlipperProtobufClient(flipper_reader_serial)
    print("Try to start proto: ", end = '')
    await proto.start()
    print("Done")
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
    print("Try to update screen: ", end = '')
    await nav_reader.update_screen()
    nav_reader.draw_screen()
    nav_reader.draw_screen()
    print("Done")
    if not (request.config.getoption("--no-init")):
        # Enabling of bluetooth
        await nav_reader.go_to_main_screen()
        await nav_reader.press_ok()
        await nav_reader.go_to("Settings")
        await nav_reader.press_ok()
        await nav_reader.go_to("Bluetooth")
        await nav_reader.press_ok()
        menu = await nav_reader.get_menu_list()
        if "BluetoothIsON" in menu:
            pass
        elif "BluetoothIsOFF" in menu:
            nav_reader.press_right()
        else:
            raise FlippigatorException("Can not enable bluetooth")

        # Enabling Debug
        await nav_reader.press_back()
        await nav_reader.go_to("System")
        await nav_reader.press_ok()
        menu = await nav_reader.get_menu_list()
        if "DebugIsON" in menu:
            pass
        elif "DebugIsOFF" in menu:
            await nav_reader.go_to("DebugIsOFF")
            await nav_reader.press_right()
        else:
            raise FlippigatorException("Can not enable debug")

    yield nav_reader
    await proto.stop()


@pytest.fixture(scope="session")
async def nav_key(flipper_key_serial, request):
    proto = FlipperProtobufClient(flipper_key_serial)
    await proto.start()
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
    await nav_key.update_screen()
    nav_key.draw_screen()
    nav_key.draw_screen()
    if not (request.config.getoption("--no-init")):
        # Enabling of bluetooth
        await nav_key.go_to_main_screen()
        await nav_key.press_ok()
        await nav_key.go_to("Settings")
        await nav_key.press_ok()
        await nav_key.go_to("Bluetooth")
        await nav_key.press_ok()
        menu = await nav_key.get_menu_list()
        if "BluetoothIsON" in menu:
            pass
        elif "BluetoothIsOFF" in menu:
            await nav_key.press_right()
        else:
            raise FlippigatorException("Can not enable bluetooth")

        # Enabling Debug
        await nav_key.press_back()
        await nav_key.go_to("System")
        await nav_key.press_ok()
        menu = await nav_key.get_menu_list()
        if "DebugIsON" in menu:
            pass
        elif "DebugIsOFF" in menu:
            await nav_key.go_to("DebugIsOFF")
            await nav_key.press_right()
        else:
            raise FlippigatorException("Can not enable debug")

    yield nav_key
    await proto.stop()


@pytest.fixture(scope="session", autouse=False)
async def gator(bench_serial, request) -> Gator:
    bench = request.config.getoption("--bench-nfc-rfid")
    if bench:
        logging.debug("Gator initialization")

        gator = Gator(bench_serial, 900, 900)
        print("Homing bench")
        await gator.home()
        print("Homing success")

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
def relay(relay_serial, request) -> Relay:
    bench = request.config.getoption("--bench-ibutton-ir")
    if bench:
        logging.debug("Relay module initialization")

        relay = Relay(relay_serial)
        relay.reset()

        logging.debug("Relay module initialization complete")
        return relay
