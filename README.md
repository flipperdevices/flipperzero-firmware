[![GitHub release](https://img.shields.io/github/release/teeebor/flipper_blackjack?include_prereleases=&sort=semver&color=blue)](https://github.com/teeebor/flipper_blackjack/releases/)
[![License](https://img.shields.io/badge/License-MIT-blue)](/LICENSE)
[![issues - flipper-zero_authenticator](https://img.shields.io/github/issues/teeebor/flipper_blackjack)](https://github.com/teeebor/flipper_blackjack/issues)
![maintained - yes](https://img.shields.io/badge/maintained-yes-blue)
![contributions - welcome](https://img.shields.io/badge/contributions-welcome-blue)
# Blackjack for Flipper Zero


![Play screen](screenshots/blackjack.gif)

## Building
> The app should be compatible with the official and custom flipper firmwares. If not, follow these steps to build it
> yourself
* Download your firmware's source code
* Clone the repository recursively `git clone REPO_URL --recursive` into the firmware's applications_user folder
* Navigate into the firmwares root folder
* Make sure you can use
  the [Fipper build tool](https://github.com/flipperdevices/flipperzero-firmware/blob/dev/documentation/fbt.md)
* To build the project, type this into your console:
  #### Linux
  > ./fbt fap_{APP_NAME}
  #### Windows
  > fbt.cmd fap_{APP_NAME}
* the finished build will be in the following location, copy this into your SD card:
  > build\f7-firmware-D\.extapps\blackjack.fap
