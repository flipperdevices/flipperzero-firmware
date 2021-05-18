#include "irda-app-remote-manager.hpp"
#include "furi.h"
#include <string>
#include <utility>


IrdaAppRemoteManager::IrdaAppRemoteManager() {
    // Read from api-hal-storage, and fill remotes
}

static const std::string default_remote_name = "remote";

void IrdaAppRemoteManager::add_button(const char* button_name, const IrdaMessage* message) {
    remotes[current_remote_index].buttons.emplace_back(button_name, message);
}

void IrdaAppRemoteManager::add_remote_with_button(const char* button_name, const IrdaMessage* message) {
    bool found = true;
    int i = 0;

    // find first free common name for remote
    do {
        found = false;
        ++i;
        for (const auto &it : remotes) {
            if (it.name == (default_remote_name + std::to_string(i))) {
                found = true;
                break;
            }
        }
    } while (found);

    remotes.emplace_back(default_remote_name + std::to_string(i));
    current_remote_index = remotes.size() - 1;
    add_button(button_name, message);
}

IrdaAppRemote::IrdaAppRemote(std::string name)
    : name(name) {}

std::vector<std::string> IrdaAppRemoteManager::get_button_list(void) const {
    std::vector<std::string> name_vector;
    auto remote = remotes[current_remote_index];
    name_vector.reserve(remote.buttons.size());

    for (const auto& it : remote.buttons) {
        name_vector.emplace_back(it.name);
    }

    // copy elision
    return name_vector;
}

std::vector<std::string> IrdaAppRemoteManager::get_remote_list() const {
    std::vector<std::string> name_vector;
    name_vector.reserve(remotes.size());

    for (const auto& it : remotes) {
        name_vector.push_back(it.name);
    }

    // copy elision
    return name_vector;
}

size_t IrdaAppRemoteManager::get_current_remote(void) const {
    return current_remote_index;
}

const char* IrdaAppRemoteManager::get_current_remote_name() const {
    return remotes[current_remote_index].name.c_str();
}

void IrdaAppRemote::add_button(size_t remote_index, const char* button_name, const IrdaMessage* message) {
    buttons.emplace_back(button_name, message);
}

const IrdaMessage* IrdaAppRemoteManager::get_button_data(size_t button_index) const {
    auto &b = remotes[current_remote_index].buttons.at(button_index);
    return &b.message;
}

void IrdaAppRemoteManager::set_current_remote(size_t index) {
    furi_check(index < remotes.size());
    current_remote_index = index;
}

