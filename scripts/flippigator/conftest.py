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
from flippigator.flippigator import FlippigatorException, Gator, Navigator, Reader

os.system("color")


def is_windows():
    # check for windows
    return sys.platform.startswith("win")


def pytest_addoption(parser):
    # here you can pass any arguments you want
    parser.addoption("--port", action="store", default=None, help="flipper serial port")
    parser.addoption(
        "--bench_port", action="store", default=None, help="bench serial port"
    )
    parser.addoption(
        "--reader_port", action="store", default=None, help="reader serial port"
    )
    parser.addoption(
        "--relay_port", action="store", default=None, help="relay controller serial port"
    )
    parser.addoption(
        "--flipper_r_port", action="store", default=None, help="flipper ibutton reader and IR serial port"
    )
    parser.addoption(
        "--flipper_k_port", action="store", default=None, help="flipper ibutton key and IR serial port"
    )
    parser.addoption(
        "--path", action="store", default="./img/ref/", help="path to reference images"
    )
    parser.addoption("--debugger", action="store", default=True, help="debug flag")
    parser.addoption("--gui", action="store", default=True, help="gui flag")
    parser.addoption("--scale", action="store", default=8, help="scale factor")
    parser.addoption("--threshold", action="store", default=0.99, help="threshold")
    parser.addoption(
        "--bench_nfc_rfid",
        action="store_true",
        default=False,
        help="use this flag for E2E rfid and nfc bench tests",
    )
    parser.addoption(
        "--bench_ibutton_ir",
        action="store_true",
        default=False,
        help="use this flag for E2E ibutton and ir bench tests",
    )


def pytest_configure(config):
    # here you can add setup before launching session!
    pass


def pytest_unconfigure(config):
    # here you can add teardown after session!
    pass


def pytest_collection_modifyitems(config, items):
    if config.getoption("--bench_nfc_rfid"):
        pass
    else:
        skip_bench_nfc_rfid = pytest.mark.skip(reason="need --bench_nfc_rfid option to run")
        for item in items:
            if "bench_nfc_rfid" in item.keywords:
                item.add_marker(skip_bench_nfc_rfid)
    if config.getoption("--bench_ibutton_ir"):
        pass
    else:
        skip_bench_ibutton_ir = pytest.mark.skip(reason="need --bench__ibutton_ir option to run")
        for item in items:
            if "bench_ibutton_ir" in item.keywords:
                item.add_marker(skip_bench_ibutton_ir)

@pytest.hookimpl(tryfirst=True, hookwrapper=True)
def pytest_runtest_makereport(item, call):
    outcome = yield
    report = outcome.get_result()
    if report.when == "call" and report.failed:
        with allure.step("Failure screenshot"):
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
        flipper_serial.baudrate = 2304000
        flipper_serial.flushOutput()
        flipper_serial.flushInput()
        flipper_serial.timeout = 5
    except serial.serialutil.SerialException:
        print("can not open serial port")
        sys.exit(0)
    except FlipperProtoException:
        print("can not open flipper proto")
        sys.exit(0)

    return flipper_serial


@pytest.fixture(scope="session")
def bench_serial(request):
    # taking port from config or returning OS based default
    port = request.config.getoption("--bench_port")
    if port:
        pass
    if is_windows():
        port = "COM5"
    else:
        port = "/dev/ttyUSB0"

    bench_serial = serial.Serial(port, timeout=1)
    bench_serial.baudrate = 115200

    time.sleep(3)

    bench_serial.flushOutput()
    bench_serial.flushInput()

    return bench_serial


@pytest.fixture(scope="session")
def reader_serial(request):
    # taking port from config or returning OS based default
    port = request.config.getoption("--reader_port")
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

    return reader_serial

@pytest.fixture(scope="session")
def flipper_reader_serial(request):
    # taking port from config or returning OS based default
    port = request.config.getoption("--flipper_r_port")
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

    return flipper_reader_serial

@pytest.fixture(scope="session")
def flipper_key_serial(request):
    # taking port from config or returning OS based default
    port = request.config.getoption("--flipper_k_port")
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

    return flipper_key_serial

@pytest.fixture(scope="session")
def relay_serial(request):
    # taking port from config or returning OS based default
    port = request.config.getoption("--flipper_k_port")
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

    return relay_serial

@pytest.fixture(scope="session")
def nav(flipper_serial, request):
    proto = FlipperProto(serial_port=flipper_serial, debug=True)
    print("Request RPC session")
    proto.start_rpc_session()
    print("RPC session started")

    path = request.config.getoption("--path")
    gui = request.config.getoption("--gui")
    debug = request.config.getoption("--debugger")
    scale = request.config.getoption("--scale")
    threshold = request.config.getoption("--threshold")

    nav = Navigator(
        proto, debug=debug, gui=gui, scale=int(scale), threshold=threshold, path=path, window_name = "Main flipper"
    )
    nav.update_screen()

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
def nav_reader(flipper_reader_serial, request):
    proto = FlipperProto(serial_port=flipper_reader_serial, debug=True)
    print("Request RPC session")
    proto.start_rpc_session()
    print("RPC session started")

    path = request.config.getoption("--path")
    gui = request.config.getoption("--gui")
    debug = request.config.getoption("--debugger")
    scale = request.config.getoption("--scale")
    threshold = request.config.getoption("--threshold")

    nav_reader = Navigator(
        proto, debug=debug, gui=gui, scale=int(scale), threshold=threshold, path=path, window_name = "Reader flipper"
    )
    nav_reader.update_screen()

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
    print("Request RPC session")
    proto.start_rpc_session()
    print("RPC session started")

    path = request.config.getoption("--path")
    gui = request.config.getoption("--gui")
    debug = request.config.getoption("--debugger")
    scale = request.config.getoption("--scale")
    threshold = request.config.getoption("--threshold")

    nav_key = Navigator(
        proto, debug=debug, gui=gui, scale=int(scale), threshold=threshold, path=path, window_name = "Key flipper"
    )
    nav_key.update_screen()

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
    bench = request.config.getoption("--bench_nfc_rfid")
    if bench:
        print("Gator initialization")

        gator = Gator(bench_serial, 900, 900)
        gator.home()

        return gator


@pytest.fixture(scope="session", autouse=False)
def reader_nfc(reader_serial, gator, request) -> Gator:
    bench = request.config.getoption("--bench_nfc_rfid")
    if bench:
        print("Reader NFC initialization")

        reader = Reader(reader_serial, gator, -935.0, -890.0)

        return reader

@pytest.fixture(scope="session", autouse=False)
def reader_em_hid(reader_serial, gator, request) -> Gator:
    bench = request.config.getoption("--bench_nfc_rfid")
    if bench:
        print("Reader RFID Indala initialization")

        reader = Reader(reader_serial, gator, -675.0, -875.0)

        return reader

@pytest.fixture(scope="session", autouse=False)
def reader_indala(reader_serial, gator, request) -> Gator:
    bench = request.config.getoption("--bench_nfc_rfid")
    if bench:
        print("Reader RFID Indala initialization")

        reader = Reader(reader_serial, gator, -935.0, -635.0)

        return reader

@pytest.fixture(scope="session", autouse=False)
def relay(reader_serial, gator, request) -> Gator:
    bench = request.config.getoption("--bench_ibutton_ir")
    if bench:
        print("Relay module initialization")

        relay = Relay(reader_serial)

        return relay
