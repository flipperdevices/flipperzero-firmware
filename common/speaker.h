#pragma once

typedef struct Speaker Speaker;

Speaker* speaker_alloc(void);
void speaker_set_frequency(Speaker* speaker, float frequency);
void speaker_set_volume(Speaker* speaker, float volume);
void speaker_free(Speaker* speaker);