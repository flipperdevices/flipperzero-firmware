import os
import time

import allure
import pytest
from flippigator.case import BaseCase
from termcolor import colored

os.system("color")


@pytest.mark.applications
class TestApplications(BaseCase):
    def test_applications_menu_negative(self, nav):
        nav.applications.go_into()
        menu = nav.get_menu_list()
        menu_ref = [
            "FileBrowserLevelUp",
            "folder_Bluetooth",
            "folder_GPIO",
            "folder_Games",
            "folder_Debug",
            "folder_Media",
            "folder_NFC",
            "folder_Sub-GHz",
            "folder_Tools",
            "folder_USB",
        ]
        for i in menu:
            if i in menu_ref:
                menu_ref.remove(i)
        if len(menu_ref):
            if "folder_Debug" in menu_ref:
                menu_ref.remove("folder_Debug")
        assert len(menu_ref) == 0, "Applications menu list is wrong"
        nav.go_to_main_screen()

    def test_aplications_subfolders(self, nav):
        nav.applications.go_into()

        with allure.step("Bluetooth apps"):
            nav.go_to("folder_Bluetooth")
            nav.press_ok()
            menu = nav.get_menu_list()
            menu_ref = [
                "FileBrowserLevelUp",
                "app_BtRemote",
            ]

            assert any(
                [item in menu for item in menu_ref]
            ), "Some of Bluetooth apps are missed"
            nav.press_back()

        with allure.step("GPIO apps"):
            nav.go_to("folder_GPIO")
            nav.press_ok()
            menu = nav.get_menu_list()
            menu_ref = [
                "FileBrowserLevelUp",
                "app_DAP Link",
                "app_Signal Generator",
                "app_SPI Mem Manager",
            ]

            assert any(
                [item in menu for item in menu_ref]
            ), "Some of GPIO apps are missed"
            nav.press_back()

        with allure.step("Game apps"):
            nav.go_to("folder_Games")
            nav.press_ok()
            menu = nav.get_menu_list()
            menu_ref = [
                "FileBrowserLevelUp",
                "app_Snake Game",
            ]

            assert any(
                [item in menu for item in menu_ref]
            ), "Some of Games apps are missed"
            nav.press_back()

        with allure.step("Media apps"):
            nav.go_to("folder_Media")
            nav.press_ok()
            menu = nav.get_menu_list()
            menu_ref = ["FileBrowserLevelUp", "app_Music Player"]

            assert any(
                [item in menu for item in menu_ref]
            ), "Some of Media apps are missed"
            nav.press_back()

        with allure.step("NFC apps"):
            nav.go_to("folder_NFC")
            nav.press_ok()
            menu = nav.get_menu_list()
            menu_ref = [
                "FileBrowserLevelUp",
                "app_Nfc Magic",
                "app_PicoPass",
            ]

            assert any(
                [item in menu for item in menu_ref]
            ), "Some of NFC apps are missed"
            nav.press_back()

        with allure.step("Sub-GHz apps"):
            nav.go_to("folder_Sub-GHz")
            nav.press_ok()
            menu = nav.get_menu_list()
            menu_ref = [
                "FileBrowserLevelUp",
                "app_Weather Station",
            ]

            assert any(
                [item in menu for item in menu_ref]
            ), "Some of Sub-GHz apps are missed"
            nav.press_back()

        with allure.step("Tool apps"):
            nav.go_to("folder_Tools")
            nav.press_ok()
            menu = nav.get_menu_list()
            menu_ref = [
                "FileBrowserLevelUp",
                "app_Clock",
            ]

            assert any(
                [item in menu for item in menu_ref]
            ), "Some of Tools apps are missed"
            nav.press_back()

        with allure.step("USB apps"):
            nav.go_to("folder_USB")
            nav.press_ok()
            menu = nav.get_menu_list()
            menu_ref = [
                "FileBrowserLevelUp",
                "app_UsbRemote",
            ]

            assert any(
                [item in menu for item in menu_ref]
            ), "Some of USB apps are missed"
            nav.press_back()

        nav.go_to_main_screen()
