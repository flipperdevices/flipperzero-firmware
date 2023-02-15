# Apps Data folder Example

This example demonstrates how to utilize the Apps Data folder to store data that is not part of the app itself, such as user data, configuration files, and so forth.

## What is the Apps Data Folder?

The **Apps Data** folder is a folder used to store data for external apps that are not part of the main firmware. 

The path to the current application folder is related to the `appsid` of the app. The `appsid` is used to identify the app in the app store and is stored in the `application.fam` file. 
The Apps Data folder is located only on the external storage, the SD card.

For example, if the `appsid` of the app is `snake_game`, the path to the Apps Data folder will be `/ext/apps_data/snake_game`.

## How to get the path to the Apps Data folder?

You can use `PathHelper` to get the path to the application data folder, or use the raw `Storage` call to get the path. All folders in the returned path will be created automatically.
