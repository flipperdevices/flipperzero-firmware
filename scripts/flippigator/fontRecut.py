#!/usr/bin/env python3

import json
import os
import sys

import cv2 as cv
import numpy
import serial
from flipperzero_protobuf_py.flipperzero_protobuf.cli_helpers import (
    print_hex,
    print_screen,
)
from flipperzero_protobuf_py.flipperzero_protobuf.flipper_proto import FlipperProto
from termcolor import colored

# Lists to store the bounding box coordinates for crop
top_left_corner = []
bottom_right_corner = []
imgToCrop = []
imgToCropOrig = []
# previous version of flipperzero_protobuf_py
# from flipperzero_protobuf_py.cli_helpers import print_hex, print_screen
# from flipperzero_protobuf_py.flipper_protobuf import ProtoFlipper

os.system("color")

APPLY_TEMPLATE_THRESHOLD = 0.999
SCREEN_H = 128
SCREEN_W = 64
SCALE = 12
newTemplates = 0

imRef = dict()

for filename in os.listdir("./font_HaxrCorp_4089/"):
    f = cv.imread("./font_HaxrCorp_4089/" + str(filename), 0)
    imRef.update({filename[:-4]: f})
    print(colored("Added resource: " + filename, "green"))

for filename in os.listdir("./img/"):
    if filename[0:11] == "newTemplate":
        os.remove("./img/" + filename)
        print(colored("Removed old template: " + filename, "red"))


def default_mouse(action, x, y, flags, *userdata):
    pass


def select_roi(action, x, y, flags, *userdata):
    global top_left_corner, bottom_right_corner, imgToCrop, imgToCropOrig, newTemplates

    x = int(x / SCALE) * SCALE
    y = int(y / SCALE) * SCALE

    if action == cv.EVENT_LBUTTONDOWN:
        top_left_corner = [(x, y)]
    elif action == cv.EVENT_MOUSEMOVE and len(top_left_corner) == 1:
        bottom_right_corner = [(x, y)]
        tempImg = imgToCrop.copy()
        cv.rectangle(
            tempImg, top_left_corner[0], bottom_right_corner[0], (0, 0, 255), 2, 8
        )
        cv.imshow("Crop", tempImg)
    elif action == cv.EVENT_MOUSEMOVE and len(top_left_corner) == 0:
        tempImg = imgToCrop.copy()
        tempImg = cv.line(tempImg, (0, y), (SCREEN_H * SCALE, y), (0, 255, 0), 1)
        tempImg = cv.line(tempImg, (x, 0), (x, SCREEN_W * SCALE), (0, 255, 0), 1)
        cv.imshow("Crop", tempImg)
    elif action == cv.EVENT_LBUTTONUP:
        if top_left_corner[0][0] < bottom_right_corner[0][0]:
            w = (top_left_corner[0][0], bottom_right_corner[0][0])
        else:
            w = (bottom_right_corner[0][0], top_left_corner[0][0])
        if top_left_corner[0][1] < bottom_right_corner[0][1]:
            h = (top_left_corner[0][1], bottom_right_corner[0][1])
        else:
            h = (bottom_right_corner[0][1], top_left_corner[0][1])
        imgToCrop = imgToCrop[h[0] : h[1], w[0] : w[1]]
        imgToCropOrig = imgToCropOrig[
            int(h[0] / SCALE) : int(h[1] / SCALE), int(w[0] / SCALE) : int(w[1] / SCALE)
        ]
        top_left_corner = []
        bottom_right_corner = []
        cv.imshow("Crop", imgToCrop)
        cv.setMouseCallback("Flipper View", default_mouse)


def he_print_screen(screen_bytes):
    global imgToCrop, imgToCropOrig, newTemplates

    data = screen_bytes

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

    tempPicList = list()

    for im in list(imRef.keys()):
        # screen_image = cv.cvtColor(screen_image, 0)
        template = cv.cvtColor(imRef.get(im), 0)
        res = cv.matchTemplate(screen_image, template, cv.TM_CCOEFF_NORMED)
        min_val, max_val, min_loc, max_loc = cv.minMaxLoc(res)
        if max_val > APPLY_TEMPLATE_THRESHOLD:
            h, w, ch = template.shape
            tempPicList.append((str(im), max_loc, (max_loc[0] + w, max_loc[1] + h)))
        # print(str(im) + ": " + str(min_val) + ' ' + str(max_val) + ' ' + str(min_loc) + ' ' + str(max_loc))

    for wr in tempPicList:
        print(wr)
    print()

    display_image = screen_image.copy()
    mask = cv.inRange(display_image, (255, 255, 255, 0), (255, 255, 255, 0))
    display_image[mask > 0] = (1, 145, 251, 0)
    display_image = cv.resize(
        display_image, (128 * SCALE, 64 * SCALE), interpolation=cv.INTER_NEAREST
    )

    found_ic = list()

    for i in tempPicList:
        display_image = cv.rectangle(
            display_image,
            (i[1][0] * SCALE, i[1][1] * SCALE),
            (i[2][0] * SCALE, i[2][1] * SCALE),
            (255, 255, 0, 0),
            2,
        )
        display_image = cv.putText(
            display_image,
            i[0],
            (i[1][0] * SCALE, (i[2][1] + 2) * SCALE),
            cv.FONT_HERSHEY_SIMPLEX,
            0.5,
            (255, 255, 0, 0),
            1,
        )
        found_ic.append(i[0])

    cv.imshow("Flipper View", display_image)
    key = cv.waitKey(2)
    # if last key is shift in ascii
    if key == 225:
        key = cv.waitKey(1000)
        press = "LONG"
    else:
        press = "SHORT"
    if key == ord("q"):
        quit()
    elif key == ord("s"):
        cv.imwrite("img/saved1.jpg", screen_image)
        cv.imwrite("img/saved2.png", screen_image)
        cv.imwrite("img/saved3.bmp", screen_image)
        print("Saved")
    elif key == ord("c"):
        cv.namedWindow("Crop")
        imgToCrop = display_image.copy()
        imgToCropOrig = screen_image.copy()
        for i in range(SCREEN_W):
            start_point = (0, i * SCALE)
            end_point = (SCREEN_H * SCALE, i * SCALE)
            color = (0, 0, 0)
            thickness = 1
            imgToCrop = cv.line(imgToCrop, start_point, end_point, color, thickness)
        for i in range(SCREEN_H):
            start_point = (i * SCALE, 0)
            end_point = (i * SCALE, SCREEN_W * SCALE)
            color = (0, 0, 0)
            thickness = 1
            imgToCrop = cv.line(imgToCrop, start_point, end_point, color, thickness)
        cv.imshow("Crop", imgToCrop)
        cv.setMouseCallback("Crop", select_roi)
    elif key == ord("v"):
        cv.imwrite("img/newTemplate" + str(newTemplates) + ".bmp", imgToCropOrig)
        newTemplates = newTemplates + 1
        cv.destroyWindow("Crop")
        print(
            colored(
                "New template save in /img folder with name newTemplate"
                + str(newTemplates),
                "green",
            )
        )
    elif key == 81 or key == ord("a"):
        proto.rpc_gui_send_input(press + " LEFT")
        print(colored(f"Press {press} LEFT", "cyan"))
    elif key == 82 or key == ord("w"):
        proto.rpc_gui_send_input(press + " UP")
        print(colored(f"Press {press} UP", "cyan"))
    elif key == 83 or key == ord("d"):
        proto.rpc_gui_send_input(press + " RIGHT")
        print(colored(f"Press {press} RIGHT", "cyan"))
    elif key == 84 or key == ord("s"):
        proto.rpc_gui_send_input(press + " DOWN")
        print(colored(f"Press {press} DOWN", "cyan"))
    elif key == ord(" "):
        proto.rpc_gui_send_input(press + " OK")
        print(colored(f"Press {press} OK", "cyan"))
    elif key == 8:
        proto.rpc_gui_send_input(press + " BACK")
        print(colored(f"Press {press} BACK", "cyan"))
    print(key)
    return found_ic


def main():
    global proto

    state = list()

    # open serial port
    flipper = serial.Serial(sys.argv[1], timeout=1)
    flipper.baudrate = 230400
    flipper.flushOutput()
    flipper.flushInput()

    # disable timeout
    flipper.timeout = None

    # construct protobuf worker
    proto = FlipperProto(flipper, 1)
    print("Request RPC session")
    proto.start_rpc_session()
    print("RPC session started")

    while 1:
        screen = proto.rpc_gui_snapshot_screen()
        he_print_screen(screen)


if __name__ == "__main__":
    main()
