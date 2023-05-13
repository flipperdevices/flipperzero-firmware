# Adding Custom Applications to your Flipper
For Applications, you don't need to rebuild a full firmware. You just need to build the fap file using source code.<br>
<br>
This is very handy, as it lets you add your prefered apps to you flipper, no matter what Firmware it is running. <br>
<br>
Note that a app may no longer run when you update your firmware. But this should not be a problem, as in most cases you can simply refresh the source of your git-downloaded firmware and rebuild the app. This should fix all issues. 

## Add the app to the correct folder
In your firmware there should be a folder named 
 ```/applications_user ```
Copy the source code of your desired application into this folder (including its folder)

## Adjust the manifest if needed
Each app contains a manifest. Depending if it is embedded in the firmware or added later it may need adjusting<br>
File: ´application.fam´
<br><br>
Make sure the AppType is set to plugin
``` apptype=FlipperAppType.EXTERNAL, ```
<br><br>

## Run the app on your Flipper
You dont need to manually copy the generated fap file to your Flipper. Simply connect your Flipper via USB and run the following command:<br>
 ```.\fbt launch_app APPSRC=applications_user/<application_folder_name> ```


