#pragma once
#include <stdint.h>
#include <string>
#include <list>
#include <vector>
#include <irda.h>

class IrdaAppRemoteButton {
    friend class IrdaAppRemoteManager;
    std::string name;
    IrdaMessage message;
public:
    IrdaAppRemoteButton(const char* name, const IrdaMessage* message)
        : name(name), message (*message) {}
    ~IrdaAppRemoteButton() {}
};

class IrdaAppRemote {
    friend class IrdaAppRemoteManager;
    std::vector<IrdaAppRemoteButton> buttons;
    std::string name;
    bool add(const IrdaMessage*);
    void add_button(size_t remote_index, const char* button_name, const IrdaMessage* message);
public:
    IrdaAppRemote(std::string name);
};

class IrdaAppRemoteManager {
    size_t current_remote_index;
    std::vector<IrdaAppRemote> remotes;
public:
    std::vector<std::string> get_remote_list() const;
    std::vector<std::string> get_button_list() const;
    void add_remote_with_button(const char* button_name, const IrdaMessage* message);
    void add_button(const char* button_name, const IrdaMessage* message);

    size_t get_current_remote(void) const;
    const IrdaMessage* get_button_data(size_t button_index) const;
    const char* get_current_remote_name() const;
    void set_current_remote(size_t index);
    IrdaAppRemoteManager();
    ~IrdaAppRemoteManager() {};
};

