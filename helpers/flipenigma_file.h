#include <stdbool.h>

typedef enum {
    FlipEnigmaFileBoard,
    FlipEnigmaFileOther,
} FlipEnigmaFile;

bool flipenigma_has_file(const FlipEnigmaFile file_type, const char* file_name, const bool remove);
bool flipenigma_load_file(char* contents, const FlipEnigmaFile file_type, const char* file_name);
bool flipenigma_save_file(
    const char* contents,
    const FlipEnigmaFile file_type,
    const char* file_name,
    const bool append,
    const bool overwrite);