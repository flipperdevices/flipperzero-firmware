# FlipperZero IFTTT Virtual Button

<h3 align="center">
<a href="https://github.com/Ferrazzi/FlipperZero_IFTTT_Virtual_Button">
<img src="https://github.com/Ferrazzi/FlipperZero_IFTTT_Virtual_Button/blob/main/Image/ifttt.jpg" align="center" alt="fzCUSTOM" border="0">
</a>
</h3>

FlipperZero IFTTT Virtual Button is an application that allows you to send IFTTT requests from FlipperZero through an esp8266 module to automate some actions.
If This Than That, which could translate to "if this happens, then make that happen". As the phrase says, it allows you to automate activities by connecting to different online services.

# First you need to prepare an additional card.
you will need: 1 ESP8266 (I recommend ESP01 for the size) and 2 buttons.

<img src="https://github.com/Ferrazzi/FlipperZero_IFTTT_Virtual_Button/blob/main/Image/FlipperZero_IFTTT_Module.png" align="center" alt="fzCUSTOM" border="0">

# Let's now move on to the configuration and operation
- First we need to create an account on IFTTT if you don't have one. Go to https://ifttt.com/ and create one account.
- Now that we have an account, let's set up a test applet.
- Let's create our first applet, or rather a mini sequence of instructions, by pressing the create button ”New applet“.
- Click on the word "this" and search for the "Webhook" service, press the option receive a web request. In this way we can perform an action when the applet receives an HTTP request to a specific URL.
- We type the name of the event "button_pressed" and create the trigger.
- Now we press "that" and we create the mobile notification service through the app that we have installed on one of our phones. Here we can change the notification message and also with the EventName tag we will insert the name of the previously created event.
- You should have your applet created after clicking "Finish"
- Now let's go back to the main page https://ifttt.com/ and search "Webhooks" and enter the card "Documentation"
- Here you can find your unique KEY API which you need to keep private.
<img src="https://github.com/Ferrazzi/FlipperZero_IFTTT_Virtual_Button/blob/main/Image/webhooks_doc-960x487.png" align="center" alt="fzCUSTOM" border="0">
- Type the name of the event, "button_pressed".
- Final URL should appear at the bottom of the web page. We'll need to copy that URL.

# Open the application for the first time, an ifttt folder will be created on the root of the SD with another config folder inside. Here you will find the config.settings file, open the file with a text editor and change the values ​​for the wifi connection and the IFTTT account. Restart application and work!!!

COMPILE AND FLASH YOUR ESP8266!!!
