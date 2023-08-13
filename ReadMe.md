# Flipper Application Files (FAPs) for Any Firmware
## Compare your firmware API version with API version in releases to avoid compatibility issues

#### [Download .zip file by using this link, unpack, and copy only specific apps folders or specific apps onto your microSD into apps folder](https://github.com/xMasterX/all-the-plugins/releases/latest)


---

Apps contains changes needed to compile them on latest firmware, fixes has been done by @xMasterX


## Thank you to the authors!

The Flipper and its community wouldn't be as rich as it is without your contributions and support. Thank you for all you have done.

### Apps checked & updated at `5 Aug 13:51 GMT +3`


# Default pack

- **RFID Fuzzer** [(by @gid9798)](https://github.com/DarkFlippers/Multi_Fuzzer) (original by Ganapati & xMasterX)
- **iButton Fuzzer** [(by @gid9798)](https://github.com/DarkFlippers/Multi_Fuzzer) (original by xMasterX)
- **Sub-GHz bruteforcer** [(by @derskythe & xMasterX)](https://github.com/derskythe/flipperzero-subbrute) [(original by Ganapati & xMasterX)](https://github.com/DarkFlippers/unleashed-firmware/pull/57)
- **Sub-GHz playlist** [(by darmiel)](https://github.com/DarkFlippers/unleashed-firmware/pull/62)
- ESP8266 Deauther plugin [(by SequoiaSan)](https://github.com/SequoiaSan/FlipperZero-Wifi-ESP8266-Deauther-Module)
- WiFi Scanner plugin [(by SequoiaSan)](https://github.com/SequoiaSan/FlipperZero-WiFi-Scanner_Module)
- MultiConverter plugin [(by theisolinearchip)](https://github.com/theisolinearchip/flipperzero_stuff)
- WAV Player [(OFW: DrZlo13)](https://github.com/flipperdevices/flipperzero-firmware/tree/zlo/wav-player) - Fixed and improved by [LTVA1](https://github.com/LTVA1/wav_player) -> Also outputs audio on `PA6` - `3(A6)` pin
- Barcode Generator [(by Kingal1337)](https://github.com/Kingal1337/flipper-barcode-generator)
- GPIO: Sentry Safe plugin [(by H4ckd4ddy)](https://github.com/H4ckd4ddy/flipperzero-sentry-safe-plugin)
- ESP32: WiFi Marauder companion plugin [(by 0xchocolate)](https://github.com/0xchocolate/flipperzero-wifi-marauder) - Saving .pcap on flipper microSD [by tcpassos](https://github.com/tcpassos/flipperzero-firmware-with-wifi-marauder-companion) -> Only with custom marauder build (It is necessary to uncomment "#define WRITE_PACKETS_SERIAL" in configs.h (in marauder fw) and compile the firmware for the wifi board.) Or download precompiled build -> [Download esp32_marauder_ver_flipper_sd_serial.bin](https://github.com/justcallmekoko/ESP32Marauder/releases/latest)
- NRF24: Sniffer & MouseJacker (with changes) [(by mothball187)](https://github.com/mothball187/flipperzero-nrf24/tree/main/mousejacker)
- Simple Clock (timer by GMMan) [(original by CompaqDisc)](https://gist.github.com/CompaqDisc/4e329c501bd03c1e801849b81f48ea61)
- **Sub-GHz Remote** [(by @gid9798)](https://github.com/gid9798)
- Spectrum Analyzer (with changes) [(by jolcese)](https://github.com/jolcese/flipperzero-firmware/tree/spectrum/applications/spectrum_analyzer) - [Ultra Narrow mode & scan channels non-consecutively](https://github.com/theY4Kman/flipperzero-firmware/commits?author=theY4Kman)
- Metronome [(by panki27)](https://github.com/panki27/Metronome)
- DTMF Dolphin [(by litui)](https://github.com/litui/dtmf_dolphin)
- **TOTP (Authenticator)** [(by akopachov)](https://github.com/akopachov/flipper-zero_authenticator)
- GPS [(by ezod)](https://github.com/ezod/flipperzero-gps) works with module `NMEA 0183` via UART (13TX, 14RX, GND pins on Flipper)
- i2c Tools [(by NaejEL)](https://github.com/NaejEL/flipperzero-i2ctools) - C0 -> SCL / C1 -> SDA / GND -> GND | 3v3 logic levels only!
- HC-SR04 Distance sensor - Ported and modified by @xMasterX [(original by Sanqui)](https://github.com/Sanqui/flipperzero-firmware/tree/hc_sr04) - How to connect -> (5V -> VCC) / (GND -> GND) / (13|TX -> Trig) / (14|RX -> Echo)
- Morse Code [(by wh00hw)](https://github.com/wh00hw/MorseCodeFAP)
- **Unitemp - Temperature sensors reader** (DHT11/22, DS18B20, BMP280, HTU21x and more) [(by quen0n)](https://github.com/quen0n/unitemp-flipperzero)
- Lightmeter [(by oleksiikutuzov)](https://github.com/oleksiikutuzov/flipperzero-lightmeter)
- HEX Viewer [(by QtRoS)](https://github.com/QtRoS/flipper-zero-hex-viewer)
- POCSAG Pager [(by xMasterX & Shmuma)](https://github.com/xMasterX/flipper-pager)
- Text Viewer [(by kowalski7cc & kyhwana)](https://github.com/kowalski7cc/flipper-zero-text-viewer/tree/refactor-text-app)
- **UART Terminal** [(by cool4uma)](https://github.com/cool4uma/UART_Terminal/tree/main)
- **ProtoView** [(by antirez)](https://github.com/antirez/protoview)
- **SWD Probe** [(by g3gg0)](https://github.com/g3gg0/flipper-swd_probe)
- IR Scope [(by kallanreed)](https://github.com/DarkFlippers/unleashed-firmware/pull/407)
- **BadBT** plugin (BT version of BadKB) [(by Willy-JL, ClaraCrazy, XFW contributors)](https://github.com/ClaraCrazy/Flipper-Xtreme/tree/dev/applications/main/bad_kb) (See in Applications->Tools) - (aka BadUSB via Bluetooth)
- **Mifare Nested** [(by AloneLiberty)](https://github.com/AloneLiberty/FlipperNested) - Works with PC and python app `FlipperNested`
- **NFC Maker** plugin (make tags with URLs, Wifi and other things) [(by Willy-JL)](https://github.com/ClaraCrazy/Flipper-Xtreme/tree/dev/applications/external/nfc_maker) 
- ESP32-CAM -> Camera Suite [(by CodyTolene)](https://github.com/CodyTolene/Flipper-Zero-Camera-Suite)

Games:
- DOOM (ported and fixed by @xMasterX & @Svarich & @hedger) [(original by p4nic4ttack)](https://github.com/p4nic4ttack/doom-flipper-zero/)
- Zombiez [(Reworked By DevMilanIan)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/240) [(Original By Dooskington)](https://github.com/Dooskington/flipperzero-zombiez)
- Flappy Bird [(by DroomOne)](https://github.com/DroomOne/flipperzero-firmware/tree/dev/applications/flappy_bird)
- Arkanoid (refactored by xMasterX) [(by gotnull)](https://github.com/gotnull/flipperzero-firmware-wPlugins)
- Tic Tac Toe (refactored by xMasterX) [(by gotnull)](https://github.com/gotnull/flipperzero-firmware-wPlugins)
- Tetris (with fixes) [(by jeffplang)](https://github.com/jeffplang/flipperzero-firmware/tree/tetris_game/applications/tetris_game)
- Minesweeper [(by panki27)](https://github.com/panki27/minesweeper)
- Heap Defence (aka Stack Attack) - Ported to latest firmware by @xMasterX - [(original by wquinoa & Vedmein)](https://github.com/Vedmein/flipperzero-firmware/tree/hd/svisto-perdelki)
- Game15 [(by x27)](https://github.com/x27/flipperzero-game15)
- Solitaire [(by teeebor)](https://github.com/teeebor/flipper_games)
- BlackJack [(by teeebor)](https://github.com/teeebor/flipper_games)
- 2048 game [(by eugene-kirzhanov)](https://github.com/eugene-kirzhanov/flipper-zero-2048-game)
- Bomberduck [(by leo-need-more-coffee)](https://github.com/leo-need-more-coffee/flipperzero-bomberduck)
- JetPack Joyride [(by timstrasser)](https://github.com/timstrasser)


# Extra pack
## Bluetooth
- [BT Camera Trigger (By Nem0oo)](https://github.com/Nem0oo/flipper-zero-bluetooth-trigger)
- [BT Serial Example (By maybe-hello-world)](https://github.com/maybe-hello-world/fbs)

## Games
- [4 in row (By leo-need-more-coffee)](https://github.com/leo-need-more-coffee/flipperzero-4inrow)
- [Asteroids (By antirez & SimplyMinimal)](https://github.com/antirez/flipper-asteroids) - Modified by [SimplyMinimal](https://github.com/SimplyMinimal/FlipperZero-Asteroids) all changes can be found by clicking on [this link](https://github.com/SimplyMinimal/FlipperZero-Asteroids)
- [Chess Clock (By ihatecsv)](https://github.com/ihatecsv/flipper_chess_clock)
- [Chess (By xtruan)](https://github.com/xtruan/flipper-chess)
- [Color Guess (By leedave)](https://github.com/leedave/Leeds-Flipper-Zero-Applications)
- [Dice (By Ka3u6y6a)](https://github.com/Ka3u6y6a/flipper-zero-dice)
- [DVD Bounce (By schaene)](https://github.com/schaene/Flipper-DVD-Bounce)
- [Etch-A-Sketch (By SimplyMinimal)](https://github.com/SimplyMinimal/FlipperZero-Etch-A-Sketch)
- [Pong (By nmrr)](https://github.com/nmrr/flipperzero-pong) - Modified by [SimplyMinimal](https://github.com/SimplyMinimal/FlipperZero-Pong)
- [Game of Life (Updated to work by tgxn) (By itsyourbedtime)](https://github.com/tgxn/flipperzero-firmware/blob/dev/applications/game_of_life/game_of_life.c)
- [Mandelbrot Set (By Possibly-Matt)](https://github.com/Possibly-Matt/flipperzero-firmware-wPlugins)
- [Monty Hall (By DevMilanIan)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/203)
- [Nu Pogodi! (Ну, погоди!) (By sionyx)](https://github.com/sionyx/flipper_nupogodi)
- [Paint (By n-o-T-I-n-s-a-n-e)](https://github.com/n-o-T-I-n-s-a-n-e)
- [Race (By zyuhel)](https://github.com/zyuhel/flipperzero-racegame)
- [Reaction test Game (By Milk-Cool)](https://github.com/Milk-Cool/fz-reaction-game)
- [Reversi (By dimat)](https://github.com/dimat/flipperzero-reversi)
- [Root of Life (By Xorboo)](https://github.com/Xorboo/root-of-life)
- [Rubik's Cube Scrambler (By RaZeSloth)](https://github.com/RaZeSloth/flipperzero-rubiks-cube-scrambler)
- [SUPER-CHIP (Chip8) Emulator (By Milk-Cool)](https://github.com/Milk-Cool/fz-schip) <- WIP, please report found issues [here](https://github.com/Milk-Cool/fz-schip/issues)
- [Scorched Tanks (By jasniec)](https://github.com/jasniec/flipper-scorched-tanks-game)
- [Simon Says (By SimplyMinimal)](https://github.com/SimplyMinimal/FlipperZero-SimonSays)
- [Slots for Flipper Zero (By Daniel-dev-s)](https://github.com/Daniel-dev-s/flipperzero-slots)
- [Snake 2.0 (By Willzvul)](https://github.com/Willzvul/Snake_2.0)
- [T-rex game (By Rrycbarm)](https://github.com/Rrycbarm/t-rex-runner)
- [TAMA P1 (By GMMan & DroomOne)](https://github.com/GMMan/flipperzero-tamagotch-p1) requires [this rom](https://tinyurl.com/tamap1) IN `tama_p1` folder on SD renamed as `rom.bin` to make it work. - Modified to allow saving game state by [DroomOne](https://github.com/DroomOne/flipperzero-tamagotch-p1) - Mute mode & settings by [Round-Pi](https://github.com/Round-Pi/flipperzero-tamagotch-p1)
- [Tanks Game (By alexgr13)](https://github.com/alexgr13/flipperzero-firmware/tree/fork/dev/applications/tanks-game) - Fixes for latest subghz changes by @Sil333033
- [Tic-Tac-Toe Multi (By RouNNdeL)](https://github.com/RouNNdeL/flipper-tictactoe-multi)
- [Video Poker (By PixlEmly)](https://github.com/PixlEmly/flipperzero-firmware-testing/blob/420/applications/VideoPoker/poker.c)
- [Yatzee (By emfleak)](https://github.com/emfleak/flipperzero-yatzee)

## GPIO
- [Air Mouse (By ginkage)](https://github.com/ginkage/FlippAirMouse/)
- [Plantower PMSx003 sensor reader (By 3cky)](https://github.com/3cky/flipperzero-airmon)
- [Bar code scanner emulator via COM port (By polarikus)](https://github.com/polarikus/flipper-zero_bc_scanner_emulator)
- [ESP32->Flipper Camera (By Z4urce)](https://github.com/Z4urce/flipperzero-camera)
- [RC2014 ColecoVision (By ezod)](https://github.com/ezod/flipperzero-rc2014-coleco)
- [ESP Flasher (By 0xchocolate)](https://github.com/0xchocolate/flipperzero-esp-flasher)
- [ESP32-C6 Gravity terminal (By chris-bc)](https://github.com/chris-bc/Flipper-Gravity)
- [IFTTT Virtual Button for ESP8266 (By Ferrazzi)](https://github.com/Ferrazzi/FlipperZero_IFTTT_Virtual_Button) - Fixes [(by Willy-JL)](https://github.com/ClaraCrazy/Flipper-Xtreme/commit/ae321fb5f4c616d3965546926b1b4b446eef8d86)
- [Wifi Deauth v2 for ESP8266 (By Timmotools)](https://github.com/Timmotools/flipperzero_esp8266_deautherv2)
- [Evil captive portal (By bigbrodude6119)](https://github.com/bigbrodude6119/flipper-zero-evil-portal) - WIP -> +2 new features [by leedave](https://github.com/leedave/flipper-zero-evil-portal/tree/leedave/ap_rename)
- [Flashlight (By xMasterX)](https://github.com/xMasterX/flipper-flashlight)
- [Pwnagotchi->Flipper interface (By Matt-London)](https://github.com/Matt-London/pwnagotchi-flipper)
- [Geiger Counter (By nmrr)](https://github.com/nmrr/flipperzero-geigercounter) - works with external module!
- [Oscilloscope (By anfractuosity)](https://github.com/anfractuosity/flipperscope) -> (Fixed furi_bus issues by @xMasterX)
- [Control 7-segment display with Flipper Zero (By jamisonderek)](https://github.com/jamisonderek/flipper-zero-tutorials/tree/main/gpio)
- [GPIO Controller (By Lokno)](https://github.com/Lokno/gpio_controller) -> `A visual tool to control the general purpose pins of the Flipper Zero`
- [GPIO Reader (By aureli1c)](https://github.com/aureli1c/flipperzero_GPIO_read)
- [Timelapse (zeitraffer) (By theageoflove)](https://github.com/theageoflove/flipperzero-zeitraffer)
- [GPIO Reader 2 (By biotinker)](https://github.com/biotinker/flipperzero-gpioreader)
- [Logic analyzer (By g3gg0)](https://github.com/g3gg0/flipper-logic_analyzer) -> [How to use](https://github.com/g3gg0/flipper-logic_analyzer#readme)
- [LORA Terminal (By aafksab)](https://github.com/aafksab/LORA-Term)
- [MagSpoof (WIP) (By zacharyweiss)](https://github.com/zacharyweiss/magspoof_flipper) - Read mode [(by hummusec)](https://github.com/hummusec/magspoof_flipper) - + GPIO & Power fixes
- [Mx2125 Dual-Axis Accelerometer (By jamisonderek)](https://github.com/jamisonderek/flipper-zero-tutorials/tree/main/gpio)
- [MH-Z19 - CO2 sensor (By meshchaninov)](https://github.com/meshchaninov/flipper-zero-mh-z19)
- [NRF24 Batch Read/Send (WIP) (By vad7)](https://github.com/vad7/nRF24-Batch)
- [NRF24 Scanner (By vad7)](https://github.com/vad7/nrf24scan)
- [Pokemon Trading (for GameBoy) (By EstebanFuentealba)](https://github.com/EstebanFuentealba/Flipper-Zero-Game-Boy-Trading-Pokemons)
- [Radiation Sensor (By sionyx)](https://github.com/sionyx/flipper_radsens) -> works with RadSens module
- [RCWL-516 Motion Sensor (By ahmedafe1)](https://github.com/ahmedafe1/rcwl_0516-Flipperzero)
- [ServoTester (By spin7ion)](https://github.com/spin7ion/flipper-servotester)
- [ServoTester (2) (By mhasbini)](https://github.com/mhasbini/ServoTesterApp/tree/master)
- [TAS playback (By rcombs)](https://github.com/rcombs/tas-playback) -> `This app plays back TAS files for retro video games. Connect the GPIO pins to the console's controller port and select a file to play back.`
- [u-blox GPS (By liamhays)](https://github.com/liamhays/ublox)
- [[USPING] Distance Sensor (By privet971)](https://github.com/privet971/FlipperZeroApps/tree/main/usping)
- [Wiegand Reader (By jamisonderek)](https://github.com/jamisonderek/flipper-zero-tutorials/tree/main/gpio)
- [ESP32 Wifi Mapping (By carvilsi)](https://github.com/carvilsi/flipper0-wifi-map/tree/main)
- [Wii EC Analyser (By csBlueChip)](https://github.com/csBlueChip/FlipperZero_WiiEC)

## Infrared
- [IR Remote (By Hong5489)](https://github.com/Hong5489/ir_remote) - improvements [(By friebel)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/535) - Hold Option, RAW support [(By d4ve10)](https://github.com/d4ve10/ir_remote/tree/infrared_hold_option)
- [IR Intervalometer for Sony Cameras (By Nitepone)](https://github.com/Nitepone/flipper-intervalometer)
- [IR Xbox Controller (By gebeto)](https://github.com/gebeto/flipper-xbox-controller)

## Media
- [BPM Tapper (By panki27)](https://github.com/panki27/bpm-tapper)
- [Flizzer Tracker (By LTVA1)](https://github.com/LTVA1/flizzer_tracker)
- [Music Beeper (By qqMajiKpp / Haseo)](https://github.com/qqmajikpp/)
- [Ocarina (By invalidna-me)](https://github.com/invalidna-me/flipperzero-ocarina)
- [Text 2 SAM (By Round-Pi)](https://github.com/Round-Pi/flipperzero-text2sam) - Fixes [(by Willy-JL)](https://github.com/ClaraCrazy/Flipper-Xtreme/commit/e688f81b53b0138d80de4b609daf1f9fca5be647)
- [Tuning Fork (By besya)](https://github.com/besya/flipperzero-tuning-fork) - Fixes [(by Willy-JL)](https://github.com/ClaraCrazy/Flipper-Xtreme/commit/44023851f7349b6ae9ca9f9bd9228d795a7e04c0)
- [USB Midi (By DrZlo13)](https://github.com/DrZlo13/flipper-zero-usb-midi)
- [Video Player (By LTVA1)](https://github.com/LTVA1/flipper-zero-video-player) <- Follow link to download examples and learn how to convert videos
- [Music Tracker (By DrZlo13)](https://github.com/DrZlo13/flipper-zero-music-tracker)

## NFC
- [Mifare Fuzzer (By spheeere98)](https://github.com/spheeere98/mifare_fuzzer) -> (+Fixed crash on exit)
- [Seader (By bettse)](https://github.com/bettse/seader/tree/main) - `Interface with a SAM from the Flipper Zero over UART`

## RFID
- [RFID Beacon (By nmrr)](https://github.com/nmrr/flipperzero-rfidbeacon) - `A letter/number Morse beacon on 125 kHz`
- [EM4100 universal keys generator (for RFID Fuzzer) (By Milk-Cool)](https://github.com/Milk-Cool/fz-em4100-generator)

## Sub-GHz
- [Enhanced Sub-GHz Chat (By twisted-pear)](https://github.com/twisted-pear/esubghz_chat)
- [TPMS Reader (By wosk)](https://github.com/wosk/flipperzero-tpms/tree/main)

## Tools
- [Analog Clock (By scrolltex)](https://github.com/scrolltex/flipper_analog_clock)
- [Brainfuck interpreter (By nymda)](https://github.com/nymda/FlipperZeroBrainfuck)
- [Ceasar Cipher (By panki27)](https://github.com/panki27/caesar-cipher)
- [Calculator (By n-o-T-I-n-s-a-n-e)](https://github.com/n-o-T-I-n-s-a-n-e)
- [CLI-GUI Bridge (By ranchordo)](https://github.com/ranchordo/flipperzero-cli-bridge) -> (Fixed by @Willy-JL)
- [Count Down Timer (By 0w0mewo)](https://github.com/0w0mewo/fpz_cntdown_timer)
- [Counter (By Krulknul)](https://github.com/Krulknul/dolphin-counter)
- [DCF77 Transmitter (By arha)](https://github.com/arha/flipper-dcf77)
- [FlipBIP Crypto Tool (By xtruan)](https://github.com/xtruan/FlipBIP)
- [Flipp Pomodoro Timer (By Th3Un1q3)](https://github.com/Th3Un1q3/flipp_pomodoro)
- [Chronometer (By nmrr)](https://github.com/nmrr/flipperzero-chronometer)
- [HEX Editor (By dunaevai135)](https://github.com/dunaevai135/flipper-zero-hex_editor)
- [Multi Counter (By JadePossible)](https://github.com/JadePossible/Flipper-Multi-Counter)
- [NightStand Clock (By nymda)](https://github.com/nymda/FlipperNightStand) - Fixes by [Willy-JL](https://github.com/Willy-JL)
- [Password Generator (By anakod)](https://github.com/anakod/flipper_passgen) - Proper random by [henrygab](https://github.com/anakod/flipper_passgen/pull/6)
- [Pomodoro Timer (By sbrin)](https://github.com/sbrin/flipperzero_pomodoro)
- [QR Code (By bmatcuk)](https://github.com/bmatcuk/flipperzero-qrcode)
- [Resistance calculator (By instantiator)](https://github.com/instantiator/flipper-zero-experimental-apps)
- [VB Lab Migration Assistant (By GMMan (cyanic))](https://github.com/GMMan/flipperzero-vb-migrate)

## USB
- [USB HID Autofire (By pbek)](https://github.com/pbek/usb_hid_autofire)

--- 

Useful source of files for flipper, also has apps pack:
## [UberGuidoZ Playground - Large collection of files - Github](https://github.com/UberGuidoZ/Flipper)
