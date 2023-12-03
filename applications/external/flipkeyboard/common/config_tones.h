#pragma once

/**
 * @file    config_tones.h
 * @brief   Configuration file for the tones.
 * @details This file contains the configuration of the tones.  The tones are
 * a set of frequencies that can be played on the buzzer.  The tones (in Hz) are
 * defined in the tone_values array.  The index of the tone in this array is
 * the same as the index of the tone in the tone_names array.
*/

/**
 * @brief    The frequency of the tones in HZ.
 * @details  The index of the tone in this array is the same as
 *  the index of the tone in the tone_names array.
*/
float tone_values[] = {
    0.000000, // Off
    //
    130.813, // C3
    138.591, // C#3
    146.832, // D3
    155.563, // D#3
    164.814, // E3
    174.614, // F3
    184.997, // F#3
    195.998, // G3
    207.652, // G#3
    220.000, // A3
    233.082, // A#3
    246.942, // B3
    //
    261.626, // C4
    277.183, // C#4
    293.665, // D4
    311.127, // D#4
    329.628, // E4
    349.228, // F4
    369.994, // F#4
    391.995, // G4
    415.305, // G#4
    440.000, // A4
    466.164, // A#4
    493.883, // B4
    //
    523.251, // C5
    554.365, // C#5
    587.330, // D5
    622.254, // D#5
    659.255, // E5
    698.456, // F5
    739.989, // F#5
    783.991, // G5
    830.609, // G#5
    880.000, // A5
    932.328, // A#5
    987.767, // B5
    //
    1046.50, // C6
    1108.73, // C#6
    1174.66, // D6
    1244.51, // D#6
    1318.51, // E6
    1396.91, // F6
    1479.98, // F#6
    1567.98, // G6
    1661.22, // G#6
};

/**
 * @brief    The names of the tones.
 * @details  The index of the tone in this array is the same as
 *  the index of the tone in the tone_values array.
*/
char* tone_names[] = {"Off", "C3",  "C#3", "D3",  "D#3", "E3",  "F3",  "F#3", "G3",  "G#3",
                      "A3",  "A#3", "B3",  "C4",  "C#4", "D4",  "D#4", "E4",  "F4",  "F#4",
                      "G4",  "G#4", "A4",  "A#4", "B4",  "C5",  "C#5", "D5",  "D#5", "E5",
                      "F5",  "F#5", "G5",  "G#5", "A5",  "A#5", "B5",  "C6",  "C#6", "D6",
                      "D#6", "E6",  "F6",  "F#6", "G6",  "G#6", "A6",  "A#6", "B6"};