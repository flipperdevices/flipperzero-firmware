#pragma once
#include <gui/view.h>

typedef struct GuiWidget GuiWidget;
typedef struct GuiElement GuiElement;

/** Allocate Gui Widget that holds Gui Elements
 * @return GuiWidget instance
 */
GuiWidget* gui_widget_alloc();

/** Free Gui Widget
 * @warning this method doesn't free Gui Elements
 * @param gui_widget GuiWidget instance
 */
void gui_widget_free(GuiWidget* gui_widget);

/** Clear Gui Widget
 * @param gui_widget GuiWidget instance
 */
void gui_widget_clear(GuiWidget* gui_widget);

/** Get Gui Widget view
 * @param gui_widget GuiWidget instance
 * @return View instance
 */
View* gui_widget_get_view(GuiWidget* gui_widget);

/** Add Gui Elements to Gui Widget
 * @param gui_widget GuiWidget instance
 * @param element GuiElement element
 */
void gui_widget_add_element(GuiWidget* gui_widget, GuiElement* element);
