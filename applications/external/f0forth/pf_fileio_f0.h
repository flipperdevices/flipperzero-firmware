#pragma once

#include <storage/storage.h>

typedef File FileStream;

extern FileStream* PF_STDIN;
extern FileStream* PF_STDOUT;

#ifdef __cplusplus
extern "C" {
#endif

/* Prototypes for stubs. */
FileStream* sdOpenFile(const char* FileName, const char* Mode);
cell_t sdFlushFile(FileStream* Stream);
cell_t sdReadFile(void* ptr, cell_t Size, int32_t nItems, FileStream* Stream);
cell_t sdWriteFile(void* ptr, cell_t Size, int32_t nItems, FileStream* Stream);
cell_t sdSeekFile(FileStream* Stream, file_offset_t Position, int32_t Mode);
cell_t sdRenameFile(const char* OldName, const char* NewName);
cell_t sdDeleteFile(const char* FileName);
ThrowCode sdResizeFile(FileStream*, uint64_t Size);
file_offset_t sdTellFile(FileStream* Stream);
cell_t sdCloseFile(FileStream* Stream);
cell_t sdInputChar(FileStream* Stream);

#ifdef __cplusplus
}
#endif

#define PF_SEEK_SET (0)
#define PF_SEEK_CUR (1)
#define PF_SEEK_END (2)
/*
** printf() is only used for debugging purposes.
** It is not required for normal operation.
*/
#define PRT(x) /* No printf(). */
