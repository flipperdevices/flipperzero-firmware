#include "lib/pforth/pforth.h"
#include "lib/pforth/pf_types.h"
#include "pf_fileio_f0.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-decls"
#include "lib/pforth/pf_guts.h"
#pragma GCC diagnostic pop

FileStream* PF_STDIN;
FileStream* PF_STDOUT;

#define UNIMPLEMENTED(name)         \
    {                               \
        MSG(name);                  \
        MSG("is unimplemented!\n"); \
    }

cell_t sdInputChar(FileStream* Stream) {
    char c;

    if(sdReadFile(&c, 1, 1, Stream) != 1) {
        return EOF;
    }

    return c;
}

FileStream* sdOpenFile(const char* FileName, const char* Mode) {
    FS_AccessMode am = FSAM_READ;
    FS_OpenMode om = FSOM_OPEN_EXISTING;

    if(strcmp(Mode, "r") == 0 || strcmp(Mode, "rb") == 0) {
        am = FSAM_READ;
        om = FSOM_OPEN_EXISTING;
    } else if(strcmp(Mode, "r+") == 0 || strcmp(Mode, "r+b") == 0 || strcmp(Mode, "rb+") == 0) {
        am = FSAM_READ_WRITE;
        om = FSOM_OPEN_EXISTING;
    } else if(strcmp(Mode, "w") == 0 || strcmp(Mode, "wb") == 0) {
        am = FSAM_WRITE;
        om = FSOM_CREATE_ALWAYS;
    } else if(strcmp(Mode, "w+") == 0 || strcmp(Mode, "w+b") == 0 || strcmp(Mode, "wb+") == 0) {
        am = FSAM_READ_WRITE;
        om = FSOM_CREATE_ALWAYS;
    } else if(strcmp(Mode, "a") == 0 || strcmp(Mode, "ab") == 0) {
        am = FSAM_READ_WRITE;
        om = FSOM_OPEN_APPEND;
    } else if(strcmp(Mode, "a+") == 0 || strcmp(Mode, "a+b") == 0 || strcmp(Mode, "ab+") == 0) {
        am = FSAM_READ_WRITE;
        om = FSOM_OPEN_APPEND;
    } else {
        UNIMPLEMENTED("sdOpenFile unsupported mode");
        return NULL;
    }

    FileStream* ret = storage_file_alloc(furi_record_open(RECORD_STORAGE));
    if(ret == NULL) {
        return NULL;
    }

    if(!storage_file_open(ret, FileName, am, om)) {
        storage_file_close(ret);
        storage_file_free(ret);
        return NULL;
    }

    return ret;
}

cell_t sdFlushFile(FileStream* Stream) {
    TOUCH(Stream);
    /*if (!storage_file_sync(Stream)) {
		return EOF;
	}*/

    return 0;
}

cell_t sdReadFile(void* ptr, cell_t Size, int32_t nItems, FileStream* Stream) {
    if(Size != 1) {
        // unsupported
        UNIMPLEMENTED("sdReadFile with Size != 1");
        return 0;
    }

    return storage_file_read(Stream, ptr, nItems);
}

cell_t sdWriteFile(void* ptr, cell_t Size, int32_t nItems, FileStream* Stream) {
    if(Size != 1) {
        // unsupported
        UNIMPLEMENTED("sdWriteFile with Size != 1");
        return 0;
    }

    return storage_file_write(Stream, ptr, nItems);
}

cell_t sdSeekFile(FileStream* Stream, file_offset_t Position, int32_t Mode) {
    bool from_start = false;

    switch(Mode) {
    case PF_SEEK_SET:
        from_start = true;
        break;
    case PF_SEEK_CUR:
        from_start = false;
        break;
    case PF_SEEK_END:
        from_start = true;
        Position += storage_file_size(Stream);
        break;
    default:
        UNIMPLEMENTED("sdSeekFile unsupported mode");
        return -1;
    }

    if(!storage_file_seek(Stream, Position, from_start)) {
        return -1;
    }

    return 0;
}

file_offset_t sdTellFile(FileStream* Stream) {
    return storage_file_tell(Stream);
}

cell_t sdCloseFile(FileStream* Stream) {
    storage_file_close(Stream);
    storage_file_free(Stream);

    return 0;
}

cell_t sdDeleteFile(const char* FileName) {
    if(storage_common_remove(furi_record_open(RECORD_STORAGE), FileName) != FSE_OK) {
        return -1;
    }

    return 0;
}

cell_t sdRenameFile(const char* OldName, const char* NewName) {
    if(storage_common_rename(furi_record_open(RECORD_STORAGE), OldName, NewName) != FSE_OK) {
        return -1;
    }

    return 0;
}

ThrowCode sdResizeFile(FileStream* File, uint64_t NewSize) {
    if(sdSeekFile(File, NewSize, PF_SEEK_SET) != 0) {
        return THROW_RESIZE_FILE;
    }

    if(!storage_file_truncate(File)) {
        return THROW_RESIZE_FILE;
    }

    return 0;
}
