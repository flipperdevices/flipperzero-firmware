#pragma once
#include <stdint.h>
#include <string>
#include <list>
#include <vector>
#include <irda.h>
#include <sd-card-api.h>
#include <filesystem-api.h>

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
    static const char* irda_directory;
    static const char* irda_extension;
    size_t current_remote_index;
    size_t current_button_index;
    std::vector<IrdaAppRemote> remotes;
    SdCard_Api* sd_ex_api;
    FS_Api* fs_api;
    void show_file_error_message(const char* error_text);
    bool parse_and_save_button(std::string& str);
    char file_buf[48];
    size_t file_buf_cnt;
public:
    std::vector<std::string> get_remote_list() const;
    std::vector<std::string> get_button_list() const;
    void add_remote_with_button(const char* button_name, const IrdaMessage* message);
    void add_button(const char* button_name, const IrdaMessage* message);

    size_t get_current_remote(void) const;
    size_t get_current_button(void) const;
    const IrdaMessage* get_button_data(size_t button_index) const;
    void set_current_remote(size_t index);
    void set_current_button(size_t index);
    void rename_button(const char* str);
    void rename_remote(const char* str);
    std::string get_current_button_name();
    std::string get_current_remote_name();
    size_t get_current_remote_buttons_number();
    void delete_current_button();
    void delete_current_remote();
    IrdaAppRemoteManager();
    ~IrdaAppRemoteManager() {};

    bool sync_current_remote();
    bool load();
};

