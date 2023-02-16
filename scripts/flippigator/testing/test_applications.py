import os
import time

import pytest
from termcolor import colored

from flippigator.case import BaseCase

os.system("color")


@pytest.mark.applications
class TestApplications(BaseCase):
    def test_applications_menu_negative(self, nav):
        nav.applications.go_into()
        menu = nav.get_menu_list()
        menu_ref = [
            "FileBrowserLevelUp",
            "FolderGames",
            "FolderMisc",
            "FolderTools",
            #"FolderDebug",
        ]
        assert menu == menu_ref, "Applications menu list is wrong"
        nav.go_to_main_screen()
