#pragma once
#include "generic-view-module.h"
#include <gui/modules/submenu.h>

class SubmenuM : public GenericViewModule {
public:
    SubmenuM();
    ~SubmenuM() final;
    View* get_view() final;

    /**
     * @brief Remove all items from submenu
     * @param submenu - Submenu instance
     */
    void clean() final;

    /**
     * @brief Add item to submenu
     * @param label - menu item label
     * @param index - menu item index, used for callback, may be the same with other items
     * @param callback - menu item callback
     * @param callback_context - menu item callback context
     * @return SubmenuItem instance that can be used to modify or delete that item
     */
    SubmenuItem* add_item(
        const char* label,
        uint32_t index,
        SubmenuItemCallback callback,
        void* callback_context);

    /**
     * @brief Set submenu item selector
     * @param index
     */
    void set_selected_item(uint32_t index);

    /**
     * @brief Set optional header for submenu
     * @param header    - header to set
     */
    void set_header(const char* header);

private:
    Submenu* submenu;
};