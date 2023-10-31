#!/usr/bin/env python3
import logging
import os
import time
from typing import Optional

import cv2 as cv
import numpy
from flippigator.modules.applications import AppApplications
from flippigator.modules.badusb import AppBadusb
from flippigator.modules.gpio import AppGpio
from flippigator.modules.ibutton import AppIbutton
from flippigator.modules.infrared import AppInfrared
from flippigator.modules.nfc import AppNfc
from flippigator.modules.rfid import AppRfid
from flippigator.modules.settings import AppSettings
from flippigator.modules.subghz import AppSubGhz
from flippigator.modules.u2f import AppU2f

# APPLY_TEMPLATE_THRESHOLD = 0.99
SCREEN_H = 128
SCREEN_W = 64
# SCALE = 8


class Navigator:
    def __init__(
        self,
        proto,
        path: str = "./img/ref/",
        debug: bool = True,
        gui: bool = True,
        scale: int = 8,
        threshold: float = 0.99,
        window_name="Default window name",
    ):
        self.imRef = dict()
        self.proto = proto
        self._debugFlag = debug
        self._guiFlag = gui
        self._scale = scale
        self._threshold = threshold
        self._window_name = window_name
        self.screen_image = numpy.zeros((SCREEN_H, SCREEN_W))

        self.logger = logging.getLogger(window_name)
        if self._debugFlag:
            self.logger.setLevel(logging.DEBUG)
        else:
            self.logger.setLevel(logging.WARNING)

        self.nfc = AppNfc(self)
        self.rfid = AppRfid(self)
        self.subghz = AppSubGhz(self)
        self.infrared = AppInfrared(self)
        self.gpio = AppGpio(self)
        self.ibutton = AppIbutton(self)
        self.badusb = AppBadusb(self)
        self.u2f = AppU2f(self)
        self.applications = AppApplications(self)
        self.settings = AppSettings(self)

        self.font_helvB08 = list()
        for i in range(256):
            self.font_helvB08.append(cv.imread("./font_helvB08/helvB08_" + str(i) + ".png"))
        self.font_HaxrCorp_4089 = list()
        for i in range(256):
            self.font_HaxrCorp_4089.append(cv.imread("./font_HaxrCorp_4089/HaxrCorp_4089_" + str(i) + ".png"))
        self.font_profont11 = list()
        for i in range(256):
            self.font_profont11.append(cv.imread("./font_profont11/profont11_" + str(i) + ".png"))
        self.font_profont22 = list()
        for i in range(256):
            self.font_profont22.append(cv.imread("./font_profont22/profont22_" + str(i) + ".png"))

        for filename in os.listdir(path):
            f = cv.imread(path + str(filename), 0)
            self.imRef.update({filename[:-4]: f})

    def __del__(self):
        pass

    def update_screen(self):
        data = self.proto.rpc_gui_snapshot_screen()

        def get_bin(x):
            return format(x, "b")

        scr = numpy.zeros((SCREEN_H + 1, SCREEN_W + 1))

        x = y = 0
        basey = 0

        for i in range(0, int(SCREEN_H * SCREEN_W / 8)):
            tmp = get_bin(data[i])
            tmp = "0" * (8 - len(tmp)) + tmp
            tmp = tmp[::-1]  # reverse

            y = basey
            x += 1
            for c in tmp:
                scr[x][y] = c
                y += 1

            if (i + 1) % SCREEN_H == 0:
                basey += 8
                x = 0

        screen_image = cv.cvtColor(
            scr[1:129, 0:64].astype(numpy.uint8) * 255, 0
        )  # cv.COLOR_GRAY2BGR)
        screen_image = 255 - screen_image
        screen_image = cv.rotate(screen_image, cv.ROTATE_90_CLOCKWISE)
        screen_image = cv.flip(screen_image, 1)

        if self._debugFlag == 1:
            self.logger.debug("Screen update")

        self.screen_image = screen_image.copy()

    def get_raw_screen(self):
        return self.screen_image.copy()

    def get_screen(self):
        display_image = self.get_raw_screen()
        mask = cv.inRange(display_image, (255, 255, 255, 0), (255, 255, 255, 0))
        display_image[mask > 0] = (1, 145, 251, 0)
        display_image = cv.resize(
            display_image,
            (128 * self._scale, 64 * self._scale),
            interpolation=cv.INTER_NEAREST,
        )

        return display_image.copy()

    def draw_screen(self):
        display_image = self.get_screen()
        mask = cv.inRange(display_image, (255, 255, 255, 0), (255, 255, 255, 0))
        display_image[mask > 0] = (1, 145, 251, 0)
        display_image = cv.resize(
            display_image,
            (128 * self._scale, 64 * self._scale),
            interpolation=cv.INTER_NEAREST,
        )

        cv.imshow(self._window_name, display_image)
        key = cv.waitKey(1)

    def get_ref_from_string(phrase, font):
        result = font[ord(phrase[0])]
        for i in phrase[1:]:
            result = numpy.concatenate((result, font[ord(i)]), axis=1)
        return result

    def recog_ref(self, ref=None, area=(0, 64, 0, 128)):
        if ref is None:
            ref = []
        # self.updateScreen()
        temp_pic_list = list()
        screen_image = self.get_raw_screen()[area[0] : area[1], area[2] : area[3]]
        if len(ref) == 0:
            ref = list(self.imRef.keys())
        for im in ref:
            template = cv.cvtColor(self.imRef.get(im), 0)
            if (template.shape[0] <= screen_image.shape[0]) and (
                (template.shape[1] <= screen_image.shape[1])
            ):
                res = cv.matchTemplate(screen_image, template, cv.TM_CCOEFF_NORMED)
                min_val, max_val, min_loc, max_loc = cv.minMaxLoc(res)
                if max_val > self._threshold:
                    h, w, ch = template.shape
                    temp_pic_list.append(
                        (
                            str(im),
                            (max_loc[0] + area[2], max_loc[1] + area[0]),
                            (max_loc[0] + w + area[2], max_loc[1] + h + area[0]),
                        )
                    )

        found_ic = list()

        # display_image = None
        # TODO: add blank image initialization

        display_image = self.get_screen()

        for i in temp_pic_list:
            if self._guiFlag == 1:
                display_image = cv.rectangle(
                    display_image,
                    (i[1][0] * self._scale, i[1][1] * self._scale),
                    (i[2][0] * self._scale, i[2][1] * self._scale),
                    (255, 255, 0, 0),
                    2,
                )
                display_image = cv.putText(
                    display_image,
                    i[0],
                    (i[1][0] * self._scale, (i[2][1] + 2) * self._scale),
                    cv.FONT_HERSHEY_SIMPLEX,
                    0.5,
                    (255, 255, 0, 0),
                    1,
                )
            found_ic.append(i[0])

        # todo: add reference display_image
        # initialize display_image
        # display_image = self.screen_image

        if self._guiFlag == 1:
            cv.imshow(self._window_name, display_image)
            key = cv.waitKey(1)

        if self._debugFlag == 1:
            self.logger.debug("Found: " + str(found_ic))

        return found_ic

    def get_current_state(self, timeout: float = 5, ref=None, area=(0, 64, 0, 128)):
        if ref is None:
            ref = []
        self.update_screen()
        state = self.recog_ref(ref, area)
        start_time = time.time()
        while len(state) == 0:
            self.update_screen()
            state = self.recog_ref(ref, area)
            if time.time() - start_time > timeout:
                self.logger.error("Recognition timeout! Screenshot will be saved!")
                cv.imwrite("img/recognitionFailPic.bmp", self.screen_image)
                # raise FlippigatorException("Recognition timeout")
                break
        return state

    def wait_for_state(self, state, timeout=5) -> int:
        start_time = time.time()
        while start_time + timeout > time.time():
            cur_state = self.get_current_state()
            if state in cur_state:
                return 0
        return -1

    def save_screen(self, filename: str):
        self.logger.info("Save screenshot: " + str(filename))
        cv.imwrite(f"img/{filename}", self.screen_image)

    def press_down(self):
        self.proto.rpc_gui_send_input("SHORT DOWN")
        self.logger.debug("Press DOWN")

    def press_up(self):
        self.proto.rpc_gui_send_input("SHORT UP")
        self.logger.debug("Press UP")

    def press_left(self):
        self.proto.rpc_gui_send_input("SHORT LEFT")
        self.logger.debug("Press LEFT")

    def press_right(self):
        self.proto.rpc_gui_send_input("SHORT RIGHT")
        self.logger.debug("Press RIGHT")

    def press_ok(self):
        self.proto.rpc_gui_send_input("SHORT OK")
        self.logger.debug("Press OK")

    def press_long_ok(self):
        self.proto.rpc_gui_send_input("LONG OK")
        self.logger.debug("Press LONG OK")

    def press_back(self):
        self.proto.rpc_gui_send_input("SHORT BACK")
        self.logger.debug("Press BACK")

    def press(self, duration: str = "SHORT", button: str = "OK"):
        if duration not in ["SHORT", "LONG"]:
            raise FlippigatorException("Invalid duration")
        if button not in ["OK", "BACK", "UP", "DOWN", "LEFT", "RIGHT"]:
            raise FlippigatorException("Invalid button")
        self.proto.rpc_gui_send_input(f"{duration} {button}")
        self.logger.debug("Press " + button)

    def get_menu_list(self):
        time.sleep(0.2)
        self.logger.info("Scanning menu list")
        menus = list()
        self.get_current_state(timeout=1)

        while True:
            cur = self.get_current_state(timeout=1)
            if not (cur == []):
                if cur[0] in menus:
                    break
                else:
                    menus.append(cur[0])
            else:
                menus.append("Missed template!!")
                self.logger.warning("Found undescribed item")
            self.press_down()

        self.logger.info("Found menus: " + str(menus))
        return menus

    def get_first_item(self, browser: Optional[bool] = False):
        """
        Get first item in menu
        browser: if True, will skip first item (folder UP icon"
        """
        time.sleep(0.2)
        menus = list()
        cur = self.get_current_state()

        if browser:
            if not (cur == []):
                self.press_down()
                cur = self.get_current_state()
                menus.append(cur[0])

        return menus

    def go_to(self, target, area=(0, 64, 0, 128), direction: Optional[str] = "down"):
        state = self.get_current_state(area=area)
        self.logger.info("Going to " + target)

        while not (target in state):
            if direction == "down":
                self.press_down()
            elif direction == "up":
                self.press_up()
            state = self.get_current_state(area=area)

    def open(self, target, area=(0, 64, 0, 128), direction: Optional[str] = "down"):
        self.go_to(target, area, direction)
        self.press_ok()

    def go_to_main_screen(self):
        self.press_back()
        self.press_back()
        self.press_back()
        time.sleep(0.1)  # try to fix freeze while emilating Mfc1K
        state = self.get_current_state()
        while not ("SDcardIcon" in state):
            state = self.get_current_state()
            if "ExitLeft" in state:
                self.press_left()
            else:
                self.press_back()

        time.sleep(1)  # wait for some time, because of missing key pressing

    def open_file(self, module, filename):
        self.logger.info("Opening file '" + filename + "' in module '" + module + "'")
        self.go_to_main_screen()
        time.sleep(1)
        self.press_down()
        time.sleep(0.4)  # some waste "heads" going at the opening of browser

        heads = list()
        start_time = time.time()

        while start_time + 10 > time.time():
            cur = self.get_current_state(area=(0, 16, 0, 128))
            if not cur == []:
                if cur[0] == ("browser_head_" + module):
                    break
                if cur[0] in heads:
                    self.logger.warning("Module not found!")
                    return -1
                heads.append(cur[0])
                self.press_right()
        if start_time + 10 <= time.time():
            self.logger.warning("Module not found! (timeout)")
            return -1

        files = list()
        start_time = time.time()

        while start_time + 10 > time.time():
            state = self.get_current_state(timeout=0.5, area=(15, 64, 0, 128))

            if not (state == []):
                if state[0] in files:
                    self.logger.warning("File not found!")
                    return -1
                if state[0] == "browser_" + filename:
                    break
                files.append(state[0])
            self.press_down()
        if start_time + 10 <= time.time():
            self.logger.warning("File not found! (timeout)")
            return -1
        self.press_ok()
        self.go_to("browser_Run in app", area=(15, 64, 0, 128))
        self.press_ok()
        self.logger.info("File opened")

    def delete_file(self, module, filename):
        self.logger.info("Deleting file '" + filename + "' in module '" + module + "'")
        self.go_to_main_screen()
        time.sleep(1)
        self.press_down()
        time.sleep(0.4)

        heads = list()
        start_time = time.time()

        while start_time + 10 > time.time():
            cur = self.get_current_state(area=(0, 16, 0, 128))

            if not cur == []:
                if cur[0] == ("browser_head_" + module):
                    break
                if cur[0] in heads:
                    self.logger.warning("Module not found!")
                    return -1
                heads.append(cur[0])
                self.press_right()
            if start_time + 10 <= time.time():
                self.logger.warning("Module not found! (timeout)")
                return -1

        files = list()
        start_time = time.time()

        while start_time + 10 > time.time():
            state = self.get_current_state(timeout=0.5, area=(15, 64, 0, 128))

            if not (state == []):
                if state[0] in files:
                    self.logger.warning("File not found!")
                    return -1
                if state[0] == "browser_" + filename:
                    break
                files.append(state[0])
            self.press_down()
        if start_time + 10 <= time.time():
            self.logger.warning("File not found! (timeout)")
            return -1

        self.press_ok()
        self.go_to("browser_Delete", area=(15, 64, 0, 128))
        self.press_ok()
        self.press_right()
        self.logger.info("File deleted")
        self.go_to_main_screen()


class Gator:
    def __init__(
        self,
        serial,
        x_size,
        y_size,
        debug: bool = True,
    ):
        self._serial = serial
        self._x_size = x_size
        self._y_size = y_size
        self._debugFlag = debug
        self.logger = logging.getLogger("Gator")
        if self._debugFlag:
            self.logger.setLevel(logging.DEBUG)

    def __del__(self):
        pass

    def home(self):
        self._serial.reset_output_buffer()
        self._serial.reset_input_buffer()
        time.sleep(0.2)
        self._serial.write(("$H\n").encode("ASCII"))
        status = self._serial.readline()
        self.logger.info("Homing in progress")
        while status.decode("ASCII").find("ok") == -1:
            status = self._serial.readline()
            time.sleep(0.2)

    def transform(self, x, y, speed=3000):
        return (-x - self._x_size, -y - self._y_size, speed)

    def swim_to(self, x, y, speed=3000):
        self._serial.reset_output_buffer()
        self._serial.write(
            ("$J = X" + str(x) + " Y" + str(y) + " F" + str(speed) + "\n").encode(
                "ASCII"
            )
        )
        if self._debugFlag == True:
            self.logger.info("Moving to X" + str(x) + " Y" + str(y) + " F" + str(speed))
        time.sleep(0.2)
        self._serial.reset_input_buffer()
        self._serial.write(("?\n").encode("ASCII"))
        status = self._serial.readline()
        while status.decode("ASCII").find("Idle") == -1:
            self.logger.debug("Moving in process")
            self._serial.reset_input_buffer()
            self._serial.write(("?\n").encode("ASCII"))
            status = self._serial.readline()
            time.sleep(0.2)


class Reader:
    def __init__(
        self,
        serial,
        gator,
        x_coord,
        y_coord,
        debug: bool = True,
    ):
        self._serial = serial
        self._gator = gator
        self._x_coord = x_coord
        self._y_coord = y_coord
        self._debugFlag = debug

        self._recieved_data = 0
        self.logger = logging.getLogger("Reader")
        if self._debugFlag:
            self.logger.setLevel(logging.DEBUG)

    def __del__(self):
        pass

    def go_to_place(self) -> None:
        self.logger.info("Moving to reader")
        self._gator.swim_to(self._x_coord, self._y_coord, 15000)

    def is_available(self) -> bool:
        if self._recieved_data:
            return True
        else:
            return False

    def update(self) -> bool:
        line = self._serial.readline()
        if len(line):
            self._recieved_data = line.decode("utf-8")
            return True
        else:
            return False

    def clear(self) -> None:
        self._serial.flushInput()
        self._recieved_data = 0

    def get(self) -> str:
        self.logger.info("Returened data: " + str(self._recieved_data))
        return self._recieved_data


class Relay:
    def __init__(
        self,
        serial,
        debug: bool = True,
    ):
        self._serial = serial
        self._debugFlag = debug
        self._recieved_data = 0
        self._curent_reader = 0
        self._curent_key = 0
        self._serial.reset_output_buffer()

        self._serial.write(("R0K0\n").encode("ASCII"))
        self.logger = logging.getLogger("Relay")
        if self._debugFlag:
            self.logger.setLevel(logging.DEBUG)

    def __del__(self):
        pass

    def set_reader(self, reader):
        self._serial.write(
            ("R" + str(reader) + "K" + str(self._curent_key) + "\n").encode("ASCII")
        )
        self._curent_reader = reader
        self.logger.info("Selected reader: " + str(self._curent_reader))

    def set_key(self, key):
        self._serial.write(
            ("R" + str(self._curent_reader) + "K" + str(key) + "\n").encode("ASCII")
        )
        self._curent_key = key
        self.logger.info("Selected key: " + str(self._curent_key))

    def get_reader(self) -> int:
        return self._curent_reader

    def get_key(self) -> int:
        return self._curent_key

    def reset(self):
        self._serial.write(("R0K0\n").encode("ASCII"))
        self._curent_reader = 0
        self._curent_key = 0
        self.logger.info("Reset relay module")


class FlipperTextKeyboard:
    def __init__(self, nav) -> None:
        self.nav = nav
        self.keeb = []
        self.keeb.append(list("qwertyuiop0123"))
        self.keeb.append(list("asdfghjkl\0\b456"))
        self.keeb.append(list("zxcvbnm_\0\0\n789"))

    def _coord(self, letter):
        col_index = -1
        row_index = -1
        for row in self.keeb:
            try:
                row_index = row_index + 1
                col_index = row.index(letter)
                break
            except:
                ValueError: None

        if col_index < 0:
            return None
        else:
            return (col_index, row_index)

    def send(self, text):
        self.nav.logger.info("Printing on Text keyboard: " + text)
        current_x, current_y = self._coord("\n")
        text = text + "\n"
        for letter in list(text):
            if letter != "\b" and letter != "\n":
                keeb_letter = str.lower(letter)
            else:
                keeb_letter = letter

            new_x, new_y = self._coord(keeb_letter)
            step_x, step_y = new_x - current_x, new_y - current_y

            dir_left = step_x < 0
            dir_up = step_y < 0

            step_x, step_y = abs(step_x), abs(step_y)

            for _ in range(step_y):
                if dir_up:
                    current_y -= 1
                    self.nav.press_up()
                    if self.keeb[current_y][current_x] == "\0":
                        step_x += 1
                else:
                    current_y += 1
                    self.nav.press_down()
                    if self.keeb[current_y][current_x] == "\0":
                        step_x -= 1

            for _ in range(step_x):
                if dir_left:
                    current_x -= 1
                    if self.keeb[current_y][current_x] != "\0":
                        self.nav.press_left()
                else:
                    current_x += 1
                    if self.keeb[current_y][current_x] != "\0":
                        self.nav.press_right()

            if letter.isupper():
                self.nav.press_long_ok()
            else:
                self.nav.press_ok()


class FlipperHEXKeyboard:
    def __init__(self, nav) -> None:
        self.nav = nav
        self.keeb = []
        self.keeb.append(list("01234567\b"))
        self.keeb.append(list("89abcdef\n"))

    def _coord(self, letter):
        col_index = -1
        row_index = -1
        for row in self.keeb:
            try:
                row_index = row_index + 1
                col_index = row.index(letter)
                break
            except:
                ValueError: None

        if col_index < 0:
            return None
        else:
            return (col_index, row_index)

    def send(self, text):
        self.nav.logger.info("Printing on HEX keyboard: " + text)
        current_x, current_y = self._coord("0")
        text = text + "\n"
        for letter in list(text):
            if letter != "\b" and letter != "\n":
                keeb_letter = str.lower(letter)
            else:
                keeb_letter = letter

            new_x, new_y = self._coord(keeb_letter)
            step_x, step_y = new_x - current_x, new_y - current_y

            dir_left = step_x < 0
            dir_up = step_y < 0

            step_x, step_y = abs(step_x), abs(step_y)

            for _ in range(step_y):
                if dir_up:
                    current_y -= 1
                    self.nav.press_up()
                else:
                    current_y += 1
                    self.nav.press_down()

            for _ in range(step_x):
                if dir_left:
                    current_x -= 1
                    self.nav.press_left()
                else:
                    current_x += 1
                    self.nav.press_right()

            self.nav.press_ok()


class FlippigatorException(Exception):
    pass


class FRecognitionException(FlippigatorException):
    pass
