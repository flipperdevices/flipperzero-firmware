/* Copyright (C) 2022-2023 Salvatore Sanfilippo -- All Rights Reserved
 * See the LICENSE file for information about the license. */

#include "app.h"

/* Return the time difference between a and b, always >= 0 since
 * the absolute value is returned. */
uint32_t duration_delta(uint32_t a, uint32_t b) {
    return a > b ? a - b : b - a;
}

/* This function starts scanning samples at offset idx looking for the
 * longest run of pulses, either high or low, that are among 10%
 * of each other, for a maximum of three classes. The classes are
 * counted separtely for high and low signals (RF on / off) because
 * many devices tend to have different pulse lenghts depending on
 * the level of the pulse.
 *
 * For instance Oregon2 sensors, in the case of protocol 2.1 will send
 * pulses of ~400us (RF on) VS ~580us (RF off). */
#define SEARCH_CLASSES 3
uint32_t search_coherent_signal(RawSamplesBuffer *s, uint32_t idx) {
    struct {
        uint32_t dur[2];     /* dur[0] = low, dur[1] = high */
        uint32_t count[2];   /* Associated observed frequency. */
    } classes[SEARCH_CLASSES];

    memset(classes,0,sizeof(classes));
    uint32_t minlen = 40, maxlen = 4000; /* Depends on data rate, here we
                                            allow for high and low. */
    uint32_t len = 0; /* Observed len of coherent samples. */
    s->short_pulse_dur = 0;
    for (uint32_t j = idx; j < idx+500; j++) {
        bool level;
        uint32_t dur;
        raw_samples_get(s, j, &level, &dur);
        if (dur < minlen || dur > maxlen) break; /* return. */

        /* Let's see if it matches a class we already have or if we
         * can populate a new (yet empty) class. */
        uint32_t k;
        for (k = 0; k < SEARCH_CLASSES; k++) {
            if (classes[k].count[level] == 0) {
                classes[k].dur[level] = dur;
                classes[k].count[level] = 1;
                break; /* Sample accepted. */
            } else {
                uint32_t classavg = classes[k].dur[level];
                uint32_t count = classes[k].count[level];
                uint32_t delta = duration_delta(dur,classavg);
                if (delta < classavg/10) {
                    /* It is useful to compute the average of the class
                     * we are observing. We know how many samples we got so
                     * far, so we can recompute the average easily.
                     * By always having a better estimate of the pulse len
                     * we can avoid missing next samples in case the first
                     * observed samples are too off. */
                    classavg = ((classavg * count) + dur) / (count+1);
                    classes[k].dur[level] = classavg;
                    classes[k].count[level]++;
                    break; /* Sample accepted. */
                }
            }
        }

        if (k == SEARCH_CLASSES) break; /* No match, return. */

        /* If we are here, we accepted this sample. Try with the next
         * one. */
        len++;
    }

    /* Update the buffer setting the shortest pulse we found
     * among the three classes. This will be used when scaling
     * for visualization. */
    for (int j = 0; j < SEARCH_CLASSES; j++) {
        for (int level = 0; level < 2; level++) {
            if (classes[j].dur[level] == 0) continue;
            if (classes[j].count[level] < 3) continue;
            if (s->short_pulse_dur == 0 ||
                s->short_pulse_dur > classes[j].dur[level])
            {
                s->short_pulse_dur = classes[j].dur[level];
            }
        }
    }
    return len;
}

/* Search the buffer with the stored signal (last N samples received)
 * in order to find a coherent signal. If a signal that does not appear to
 * be just noise is found, it is set in DetectedSamples global signal
 * buffer, that is what is rendered on the screen. */
void scan_for_signal(ProtoViewApp *app) {
    /* We need to work on a copy: the RawSamples buffer is populated
     * by the background thread receiving data. */
    RawSamplesBuffer *copy = raw_samples_alloc();
    raw_samples_copy(copy,RawSamples);

    /* Try to seek on data that looks to have a regular high low high low
     * pattern. */
    uint32_t minlen = 13;           /* Min run of coherent samples. Up to
                                       12 samples it's very easy to mistake
                                       noise for signal. */

    uint32_t i = 0;
    while (i < copy->total-1) {
        uint32_t thislen = search_coherent_signal(copy,i);
        if (thislen > minlen && thislen > app->signal_bestlen) {
            app->signal_bestlen = thislen;
            raw_samples_copy(DetectedSamples,copy);
            DetectedSamples->idx = (DetectedSamples->idx+i)%
                                   DetectedSamples->total;
            FURI_LOG_E(TAG, "Displayed sample updated (%d samples)",
                (int)thislen);
        }
        i += thislen ? thislen : 1;
    }
    raw_samples_free(copy);
}


