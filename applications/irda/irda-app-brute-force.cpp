#include "irda-app-brute-force.hpp"

void IrdaAppBruteForce::add_record(int index, const char* name) {
    records[name].index = index;
    records[name].amount = 0;
}

bool IrdaAppBruteForce::calculate_messages() {
    bool fs_res = false;
    fs_res = file_reader.get_fs_api().file.open(&file, universal_db_filename, FSAM_READ, FSOM_OPEN_EXISTING);
    if (!fs_res) {
        file_reader.get_sd_api().show_error(file_reader.get_sd_api().context, "Can't open file");
        return false;
    }

    file_reader.reset();
    while(1) {
        auto message = file_reader.read_message(&file);
        if (!message) break;
        auto element = records.find(message->name);
        if (element != records.cend()) {
            ++element->second.amount;
        }
    }

    file_reader.get_fs_api().file.close(&file);

    return true;
}

void IrdaAppBruteForce::stop_bruteforce() {
    if (current_record.size()) {
        file_reader.get_fs_api().file.close(&file);
        current_record.clear();
    }
}

// TODO: [FL-1418] replace with timer-chained consequence of messages.
bool IrdaAppBruteForce::send_next_bruteforce(const IrdaAppSignalReceiver& receiver) {
    furi_assert(current_record.size());

    std::unique_ptr<IrdaAppFileReader::IrdaFileMessage> message;

    do {
        message = file_reader.read_message(&file);
    } while (message && current_record.compare(message->name));

    if (message) {
        receiver.send_message(&message->message);
    }
    return !!message;
}

bool IrdaAppBruteForce::start_bruteforce(int index, int& record_amount) {
    file_reader.reset();
    for (const auto& it : records) {
        if (it.second.index == index) {
            record_amount = it.second.amount;
            current_record = it.first;
            break;
        }
    }

    if (record_amount) {
        bool fs_res = file_reader.get_fs_api().file.open(&file, universal_db_filename, FSAM_READ, FSOM_OPEN_EXISTING);
        if (fs_res) {
            return true;
        } else {
            file_reader.get_sd_api().show_error(file_reader.get_sd_api().context, "Can't open file");
        }
    }

    return false;
}

