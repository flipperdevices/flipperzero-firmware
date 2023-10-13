# Tips & Tricks

[TOC]

## Diagrams with Graphviz

To get the best looking class diagrams for your documentation, generate them with Graphviz as vector graphics with transparent background:

```
# Doxyfile
HAVE_DOT = YES
DOT_IMAGE_FORMAT = svg
DOT_TRANSPARENT = YES
```

## Disable Dark Mode

If for some reason you don't want the theme to automatically switch to dark mode depending on the browser preference,
you can disable dark mode by adding the `light-mode` class to the html-tag in the header template:

```html
<html xmlns="http://www.w3.org/1999/xhtml" class="light-mode">
```

The same can be done to always enable dark-mode:

```html
<html xmlns="http://www.w3.org/1999/xhtml" class="dark-mode">
```


**This only works if you don't use the dark-mode toggle extension.**

## Choosing Sidebar Width

If you have enabled the sidebar-only theme variant, make sure to carefully choose a proper width for your sidebar.
It should be wide enough to hold the icon, project title and version number. If the content is too wide, it will be
cut off.

```css
html {
    /* Make sure sidebar is wide enough to contain the page title (logo + title + version) */
    --side-nav-fixed-width: 335px;
}
```

The choosen width should also be set in the Doxyfile:

```
# Doxyfile
TREEVIEW_WIDTH = 335
```

<span class="next_section_button">

Read Next: [Example](https://jothepro.github.io/doxygen-awesome-css/class_my_library_1_1_example.html)
</span>