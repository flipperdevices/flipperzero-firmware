# WolfSSL Example using the OM13076 (LPCXpresso18S37) board

To use, install the NXP LPCXpresso IDE and import the projects in a new workspace.

1. Change names of `LPCExpresso.project` and `LPCExpresso.cproject` files to `.project` and `.cproject`
2. Run LPCXpresso and choose a workspace location.
3. Right click in the project explorer window and choose Import.
4. Under General choose "Existing Projects into Workspace".
5. Under "Select root directory" click browse and select the wolfSSL root.
6. Check the "Search for nested projects" box.
7. Make sure "wolfssl" and "wolfssl_example" are checked under "Projects:".
8. Click finish.
9. Download the board and chip LPCOpen package for your platform.
10. Import the projects. For example "lpc_board_nxp_lpcxpresso_1837" and "lpc_chip_18xx" are the ones for the LPC18S37.

To setup this example to work with different baords/chips you will need to locate the LPCOpen sources for LPCXpresso on the NXP website and import the board and chip projects. Then you will need to update the "wolfssl_example" project properties to reference these projects (C/C++ General -> Paths and Symbols -> References). See the [LPCOpen v2.xx LPCXpresso quickstart guide for all platforms](https://www.lpcware.com/content/project/lpcopen-platform-nxp-lpc-microcontrollers/lpcopen-v200-quickstart-guides/lpcopen-1) for additional information.


## WolfSSL example projects:

1. `wolf_example`. It has console options to run the Wolf tests and benchmarks ('t' for the WolfSSL Tests and 'b' for the WolfSSL Benchmarks).

## Static libraries projects:

1. `wolfssl` for WolfSSL. The WolfSSL port for the LPC18XX platform is located in `IDE/LPCXPRESSO/lpc_18xx_port.c`. This has platform specific functions for `current_time` and `rand_gen`. The `WOLF_USER_SETTINGS` define is set which allows all WolfSSL settings to exist in the `user_settings.h` file (see this file for all customizations used).

## Important Files

1. `IDE/LPCXPRESSO/user_settings.h`. This provides a reference for library settings used to optimize for this embedded platform.

2. `IDE/LPCXPRESSO/lpc_18xx_port.c`. This defines the required time and random number functions for the WolfSSL library.

3. `IDE/LPCXPRESSO/wolf_example/wolf_example.c`. This shows use of the WolfSSL tests and benchmarks.
