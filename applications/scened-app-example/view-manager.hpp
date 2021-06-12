#pragma once
#include "view-modules/generic-view-module.h"
#include <map>
#include <furi/check.h>
#include <gui/view_dispatcher.h>
#include <callback-connector.h>
#include "typeindex_no_rtti.hpp"

template <typename TApp, typename... TViewModules> class ViewManager {
public:
    ViewManager() {
        event_queue = osMessageQueueNew(10, sizeof(typename TApp::Event), NULL);

        view_dispatcher = view_dispatcher_alloc();
        previous_view_callback_pointer = cbc::obtain_connector(
            this, &ViewManager<TApp, TViewModules...>::previous_view_callback);

        [](...) {
        }((this->add_view(ext::make_type_index<TViewModules>().hash_code(), new TViewModules()),
           0)...);

        gui = static_cast<Gui*>(furi_record_open("gui"));
        view_dispatcher_attach_to_gui(view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    };

    ~ViewManager() {
        for(auto& it : holder) {
            view_dispatcher_remove_view(view_dispatcher, static_cast<uint32_t>(it.first));
            delete it.second;
        }

        view_dispatcher_free(view_dispatcher);
        osMessageQueueDelete(event_queue);
    }

    template <typename T> T* get() {
        uint32_t view_index = ext::make_type_index<T>().hash_code();
        furi_check(holder.count(view_index) != 0);
        return static_cast<T*>(holder[view_index]);
    }

    template <typename T> void switch_to() {
        uint32_t view_index = ext::make_type_index<T>().hash_code();
        furi_check(holder.count(view_index) != 0);
        view_dispatcher_switch_to_view(view_dispatcher, view_index);
    }

    void receive_event(typename TApp::Event* event) {
        if(osMessageQueueGet(event_queue, event, NULL, 100) != osOK) {
            event->type = TApp::EventType::Tick;
        }
    }

    void send_event(typename TApp::Event* event) {
        osStatus_t result = osMessageQueuePut(event_queue, event, 0, osWaitForever);
        furi_check(result == osOK);
    }

private:
    std::map<size_t, GenericViewModule*> holder;

    osMessageQueueId_t event_queue;
    ViewDispatcher* view_dispatcher;
    Gui* gui;
    ViewNavigationCallback previous_view_callback_pointer;

    uint32_t previous_view_callback(void* context) {
        typename TApp::Event event;
        event.type = TApp::EventType::Back;

        if(event_queue != NULL) {
            send_event(&event);
        }

        return VIEW_IGNORE;
    }

    void add_view(size_t view_index, GenericViewModule* view_module) {
        furi_check(holder.count(view_index) == 0);
        holder[view_index] = view_module;

        View* view = view_module->get_view();
        view_dispatcher_add_view(view_dispatcher, static_cast<uint32_t>(view_index), view);
        view_set_previous_callback(view, previous_view_callback_pointer);
    }
};