#!/usr/bin/env python3
import logging
import os
import time

import cv2 as cv
import numpy
from termcolor import colored

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

    def recog_ref(self, ref = []):
        # self.updateScreen()
        temp_pic_list = list()
        screen_image = self.get_raw_screen()
        if len(ref) == 0:
            ref = list(self.imRef.keys())
        for im in ref:
            template = cv.cvtColor(self.imRef.get(im), 0)
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

    def get_current_state(self, timeout=3, ref = []):
        self.update_screen()
        state = self.recog_ref(ref)
        start_time = time.time()
        while len(state) == 0:
            self.update_screen()
            state = self.recog_ref()
            if time.time() - start_time > timeout:
                cv.imwrite("img/recognitionFailPic.bmp", self.screen_image)
                raise FlippigatorException("Recognition timeout")
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
            for i in cur:
                menus.append(i)
            self.press_down()
            cur = self.get_current_state()

        return menus

    def go_to(self, target):
        state = self.get_current_state()
        while not (target in state):
            self.press_down()
            state = self.get_current_state()

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

    def transform(self, x, y, speed = 3000):
        return (-x-self._x_size, -y-self._y_size, speed)


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


class FlippigatorException(Exception):
    pass


class FRecognitionException(FlippigatorException):
    pass
