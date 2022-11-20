#pragma once
#include "tracker_notes.h"
#include "tracker_song.h"

typedef enum {
    TrackerPositionChanged,
    TrackerEndOfSong,
} TrackerMessageType;

typedef struct {
    TrackerMessageType type;
    union tracker_message_data {
        struct {
            uint8_t pattern;
            uint8_t row;
        } position;
    } data;
} TrackerMessage;

typedef void (*TrackerMessageCallback)(TrackerMessage message, void* context);

typedef struct Tracker Tracker;

Tracker* tracker_alloc();

void tracker_free(Tracker* tracker);

void tracker_set_message_callback(Tracker* tracker, TrackerMessageCallback callback, void* context);

void tracker_set_song(Tracker* tracker, Song* song);

void tracker_set_pattern(Tracker* tracker, uint8_t pattern);

void tracker_set_row(Tracker* tracker, uint8_t row);

void tracker_start(Tracker* tracker);

void tracker_stop(Tracker* tracker);