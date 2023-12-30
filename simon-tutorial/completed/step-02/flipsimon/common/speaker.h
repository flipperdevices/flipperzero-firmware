#pragma once

/**
 * @file speaker.h
 * @brief This file contains the speaker module.
 * @details The speaker module is used to play tones on the internal
 * Flipper Zero speaker.
*/

typedef struct Speaker Speaker;

/**
 * @brief Allocates a new speaker.
 * @details Allocates a new speaker.  The internal Flipper Zero speaker is
 * used to play tones.  Defaults the frequency to 0 and the volume to 1.0
 * (maximum volume).
 * @return The new speaker.
*/
Speaker* speaker_alloc(void);

/**
 * @brief Frees a speaker.
 * @param speaker The speaker to free.
*/
void speaker_free(Speaker* speaker);

/**
 * @brief Sets the frequency of the tone to play.
 * @param speaker The speaker to set the frequency for.
 * @param frequency The frequency to play (in Hz).
*/
void speaker_set_frequency(Speaker* speaker, float frequency);

/**
 * @brief Sets the volume of the tone to play.
 * @param speaker The speaker to set the volume for.
 * @param volume The volume to play (0.0 - 1.0).
*/
void speaker_set_volume(Speaker* speaker, float volume);
