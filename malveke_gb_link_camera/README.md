# ***GAME BOY*** Link Camera MALVEKE

## Introduction

<div align="center">

**Official** | **Unleashed** | **RogueMaster** | **Xtreme**
:- | :- | :- | :- 
[![FlipC.org](https://flipc.org/EstebanFuentealba/MALVEKE-Flipper-Zero/badge?branch=main&root=flipper_companion_apps%2Fapplications%2Fexternal%2Fmalveke_gb_link_camera&firmware=official)](https://flipc.org/EstebanFuentealba/MALVEKE-Flipper-Zero?branch=main&root=flipper_companion_apps%2Fapplications%2Fexternal%2Fmalveke_gb_link_camera&firmware=official)|[![FlipC.org](https://flipc.org/EstebanFuentealba/MALVEKE-Flipper-Zero/badge?branch=main&root=flipper_companion_apps%2Fapplications%2Fexternal%2Fmalveke_gb_link_camera&firmware=unleashed)](https://flipc.org/EstebanFuentealba/MALVEKE-Flipper-Zero?branch=main&root=flipper_companion_apps%2Fapplications%2Fexternal%2Fmalveke_gb_link_camera&firmware=unleashed)|[![FlipC.org](https://flipc.org/EstebanFuentealba/MALVEKE-Flipper-Zero/badge?branch=main&root=flipper_companion_apps%2Fapplications%2Fexternal%2Fmalveke_gb_link_camera&firmware=roguemaster)](https://flipc.org/EstebanFuentealba/MALVEKE-Flipper-Zero?branch=main&root=flipper_companion_apps%2Fapplications%2Fexternal%2Fmalveke_gb_link_camera&firmware=roguemaster)|[![FlipC.org](https://flipc.org/EstebanFuentealba/MALVEKE-Flipper-Zero/badge?branch=main&root=flipper_companion_apps%2Fapplications%2Fexternal%2Fmalveke_gb_link_camera&firmware=xtreme)](https://flipc.org/EstebanFuentealba/MALVEKE-Flipper-Zero?branch=main&root=flipper_companion_apps%2Fapplications%2Fexternal%2Fmalveke_gb_link_camera&firmware=xtreme)
</div>

<p align='center'>
<a href="https://www.tindie.com/stores/efuentealba/?ref=offsite_badges&utm_source=sellers_efuentealba&utm_medium=badges&utm_campaign=badge_large"><img src="https://d2ss6ovg47m0r5.cloudfront.net/badges/tindie-larges.png" alt="I sell on Tindie" width="200" height="104"></a>
</p>

Extract your  ***GAME BOY*** Camera picture via WIFI, so they can be easily shared with your phone, tablet or pc. Easy to use, just hook up to your ***GAME BOY*** and print as usual, the device will store the images and share them on a web server via WIFI. You will need a printer cable or gameboy Color link cable.
<br>


## Instructions for use.

These instructions assume that you are starting at the Flipper Zero desktop. Otherwise, press the Back button until you are at the desktop.

- Press the `OK` button on the Flipper to open the main menu.
- Choose `Applications` from the menu.
- Choose `GPIO` from the submenu.
- Choose `GAME BOY Link-Camera MALVEKE`
- The Flipper Zero will show the loading screen of the application. 

    <p align='center'>
        <br />
        <img src="./docs/images/flipper-zero-flat-1.png" width="400" />
        <br />
    </p>

- If the **MALVEKE** board is not connected, the following message will appear. You must connect it and press `OK`.

    <p align='center'>
        <br />
        <img src="./docs/images/flipper-zero-flat-2.png" width="400" />
        <br />
    </p>

- **MALVEKE** will create a Wi-Fi network using the data displayed on the screen.
    <p align='center'>
        <br />
        <img src="./docs/images/flipper-zero-flat-3.png" width="400" />
        <br />
    </p>

- On your computer or phone, search for the network and connect using the provided access credentials.
    <p align='center'>
        <br />
        <img src="./docs/images/wifi-4.png" width="400" />
        <br />
    </p>
    <p align='center'>
        <br />
        <img src="./docs/images/wifi-5.png" width="400" />
        <br />
    </p>

- When you connect, it will automatically open a browser window with the host. If this doesn't happen, you should manually enter the IP provided on the Flipper Zero as the **host** in your browser.
    <p align='center'>
        <br />
        <img src="./docs/images/captive-portal-6.png" width="400" />
        <br />
    </p>

- Now, on your ***GAME BOY***, connect the Link Cable to **MALVEKE** `EXT2` port and proceed to print images as usual; they will appear in the web browser.


    <p align='center'>
        <br />
        <img src="../../../../assets/GAME-BOY-MALVEKE-connection-EXT2.png" width="400" />
        <br />
    </p>

    <p align='center'>
        <br />
        <img src="./docs/images/gbp-6.png" width="400" />
        <br />
    </p>

    <p align='center'>
        <br />
        <img src="./docs/images/transfer-7.gif" width="400" />
        <br />
    </p>
- In web you can Change palette and with button `A` Download image


## Acknowledgements
- [@brundonsmith](https://github.com/brundonsmith) for [GAME BOY Color in CSS](https://codepen.io/brundolf/pen/beagbQ).
- [@mofosyne](https://github.com/mofosyne) Code to emulate a gameboy printer via the gameboy link cable.
- [@HerrZatacke](https://github.com/HerrZatacke) I extracted many ideas from their repositories.

## TODO
- [ ] Refactor Code
- [ ] Documentation
- [ ] Save transfered imagen in Flipper Zero

<p align='center'>
<br />
<br />
From Talcahuano 🇨🇱 with ❤ 
</p>