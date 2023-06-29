/**
 * A ring buffer.  This is a circular buffer that can be used to store data until a
 * delimiter is found, at which point the line can be extracted.  The ring buffer is a
 * fixed size and will overwrite old data if the buffer is full.
 * 
 * @author CodeAllNight
*/

#include <furi.h>

/** 
 * The size of the ring buffer.  This is the maximum number of bytes that can be stored in the
 * buffer.  If the buffer is full, the oldest data will be overwritten.
*/
static const uint32_t ring_buffer_size = 4096;

/**
 * A ring buffer.  This is used to store data received from the UART.  The data is stored in a
 * ring buffer so that it can be processed in the background while the main loop is doing other
 * things.
*/
typedef struct {
    // The delimiter character
    char delimiter;

    // If true, the delimiter will be included in the extracted line
    bool include_delimiter;

    // The ring buffer. This is a circular buffer that can be used to store data until a
    // delimiter is found, at which point the line can be extracted.  The ring buffer is a
    // fixed size and will overwrite old data if the buffer is full.
    uint8_t* ring_buffer;

    // The next index to read from the ring buffer.  An empty buffer will have
    // ring_buffer_read == ring_buffer_write
    size_t ring_buffer_read;

    // The next index to write to the ring buffer
    size_t ring_buffer_write;
} RingBuffer;

RingBuffer* ring_buffer_alloc() {
    RingBuffer* buffer = malloc(sizeof(RingBuffer));
    buffer->ring_buffer = malloc(ring_buffer_size);
    buffer->ring_buffer_read = 0;
    buffer->ring_buffer_write = 0;
    buffer->delimiter = '\n';
    buffer->include_delimiter = false;
    return buffer;
}

void ring_buffer_free(RingBuffer* buffer) {
    free(buffer->ring_buffer);
    free(buffer);
}

void ring_buffer_set_delimiter(RingBuffer* rb, char delimiter, bool include_delimiter) {
    rb->delimiter = delimiter;
    rb->include_delimiter = include_delimiter;
}

size_t ring_buffer_available(RingBuffer* rb) {
    size_t available;
    if(rb->ring_buffer_write == rb->ring_buffer_read) {
        // Empty buffer has size - 1 available bytes
        available = ring_buffer_size - 1;
    } else if(rb->ring_buffer_read > rb->ring_buffer_write) {
        // Write can go up to read - 1
        available = rb->ring_buffer_read - rb->ring_buffer_write;
    } else {
        // Write can go up to end of buffer, then from start to read - 1
        available = (ring_buffer_size - rb->ring_buffer_write) + rb->ring_buffer_read;
    }
    return available;
}

bool ring_buffer_add(RingBuffer* rb, uint8_t* data, size_t length) {
    bool hasDelim = false;

    for(size_t i = 0; i < length; i++) {
        // Copy the data into the ring buffer
        rb->ring_buffer[rb->ring_buffer_write] = data[i];

        // Check if the data is the delimiter
        if(data[i] == (uint8_t)rb->delimiter) {
            hasDelim = true;
        }

        // Update the write pointer, wrapping if necessary
        if(++rb->ring_buffer_write >= ring_buffer_size) {
            rb->ring_buffer_write = 0;
        }

        // Check if the buffer is full
        if(rb->ring_buffer_write == rb->ring_buffer_read) {
            // ERROR: buffer is full, discard oldest byte (read index)
            if(++rb->ring_buffer_read >= ring_buffer_size) {
                rb->ring_buffer_read = 0;
            }
        }
    }

    return hasDelim;
}

size_t ring_buffer_find_delim(RingBuffer* rb) {
    size_t index = FURI_STRING_FAILURE;

    // Search for the delimiter, starting at the read index
    size_t i = rb->ring_buffer_read;

    // While the buffer is not empty and the delimiter has not been found
    while(i != rb->ring_buffer_write) {
        // Check if the current byte is the delimiter
        if(rb->ring_buffer[i] == (uint8_t)rb->delimiter) {
            // Found the delimiter
            index = i;
            break;
        }

        // Update the index, wrapping if necessary
        if(++i >= ring_buffer_size) {
            i = 0;
        }
    }

    return index;
}

void ring_buffer_extract_line(RingBuffer* rb, size_t delim_index, FuriString* line) {
    if(delim_index > rb->ring_buffer_read) {
        // line is in one chunk
        furi_string_set_strn(
            line,
            (char*)&rb->ring_buffer[rb->ring_buffer_read],
            delim_index - rb->ring_buffer_read + (rb->include_delimiter ? 1 : 0));
    } else {
        // line is split across the buffer wrap, so we need to copy it in two chunks
        // first chunk is from read index to end of buffer
        furi_string_set_strn(
            line,
            (char*)&rb->ring_buffer[rb->ring_buffer_read],
            ring_buffer_size - rb->ring_buffer_read);

        // second chunk is from start of buffer to delimiter
        for(size_t i = 0; i < delim_index; i++) {
            furi_string_push_back(line, (char)rb->ring_buffer[i]);
        }

        // add the delimiter if required
        if(rb->include_delimiter) {
            furi_string_push_back(line, (char)rb->ring_buffer[delim_index]);
        }
    }

    // update the buffer read pointer, wrapping if necessary
    rb->ring_buffer_read = delim_index + 1;
    if(rb->ring_buffer_read >= ring_buffer_size) {
        rb->ring_buffer_read = 0;
    }
}

void ring_buffer_clear(RingBuffer* rb) {
    rb->ring_buffer_read = 0;
    rb->ring_buffer_write = 0;
}