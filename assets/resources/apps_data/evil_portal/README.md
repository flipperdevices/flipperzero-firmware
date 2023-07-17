# Community Portals

Do you have a great portal that you would like to share? Please submit a pull request placing your portal in this folder. It'd be great to see what the community can come up with! Please also add yourself to this readme so your contribution is recognized.

## Limitations

The ESP32 access point will not have internet access while hosting the portal, as a result there cannot be any requests for stylesheets or javascript such as CDNs for bootstrap and JQuery.

All HTML/CSS/JS must be in a single HTML file. This is due to the fact that the index.html kept in the memory of the esp32.

There is a 20k character limit for each HTML file.

The form data must be sent to the `/get` endpoint as a GET request with the params `email` & `password`. You can put any information you want in these two fields. For example the `email` param can contain a username instead, just keep the param name as `email`.

Please check the example `index.html` to get an idea of what this has to look like. 


## Contributors

Thank you so much to the following contributors for providing awesome portals. 

- `Google_Modern.html` by [roshanravan](https://github.com/roshanravan)
- `Twitter.html` by [roshanravan](https://github.com/roshanravan)
- `Facebook.html` by [roshanravan](https://github.com/roshanravan)
- `CoxWifi.html` by [qqmajikpp](https://github.com/qqmajikpp)
- `Starlink.html` by [roshanravan](https://github.com/roshanravan)
- `Spectrum.html` by [roshanravan](https://github.com/roshanravan)
- `T_Mobile.html` by [roshanravan](https://github.com/roshanravan)
- `Verizon.html` by [roshanravan](https://github.com/roshanravan)
- `AT&T.html` by [roshanravan](https://github.com/roshanravan)
- `Southwest_Airlines.html` by [roshanravan](https://github.com/roshanravan)
- `Delta_Airlines.html` by [roshanravan](https://github.com/roshanravan)
- `United_Airlines.html` by [roshanravan](https://github.com/roshanravan)
- `American_Airlines.html` by [roshanravan](https://github.com/roshanravan)
- `Jet_Blue.html` by [roshanravan](https://github.com/roshanravan)
- `Google_Mobile.html` by [breaching](https://github.com/breaching)
- `Microsoft.html` by [roshanravan](https://github.com/roshanravan)
- `Twitch.html` by [roshanravan](https://github.com/roshanravan)
- `PornHub.html` by [roshanravan](https://github.com/roshanravan)
- `Alaska_Airlines.html` by [roshanravan](https://github.com/roshanravan)
- `Amazon.html` by [roshanravan](https://github.com/roshanravan)
- `Apple.html` by [Jules](https://github.com/jules0835)
- `FakeHack.html` by [roshanravan](https://github.com/roshanravan)
- `FakeHack2.html` by [roshanravan](https://github.com/roshanravan)
- `Prank_Game.html` by [roshanravan](https://github.com/roshanravan)
- `Matrix.html` by [roshanravan](https://github.com/roshanravan)
- `SpiritAirlines.html` by [roshanravan](https://github.com/roshanravan)