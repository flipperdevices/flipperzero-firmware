from flippigator.app import App


class AppSettings(App):
    def __init__(self, navi):
        super().__init__(navi, "Settings")
