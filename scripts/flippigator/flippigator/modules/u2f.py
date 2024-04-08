from flippigator.app import App


class AppU2f(App):
    def __init__(self, navi):
        super().__init__(navi, "U2F")
