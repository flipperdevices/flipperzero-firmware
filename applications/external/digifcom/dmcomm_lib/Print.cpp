#include "Print.h"
#include "flipper.h"

Stream::Stream(App* app)
    : app(app)
    , cb(NULL) {
}

void Stream::set_callback(DmcommCallback callback) {
    FURI_LOG_I(TAG, "set_callback %p", callback);
    this->cb = callback;
}

size_t Stream::write(uint8_t i) {
    if(cb == NULL) return 0;
    char str[10];
    snprintf(str, 10, "%c", (char)i);
    size_t sent = furi_stream_buffer_send(app->dmcomm_output_stream, str, strlen(str), 0);
    return sent;
}

size_t Stream::write(const char* buffer, size_t size) {
    if(cb == NULL) return 0;
    char str[10];
    size_t sent = 0;
    for(size_t i = 0; i < size; i++) {
        snprintf(str, 10, "%c", buffer[i]);
        sent += furi_stream_buffer_send(app->dmcomm_output_stream, str, 1, 0);
    }
    return sent;
}

size_t Stream::print(const char buffer[]) {
    if(cb == NULL) return 0;
    return furi_stream_buffer_send(app->dmcomm_output_stream, buffer, strlen(buffer), 0);
}

size_t Stream::print(int i, int fmt) {
    if(cb == NULL) return 0;
    UNUSED(fmt);
    char str[10];
    snprintf(str, 10, "%d", i);

    size_t sent = furi_stream_buffer_send(app->dmcomm_output_stream, str, strlen(str), 0);
    return sent;
}

size_t Stream::println(void) {
    if(cb == NULL) return 0;
    size_t sent = furi_stream_buffer_send(app->dmcomm_output_stream, "\n", 1, 0);
    if(cb != NULL) cb(app);
    return sent;
}

size_t Stream::println(const char buffer[]) {
    if(cb == NULL) return 0;
    size_t sent = furi_stream_buffer_send(app->dmcomm_output_stream, buffer, strlen(buffer), 0);
    sent += furi_stream_buffer_send(app->dmcomm_output_stream, "\n", 1, 0);
    if(cb != NULL) cb(app);
    return sent;
}

size_t Stream::println(int i, int fmt) {
    if(cb == NULL) return 0;
    UNUSED(fmt);
    char str[10];
    snprintf(str, 10, "%d\n", i);

    size_t sent = furi_stream_buffer_send(app->dmcomm_output_stream, str, strlen(str), 0);
    if(cb != NULL) cb(app);
    return sent;
}

int Stream::read() {
    int ret = -1;
    char s;
    size_t recieved = 1;

    recieved = furi_stream_buffer_receive(app->dmcomm_input_stream, &s, 1, 1);

    if(recieved > 0) {
        ret = (int)s;
    }

    return ret;
}