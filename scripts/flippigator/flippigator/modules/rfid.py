from flippigator.app import App


class AppRfid(App):
    def __init__(self, navi):
        super().__init__(navi, "125 kHz RFID")
