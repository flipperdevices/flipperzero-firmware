#include "lib/pforth/pforth.h"
#include "lib/pforth/pf_types.h"
#include "pf_fileio_f0.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-decls"
#include "lib/pforth/pf_guts.h"
#pragma GCC diagnostic pop

FileStream *PF_STDIN;
FileStream *PF_STDOUT;

#define UNIMPLEMENTED(name) { MSG(name); MSG("is unimplemented!\n"); }

cell_t sdInputChar(FileStream *Stream)
{
	char c;

	if (sdReadFile(&c, 1, 1, Stream) != 1) {
		return EOF;
	}

	return c;
}

FileStream *sdOpenFile(const char *FileName, const char *Mode)
{
	if (strcmp(Mode, "r") != 0 && strcmp(Mode, "rb") != 0) {
		/* we only support reading */
    		UNIMPLEMENTED("sdOpenFile for writing");
		return NULL;
	}

	FileStream *ret = storage_file_alloc(furi_record_open(RECORD_STORAGE));
	if (ret == NULL) {
		return NULL;
	}

	if (!storage_file_open(ret, FileName, FSAM_READ, FSOM_OPEN_EXISTING)) {
		storage_file_close(ret);
		storage_file_free(ret);
		return NULL;
	}

	return ret;
}

cell_t sdFlushFile(FileStream *Stream)
{
	TOUCH(Stream);
	return 0;
}

cell_t sdReadFile(void *ptr, cell_t Size, int32_t nItems, FileStream *Stream)
{
	if (Size != 1) {
		// unsupported
    		UNIMPLEMENTED("sdReadFile with Size != 1");
		return 0;
	}

	return storage_file_read(Stream, ptr, nItems);
}

cell_t sdWriteFile(void *ptr, cell_t Size, int32_t nItems, FileStream *Stream)
{
	UNIMPLEMENTED("sdWriteFile");
	TOUCH(ptr);
	TOUCH(Size);
	TOUCH(nItems);
	TOUCH(Stream);
	return 0;
}

cell_t sdSeekFile(FileStream *Stream, file_offset_t Position, int32_t Mode)
{
	bool from_start = false;

	switch(Mode) {
	case PF_SEEK_SET:
		from_start = true;
		break;
	case PF_SEEK_CUR:
		from_start = false;
		break;
	default:
		UNIMPLEMENTED("sdSeekFile with given mode");
		return -1;
	}

	if (!storage_file_seek(Stream, Position, from_start)) {
		return -1;
	}

	return 0;
}

file_offset_t sdTellFile(FileStream *Stream)
{
	return storage_file_tell(Stream);
}

cell_t sdCloseFile(FileStream *Stream)
{
	storage_file_close(Stream);
	storage_file_free(Stream);

	return 0;
}

cell_t sdDeleteFile(const char *FileName)
{
	UNIMPLEMENTED("sdDeleteFile");
	TOUCH(FileName);
	return -1;
}

cell_t sdRenameFile(const char *OldName, const char *NewName)
{
	UNIMPLEMENTED("sdRenameFile");
	TOUCH(OldName);
	TOUCH(NewName);
	return -1;
}

ThrowCode sdResizeFile(FileStream *File, uint64_t NewSize)
{
	UNIMPLEMENTED("sdResizeFile");
	TOUCH(File);
	TOUCH(NewSize);
	return THROW_RESIZE_FILE;
}
