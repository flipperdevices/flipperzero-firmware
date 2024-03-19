Internal readme for nfc team

How to install framework
========================
1. Install all submodules and make sure all works
2. Install python 3.11 or higher
3. Create your own venv (optional and prefered)
4. Install proxmark3 client with a make SKIPQT=1 (or alternatively use headless openCV example later)
5. Go to scripts/flippigator
6. Install requirements.txt with python3 -m pip install -r requirements.txt 
(or alternatively python -m pip install -r requirements-headless.txt)

(you might run into issues with pyreadline on windows, just message me to fix it or google errors and do some additional imports)


Run the framework
=================
1. Go to scripts/flippigator
2. pytest -vv --no_init --port=/dev/ttyACM0 --px3_path="/home/vk/work/proxmark3/pm3" -k TestEmulateNtag213 where:
--vv very verbose for additional debug output
--no_init do not init flipper (checks settings and enables debug, takes 30+ seconds)
--port=/dev/ttyACM0 port to flipper
--px3_path="/home/vk/work/proxmark3/pm3" path to proxmark3 client (make sure you either use noqt version or headless openCV), pm3 client finds the port automatically
-k <text> here you can select any test name, part of it or class name
-m <mark name> runs a specific mark (tag), they are added via decorator and must be specified in pytest.ini file

Writing your own tests
======================
1. All NFC related tests are in scripts/flippigator/nfc and are split by type (emulation, read etc) and by protocol
2. Typically, all tests that are ran with same idea or card ran inside same class
3. In test_emulate_ntag213.py you can see that class has own set of tests and can be ran with -k TestEmulateNtag213.
Additionally test has its own fixture that is ran once on launch of any test from that class, that starts the emulation.
If you wish to do this manually, just comment the fixture emulate_ntag213
4. Currently, proxmark wrapper is in really poor testing state, preferably add additional result parsers into test class or move them to separate module,
fewer modifications we do to output, the better

Also if you wish to crop your own images for framework recognition, then you need to do the following:
1. Run python3 testOpenCV.py /dev/ttyACM0
2. Go to needed screen on flippper (or via arrow keys + space + backspace)
3. Press C (as for crop)
4. Select the area and press V (no clue why V, probably paste, ask Daniil)
5. File will appear in /img folder
6. Name the file, usually it is same as text and case sensitive. If you are selecting text in browser, then you need to add "browser_"
7. Copy that image to /img/ref and now you can look up your own things on screen

(you can look examples in /ref and in code)
