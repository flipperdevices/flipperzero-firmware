# Visual Studio Code workspace for Flipper Zero

## Setup

 * To start developing with VSCode, run `./fbt vscode_dist` in project root. That should only be done once. 
 * After that, open firmware folder in VSCode: "File" > "Open folder".

 * For more details on fbt, see (fbt docs)[../documentation/fbt.md].


## Workflow

Command for building firmware are invoked through Build menu: Ctrl+Shift+B.

To attack a debugging session, first build and flash firmware, then choose your debug probe in Debug menu (Ctrl+Shift+D).

Note that you have to detach debugging session before rebuilding and re-flashing firmware.
