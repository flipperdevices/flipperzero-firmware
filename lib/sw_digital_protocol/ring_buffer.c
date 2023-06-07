#include "ring_buffer.h"

struct RingBuffer {
    uint8_t* read_prt;
    uint8_t* write_prt;
    uint8_t* start_prt;
    uint8_t* end_prt;
    volatile bool is_full;
};

RingBuffer* ring_buffer_alloc(uint32_t size) {
    RingBuffer* ring_buffer = (RingBuffer*)malloc(sizeof(RingBuffer));
    ring_buffer->start_prt = (uint8_t*)malloc(size * sizeof(uint8_t));
    ring_buffer->end_prt = ring_buffer->start_prt + size;
    ring_buffer->read_prt = ring_buffer->start_prt;
    ring_buffer->write_prt = ring_buffer->start_prt;
    ring_buffer->is_full = false;

    return ring_buffer;
}

void ring_buffer_free(RingBuffer* ring_buffer) {
    furi_assert(ring_buffer);
    free(ring_buffer->start_prt);
    free(ring_buffer);
}

void ring_buffer_reset(RingBuffer* ring_buffer) {
    furi_assert(ring_buffer);
    ring_buffer->read_prt = ring_buffer->start_prt;
    ring_buffer->write_prt = ring_buffer->start_prt;
    ring_buffer->is_full = false;
}

void ring_buffer_write_byte(RingBuffer* ring_buffer, uint8_t data) {
    furi_assert(ring_buffer);

    if(ring_buffer->is_full) {
        if(ring_buffer->write_prt != ring_buffer->read_prt) {
            ring_buffer->is_full = false;
        }
    }

    if(!ring_buffer->is_full) {
        if(ring_buffer->write_prt + 1 < ring_buffer->end_prt) {
            *ring_buffer->write_prt = data;
            ring_buffer->write_prt++;
        } else {
            *ring_buffer->write_prt = data;
            ring_buffer->write_prt = ring_buffer->start_prt;
        }

        if(ring_buffer->write_prt == ring_buffer->read_prt) {
            ring_buffer->is_full = true;
        }
    }
}

bool ring_buffer_write(RingBuffer* ring_buffer, uint8_t* data, size_t size) {
    furi_assert(ring_buffer);
    furi_assert(data);
    furi_assert(size);
    bool ret = true;
    for(size_t i = 0; i < size; i++) {
        if(ring_buffer->is_full) {
            ret = false;
            break;
        }
        ring_buffer_write_byte(ring_buffer, data[i]);
    }
    return ret;
}

bool ring_buffer_read_byte(RingBuffer* ring_buffer, uint8_t* data) {
    furi_assert(ring_buffer);
    furi_assert(data);

    if(ring_buffer->read_prt == ring_buffer->write_prt && !ring_buffer->is_full) {
        return false;
    }

    if(ring_buffer->read_prt + 1 < ring_buffer->end_prt) {
        *data = *ring_buffer->read_prt;
        ring_buffer->read_prt++;
    } else {
        *data = *ring_buffer->read_prt;
        ring_buffer->read_prt = ring_buffer->start_prt;
    }
    return true;
}

size_t ring_buffer_read(RingBuffer* ring_buffer, uint8_t* data, size_t size) {
    furi_assert(ring_buffer);
    furi_assert(data);
    furi_assert(size);
    size_t ret = 0;
    for(size_t i = 0; i < size; i++) {
        if(!ring_buffer_read_byte(ring_buffer, &data[i])) {
            break;
        }
        ret++;
    }
    return ret;
}

size_t ring_buffer_spaces_available(RingBuffer* ring_buffer) {
    furi_assert(ring_buffer);
    return ring_buffer->end_prt - ring_buffer->start_prt - ring_buffer_available(ring_buffer);
}

size_t ring_buffer_available(RingBuffer* ring_buffer) {
    furi_assert(ring_buffer);
    if(ring_buffer->write_prt >= ring_buffer->read_prt) {
        return ring_buffer->write_prt - ring_buffer->read_prt;
    } else {
        return (ring_buffer->end_prt - ring_buffer->read_prt) +
               (ring_buffer->write_prt - ring_buffer->start_prt);
    }
}