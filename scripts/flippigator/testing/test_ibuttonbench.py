import os
import time

import allure
import pytest
from flippigator.case import BaseCase
from termcolor import colored
from flippigator.flippigator import FlipperTextKeyboard, FlipperHEXKeyboard

os.system("color")


@pytest.mark.bench_ibutton_ir
class TestIbuttonBench(BaseCase):
    def test_read_cyfral(self, nav_reader, nav_key, relay):
        pass