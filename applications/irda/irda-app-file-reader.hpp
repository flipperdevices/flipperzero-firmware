#include <string>
#include <memory>
#include "sd-card-api.h"
#include "filesystem-api.h"
#include "irda.h"

class IrdaAppFileReader {
    typedef struct {
        char name[32];
        IrdaMessage message;
    } IrdaFileMessage;

    char file_buf[48];
    size_t file_buf_cnt = 0;
    File file;
    SdCard_Api* sd_ex_api;
    FS_Api* fs_api;

    std::unique_ptr<IrdaFileMessage> parse_message(const std::string& str) const;

public:
    std::unique_ptr<IrdaAppFileReader::IrdaFileMessage> read_message();
    bool open_file(const char* filename);
    void close_file();
    IrdaAppFileReader(SdCard_Api* sd_ex_api, FS_Api* fs_api)
        : file_buf_cnt(0) , sd_ex_api(sd_ex_api), fs_api(fs_api) {}
    ~IrdaAppFileReader() {
        close_file();
    }
};

