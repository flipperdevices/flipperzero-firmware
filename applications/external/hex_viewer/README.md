# #️⃣ Hex Viewer

Hex Viewer application for Flipper Zero!  
The app allows you to view various files as HEX

**Some facts**:
- Written with pure C in a very simple and effective manner
- Tested on files up to 16Mb
- Very effective: calls `canvas_draw_str` 8 times during repaint and that's almost it
- Can also view text representation of bytes (makes it kinda poor man's text viewer)
- Has "Scroll to ..." feature which allows you to jump to any percent of file

Feel free to send PRs! 

App URL: https://lab.flipper.net/apps/hex_viewer
Catalog's manifest: [flipper-application-catalog/applications/Tools/hex_viewer/manifest.yml](https://github.com/flipperdevices/flipper-application-catalog/blob/main/applications/Tools/hex_viewer/manifest.yml)
