#include "irda-app-file-reader.hpp"

std::string getline(
    const FS_Api* fs_api,
    File* file,
    char file_buf[],
    size_t file_buf_size,
    size_t& file_buf_cnt) {
    std::string str;
    size_t newline_index = 0;
    bool found_eol = false;

    while(1) {
        if(file_buf_cnt > 0) {
            size_t end_index = 0;
            char* endline_ptr = (char*)memchr(file_buf, '\n', file_buf_cnt);
            newline_index = endline_ptr - file_buf;

            if(endline_ptr == 0) {
                end_index = file_buf_cnt;
            } else if(newline_index < file_buf_cnt) {
                end_index = newline_index + 1;
                found_eol = true;
            } else {
                furi_assert(0);
            }

            str.append(file_buf, end_index);
            memmove(file_buf, &file_buf[end_index], file_buf_cnt - end_index);
            file_buf_cnt = file_buf_cnt - end_index;
            if(found_eol) break;
        }

        file_buf_cnt +=
            fs_api->file.read(file, &file_buf[file_buf_cnt], file_buf_size - file_buf_cnt);
        if(file_buf_cnt == 0) {
            break; // end of reading
        }
    }

    return str;
}

std::unique_ptr<IrdaAppFileReader::IrdaFileMessage>
IrdaAppFileReader::read_message(File* file) {
    while(1) {
        auto str = getline(fs_api, file, file_buf, sizeof(file_buf), file_buf_cnt);
        if(str.empty()) return nullptr;

        auto message = parse_message(str);
        if(message)
            return message;
        else {
            printf("can't parse: \'%s\'\n", str.c_str());
        }
    }
}

std::unique_ptr<IrdaAppFileReader::IrdaFileMessage> IrdaAppFileReader::parse_message(const std::string& str) const {
    char protocol_name[32];
    uint32_t address;
    uint32_t command;
    auto irda_file_message = std::make_unique<IrdaFileMessage>();

    int parsed = std::sscanf(
        str.c_str(), "%31s %31s A:%lX C:%lX", irda_file_message->name, protocol_name, &address, &command);

    if(parsed != 4) {
        return nullptr;
    }

    IrdaProtocol protocol = irda_get_protocol_by_name(protocol_name);

    if(!irda_is_protocol_valid((IrdaProtocol)protocol)) {
        return nullptr;
    }

    int address_length = irda_get_protocol_address_length(protocol);
    uint32_t address_mask = (1LU << (4 * address_length)) - 1;
    if(address != (address & address_mask)) {
        return nullptr;
    }

    int command_length = irda_get_protocol_command_length(protocol);
    uint32_t command_mask = (1LU << (4 * command_length)) - 1;
    if(command != (command & command_mask)) {
        return nullptr;
    }

    irda_file_message->message = {
        .protocol = protocol,
        .address = address,
        .command = command,
        .repeat = false,
    };

    return irda_file_message;
}

