# Setting up a development environment

## Get an IDE
I recommend getting <b>Visual Studio Code</b>. Mainly because I'm not an experienced C developer an this seems like the easiest starting point. It works on all platforms and also Flipper provides some configs for this editor. 
<br><br>
<a href="https://code.visualstudio.com" target="_blank">Get Visual Studio here</a>
<br>

## Get the latest firmware code
It is important, that you get this using a Git client. If you just download it as a file, you will run into problems when compiling your projects. If you do not yet have a git client, please search online for one. For Windows I recommend getting Git-Scm <a href="https://git-scm.com/" target="_blank">Git-Scm</a>. Other systems normally provide a git client in their packet managers. 
<br><br>
Download the firmware of you choice using Git<br>
<a href="https://github.com/flipperdevices/flipperzero-firmware" target="_blank">Original Firmware (recommended)</a><br>
<a href="https://github.com/IamUSER/Flipper-Fusion">FZ Fusion Firmware</a><br>
<a href="https://github.com/ClaraCrazy/Flipper-Xtreme" target="_blank">Xtreme Firmware</a><br>
<a href="https://github.com/DarkFlippers/unleashed-firmware" target="_blank">Unleashed Firmware</a><br>
<a href="https://github.com/RogueMaster/flipperzero-firmware-wPlugins" target="_blank">Roguemaster Firmware</a><br>
<br>
I recommend creating a personal folder for your flipper content first and then checking out your firmware into a subfolder
<br>

## Install Flipper Configs for Visual Studio Code
1. Open Visual Studio
2. Open your Firmware Folder
3. If VS Code recommends extensions, install them
4. Make sure you have the Terminal view activated
5. In the terminal, navigate to your FW folder and run 
     ```./fbt ``` Installs Flipper Build Tool
6. Also run
     ```./fbt vscode_dist ``` to install Settings for Visual Studio Code


