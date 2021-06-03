#include "irda-app-remote-manager.hpp"
#include "furi.h"
#include "gui/modules/button_menu.h"
#include "irda.h"
#include <cstdio>
#include <string>
#include <utility>

const char* IrdaAppRemoteManager::irda_directory = "irda";
const char* IrdaAppRemoteManager::irda_extension = ".ir";

IrdaAppRemoteManager::IrdaAppRemoteManager() {
    sd_ex_api = static_cast<SdCard_Api*>(furi_record_open("sdcard-ex"));
    fs_api = static_cast<FS_Api*>(furi_record_open("sdcard"));
    // Read from api-hal-storage, and fill remotes
}

static const std::string default_remote_name = "remote";

void IrdaAppRemoteManager::add_button(const char* button_name, const IrdaMessage* message) {
    remotes[current_remote_index].buttons.emplace_back(button_name, message);
}

void IrdaAppRemoteManager::add_remote_with_button(
    const char* button_name,
    const IrdaMessage* message) {
    bool found = true;
    int i = 0;

    // find first free common name for remote
    do {
        found = false;
        ++i;
        for(const auto& it : remotes) {
            if(it.name == (default_remote_name + std::to_string(i))) {
                found = true;
                break;
            }
        }
    } while(found);

    remotes.emplace_back(default_remote_name + std::to_string(i));
    current_remote_index = remotes.size() - 1;
    add_button(button_name, message);
}

IrdaAppRemote::IrdaAppRemote(std::string name)
    : name(name) {
}

std::vector<std::string> IrdaAppRemoteManager::get_button_list(void) const {
    std::vector<std::string> name_vector;
    auto remote = remotes[current_remote_index];
    name_vector.reserve(remote.buttons.size());

    for(const auto& it : remote.buttons) {
        name_vector.emplace_back(it.name);
    }

    // copy elision
    return name_vector;
}

std::vector<std::string> IrdaAppRemoteManager::get_remote_list() const {
    std::vector<std::string> name_vector;
    name_vector.reserve(remotes.size());

    for(const auto& it : remotes) {
        name_vector.push_back(it.name);
    }

    // copy elision
    return name_vector;
}

size_t IrdaAppRemoteManager::get_current_remote(void) const {
    return current_remote_index;
}

size_t IrdaAppRemoteManager::get_current_button(void) const {
    return current_button_index;
}

void IrdaAppRemote::add_button(
    size_t remote_index,
    const char* button_name,
    const IrdaMessage* message) {
    buttons.emplace_back(button_name, message);
}

const IrdaMessage* IrdaAppRemoteManager::get_button_data(size_t button_index) const {
    furi_check(remotes[current_remote_index].buttons.size() > button_index);
    auto& b = remotes[current_remote_index].buttons.at(button_index);
    return &b.message;
}

void IrdaAppRemoteManager::set_current_remote(size_t index) {
    furi_check(index < remotes.size());
    current_remote_index = index;
}

void IrdaAppRemoteManager::set_current_button(size_t index) {
    furi_check(current_remote_index < remotes.size());
    furi_check(index < remotes[current_remote_index].buttons.size());
    current_button_index = index;
}

void IrdaAppRemoteManager::delete_current_remote() {
    remotes.erase(remotes.begin() + current_remote_index);
    current_remote_index = 0;
}

void IrdaAppRemoteManager::delete_current_button() {
    auto& buttons = remotes[current_remote_index].buttons;
    buttons.erase(buttons.begin() + current_button_index);
    current_button_index = 0;
}

std::string IrdaAppRemoteManager::get_current_button_name() {
    auto buttons = remotes[current_remote_index].buttons;
    return buttons[current_button_index].name;
}

std::string IrdaAppRemoteManager::get_current_remote_name() {
    return remotes[current_remote_index].name;
}

void IrdaAppRemoteManager::rename_remote(const char* str) {
    remotes[current_remote_index].name = str;
}

void IrdaAppRemoteManager::rename_button(const char* str) {
    remotes[current_remote_index].buttons[current_button_index].name = str;
}

size_t IrdaAppRemoteManager::get_current_remote_buttons_number() {
    return remotes[current_remote_index].buttons.size();
}

void IrdaAppRemoteManager::show_file_error_message(const char* error_text) {
    sd_ex_api->show_error(sd_ex_api->context, error_text);
}

bool IrdaAppRemoteManager::sync_current_remote(void) {
    File file;
    FS_Error fs_result;
    uint16_t write_count;

    // Create directory if necessary
    fs_result = fs_api->common.mkdir(irda_directory);
    if(fs_result != FSE_OK && fs_result != FSE_EXIST) {
        show_file_error_message("Cannot create\napplication directory");
        return false;
    }

    auto& current_remote = remotes[current_remote_index];
    std::string filename = std::string(irda_directory) + "/" + current_remote.name + irda_extension;
    bool res = fs_api->file.open(
        &file, filename.c_str(), FSAM_WRITE, FSOM_CREATE_ALWAYS);

    if(!res) {
        show_file_error_message("Cannot create\nnew key file");
        return false;
    }

    char content[128];
    auto button_message = get_button_data(get_current_button());

    sniprintf(content, sizeof(content),
              "%.32s %02X [%.20s] A:%0*lX C:%0*lX\n",
              get_current_button_name().c_str(),
              button_message->protocol,
              irda_get_protocol_name(button_message->protocol),
              irda_get_protocol_address_length(button_message->protocol),
              button_message->address,
              irda_get_protocol_command_length(button_message->protocol),
              button_message->command);

    auto content_len = strlen(content);
    write_count = fs_api->file.write(&file, content, content_len);
    if(file.error_id != FSE_OK || write_count != content_len) {
        show_file_error_message("Cannot write\nto key file");
        fs_api->file.close(&file);
        return false;
    }

    fs_api->file.close(&file);
    sd_ex_api->check_error(sd_ex_api->context);

    return true;
}

bool IrdaAppRemoteManager::parse_and_save_button(std::string& str) {
    char button_name[32];
    int protocol_scan;
    uint32_t address;
    uint32_t command;

    int parsed = std::sscanf(str.c_str(), "%31s %u %*s A:%lX C:%lX", button_name, &protocol_scan, &address, &command);

    if (parsed != 4) {
        return false;
    }

    IrdaProtocol protocol = (IrdaProtocol) protocol_scan;

    if (!irda_is_protocol_valid((IrdaProtocol) protocol)) {
        return false;
    }

    int address_length = irda_get_protocol_address_length(protocol);
    uint32_t address_mask = (1LU << (4 * address_length)) - 1;
    if (address != (address & address_mask)) {
        return false;
    }

    int command_length = irda_get_protocol_command_length(protocol);
    uint32_t command_mask = (1LU << (4 * command_length)) - 1;
    if (command != (command & command_mask)) {
        return false;
    }

    IrdaMessage irda_message = {
        .protocol = protocol,
        .address = address,
        .command = command,
        .repeat = false,
    };
    add_button(button_name, &irda_message);

    return true;
}

std::string getline(const FS_Api* fs_api, File& file, char file_buf[], size_t file_buf_size, size_t& file_buf_cnt) {
    std::string str;
    size_t newline_index = 0;
    bool found_eol = false;

    while (1) {
        if (file_buf_cnt > 0) {
            size_t end_index = 0;
            char* endline_ptr = (char*) memchr(file_buf, '\n', file_buf_cnt);
            newline_index = endline_ptr - file_buf;

            if (endline_ptr == 0) {
                end_index = file_buf_cnt;
            } else if (newline_index < file_buf_cnt) {
                end_index = newline_index + 1;
                found_eol = true;
            } else {
                furi_assert(0);
            }

            str.append(file_buf, end_index);
            memmove(file_buf, &file_buf[end_index], file_buf_cnt - end_index);
            file_buf_cnt = file_buf_cnt - end_index;
            if (found_eol)
                break;
        }

        file_buf_cnt += fs_api->file.read(&file, &file_buf[file_buf_cnt], file_buf_size - file_buf_cnt);
        if (file_buf_cnt == 0) {
            break;      // end of reading
        }
    }

    return str;
}

bool IrdaAppRemoteManager::load() {
    bool fs_res = false;
    bool result = false;
    int remote_cnt = 0;
    File dir;
    File file;
    std::string dirname(std::string("/") + irda_directory);

    fs_res = fs_api->dir.open(&dir, dirname.c_str());
    if(!fs_res) {
        show_file_error_message("Cannot open\napplication directory");
        return false;
    }

    std::list<std::string> files;
    char name[128];
    while(fs_api->dir.read(&dir, nullptr, name, sizeof(name))
            && strlen(name)) {
        std::string filename(name);
        auto extension_index = filename.rfind(irda_extension);
        if ((extension_index == std::string::npos)
                || (extension_index + strlen(irda_extension) != filename.size())) {
            continue;
        }
        files.emplace_front(name);
    }

    remotes.clear();
    remotes.reserve(files.size());

    for (auto &filename : files) {
        std::string full_filename(dirname + "/" + filename);
        fs_res = fs_api->file.open(&file, full_filename.c_str(), FSAM_READ, FSOM_OPEN_EXISTING);
        if (!fs_res) {
            show_file_error_message("Error opening file");
            result = false;
            break;
        }
 
        auto extension_index = filename.rfind(irda_extension);
        remotes.emplace_back(filename.erase(extension_index));
        set_current_remote(remote_cnt);
        ++remote_cnt;

        while (1) {
            auto str = getline(fs_api, file, file_buf, sizeof(file_buf), file_buf_cnt);
            if (str.empty())
                break;
            parse_and_save_button(str);
        }
        fs_api->file.close(&file);
        result = true;
    }
    files.clear();
    fs_api->dir.close(&dir);
    return result;
}

