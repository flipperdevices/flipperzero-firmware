#ifndef GameBoyAdvanceCartridge_h
#define GameBoyAdvanceCartridge_h

#include <iostream>
#include <string>
#include <esp32-hal-timer.h>
#include <esp32-hal-ledc.h>
#include <ArduinoJson.h>
#include "Gameboy.h"

#include "MemoryFile.h"
#include "gba.h"



class GameBoyAdvanceCartridge {

  private:
    char gameTitle[100];
  
    uint16_t romSize;
    unsigned long sramSize = 0;
    word romAddress = 0;
    uint8_t currentBank = 0;
    uint32_t processedProgressBar = 0;
    uint32_t totalProgressBar = 0;

    bool runGameBoyAdvanceCartridge;
    bool writtingRAM;
    bool restoringRAM;
    bool writtingROM;
    void setup();
    

    uint8_t checksum_header_GBA(const uint8_t *header);


    word read_word_GBA(word address);
    // void write_word_GBA(int address, word data);
    void set_address_GBA(word address);
   
    void dataBusAsOutput();
    void dataBusAsInput();


  public:
    GameBoyAdvanceCartridge();
    void main();
    void begin();
    void start();
    void stop();
    void headerROM_GBA(bool printInfo);
};



// Extract ASCII printable characters from input, collapsing underscores and spaces.
// Use when extracting titles from cartridges, to build a rom title.
byte buildRomName(char* output, const byte* input, byte length) {
    byte input_char;
    byte output_len = 0;
    for (unsigned int i = 0; i < length; i++) {
        input_char = input[i];
        if (isprint(input_char) && input_char != '<' && input_char != '>' && input_char != ':' && input_char != '"' && input_char != '/' && input_char != '\\' && input_char != '|' && input_char != '?' && input_char != '*') {
        output[output_len++] = input_char;
        } else {
        if (output_len == 0 || output[output_len - 1] != '_') {
            output[output_len++] = '_';
        }
        }
    }
    while (
        output_len && (output[output_len - 1] == '_' || output[output_len - 1] == ' ')) {
        output_len--;
    }
    output[output_len] = 0;
    return output_len;
}


// ******************************************
// Functions for CRC32 database
// ******************************************
// Skip line
void skip_line(MemoryFile* readfile) {
    int i = 0;
    char str_buf;

    while (readfile->available()) {
        //Read 1 byte from file
        str_buf = static_cast<char>(readfile->read());

        //if end of file or newline found, execute command
        if (str_buf == '\r') {
            readfile->read();  //dispose \n because \r\n
            break;
        }
        i++;
    }  //End while
}

//Get line from file
void get_line(char* str_buf, MemoryFile* readfile, uint8_t maxi) {
    int read_len = readfile->read(str_buf, maxi - 1);

    for (int i = 0; i < read_len; i++) {
        //if end of file or newline found, execute command
        if (str_buf[i] == '\r') {
            str_buf[i] = 0;
            readfile->seekCur(i - read_len + 2);  // +2 to skip over \n because \r\n
            return;
        }
    }
    str_buf[maxi - 1] = 0;
    // EOL was not found, keep looking (slower)
    while (readfile->available()) {
        if (readfile->read() == '\r') {
            readfile->read();  // read \n because \r\n
            break;
        }
    }
}
void rewind_line(MemoryFile& readfile, byte count = 1) {
    uint32_t position = readfile.curPosition();
    // To seek one line back, this code must step over the first newline it finds
    // in order to exit the current line and enter the end of the previous one.
    // Convert <count> from how-many-lines-back into how-many-newlines-to-look-for
    // by incrementing it by 1.
    count++;
    for (byte count_newline = 0; count_newline < count; count_newline++) {
        // Go to the strictly previous '\n', or file start.
        while (position) {
        // Seek back first (keeping position updated)...
        position--;
        readfile.seekCur(-1);
        // ...and check current byte second.
        // Note: this code assumed all files use ASCII with DOS-style newlines
        // so \n is encountered first when seeking backwards.
        if (readfile.peek() == '\n')
            break;
        }
    }
    // If not at file start, the current character is the '\n' just before the
    // desired line, so advance by one.
    if (position)
        readfile.seekCur(1);
}

#endif