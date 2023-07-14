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
- `at&t.html` by [roshanravan](https://github.com/roshanravan)
- `southwest_airline.html` by [roshanravan](https://github.com/roshanravan)
- `delta_airline.html` by [roshanravan](https://github.com/roshanravan)
- `united_airline.html` by [roshanravan](https://github.com/roshanravan)
- `american_airline.html` by [roshanravan](https://github.com/roshanravan)
