#!/usr/bin/env python3
import logging
import os
import time

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
from termcolor import colored

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
        scale: int = 12,
        threshold: float = 0.99,
    ):
        self.imRef = dict()
        self.proto = proto
        self._debugFlag = debug
        self._guiFlag = gui
        self._scale = scale
        self._threshold = threshold
        self.screen_image = numpy.zeros((SCREEN_H, SCREEN_W))

        self.logger = logging.getLogger("Navigator")

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

        for filename in os.listdir(path):
            f = cv.imread(path + str(filename), 0)
            self.imRef.update({filename[:-4]: f})
            if self._debugFlag == 1:
                print(colored("Added resource: " + filename, "green"))

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
            print(colored("Screen update", "cyan"))

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

    def draw_screen(self, window_name="Flipper screen"):
        display_image = self.get_screen()
        mask = cv.inRange(display_image, (255, 255, 255, 0), (255, 255, 255, 0))
        display_image[mask > 0] = (1, 145, 251, 0)
        display_image = cv.resize(
            display_image,
            (128 * self._scale, 64 * self._scale),
            interpolation=cv.INTER_NEAREST,
        )

        cv.imshow(window_name, display_image)
        key = cv.waitKey(1)

    def recog_ref(self, ref=[], area=(0 , 128, 0, 64)):
        # self.updateScreen()
        temp_pic_list = list()
        screen_image = self.get_raw_screen()[area[0]:area[1], area[2]:area[3]]
        if len(ref) == 0:
            ref = list(self.imRef.keys())
        for im in ref:
            template = cv.cvtColor(self.imRef.get(im), 0)
            if (template.shape[0] <= screen_image.shape[0]) and ((template.shape[1] <= screen_image.shape[1])):
                res = cv.matchTemplate(screen_image, template, cv.TM_CCOEFF_NORMED)
                min_val, max_val, min_loc, max_loc = cv.minMaxLoc(res)
                if max_val > self._threshold:
                    h, w, ch = template.shape
                    temp_pic_list.append(
                        (str(im), max_loc, (max_loc[0] + w, max_loc[1] + h))
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
            cv.imshow("Debug recog screen", display_image)
            key = cv.waitKey(1)

        if self._debugFlag == 1:
            print(colored("Found: " + str(found_ic), "cyan"))

        return found_ic

    def get_current_state(self, timeout=5, ref=[], area=(0, 64, 0, 128)):
        self.update_screen()
        state = self.recog_ref(ref, area)
        start_time = time.time()
        while len(state) == 0:
            self.update_screen()
            state = self.recog_ref(ref, area)
            if time.time() - start_time > timeout:
                cv.imwrite("img/recognitionFailPic.bmp", self.screen_image)
                #raise FlippigatorException("Recognition timeout")
                break
        return state

    def save_screen(self, filename: str):
        cv.imwrite(f"img/{filename}", self.screen_image)

    def press_down(self):
        self.proto.rpc_gui_send_input("SHORT DOWN")
        if self._debugFlag == 1:
            print(colored("Press DOWN", "cyan"))

    def press_up(self):
        self.proto.rpc_gui_send_input("SHORT UP")
        if self._debugFlag == 1:
            print(colored("Press UP", "cyan"))

    def press_left(self):
        self.proto.rpc_gui_send_input("SHORT LEFT")
        if self._debugFlag == 1:
            print(colored("Press LEFT", "cyan"))

    def press_right(self):
        self.proto.rpc_gui_send_input("SHORT RIGHT")
        if self._debugFlag == 1:
            print(colored("Press RIGHT", "cyan"))

    def press_ok(self):
        self.proto.rpc_gui_send_input("SHORT OK")
        if self._debugFlag == 1:
            print(colored("Press OK", "cyan"))

    def press_long_ok(self):
        self.proto.rpc_gui_send_input("LONG OK")
        if self._debugFlag == 1:
            print(colored("Press LONG OK", "cyan"))

    def press_back(self):
        self.proto.rpc_gui_send_input("SHORT BACK")
        if self._debugFlag == 1:
            print(colored("Press BACK", "cyan"))

    def press(self, duration: str = "SHORT", button: str = "OK"):
        if duration not in ["SHORT", "LONG"]:
            raise FlippigatorException("Invalid duration")
        if button not in ["OK", "BACK", "UP", "DOWN", "LEFT", "RIGHT"]:
            raise FlippigatorException("Invalid button")
        self.proto.rpc_gui_send_input(f"{duration} {button}")
        if self._debugFlag:
            print(colored("Press " + button, "cyan"))

    def get_menu_list(self):
        time.sleep(0.2)
        menus = list()
        cur = self.get_current_state()

        while not (cur[0] in menus):
            if not (cur == []):
                menus.append(cur[0])
            self.press_down()
            cur = self.get_current_state()

        return menus

    def go_to(self, target, area = (0, 64, 0, 128)):
        state = self.get_current_state(area = area)
        while not (target in state):
            self.press_down()
            state = self.get_current_state(area = area)

    def go_to_main_screen(self):
        self.press_back()
        state = self.get_current_state()
        while not ("SDcardIcon" in state):
            state = self.get_current_state()
            if "ExitLeft" in state:
                self.press_left()
            else:
                self.press_back()
            if self._debugFlag == 1:
                print(colored("Going back to main screen", "cyan"))

    def open_file(self, module, filename):
        self.go_to_main_screen()
        self.press_down()

        heads = list()
        start_time = time.time()
        while start_time + 60 > time.time():
            cur = self.get_current_state(area = (0, 16, 0, 128))
            if not cur == []:
                if cur[0] == ("browser_head_" + module):
                    break
                if cur[0] in heads:
                    return -1
                heads.append(cur[0])
                self.press_right()

        files = list()
        start_time = time.time()
        while start_time + 60 > time.time():
            state = self.get_current_state(timeout = 0.5, area = (15, 64, 0, 128))
            if not (state == []):
                if state[0] in files:
                    return -1
                if state[0] == "browser_" + filename:
                    break
                files.append(state[0])
            self.press_down()
        self.press_ok()
        self.go_to("browser_Run in app", area = (15, 64, 0, 128))
        self.press_ok()

    def delete_file(self, module, filename):
        self.go_to_main_screen()
        self.press_down()

        heads = list()
        start_time = time.time()
        while start_time + 60 > time.time():
            cur = self.get_current_state(area = (0, 16, 0, 128))
            if not cur == []:
                if cur[0] == ("browser_head_" + module):
                    break
                if cur[0] in heads:
                    return -1
                heads.append(cur[0])
                self.press_right()

        files = list()
        start_time = time.time()
        while start_time + 60 > time.time():
            state = self.get_current_state(timeout = 0.5, area = (15, 64, 0, 128))
            if not (state == []):
                if state[0] in files:
                    return -1
                if state[0] == "browser_" + filename:
                    break
                files.append(state[0])
            self.press_down()
        self.press_ok()
        self.go_to("browser_Delete", area = (15, 64, 0, 128))
        self.press_ok()
        self.press_right()
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

    def __del__(self):
        pass

    def home(self):
        self._serial.reset_output_buffer()
        self._serial.reset_input_buffer()
        time.sleep(0.2)
        self._serial.write(("$H\n").encode("ASCII"))
        status = self._serial.readline()
        print(status)
        while status.decode("ASCII").find("ok") == -1:
            if self._debugFlag == True:
                print(colored("Homing in progress", "magenta"))
            status = self._serial.readline()
            print(status)
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
            print(
                colored(
                    "Moving to X" + str(x) + " Y" + str(y) + " F" + str(speed),
                    "magenta",
                )
            )
        time.sleep(0.2)
        self._serial.reset_input_buffer()
        self._serial.write(("?\n").encode("ASCII"))
        status = self._serial.readline()
        print(status)
        while status.decode("ASCII").find("Idle") == -1:
            if self._debugFlag == True:
                print(colored("Moving in process", "magenta"))
            self._serial.reset_input_buffer()
            self._serial.write(("?\n").encode("ASCII"))
            status = self._serial.readline()
            print(status)
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

    def __del__(self):
        pass

    def go_to_place(self) -> None:
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
        return self._recieved_data


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
