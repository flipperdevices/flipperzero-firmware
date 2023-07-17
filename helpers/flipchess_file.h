#include <stdbool.h>

typedef enum {
    FlipChessFileBoard,
    FlipChessFileOther,
} FlipChessFile;

bool flipchess_has_file(const FlipChessFile file_type, const char* file_name, const bool remove);
bool flipchess_load_file(char* contents, const FlipChessFile file_type, const char* file_name);
bool flipchess_save_file(
    const char* contents,
    const FlipChessFile file_type,
    const char* file_name,
    const bool append,
    const bool overwrite);