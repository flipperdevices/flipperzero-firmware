from flippigator.app import App
from termcolor import colored


class AppNfc(App):
    def __init__(self, navi):
        super().__init__(navi, "NFC")

    def check_menu(self):
        menu = self._navi.get_menu_list()
        menu_ref = [
            "Read",
            "Detect R(up)eader",
            "Saved",
            "Extra Actions",
            "Add Manually",
            "Debug",
        ]
        print(menu)
        print(menu_ref)
        for i in menu:
            if i in menu_ref:
                menu_ref.remove(i)
        if len(menu_ref) == 0:
            print(colored("NFC menu is completed", "green"))
        else:
            print(colored("Error in NFC menu list", "red"))
        return len(menu_ref)
