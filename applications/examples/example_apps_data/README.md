# Apps Data folder Example {#example_app_data}

This example demonstrates how to utilize the Apps Data folder to store data that is not part of the app itself, such as user data, configuration files, and so forth.

## Source code

Source code for this example can be found [here](https://github.com/flipperdevices/flipperzero-firmware/tree/dev/applications/examples/example_apps_data).

## What is the Apps Data Folder?

The **Apps Data** folder is a folder used to store data for external apps that are not part of the main firmware. 

The path to the current application folder is related to the `appid` of the app. The `appid` is used to identify the app in the app store and is stored in the `application.fam` file. 
The Apps Data folder is located only on the external storage, the SD card.

For example, if the `appid` of the app is `snake_game`, the path to the Apps Data folder will be `/ext/apps_data/snake_game`. But using raw paths is not recommended, because the path to the Apps Data folder can change in the future. Use the `APP_DATA_PATH()` and/or `STORAGE_APP_DATA_PATH_PREFIX` macros instead.

Accessing a path like this will also ensure the directory exists (just the root `/ext/apps_data/snake_game` for example, not subdirectories), so you won't need to create it.

## How to get the path to the Apps Data folder?

You can use `/ext/apps_data/appid` directly (replacing `appid`) to access your application data folder. For example, if you want to open a file `config.txt` in the Apps Data folder, you can use this path: `/ext/apps_data/appid/config.txt`. But this way is not recommended, because the path can change in the future.

We recommend to use the `APP_DATA_PATH()` macro to get the path to the Apps Data folder. For example, if you want to open a file `config.txt` in the Apps Data folder, you can use this path: `APP_DATA_PATH("config.txt")`.

If you want to construct paths at run-time, you can use the `STORAGE_APP_DATA_PATH_PREFIX` macro, which evaluates to `/ext/apps_data/appid` at compile-time.

## What is the difference between the Apps Assets folder and the Apps Data folder?

The Apps Assets folder is used to store the data <u>provided</u> with the application. For example, if you want to create a game, you can store game levels (content data) in the Apps Assets folder.

The Apps Data folder is used to store data <u>generated</u> by the application. For example, if you want to create a game, you can save the progress of the game (user-generated data) in the Apps Data folder.
