# Apps Data folder Example

This example shows how to use the Apps Data folder to store data that is not part of the app itself, such as user data, configuration files, and so on.

## What is the Apps Data Folder?

The Apps Data folder is a folder that is used to store data for external apps that are not part of the main firmware.
Path to current application folder is related to the `appsid` of the app. The `appsid` is used to identify the app in the app store and stored in app manifest, `application.fam` file.
Also, Apps Data folder is located only on the external storage, SD card.

For example, if the `appsid` of the app is `snake_game`, the path to the Apps Data folder will be `/ext/apps_data/snake_game`.

## How to get the path to the Apps Data folder?

You can use `PathHelper` to get the path to the application data folder, or use the raw `Storage` call to get the path. All folders in the returned path will be created automatically.
