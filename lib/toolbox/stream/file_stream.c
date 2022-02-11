#include "stream.h"
#include "stream_i.h"
#include "file_stream.h"
#include <flipper_file/file_helper.h>

typedef struct {
    Stream stream_base;
    Storage* storage;
    File* file;
} FileStream;

static void file_stream_free(FileStream* stream);
static bool file_stream_eof(FileStream* stream);
static void file_stream_clean(FileStream* stream);
static bool file_stream_seek(FileStream* stream, int32_t offset, StreamOffset offset_type);
static size_t file_stream_tell(FileStream* stream);
static size_t file_stream_size(FileStream* stream);
static size_t file_stream_write(FileStream* stream, const uint8_t* data, size_t size);
static size_t file_stream_read(FileStream* stream, uint8_t* data, size_t size);
static int32_t file_stream_delete_and_insert(
    FileStream* stream,
    size_t delete_size,
    StreamWriteCB write_callback,
    const void* ctx);

const StreamVTable file_stream_vtable = {
    .free = (StreamFreeFn)file_stream_free,
    .eof = (StreamEOFFn)file_stream_eof,
    .clean = (StreamCleanFn)file_stream_clean,
    .seek = (StreamSeekFn)file_stream_seek,
    .tell = (StreamTellFn)file_stream_tell,
    .size = (StreamSizeFn)file_stream_size,
    .write = (StreamWriteFn)file_stream_write,
    .read = (StreamReadFn)file_stream_read,
    .delete_and_insert = (StreamDeleteAndInsertFn)file_stream_delete_and_insert,
};

Stream* file_stream_alloc(Storage* storage) {
    FileStream* stream = malloc(sizeof(FileStream));
    stream->file = storage_file_alloc(storage);
    stream->storage = storage;

    stream->stream_base.vtable = &file_stream_vtable;
    return (Stream*)stream;
}

bool file_stream_open(
    Stream* _stream,
    const char* path,
    FS_AccessMode access_mode,
    FS_OpenMode open_mode) {
    FileStream* stream = (FileStream*)_stream;
    furi_check(stream->stream_base.vtable == &file_stream_vtable);
    return storage_file_open(stream->file, path, access_mode, open_mode);
}

static void file_stream_free(FileStream* stream) {
    storage_file_free(stream->file);
    free(stream);
}

static bool file_stream_eof(FileStream* stream) {
    return storage_file_eof(stream->file);
}

static void file_stream_clean(FileStream* stream) {
    storage_file_seek(stream->file, 0, true);
    storage_file_truncate(stream->file);
}

static bool file_stream_seek(FileStream* stream, int32_t offset, StreamOffset offset_type) {
    bool result = false;
    size_t seek_position = 0;
    size_t current_position = file_stream_tell(stream);
    size_t size = file_stream_size(stream);

    // calc offset and limit to bottom
    switch(offset_type) {
    case StreamOffsetFromCurrent: {
        if(((int32_t)current_position + offset) >= 0) {
            seek_position = current_position + offset;
            result = true;
        }
    } break;
    case StreamOffsetFromStart: {
        if(offset >= 0) {
            seek_position = offset;
            result = true;
        }
    } break;
    case StreamOffsetFromEnd: {
        if(((int32_t)size + offset) >= 0) {
            seek_position = size + offset;
            result = true;
        }
    } break;
    }

    if(result) {
        // limit to top
        int32_t diff = (seek_position - size);
        if(diff > 0) {
            seek_position -= diff;
            storage_file_seek(stream->file, seek_position, true);
            result = false;
        } else {
            result = storage_file_seek(stream->file, seek_position, true);
        }
    } else {
        storage_file_seek(stream->file, 0, true);
    }

    return result;
}

static size_t file_stream_tell(FileStream* stream) {
    return storage_file_tell(stream->file);
}

static size_t file_stream_size(FileStream* stream) {
    return storage_file_size(stream->file);
}

static size_t file_stream_write(FileStream* stream, const uint8_t* data, size_t size) {
    size_t need_to_write = size;
    while(need_to_write > 0) {
        uint16_t was_written =
            storage_file_write(stream->file, data + (size - need_to_write), need_to_write);
        need_to_write -= was_written;

        if(was_written == 0) break;
    }

    return size - need_to_write;
}

static size_t file_stream_read(FileStream* stream, uint8_t* data, size_t size) {
    size_t need_to_read = size;
    while(need_to_read > 0) {
        uint16_t was_read =
            storage_file_read(stream->file, data + (size - need_to_read), need_to_read);
        need_to_read -= was_read;

        if(was_read == 0) break;
    }

    return size - need_to_read;
}

static const char* flipper_file_scratchpad = "/any/.scratch.pad";
static bool file_stream_get_scratchpad_name(const char** name) {
    // TODO do not rewrite existing file
    *name = flipper_file_scratchpad;
    return true;
}

static int32_t file_stream_delete_and_insert(
    FileStream* _stream,
    size_t delete_size,
    StreamWriteCB write_callback,
    const void* ctx) {
    int32_t result = 0;
    Stream* stream = (Stream*)_stream;

    // open scratchpad
    Stream* scratch_stream = file_stream_alloc(_stream->storage);
    const char* scratch_name = "";
    file_stream_get_scratchpad_name(&scratch_name);

    do {
        size_t current_position = stream_tell(stream);
        size_t file_size = stream_size(stream);

        size_t size_to_delete = file_size - current_position;
        size_to_delete = MIN(delete_size, size_to_delete);

        size_t size_to_copy_before = current_position;
        size_t size_to_copy_after = file_size - current_position - size_to_delete;

        if(!file_stream_open(scratch_stream, scratch_name, FSAM_READ_WRITE, FSOM_CREATE_ALWAYS))
            break;

        // copy file from 0 to insert position to scratchpad
        if(!stream_seek(stream, 0, StreamOffsetFromStart)) break;
        if(stream_copy(stream, scratch_stream, size_to_copy_before) != size_to_copy_before) break;

        if(write_callback) {
            result += write_callback(scratch_stream, ctx);
        }

        // copy key file after insert position + size_to_delete to scratchpad
        if(!stream_seek(stream, size_to_delete, StreamOffsetFromCurrent)) break;
        if(stream_copy(stream, scratch_stream, size_to_copy_after) != size_to_copy_after) break;

        size_t new_file_size = stream_size(scratch_stream);
        if(new_file_size != (size_to_copy_before + size_to_copy_after + result)) break;

        // copy whole scratchpad file to the original file
        if(!stream_seek(stream, 0, StreamOffsetFromStart)) break;
        if(!stream_seek(scratch_stream, 0, StreamOffsetFromStart)) break;
        if(stream_copy(scratch_stream, stream, new_file_size) != new_file_size) break;

        // and truncate original file
        if(!storage_file_truncate(_stream->file)) break;

        // move seek pointer at insert end
        if(!stream_seek(stream, current_position + result, StreamOffsetFromStart)) break;

        result -= size_to_delete;
    } while(false);

    storage_simply_remove(_stream->storage, scratch_name);
    stream_free(scratch_stream);
    return result;
}