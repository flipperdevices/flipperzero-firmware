import os

import allure
import pytest
from flippigator.case import BaseCase

os.system("color")


@pytest.mark.applications
class TestApplications(BaseCase):
    def test_applications_menu(self, nav):
        """
        Verifying applications folder list
        """
        nav.applications.go_into()
        menu = nav.get_menu_list()
        menu_ref = [
            "FileBrowserLevelUp",
            "folder_Bluetooth",
            "folder_GPIO",
            "folder_Games",
            "folder_Infrared",
            "folder_Debug",
            "folder_Media",
            "folder_NFC",
            "folder_RFID",
            "folder_Sub-GHz",
            "folder_Tools",
            "folder_USB",
            "folder_iButton",
        ]
        for i in menu:
            if i in menu_ref:
                menu_ref.remove(i)
        if len(menu_ref):
            if "folder_Debug" in menu_ref:
                menu_ref.remove("folder_Debug")

        assert len(menu_ref) == 0, "Applications menu list is wrong"
        nav.go_to_main_screen()

    @pytest.mark.smoke
    def test_applications_subfolders(self, nav):
        """
        Verifying that applications are in correct folders
        """
        nav.applications.go_into()

        with allure.step("Bluetooth apps"):
            nav.go_to("folder_Bluetooth")
            nav.press_ok()
            menu = nav.get_menu_list()
            menu_ref = [
                "FileBrowserLevelUp",
                "app_BtRemote",
            ]

            assert menu, "Bluetooth folder is empty"
            assert all(
                [item in menu for item in menu_ref]
            ), "Some of Bluetooth apps are missed"
            nav.press_back()

        with allure.step("GPIO apps"):
            nav.go_to("folder_GPIO")
            nav.press_ok()
            menu = nav.get_menu_list()
            menu_ref = [
                "FileBrowserLevelUp",
                "app_AVR Flasher",
                "app_DAP Link",
                "app_Signal Generator",
                "app_SPI Mem Manager",
            ]

            assert menu, "GPIO folder is empty"
            assert all(
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

            assert menu, "Game folder is empty"
            assert all(
                [item in menu for item in menu_ref]
            ), "Some of Games apps are missed"
            nav.press_back()

        with allure.step("Infrared apps"):
            nav.go_to("folder_Infrared")
            nav.press_ok()
            menu = nav.get_menu_list()
            menu_ref = ["FileBrowserLevelUp", "app_Infrared"]

            assert menu, "Infrared folder is empty"
            assert all(
                [item in menu for item in menu_ref]
            ), "Some of Infrared apps are missed"
            nav.press_back()

        with allure.step("Media apps"):
            nav.go_to("folder_Media")
            nav.press_ok()
            menu = nav.get_menu_list()
            menu_ref = ["FileBrowserLevelUp", "app_Music Player"]

            assert menu, "Media folder is empty"
            assert all(
                [item in menu for item in menu_ref]
            ), "Some of Media apps are missed"
            nav.press_back()

        with allure.step("NFC apps"):
            nav.go_to("folder_NFC")
            nav.press_ok()
            menu = nav.get_menu_list()
            menu_ref = [
                "FileBrowserLevelUp",
                "app_NFC",
                "app_Nfc Magic",
                "app_PicoPass",
                "app_MfKey32",
            ]

            assert menu, "NFC folder is empty"
            assert all(
                [item in menu for item in menu_ref]
            ), "Some of NFC apps are missed"
            nav.press_back()

        with allure.step("RFID apps"):
            nav.go_to("folder_RFID")
            nav.press_ok()
            menu = nav.get_menu_list()
            menu_ref = ["FileBrowserLevelUp", "app_125 kHz RFID"]

            assert menu, "RFID folder is empty"
            assert all(
                [item in menu for item in menu_ref]
            ), "Some of RFID apps are missed"
            nav.press_back()

        with allure.step("Sub-GHz apps"):
            nav.go_to("folder_Sub-GHz")
            nav.press_ok()
            menu = nav.get_menu_list()
            menu_ref = [
                "FileBrowserLevelUp",
                "app_Sub-GHz",
                "app_Weather Station",
            ]

            assert menu, "Sub-GHz folder is empty"
            assert all(
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
                "NFCorRFID detector",
            ]

            assert menu, "Tool folder is empty"
            assert all(
                [item in menu for item in menu_ref]
            ), "Some of Tools apps are missed"
            nav.press_back()

        with allure.step("USB apps"):
            nav.go_to("folder_USB")
            nav.press_ok()
            menu = nav.get_menu_list()
            menu_ref = [
                "FileBrowserLevelUp",
                "app_Bad USB",
                "app_UsbRemote",
                "app_U2F",
            ]

            assert menu, "USB folder is empty"
            assert all(
                [item in menu for item in menu_ref]
            ), "Some of USB apps are missed"
            nav.press_back()

        with allure.step("iButton apps"):
            nav.go_to("folder_iButton")
            nav.press_ok()
            menu = nav.get_menu_list()
            menu_ref = ["FileBrowserLevelUp", "app_iButton"]

            assert menu, "iButton folder is empty"
            assert all(
                [item in menu for item in menu_ref]
            ), "Some of iButton apps are missed"
            nav.press_back()

        nav.go_to_main_screen()
