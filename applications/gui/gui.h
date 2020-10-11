#pragma once

typedef struct widget_t widget_t;
typedef struct gui_t gui_t;

void gui_widget_status_bar_add(gui_t *gui, widget_t *widget);

void gui_widget_add(gui_t *gui, widget_t *widget);

void gui_widget_fs_add(gui_t *gui, widget_t *widget);

void gui_widget_dialog_add(gui_t *gui, widget_t *widget);

