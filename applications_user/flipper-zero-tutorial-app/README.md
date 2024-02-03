# flipper-zero-tutorial-app

This is a simple app for the Flipper Zero, intended to accompany the Flipper Zero developer tutorials at: [instantiator.dev](https://instantiator.dev)

## Known issues

- [x] ~~This code runs on the `release` firmware, but hangs on the `dev` firmware. I'm working on it.~~
  - Issue was twofold:
    1. I was calling `view_set_context` on each GUI module's views. This is unnecessary, and the various module functions rely on view contexts pointing to the modules. Callback contexts for our callback methods are set using the various module functions (`popup_set_context`, and `menu_add_item`).
    2. I was providing static icons (a single frame, no framerate) to the menu - which _always_ animates its icons. Having a framerate of zero results in a division by zero error.

## Supporting references

A lot of the code here is derived from the work I did building the [resistance calculator](https://github.com/instantiator/flipper-zero-experimental-apps/tree/main/resistors) app, and you're welcome to sift through that code and plunder what you need for your own projects.

In turn, that app relies heavily on the patterns provided in Derek Jamison's [basic scenes](https://github.com/jamisonderek/flipper-zero-tutorials/tree/main/plugins/basic_scenes) tutorial. Derek has been kindly supporting and debugging my work in the background, and has produced a vast library of valuable learning resources.

### With thanks

This tutorial series would not have been possible without the help and support of various people in the Flipper Zero community. Many thanks to:

* [Derek Jamison](https://github.com/jamisonderek) for the [Flipper Zero tutorials](https://github.com/jamisonderek/flipper-zero-tutorials) repository (in particular, [plugins](https://github.com/jamisonderek/flipper-zero-tutorials/tree/main/plugins))
* [DroomOne](https://github.com/DroomOne) for the [Flipper Plugin tutorial](https://github.com/DroomOne/Flipper-Plugin-Tutorial).
* [Chris Hranj](https://brodan.biz/) (Brodan) for the [guide to Flipper Zero components](https://brodan.biz/blog/a-visual-guide-to-flipper-zero-gui-components/).

If you're interested in developing apps for the Flipper, you should really check out Derek's YouTube channel: [@MrDerekJamison](https://www.youtube.com/@MrDerekJamison)
