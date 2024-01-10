from flippigator.app import App


class AppBadusb(App):
    def __init__(self, navi):
        super().__init__(navi, "Bad USB")
