#ifndef MEMORYFILE_H
#define MEMORYFILE_H

#include <Arduino.h>

class MemoryFile {
public:
    MemoryFile(const uint8_t* data, size_t size);

    bool open();
    void close();
    int read();
    size_t available();
    bool seekCur(long offset);
    int read(char* buffer, size_t length);
    int peek();
    uint32_t curPosition() const;

private:
    const uint8_t* data_;
    size_t size_;
    size_t position_;
};

#endif // MEMORYFILE_H
