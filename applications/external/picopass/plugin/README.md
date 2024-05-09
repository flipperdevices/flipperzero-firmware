# Flipper zero wiegand plugin

Add as git submodule: `git submodule add https://gitlab.com/bettse/flipper-wiegand-plugin.git plugin`

Add to your `application.fam`
```
App(
    appid="plugin_wiegand",
    apptype=FlipperAppType.PLUGIN,
    entry_point="plugin_wiegand_ep",
    requires=["seader"],
    sources=["plugin/wiegand.c"],
)
```
