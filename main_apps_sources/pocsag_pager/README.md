# Flipper POCSAG Receiver plugin

App can capture POCSAG 1200 messages using internal or external CC1101 module

Plugin based on Weather Station - https://github.com/flipperdevices/flipperzero-firmware/tree/dev/applications/plugins/weather_station

Protocol implementation made by @Shmuma
Icons by @Svaarich

Includes new FM preset built into code - 2FSK with 9.5KHz freq deviation.

Default frequency is set to DAPNET - 439987500

To add new presets and frequencies create file - yourMicroSD/pocsag/settings.txt
And put THIS - https://github.com/flipperdevices/flipperzero-firmware/blob/dev/assets/resources/subghz/assets/setting_user.example file contents into it, and edit this example for yourself, add needed frequencies


All contributions are welcome!
