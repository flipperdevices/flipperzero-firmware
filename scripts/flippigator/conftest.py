import os
import sys
import time

import allure
import pytest
import serial

from flipperzero_protobuf_py.flipperzero_protobuf.flipper_base import \
    FlipperProtoException
from flipperzero_protobuf_py.flipperzero_protobuf.flipper_proto import \
    FlipperProto
from flippigator.flippigator import FlippigatorException, Gator, Navigator, Reader

os.system("color")


def is_windows():
    # check for windows
    return sys.platform.startswith("win")


def pytest_addoption(parser):
    # here you can pass any arguments you want
    parser.addoption("--port", action="store", default=None, help="flipper serial port")
    parser.addoption("--bench_port", action="store", default=None, help="bench serial port")
    parser.addoption("--reader_port", action="store", default=None, help="reader serial port")
    parser.addoption(
        "--path", action="store", default="./img/ref/", help="path to reference images"
    )
    parser.addoption("--debugger", action="store", default=True, help="debug flag")
    parser.addoption("--gui", action="store", default=True, help="gui flag")
    parser.addoption("--scale", action="store", default=12, help="scale factor")
    parser.addoption("--threshold", action="store", default=0.99, help="threshold")
    parser.addoption("--bench", action="store_true", default=False, help="use this flag for E2E bench tests")


def pytest_configure(config):
    # here you can add setup before launching session!
    pass


def pytest_unconfigure(config):
    # here you can add teardown after session!
    pass

def pytest_collection_modifyitems(config, items):
    if config.getoption("--bench"):
        return
    skip_bench = pytest.mark.skip(reason="need --bench option to run")
    for item in items:
        if "bench" in item.keywords:
            item.add_marker(skip_bench)


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
        flipper_serial.timeout = None
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
        proto, debug=debug, gui=gui, scale=scale, threshold=threshold, path=path
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


@pytest.fixture(scope="session", autouse=False)
def gator(bench_serial, request) -> Gator:
    bench = request.config.getoption("--bench")
    if bench:
        print("Gator initialization")

        gator = Gator(bench_serial, 900, 900)
        gator.home()

        return gator

@pytest.fixture(scope="session", autouse=False)
def reader_nfc(reader_serial, gator, request) -> Gator:
    bench = request.config.getoption("--bench")
    if bench:
        print("Reader initialization")

        reader = Reader(reader_serial, gator, -935.0, -890.0)

        return reader
