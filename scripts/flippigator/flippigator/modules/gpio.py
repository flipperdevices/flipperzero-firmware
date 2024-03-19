from flippigator.app import App


class AppGpio(App):
    def __init__(self, navi):
        super().__init__(navi, "GPIO")
