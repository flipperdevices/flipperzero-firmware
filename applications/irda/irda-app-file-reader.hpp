#include <string>
#include <memory>
#include "sd-card-api.h"
#include "filesystem-api.h"
#include "irda.h"

class IrdaAppFileReader {
public:
    typedef struct {
        char name[32];
        IrdaMessage message;
    } IrdaFileMessage;

private:
    char file_buf[48];
    size_t file_buf_cnt = 0;
    SdCard_Api* sd_ex_api;
    FS_Api* fs_api;

    std::unique_ptr<IrdaFileMessage> parse_message(const std::string& str) const;

public:
    std::unique_ptr<IrdaAppFileReader::IrdaFileMessage> read_message(File* file);

    IrdaAppFileReader()
        : file_buf_cnt(0) {
        sd_ex_api = static_cast<SdCard_Api*>(furi_record_open("sdcard-ex"));
        fs_api = static_cast<FS_Api*>(furi_record_open("sdcard"));
    }
    ~IrdaAppFileReader() {
        furi_record_close("sdcard");
        furi_record_close("sdcard-ex");
    }

    SdCard_Api& get_sd_api() {
        return *sd_ex_api;
    }

    FS_Api& get_fs_api() {
        return *fs_api;
    }
};

