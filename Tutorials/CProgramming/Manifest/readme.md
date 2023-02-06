# Application Manifest

The Manifest is here to tell the Firmware OS important details about your Application. 
<br>

## Official Documentation

The best info is already available in the <a href="https://github.com/flipperdevices/flipperzero-firmware/blob/dev/documentation/AppManifests.md" target="_blank">Official Documentation</a>
<br><br>

## Hints & Recommendations

Example File

```
App(
    appid="app_code",
    name="App Name",
    apptype=FlipperAppType.PLUGIN,
    entry_point="app_function_name",
    cdefines=["APP_CODE"],
    requires=[
        "gui",
    ],
    stack_size=2 * 1024,
    order=10,
    fap_icon="app_icon_10px.png",
    fap_icon_assets="icons",
    fap_category="MyFolder\Games",
)
```

<ul>
<li><b>appid</b>: Needs to be unique</li>
<li><b>apptype</b>: FlipperAppType.PLUGIN is what you mainly need</li>
<li><b>fap_icon</b>: Icon displayed in the App Menu. Use a 10x10 pixel PNG, Indexed colors, only use black & white pixels (1 Bit color depth)</li>
<li><b>fap_icon_assets</b>: Very handy. Defines a folder name for images used in the app. Check the <a href="https://github.com/flipperdevices/flipperzero-firmware/blob/dev/documentation/AppsOnSDCard.md#fap-assets" target="_blank">official docs.</a></li>
<li><b>fap_category</b>: Defines where the app will be found in the devices menu. `"MyFolder\Games"` if "Games" does not exist it will be created, but if "MyFolder" doesn't exist, your app won't be transferred. So make sure that any folder before the last one is created first.</li>
</ul>