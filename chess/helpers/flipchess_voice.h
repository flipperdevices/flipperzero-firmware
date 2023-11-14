#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

EXTERNC void flipchess_voice_shall_we_play();
EXTERNC void flipchess_voice_which_side();
EXTERNC void flipchess_voice_how_about_chess();
EXTERNC void flipchess_voice_a_strange_game();

#undef EXTERNC
