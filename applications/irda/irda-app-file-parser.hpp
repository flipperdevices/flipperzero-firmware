#pragma once
#include <file_reader/file_reader.hpp>
#include <irda.h>
#include "irda-app-remote-manager.hpp"

class IrdaAppFileParser : public FileReader {
public:
    typedef struct {
        char name[32];
        IrdaAppSignal signal;
    } IrdaFileSignal;

    std::unique_ptr<IrdaAppFileParser::IrdaFileSignal> read_signal(File* file);
    bool store_signal(File* file, const IrdaAppSignal& signal, const char* name);

private:
    std::unique_ptr<IrdaFileSignal> parse_signal(const std::string& str) const;
    std::unique_ptr<IrdaFileSignal> parse_signal_raw(const std::string& str) const;
};

