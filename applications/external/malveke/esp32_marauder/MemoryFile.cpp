#include "MemoryFile.h"

MemoryFile::MemoryFile(const uint8_t* data, size_t size) : data_(data), size_(size), position_(0) {}

bool MemoryFile::open() {
    position_ = 0;
    return true;
}

void MemoryFile::close() {
    position_ = 0;
}

int MemoryFile::read() {
    if (position_ < size_) {
        return data_[position_++];
    }
    return -1; // Return -1 at end of file
}

size_t MemoryFile::available() {
    return size_ - position_;
}

bool MemoryFile::seekCur(long offset) {
    if (position_ + offset >= 0 && position_ + offset <= size_) {
        position_ += offset;
        return true;
    }
    return false;
}

int MemoryFile::read(char* buffer, size_t length) {
    int bytesRead = 0;
    for (size_t i = 0; i < length; i++) {
        int nextByte = read();
        if (nextByte == -1) {
            break;
        }
        buffer[i] = static_cast<char>(nextByte);
        bytesRead++;
    }
    return bytesRead;
}

int MemoryFile::peek() {
    if (position_ < size_) {
        return data_[position_];
    }
    return -1; // Return -1 at end of file
}

uint32_t MemoryFile::curPosition() const {
    return position_;
}
