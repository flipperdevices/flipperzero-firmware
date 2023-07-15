#include <stdint.h>

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

EXTERNC int32_t flipchess_voice_game(void* p);
EXTERNC int32_t flipchess_voice_no(void* p);

#undef EXTERNC
